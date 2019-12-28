#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "common.h"

#include "WinSCard-x3U4.h"

#include <Windows.h>
#include <algorithm>

#include "t1.h"
#include "atr.h"
#include "CCOMProc-x3U4.h"
#include "CIniFileAccess.h"

static constexpr SCARDHANDLE DUMMY_SCARDHANDLE = 0x5ca2d4a1;
static constexpr SCARDCONTEXT DUMMY_SCARDCONTEXT = 0xc013e103;
static constexpr CHAR READER_NAME_A[] = "Plex PX-x3U4 Card Reader 0";
static constexpr WCHAR READER_NAME_W[] = L"Plex PX-x3U4 Card Reader 0";
static constexpr CHAR LIST_READERS_A[] = "Plex PX-x3U4 Card Reader 0\0";
static constexpr WCHAR LIST_READERS_W[] = L"Plex PX-x3U4 Card Reader 0\0";

static BYTE IFSD = 254;						// IFD���̍ő��M�\�u���b�N�T�C�Y

static DWORD l_dwEstablishedContext = 0;
static HANDLE l_hStartedEvent = NULL;
static HMODULE l_hModule = NULL;
static HANDLE l_hSemaphore = NULL;
static HANDLE l_hMapFile = NULL;

static SharedMemory *l_pShMem = NULL;

static CParseATR ParseATR;
static CCOMProc COMProc;
static CComProtocolT1x3U4 Protocol;
static CRITICAL_SECTION l_csInit;

static void CloseAllHandle(void) {
	if (l_pShMem) {
		::UnmapViewOfFile(l_pShMem);
		l_pShMem = NULL;
	}
	if (l_hMapFile) {
		try {
			SAFE_CLOSE_HANDLE(l_hMapFile);
		}
		catch (...) {
		}
	}
	if (l_hSemaphore) {
		try {
			SAFE_CLOSE_HANDLE(l_hSemaphore);
		}
		catch (...) {
		}
	}
	return;
}

static void ProcATR(BYTE *atr) {
	ParseATR.Parse(atr, sizeof(atr));
	Protocol.SetCardIFSC(ParseATR.ParsedInfo.IFSC);
	Protocol.SetEDCType(ParseATR.ParsedInfo.ErrorDetection == CParseATR::ERROR_DETECTION_CRC ? CComProtocolT1::EDC_TYPE_CRC : CComProtocolT1::EDC_TYPE_LRC);
	Protocol.SetNodeAddress(0, 0);

	return;
}

static BOOL InitDevice(void) {
	OutputDebug(L"InitDevice: Started.\n");

	if (!COMProc.CreateThread()) {
		OutputDebug(L"InitDevice: Error in creating CCOMProc thread.\n");
		return FALSE;
	}

	do {
		// �v���Z�X�Ԕr���p�̃Z�}�t�H�쐬
		std::wstring guid = COMProc.GetTunerDisplayName();
		std::wstring::size_type len = guid.find_last_of(L"#");
		std::wstring str = guid.substr(0, len);
		std::replace(str.begin(), str.end(), L'\\', L'/');
		std::wstring semname1 = L"Global\\WinSCard-x3U4_Lock" + str;
		std::wstring semname2 = L"Local\\WinSCard-x3U4_Lock" + str;
		std::wstring mapname1 = L"Global\\WinSCard-x3U4_MapFile" + str;
		std::wstring mapname2 = L"Local\\WinSCard-x3U4_MapFile" + str;
		l_hSemaphore = ::CreateSemaphoreW(NULL, 1, 1, semname1.c_str());
		if (!l_hSemaphore) {
			OutputDebug(L"InitDevice: Error creating Semaphore Object for Global Namespace. Trying OpenSemaphore().\n");
			l_hSemaphore = ::OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, semname1.c_str());
			if (!l_hSemaphore) {
				OutputDebug(L"InitDevice: Error opening Semaphore Object for Global Namespace. Trying Session Namespace.\n");
				l_hSemaphore = ::CreateSemaphoreW(NULL, 1, 1, semname2.c_str());
				if (!l_hSemaphore) {
					OutputDebug(L"InitDevice: Error creating Semaphore Object for Session Namespace. Canceled.\n");
					break;
				}
			}
		}
		l_hMapFile = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, sizeof(SharedMemory) / 0x10000, sizeof(SharedMemory) % 0x10000, mapname1.c_str());
		if (!l_hMapFile) {
			OutputDebug(L"InitDevice: Error creating File Mapping Object for Global Namespace. Trying OpenFileMapping().\n");
			l_hMapFile = ::OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, mapname1.c_str());
			if (!l_hMapFile) {
				OutputDebug(L"InitDevice: Error opening File Mapping Object for Global Namespace. Trying Session Namespace.\n");
				l_hMapFile = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, sizeof(SharedMemory) / 0x10000, sizeof(SharedMemory) % 0x10000, mapname2.c_str());
				if (!l_hMapFile) {
					OutputDebug(L"InitDevice: Error creating File Mapping Object for Session Namespace. Canceled.\n");
					break;
				}
			}
		}
		l_pShMem = (SharedMemory*)::MapViewOfFile(l_hMapFile, FILE_MAP_WRITE, 0, 0, 0);
		if (!l_pShMem) {
			OutputDebug(L"InitDevice: Error in MapViewOfFile().\n");
			break;
		}

		if (l_pShMem->ATRLength > 2 && l_pShMem->ATR[0] == CParseATR::CONVENTION_DIRECT) {
			ProcATR(l_pShMem->ATR);
			OutputDebug(L"InitDevice: ATR was read from shared memory.\n");
		}

		OutputDebug(L"InitDevice: Completed.\n");
		return TRUE;
	} while (0);

	COMProc.TerminateThread();
	CloseAllHandle();

	OutputDebug(L"InitDevice: Exit with error.\n");
	return FALSE;
}

static BOOL ResetCard(void)
{
	OutputDebug(L"ResetCard: Started.\n");

	if (!l_pShMem) {
		OutputDebug(L"ResetCard: Device has not been initialized.\n");
		return FALSE;
	}

	HRESULT hr;
	CComProtocolT1::COM_PROTOCOL_T1_ERROR_CODE r;
	BOOL present;
	BYTE rxpcb;
	BYTE rxbuf[3 + 254 + 2] = {};
	BYTE rxlen;
	BYTE atrbuf[33];
	DWORD atrlen;

	// Card�����݂��Ă��邩�m�F
	if (FAILED(hr = COMProc.DetectCard(&present))) {
		OutputDebug(L"ResetCard: Error in it35_CardDetect(). code=0x%x\n", hr);
		return FALSE;
	}
	if (!present) {
		OutputDebug(L"ResetCard: Card is not present.\n");
		return FALSE;
	}

	int retry = 0;
	while (!l_pShMem->CardReady) {
		if (!l_pShMem->DoneReset) {
			// Card ���Z�b�g�����K�v
			OutputDebug(L"ResetCard: Need reset.\n");

			// Card ���Z�b�g
			if (FAILED(hr = COMProc.ResetCard())) {
				OutputDebug(L"ResetCard: Error in it35_ResetSmartCard(). code=0x%x\n", hr);
				return FALSE;
			}

			::Sleep(50);

			// ATR�擾
			if (FAILED(hr = COMProc.GetATRData(atrbuf))) {
				OutputDebug(L"ResetCard: Error in it35_GetATR(). code=0x%x\n", hr);
				return FALSE;
			}

			atrlen = sizeof(atrbuf);
			if (FAILED(hr = COMProc.GetUARTData(atrbuf, &atrlen))) {
				OutputDebug(L"ResetCard: Error in it35_GetUartData() for getting ATR. code=0x%x\n", hr);
				return FALSE;
			}

			// ATR ���
			ProcATR(atrbuf);
			memcpy(l_pShMem->ATR, ParseATR.RawData, ParseATR.RawDataLength);
			l_pShMem->ATRLength = ParseATR.RawDataLength;

			// �{�[���[�g�Z�b�g...9600��19200������t���Ȃ����ۂ�...
			if (FAILED(hr = COMProc.SetUARTBaudRate(19200))) {
				OutputDebug(L"ResetCard: Error in it35_SetUartBaudRate(). code=0x%x\n", hr);
				return FALSE;
			}
			l_pShMem->DoneReset = TRUE;
		}

		// RESYNCH �v��
		if ((r = Protocol.SendSBlock(FALSE, CComProtocolT1::SBLOCK_FUNCTION_RESYNCH, NULL, 0)) != CComProtocolT1::COM_PROTOCOL_T1_S_NO_ERROR) {
			OutputDebug(L"ResetCard: Error sending RESYNCH. code=%d\n", r);
			l_pShMem->DoneReset = FALSE;
			retry++;
			if (retry > 2)
				return FALSE;
			break;
		}

		// ���X�|���X�擾
		if ((r = Protocol.RecvBlock(&rxpcb, rxbuf, &rxlen)) != CComProtocolT1::COM_PROTOCOL_T1_S_NO_ERROR) {
			OutputDebug(L"ResetCard: Error Receiving RESYNCH response. code=%d\n", r);
			l_pShMem->DoneReset = FALSE;
			retry++;
			if (retry > 2)
				return FALSE;
			break;
		}

		// ���X�|���X�m�F
		if (rxpcb != (CComProtocolT1::SBLOCK_FUNCTION_RESYNCH | CComProtocolT1::SBLOCK_RESPONSE)) {
			OutputDebug(L"ResetCard: PCB does not match RESYNCH response.\n");
			l_pShMem->DoneReset = FALSE;
			retry++;
			if (retry > 2)
				return FALSE;
			break;
		}

		// IFSD �l�ʒm
		// �f�t�H���g�l��32�ł�M�n�̃J�[�h�̕s��i���Ȃ��Ƃ�ISO7816-4�̎d�l�ǂ���ɂ͓��삵�Ȃ��j�Ɉ���������...
		// ���Ă������AARIB STD-B25�ɂ͕K���ő�l��254�ɕύX����Ɩ��L����Ă��邵...
		if ((r = Protocol.SendSBlock(FALSE, CComProtocolT1::SBLOCK_FUNCTION_IFS, &IFSD, sizeof(IFSD))) != CComProtocolT1::COM_PROTOCOL_T1_S_NO_ERROR) {
			OutputDebug(L"ResetCard: Error sending IFS. code=%d\n", r);
			l_pShMem->DoneReset = FALSE;
			retry++;
			if (retry > 2)
				return FALSE;
			break;
		}

		// ���X�|���X�擾
		if ((r = Protocol.RecvBlock(&rxpcb, rxbuf, &rxlen)) != 0) {
			OutputDebug(L"ResetCard: Error Receiving IFS response. code=%d\n", r);
			l_pShMem->DoneReset = FALSE;
			retry++;
			if (retry > 2)
				return FALSE;
			break;
		}

		// ���X�|���X�m�F
		if (rxpcb != (CComProtocolT1::SBLOCK_FUNCTION_IFS | CComProtocolT1::SBLOCK_RESPONSE)) {
			OutputDebug(L"ResetCard: PCB does not match IFS response.\n");
			l_pShMem->DoneReset = FALSE;
			retry++;
			if (retry > 2)
				return FALSE;
			break;
		}
		l_pShMem->SequenceNumber = FALSE;
		l_pShMem->CardReady = TRUE;
		OutputDebug(L"ResetCard: Reset Complete.\n");
	}

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	{
#ifdef _DEBUG
		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		l_hModule = hModule;
		DisableThreadLibraryCalls(hModule);
		l_hStartedEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		::InitializeCriticalSection(&l_csInit);
		// ini�t�@�C����path�擾
		std::wstring tempPath = common::GetModuleName(l_hModule);
		CIniFileAccess IniFileAccess(tempPath + L"ini");
		IniFileAccess.SetSectionName(L"SCard");

		// tuner��FriendlyName�擾
		std::wstring name, dip;
		name = IniFileAccess.ReadKeyS(L"TunerFriendlyName", L"PXW3U4 Multi Tuner ISDB-T BDA Filter #0");
		name = IniFileAccess.ReadKeyS(L"FriendlyName", name);
		// tuner�̃f�o�C�X�C���X�^���X�p�X�擾
		dip = IniFileAccess.ReadKeyS(L"TunerInstancePath", L"");
		COMProc.SetTunerFriendlyName(name, dip);

		// Debug Log���L�^���邩�ǂ���
		if (IniFileAccess.ReadKeyB(L"DebugLog", FALSE)) {
			SetDebugLog(tempPath + L"log");
		}

		// �ڍ�Log���L�^���邩�ǂ���
		if (IniFileAccess.ReadKeyB(L"DetailLog", FALSE))
			Protocol.SetDetailLog(TRUE);

		// ����M Guard Interval ����
		// �J�[�h����2�`3msec������Ώ\���Ȃ͂������ǉ��̂�UARTReady�������Ă��܂����Ƃ�����݂���
		Protocol.SetGuardInterval(IniFileAccess.ReadKeyI(L"GuardInterval", 50));

		// IFD���̍ő��M�\�u���b�N�T�C�Y
		// �{���ݒ肷��K�v�͖�������M�n�J�[�h�̕s����ؗp�Ƃ��ėp�ӂ��Ă���
		IFSD = (BYTE)IniFileAccess.ReadKeyI(L"IFSD", 254);
	}
		break;

	case DLL_PROCESS_DETACH:
	{
		CloseAllHandle();
		::DeleteCriticalSection(&l_csInit);
		if (l_hStartedEvent) {
			try {
				SAFE_CLOSE_HANDLE(l_hStartedEvent);
			}
			catch (...) {
			}
		}
		// �f�o�b�O���O�t�@�C���̃N���[�Y
		CloseDebugLog();
	}
		break;
	}

	return TRUE;
}

LONG WINAPI SCardConnectA_(SCARDCONTEXT /*hContext*/, LPCSTR /*szReader*/, DWORD /*dwShareMode*/, DWORD /*dwPreferredProtocols*/, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	if (!l_pShMem) {
		::EnterCriticalSection(&l_csInit);
		BOOL suc = InitDevice();
		::LeaveCriticalSection(&l_csInit);
		if (!suc) {
			OutputDebug(L"SCardConnectA: Error in InitDevice()\n");
			return SCARD_E_NO_ACCESS;
		}
	}

	if (!l_pShMem->CardReady) {
		LockProc Lock;
		if (!Lock.IsSuccess()) {
			OutputDebug(L"SCardConnectA: Error in Lock.\n");
			return SCARD_E_TIMEOUT;
		}
		if (!ResetCard())
		{
			OutputDebug(L"SCardConnectA: Error in ResetCard()\n");
			return SCARD_E_NOT_READY;
		}
	}

	*phCard = DUMMY_SCARDHANDLE;
	*pdwActiveProtocol = SCARD_PROTOCOL_T1;

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardConnectW_(SCARDCONTEXT /*hContext*/, LPWSTR /*szReader*/, DWORD /*dwShareMode*/, DWORD /*dwPreferredProtocols*/, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	if (!l_pShMem) {
		::EnterCriticalSection(&l_csInit);
		BOOL suc = InitDevice();
		::LeaveCriticalSection(&l_csInit);
		if (!suc) {
			OutputDebug(L"SCardConnectW: Error in InitDevice()\n");
			return SCARD_E_NO_ACCESS;
		}
	}

	if (!l_pShMem->CardReady) {
		LockProc Lock;
		if (!Lock.IsSuccess()) {
			OutputDebug(L"SCardConnectW: Error in Lock.\n");
			return SCARD_E_TIMEOUT;
		}
		if (!ResetCard())
		{
			OutputDebug(L"SCardConnectW: Error in ResetCard()\n");
			return SCARD_E_NOT_READY;
		}
	}

	*phCard = DUMMY_SCARDHANDLE;
	*pdwActiveProtocol = SCARD_PROTOCOL_T1;

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardDisconnect_(SCARDHANDLE /*hCard*/, DWORD /*dwDisposition*/)
{
	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardEstablishContext_(DWORD /*dwScope*/, LPCVOID /*pvReserved1*/, LPCVOID /*pvReserved2*/, LPSCARDCONTEXT phContext)
{
	::EnterCriticalSection(&l_csInit);
	l_dwEstablishedContext++;
	OutputDebug(L"SCardEstablishContext: Count=%d.\n", l_dwEstablishedContext);
	::LeaveCriticalSection(&l_csInit);

	*phContext = DUMMY_SCARDCONTEXT;

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardFreeMemory_(SCARDCONTEXT /*hContext*/, LPCVOID /*pvMem*/)
{
	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardGetStatusChangeA_(SCARDCONTEXT /*hContext*/, DWORD /*dwTimeout*/, LPSCARD_READERSTATEA rgReaderStates, DWORD /*cReaders*/)
{
	rgReaderStates->dwEventState = SCARD_STATE_PRESENT;

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardGetStatusChangeW_(SCARDCONTEXT /*hContext*/, DWORD /*dwTimeout*/, LPSCARD_READERSTATEW rgReaderStates, DWORD /*cReaders*/)
{
	rgReaderStates->dwEventState = SCARD_STATE_PRESENT;

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardIsValidContext_(SCARDCONTEXT hContext)
{
	return hContext ? SCARD_S_SUCCESS : ERROR_INVALID_HANDLE;
}

LONG WINAPI SCardListReadersA_(SCARDCONTEXT /*hContext*/, LPCSTR /*mszGroups*/, LPSTR mszReaders, LPDWORD pcchReaders)
{
	if (pcchReaders) {
		if (mszReaders) {
			if (*pcchReaders == SCARD_AUTOALLOCATE)
				*(LPCSTR*)mszReaders = LIST_READERS_A;
			else
				memcpy(mszReaders, LIST_READERS_A, sizeof(LIST_READERS_A));
		}
		*pcchReaders = sizeof(LIST_READERS_A) / sizeof(LIST_READERS_A[0]);
	}

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardListReadersW_(SCARDCONTEXT /*hContext*/, LPCWSTR /*mszGroups*/, LPWSTR mszReaders, LPDWORD pcchReaders)
{
	if (pcchReaders) {
		if (mszReaders) {
			if (*pcchReaders == SCARD_AUTOALLOCATE)
				*(LPCWSTR*)mszReaders = LIST_READERS_W;
			else
				memcpy(mszReaders, LIST_READERS_W, sizeof(LIST_READERS_W));
		}
		*pcchReaders = sizeof(LIST_READERS_W) / sizeof(LIST_READERS_W[0]);
	}

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardReleaseContext_(SCARDCONTEXT /*hContext*/)
{
	::EnterCriticalSection(&l_csInit);
	if (l_dwEstablishedContext)
		l_dwEstablishedContext--;
	OutputDebug(L"SCardReleaseContext: Count=%d.\n", l_dwEstablishedContext);

	if (!l_dwEstablishedContext) {
		COMProc.TerminateThread();
		CloseAllHandle();
	}
	::LeaveCriticalSection(&l_csInit);

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardStatusA_(SCARDHANDLE /*hCard*/, LPSTR szReaderNames, LPDWORD pcchReaderLen, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
	if (pcchReaderLen) {
		if (szReaderNames) {
			if (*pcchReaderLen == SCARD_AUTOALLOCATE)
				*(LPCSTR*)szReaderNames = READER_NAME_A;
			else
				memcpy(szReaderNames, READER_NAME_A, sizeof(READER_NAME_A));
		}
		*pcchReaderLen = sizeof(READER_NAME_A) / sizeof(READER_NAME_A[0]);
	}
	if (pdwState)
		*pdwState = SCARD_PRESENT;
	if (pdwProtocol)
		*pdwProtocol = SCARD_PROTOCOL_T1;
	if (pcbAtrLen) {
		if (pbAtr) {
			if (*pcbAtrLen == SCARD_AUTOALLOCATE)
				*(LPBYTE*)pbAtr = ParseATR.RawData;
			else
				memcpy(pbAtr, ParseATR.RawData, ParseATR.RawDataLength);
		}
		*pcbAtrLen = ParseATR.RawDataLength;
	}

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardStatusW_(SCARDHANDLE /*hCard*/, LPWSTR szReaderNames, LPDWORD pcchReaderLen, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
	if (pcchReaderLen) {
		if (szReaderNames) {
			if (*pcchReaderLen == SCARD_AUTOALLOCATE)
				*(LPCWSTR*)szReaderNames = READER_NAME_W;
			else
				memcpy(szReaderNames, READER_NAME_W, sizeof(READER_NAME_W));
		}
		*pcchReaderLen = sizeof(READER_NAME_W) / sizeof(READER_NAME_W[0]);
	}
	if (pdwState)
		*pdwState = SCARD_PRESENT;
	if (pdwProtocol)
		*pdwProtocol = SCARD_PROTOCOL_T1;
	if (pcbAtrLen) {
		if (pbAtr) {
			if (*pcbAtrLen == SCARD_AUTOALLOCATE)
				*(LPBYTE*)pbAtr = ParseATR.RawData;
			else
				memcpy(pbAtr, ParseATR.RawData, ParseATR.RawDataLength);
		}
		*pcbAtrLen = ParseATR.RawDataLength;
	}

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardTransmit_(SCARDHANDLE /*hCard*/, LPCSCARD_IO_REQUEST /*pioSendPci*/, LPCBYTE pbSendBuffer, DWORD cbSendLength, LPSCARD_IO_REQUEST /*pioRecvPci*/, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
	CComProtocolT1::COM_PROTOCOL_T1_ERROR_CODE r;
	int retry = 0;
	BOOL success = FALSE;

	LockProc Lock;
	if (!Lock.IsSuccess()) {
		OutputDebug(L"SCardTransmit: Error in Lock.\n");
		return SCARD_E_TIMEOUT;
	}

	while (!success) {
		if (!l_pShMem->CardReady) {
			if (!ResetCard())
			{
				OutputDebug(L"SCardTransmit: Error in ResetCard()\n");
				return SCARD_E_NOT_READY;
			}
		}

		switch (r = Protocol.Transmit(pbSendBuffer, cbSendLength, pbRecvBuffer, pcbRecvLength, &(l_pShMem->SequenceNumber))) {
		case CComProtocolT1::COM_PROTOCOL_T1_S_NO_ERROR:
			success = TRUE;
			break;

		case CComProtocolT1::COM_PROTOCOL_T1_E_POINTER:
		case CComProtocolT1::COM_PROTOCOL_T1_E_NO_CARD:
			OutputDebug(L"SCardTransmit: Fatal error in Transmit()\n");
			return SCARD_E_NOT_READY;

		default:
			OutputDebug(L"SCardTransmit: Error in Transmit()\n");
			l_pShMem->CardReady = FALSE;
			retry++;
			if (retry > 2) {
				OutputDebug(L"SCardTransmit: Aborted.\n");
				return SCARD_F_COMM_ERROR;
			}
			else if (retry > 1) {
				OutputDebug(L"SCardTransmit: Trying reset...\n");
				l_pShMem->DoneReset = FALSE;
				break;
			}
			else {
				OutputDebug(L"SCardTransmit: Trying RESYNCH...\n");
				break;
			}
		}
	}

	return SCARD_S_SUCCESS;
}

LONG WINAPI SCardReconnect_(SCARDHANDLE /*hCard*/, DWORD /*dwShareMode*/, DWORD /*dwPreferredProtocols*/, DWORD /*dwInitialization*/, LPDWORD pdwActiveProtocol)
{
	*pdwActiveProtocol = SCARD_PROTOCOL_T1;

	return SCARD_S_SUCCESS;
}

HANDLE WINAPI SCardAccessStartedEvent_(void)
{
	return l_hStartedEvent;
}

void WINAPI SCardReleaseStartedEvent_(void)
{
	return;
}

LONG WINAPI SCardCancel_(SCARDCONTEXT /*hContext*/)
{
	return SCARD_S_SUCCESS;
}

CComProtocolT1x3U4::CComProtocolT1x3U4(void)
	: LastTickCount(0),
	GuardInterval(0)
{
	IgnoreEDCError = FALSE;
}

CComProtocolT1x3U4::COM_PROTOCOL_T1_ERROR_CODE CComProtocolT1x3U4::TxBlock(void)
{
	HRESULT hr;
	BOOL present = FALSE;
	if (FAILED(hr = COMProc.DetectCard(&present))) {
		OutputDebug(L"TxBlock: Error in it35_CardDetect(). code=0x%x\n", hr);
		return COM_PROTOCOL_T1_E_NOT_FUNCTIONING;
	}
	if (!present) {
		OutputDebug(L"TxBlock: Card not present.\n", hr);
		return COM_PROTOCOL_T1_E_NO_CARD;
	}
	WaitGuardInterval();
	if (FAILED(hr = COMProc.SendUART(SendFrame, SendFrameLen))) {
		OutputDebug(L"TxBlock: Error in it35_SentUart(). code=0x%x\n", hr);
		return COM_PROTOCOL_T1_E_NOT_FUNCTIONING;
	}
	return COM_PROTOCOL_T1_S_NO_ERROR;
};

CComProtocolT1x3U4::COM_PROTOCOL_T1_ERROR_CODE CComProtocolT1x3U4::RxBlock(void)
{
	HRESULT hr;
	BOOL ready = FALSE;
	int retry = 0;
	while (1) {
		if (FAILED(hr = COMProc.IsUARTReady(&ready))) {
			OutputDebug(L"RxBlock: Error in it35_IsUartReady(). code=0x%x\n", hr);
			return COM_PROTOCOL_T1_E_NOT_FUNCTIONING;
		}
		if (ready)
			break;
		retry++;
		if (retry > 20) {
			OutputDebug(L"RxBlock: Retry time out for it35_IsUartReady().\n");
			return COM_PROTOCOL_T1_E_NOT_READY;
		}
		::Sleep(10);
	}
	DWORD len = sizeof(RecvFrame);
	if (FAILED(hr = COMProc.GetUARTData(RecvFrame, &len))) {
		OutputDebug(L"RxBlock: Error in it35_GetUartData(). code=0x%x\n", hr);
		return COM_PROTOCOL_T1_E_NOT_FUNCTIONING;
	}
	RecvFrameLen = len;
	SetLastTickCount();
	return COM_PROTOCOL_T1_S_NO_ERROR;
}

void CComProtocolT1x3U4::SetGuardInterval(DWORD dwMilliSec) {
	GuardInterval = dwMilliSec;
	return;
}

void CComProtocolT1x3U4::SetLastTickCount(void) {
	LastTickCount = ::GetTickCount();
	return;
}

void CComProtocolT1x3U4::WaitGuardInterval(void) {
	DWORD t = ::GetTickCount() - LastTickCount;
	if (t < GuardInterval)
		::Sleep(GuardInterval - t);
}

LockProc::LockProc(DWORD dwMilliSeconds)
	: result(WAIT_FAILED)
{
	result = ::WaitForSingleObject(l_hSemaphore, dwMilliSeconds);
	return;
};

LockProc::LockProc(void)
	: LockProc(10000)
{
};

LockProc::~LockProc(void)
{
	::ReleaseSemaphore(l_hSemaphore, 1, NULL);
	return;
};

BOOL LockProc::IsSuccess(void)
{
	return (result == WAIT_OBJECT_0);
};
