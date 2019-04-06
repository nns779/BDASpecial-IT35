#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "common.h"

#include "IT35.h"

#include <Windows.h>
#include <string>

#include <dshow.h>

// KSPROPSETID_BdaPIDFilter
#include <ks.h>
#pragma warning (push)
#pragma warning (disable: 4091)
#include <ksmedia.h>
#pragma warning (pop)
#include <bdatypes.h>
#include <bdamedia.h>

#include "IT35propset.h"
#include "CIniFileAccess.h"
#include "DSFilterEnum.h"
#include "WaitWithMsg.h"

FILE *g_fpLog = NULL;

HMODULE hMySelf;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		// ���W���[���n���h���ۑ�
		hMySelf = hModule;
		break;

	case DLL_PROCESS_DETACH:
		// �f�o�b�O���O�t�@�C���̃N���[�Y
		CloseDebugLog();
		break;
	}
    return TRUE;
}

__declspec(dllexport) IBdaSpecials * CreateBdaSpecials(CComPtr<IBaseFilter> pTunerDevice)
{
	return new CIT35Specials(hMySelf, pTunerDevice);
}

__declspec(dllexport) HRESULT CheckAndInitTuner(IBaseFilter *pTunerDevice, const WCHAR *szDisplayName, const WCHAR *szFriendlyName, const WCHAR *szIniFilePath)
{
	CIniFileAccess IniFileAccess(szIniFilePath);

	// DebugLog���L�^���邩�ǂ���
	if (IniFileAccess.ReadKeyB(L"IT35", L"DebugLog", FALSE)) {
		// INI�t�@�C���̃t�@�C�����擾
		// DebugLog�̃t�@�C�����擾
		SetDebugLog(common::GetModuleName(hMySelf) + L"log");
	}

	return S_OK;
}

__declspec(dllexport) HRESULT CheckCapture(const WCHAR *szTunerDisplayName, const WCHAR *szTunerFriendlyName,
	const WCHAR *szCaptureDisplayName, const WCHAR *szCaptureFriendlyName, const WCHAR *szIniFilePath)
{
	// ���ꂪ�Ă΂ꂽ�Ƃ������Ƃ�Bondriver_BDA.ini�̐ݒ肪��������
	OutputDebug(L"CheckCapture called.\n");

	// connect()�����݂Ă����ʂȂ̂� E_FAIL ��Ԃ��Ă���
	return E_FAIL;
}

CIT35Specials::CIT35Specials(HMODULE hMySelf, CComPtr<IBaseFilter> pTunerDevice)
	: m_hMySelf(hMySelf),
	  m_pTunerDevice(pTunerDevice),
	  m_pIKsPropertySet(m_pTunerDevice),
	  m_bRewriteIFFreq(FALSE),
	  m_nPrivateSetTSID(enumPrivateSetTSID::ePrivateSetTSIDNone),
	  m_bLNBPowerON(FALSE),
	  m_bDualModeISDB(FALSE),
	  m_nSpecialLockConfirmTime(2000),
	  m_nSpecialLockSetTSIDInterval(100),
	  m_nTunerPowerMode(enumTunerPowerMode::eTunerPowerModeNoTouch)
{
	::InitializeCriticalSection(&m_CriticalSection);

	return;
}

CIT35Specials::~CIT35Specials()
{
	m_hMySelf = NULL;

	::DeleteCriticalSection(&m_CriticalSection);

	return;
}

const HRESULT CIT35Specials::InitializeHook(void)
{
	if (!m_pTunerDevice) {
		return E_POINTER;
	}

	if (!m_pIKsPropertySet)
		return E_FAIL;

	HRESULT hr;

	// IBDA_DeviceControl
	{
		CComQIPtr<IBDA_DeviceControl> pDeviceControl(m_pTunerDevice);
		if (!pDeviceControl) {
			OutputDebug(L"Can not get IBDA_DeviceControl.\n");
			return E_NOINTERFACE;
		}
		m_pIBDA_DeviceControl = pDeviceControl;
	}

	// Control Node �擾
	if (m_nPrivateSetTSID == enumPrivateSetTSID::ePrivateSetTSIDSpecial) {
		CDSEnumNodes DSEnumNodes(m_pTunerDevice);

		// IBDA_FrequencyFilter / IBDA_SignalStatistics / IBDA_LNBInfo / IBDA_DiseqCommand
		{
			ULONG NodeTypeTuner = DSEnumNodes.findControlNode(__uuidof(IBDA_FrequencyFilter));
			if (NodeTypeTuner != -1) {
				OutputDebug(L"Found RF Tuner Node. NodeType=%ld.\n", NodeTypeTuner);
				CComPtr<IUnknown> pControlNodeTuner;
				if (FAILED(hr = DSEnumNodes.getControlNode(NodeTypeTuner, &pControlNodeTuner))) {
					OutputDebug(L"Fail to get control node.\n");
				}
				else {
					CComQIPtr<IBDA_FrequencyFilter> pIBDA_FrequencyFilter(pControlNodeTuner);
					if (pIBDA_FrequencyFilter) {
						m_pIBDA_FrequencyFilter = pIBDA_FrequencyFilter;
						OutputDebug(L"  Found IBDA_FrequencyFilter.\n");
					}
					CComQIPtr<IBDA_SignalStatistics> pIBDA_SignalStatistics(pControlNodeTuner);
					if (pIBDA_SignalStatistics) {
						m_pIBDA_SignalStatistics = pIBDA_SignalStatistics;
						OutputDebug(L"  Found IBDA_SignalStatistics.\n");
					}
					CComQIPtr<IBDA_LNBInfo> pIBDA_LNBInfo(pControlNodeTuner);
					if (pIBDA_LNBInfo) {
						m_pIBDA_LNBInfo = pIBDA_LNBInfo;
						OutputDebug(L"  Found IBDA_LNBInfo.\n");
					}
				}
			}
		}

		// IBDA_DigitalDemodulator
		{
			ULONG NodeTypeDemod = DSEnumNodes.findControlNode(__uuidof(IBDA_DigitalDemodulator));
			if (NodeTypeDemod != -1) {
				OutputDebug(L"Found Demodulator Node. NodeType=%ld.\n", NodeTypeDemod);
				CComPtr<IUnknown> pControlNodeDemod;
				if (FAILED(hr = DSEnumNodes.getControlNode(NodeTypeDemod, &pControlNodeDemod))) {
					OutputDebug(L"Fail to get control node.\n");
				}
				else {
					CComQIPtr<IBDA_DigitalDemodulator> pIBDA_DigitalDemodulator(pControlNodeDemod);
					if (pIBDA_DigitalDemodulator) {
						m_pIBDA_DigitalDemodulator = pIBDA_DigitalDemodulator;
						OutputDebug(L"  Found IBDA_DigitalDemodulator.\n");
					}
				}
			}
		}
	}

	if (m_nTunerPowerMode != enumTunerPowerMode::eTunerPowerModeNoTouch) {
		// Tuner Power�蓮���[�h�ɂ���
		hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_TUNER_POWER_MODE_MANUAL);
		// Tuner Power����UOFF�ɂ���
		hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_SET_TUNER_POWER_OFF);
		// Tuner Power��ON����
		hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_SET_TUNER_POWER_ON);
	}
	if (m_nTunerPowerMode == enumTunerPowerMode::eTunerPowerModeAuto) {
		// Tuner Power�������[�h�ɖ߂�(Tuner��Close����Ǝ����I��OFF�ɂȂ�)
		hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_TUNER_POWER_MODE_AUTO);
	}

	if (m_bLNBPowerON) {
		// ini�t�@�C���Ŏw�肳��Ă���� ������LNB Power ��ON����
		// LNB Power ��OFF��BDA driver������ɂ���Ă����݂���
		::EnterCriticalSection(&m_CriticalSection);
		hr = it35_PutLNBPower(m_pIKsPropertySet, 1);
		::LeaveCriticalSection(&m_CriticalSection);

		if FAILED(hr)
			OutputDebug(L"SetLNBPower failed.\n");
		else
			OutputDebug(L"SetLNBPower Success.\n");
	}
	return S_OK;
}

const HRESULT CIT35Specials::Set22KHz(bool bActive)
{
	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::Set22KHz(long nTone)
{
	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::FinalizeHook(void)
{
	return S_OK;
}

const HRESULT CIT35Specials::GetSignalState(int *pnStrength, int *pnQuality, int *pnLock)
{
	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::LockChannel(BYTE bySatellite, BOOL bHorizontal, unsigned long ulFrequency, BOOL bDvbS2)
{
	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::LockChannel(const TuningParam *pTuningParam)
{
	if (m_nPrivateSetTSID == enumPrivateSetTSID::ePrivateSetTSIDSpecial) {
		if (m_pTunerDevice == NULL) {
			return E_POINTER;
		}

		if (!m_pIBDA_SignalStatistics) {
			return E_FAIL;
		}

		HRESULT hr;
		SpectralInversion eSpectralInversion = BDA_SPECTRAL_INVERSION_NOT_SET;
		FECMethod eInnerFECMethod = BDA_FEC_METHOD_NOT_SET;
		BinaryConvolutionCodeRate eInnerFECRate = BDA_BCC_RATE_NOT_SET;
		ModulationType eModulationType = BDA_MOD_NOT_SET;
		FECMethod eOuterFECMethod = BDA_FEC_METHOD_NOT_SET;
		BinaryConvolutionCodeRate eOuterFECRate = BDA_BCC_RATE_NOT_SET;
		ULONG SymbolRate = (ULONG)-1L;

		OutputDebug(L"LockChannel: Start.\n");

		BOOL isISDBS = pTuningParam->Modulation.Modulation == BDA_MOD_ISDB_S_TMCC;
		BOOL success = FALSE;
		BOOL failure = FALSE;
		// Dual Mode ISDB Tuner�̏ꍇ�̓f���W�����[�^�[�̕���Mode��ݒ�
		if (m_bDualModeISDB) {
			switch (pTuningParam->Modulation.Modulation) {
			case BDA_MOD_ISDB_T_TMCC:
				::EnterCriticalSection(&m_CriticalSection);
				hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_DEMOD_OFDM);
				::LeaveCriticalSection(&m_CriticalSection);
				break;
			case BDA_MOD_ISDB_S_TMCC:
				::EnterCriticalSection(&m_CriticalSection);
				hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_DEMOD_PSK);
				::LeaveCriticalSection(&m_CriticalSection);
				break;
			}
		}

		::EnterCriticalSection(&m_CriticalSection);
		do {
			ULONG State;
			if (FAILED(hr = m_pIBDA_DeviceControl->GetChangeState(&State))) {
				OutputDebug(L"  Fail to IBDA_DeviceControl::GetChangeState() function. ret=0x%08lx\n", hr);
				failure = TRUE;
				break;
			}
			// �y���f�B���O��Ԃ̃g�����U�N�V���������邩�m�F
			if (State == BDA_CHANGES_PENDING) {
				OutputDebug(L"  Some changes are pending. Trying CommitChanges.\n");
				// �g�����U�N�V�����̃R�~�b�g
				if (FAILED(hr = m_pIBDA_DeviceControl->CommitChanges())) {
					OutputDebug(L"    Fail to CommitChanges. ret=0x%08lx\n", hr);
				}
				else {
					OutputDebug(L"    Succeeded to CommitChanges.\n");
				}
			}

			// �g�����U�N�V�����J�n�ʒm
			if (FAILED(hr = m_pIBDA_DeviceControl->StartChanges())) {
				OutputDebug(L"  Fail to IBDA_DeviceControl::StartChanges() function. ret=0x%08lx\n", hr);
				failure = TRUE;
				break;
			}

			// IBDA_LNBInfo
			if (m_pIBDA_LNBInfo) {
				// LNB ���g����ݒ�
				m_pIBDA_LNBInfo->put_LocalOscilatorFrequencyHighBand((ULONG)pTuningParam->Antenna.HighOscillator);
				m_pIBDA_LNBInfo->put_LocalOscilatorFrequencyLowBand((ULONG)pTuningParam->Antenna.LowOscillator);

				// LNB�X�C�b�`�̎��g����ݒ�
				if (pTuningParam->Antenna.LNBSwitch != -1L) {
					// LNBSwitch���g���̐ݒ肪����Ă���
					m_pIBDA_LNBInfo->put_HighLowSwitchFrequency((ULONG)pTuningParam->Antenna.LNBSwitch);
				}
				else {
					// 10GHz��ݒ肵�Ă�����High���ɁA20GHz��ݒ肵�Ă�����Low���ɐؑւ��͂�
					m_pIBDA_LNBInfo->put_HighLowSwitchFrequency((pTuningParam->Antenna.Tone != 0L) ? 10000000UL : 20000000UL);
				}
			}

			// IBDA_DigitalDemodulator
			if (m_pIBDA_DigitalDemodulator) {
				// �ʑ��ϒ��X�y�N�g�����]�̎��
				eSpectralInversion = BDA_SPECTRAL_INVERSION_AUTOMATIC;
				m_pIBDA_DigitalDemodulator->put_SpectralInversion(&eSpectralInversion);

				// �����O���������̃^�C�v��ݒ�
				eInnerFECMethod = pTuningParam->Modulation.InnerFEC;
				m_pIBDA_DigitalDemodulator->put_InnerFECMethod(&eInnerFECMethod);

				// ���� FEC ���[�g��ݒ�
				eInnerFECRate = pTuningParam->Modulation.InnerFECRate;
				m_pIBDA_DigitalDemodulator->put_InnerFECRate(&eInnerFECRate);

				// �ϒ��^�C�v��ݒ�
				eModulationType = pTuningParam->Modulation.Modulation;
				m_pIBDA_DigitalDemodulator->put_ModulationType(&eModulationType);

				// �O���O���������̃^�C�v��ݒ�
				eOuterFECMethod = pTuningParam->Modulation.OuterFEC;
				m_pIBDA_DigitalDemodulator->put_OuterFECMethod(&eOuterFECMethod);

				// �O�� FEC ���[�g��ݒ�
				eOuterFECRate = pTuningParam->Modulation.OuterFECRate;
				m_pIBDA_DigitalDemodulator->put_OuterFECRate(&eOuterFECRate);

				// �V���{�� ���[�g��ݒ�
				SymbolRate = (ULONG)pTuningParam->Modulation.SymbolRate;
				m_pIBDA_DigitalDemodulator->put_SymbolRate(&SymbolRate);
			}

			// IBDA_FrequencyFilter
			if (m_pIBDA_FrequencyFilter) {
				// ���g���̒P��(Hz)��ݒ�
				m_pIBDA_FrequencyFilter->put_FrequencyMultiplier(1000UL);

				// �M���̕Δg��ݒ�
				m_pIBDA_FrequencyFilter->put_Polarity(pTuningParam->Polarisation);

				// ���g���̑ш敝 (MHz)��ݒ�
				long bw = pTuningParam->Modulation.BandWidth;
				if (isISDBS && bw == -1L) {
					bw = 9L;
				}
				m_pIBDA_FrequencyFilter->put_Bandwidth(bw);

				// RF �M���̎��g����ݒ�
				long freq = pTuningParam->Frequency;
				// IF���g���ɕϊ�
				if (m_bRewriteIFFreq) {
					if (pTuningParam->Antenna.LNBSwitch != -1) {
						if (pTuningParam->Frequency < pTuningParam->Antenna.LNBSwitch) {
							if (pTuningParam->Frequency > pTuningParam->Antenna.LowOscillator && pTuningParam->Antenna.HighOscillator != -1)
								freq -= pTuningParam->Antenna.LowOscillator;
						}
						else {
							if (pTuningParam->Frequency > pTuningParam->Antenna.HighOscillator && pTuningParam->Antenna.HighOscillator != -1)
								freq -= pTuningParam->Antenna.HighOscillator;
						}
					}
					else {
						if (pTuningParam->Antenna.Tone == 0) {
							if (pTuningParam->Frequency > pTuningParam->Antenna.LowOscillator && pTuningParam->Antenna.HighOscillator != -1)
								freq -= pTuningParam->Antenna.LowOscillator;
						}
						else {
							if (pTuningParam->Frequency > pTuningParam->Antenna.HighOscillator && pTuningParam->Antenna.HighOscillator != -1)
								freq -= pTuningParam->Antenna.HighOscillator;
						}
					}
				}
				m_pIBDA_FrequencyFilter->put_Frequency((ULONG)freq);
			}

			// �g�����U�N�V�����̃R�~�b�g
			if (FAILED(hr = m_pIBDA_DeviceControl->CommitChanges())) {
				OutputDebug(L"  Fail to IBDA_DeviceControl::CommitChanges() function. ret=0x%08lx\n", hr);
				// ���s������S�Ă̕ύX��������
				hr = m_pIBDA_DeviceControl->StartChanges();
				hr = m_pIBDA_DeviceControl->CommitChanges();
				failure = TRUE;
				break;
			}
			OutputDebug(L"  Succeeded to IBDA_DeviceControl::CommitChanges() function.\n");
		} while (0);
		::LeaveCriticalSection(&m_CriticalSection);

		if (failure) {
			return E_FAIL;
		}

		long tsid = pTuningParam->TSID == 0 ? -1L : pTuningParam->TSID;
		static constexpr int CONFIRM_RETRY_TIME = 50;
		unsigned int confirmRemain = m_nSpecialLockConfirmTime;
		unsigned int tsidInterval = 0;
		while (1) {
			if (!tsidInterval) {
				if (isISDBS) {
					// TSID���Z�b�g
					::EnterCriticalSection(&m_CriticalSection);
					hr = it35_PutISDBIoCtl(m_pIKsPropertySet, (DWORD)tsid);
					::LeaveCriticalSection(&m_CriticalSection);
				}

				// PID off
				::EnterCriticalSection(&m_CriticalSection);
				hr = it35_PutPidFilterOnOff(m_pIKsPropertySet, FALSE);
				::LeaveCriticalSection(&m_CriticalSection);

				// PID Map
				BDA_PID_MAP pidMap = { MEDIA_SAMPLE_CONTENT::MEDIA_TRANSPORT_PACKET, 1, { 0x1fff } };
				::EnterCriticalSection(&m_CriticalSection);
				hr = m_pIKsPropertySet->Set(KSPROPSETID_BdaPIDFilter, KSPROPERTY_BDA_PIDFILTER_MAP_PIDS, &pidMap, sizeof(pidMap), &pidMap, sizeof(pidMap));
				::LeaveCriticalSection(&m_CriticalSection);

				tsidInterval = max(m_nSpecialLockSetTSIDInterval, CONFIRM_RETRY_TIME);
			}

			BOOLEAN sl = 0;
			::EnterCriticalSection(&m_CriticalSection);
			hr = m_pIBDA_SignalStatistics->get_SignalLocked(&sl);
			::LeaveCriticalSection(&m_CriticalSection);
			if (sl) {
				OutputDebug(L"  Lock success.\n");
				success = TRUE;
				break;
			}
			unsigned int sleepTime = min(tsidInterval, min(confirmRemain, CONFIRM_RETRY_TIME));
			if (!sleepTime) {
				OutputDebug(L"  Timed out.\n");
				break;
			}
			OutputDebug(L"    Waiting lock status remaining %d msec.\n", confirmRemain);
			SleepWithMessageLoop((DWORD)sleepTime);
			confirmRemain -= sleepTime;
			tsidInterval -= sleepTime;
		}

		OutputDebug(L"LockChannel: Complete.\n");

		return success ? S_OK : S_FALSE;
	}

	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::SetLNBPower(bool bActive)
{
	// �g���ĂȂ����A�܂�������
	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::ReadIniFile(const WCHAR *szIniFilePath)
{
	static const std::map<const std::wstring, const int, std::less<>> mapPrivateSetTSID = {
		{ L"NO",      enumPrivateSetTSID::ePrivateSetTSIDNone },
		{ L"YES",     enumPrivateSetTSID::ePrivateSetTSIDPreTR },
		{ L"PRETR",   enumPrivateSetTSID::ePrivateSetTSIDPreTR },
		{ L"POSTTR",  enumPrivateSetTSID::ePrivateSetTSIDPostTR },
		{ L"SPECIAL", enumPrivateSetTSID::ePrivateSetTSIDSpecial },
	};

	static const std::map<const std::wstring, const int, std::less<>> mapTunerPowerMode = {
		{ L"NOTOUCH",  enumTunerPowerMode::eTunerPowerModeNoTouch },
		{ L"AUTOOFF",  enumTunerPowerMode::eTunerPowerModeAuto },
		{ L"ALWAYSON", enumTunerPowerMode::eTunerPowerModeAlwaysOn },
	};

	CIniFileAccess IniFileAccess(szIniFilePath);
	IniFileAccess.SetSectionName(L"IT35");

	// IF���g���� put_CarrierFrequency() ���s��
	m_bRewriteIFFreq = IniFileAccess.ReadKeyB(L"RewriteIFFreq", FALSE);

	// �ŗL�� Property set ���g�p����TSID�̏����݂��s�����[�h
	m_nPrivateSetTSID = (enumPrivateSetTSID)IniFileAccess.ReadKeyIValueMap(L"PrivateSetTSID", enumPrivateSetTSID::ePrivateSetTSIDNone, mapPrivateSetTSID);

	// LNB�d���̋�����ON����
	m_bLNBPowerON = IniFileAccess.ReadKeyB(L"LNBPowerON", FALSE);

	// Dual Mode ISDB Tuner
	m_bDualModeISDB = IniFileAccess.ReadKeyB(L"DualModeISDB", FALSE);

	// BDASpecial�ŗL��LockChannel���g�p����ꍇ��Lock�����m�F����
	m_nSpecialLockConfirmTime = IniFileAccess.ReadKeyI(L"SpecialLockConfirmTime", 2000);

	// BDASpecial�ŗL��LockChannel���g�p����ꍇ��Lock�����҂�����TSID / PID map�̍ăZ�b�g���s���C���^�[�o������
	m_nSpecialLockSetTSIDInterval = IniFileAccess.ReadKeyI(L"SpecialLockSetTSIDInterval", 100);

	// Tuner Power �������䃂�[�h
	m_nTunerPowerMode = (enumTunerPowerMode)IniFileAccess.ReadKeyIValueMap(L"TunerPowerMode", enumTunerPowerMode::eTunerPowerModeNoTouch, mapTunerPowerMode);

	return S_OK;
}

const HRESULT CIT35Specials::IsDecodingNeeded(BOOL *pbAns)
{
	if (pbAns)
		*pbAns = FALSE;

	return S_OK;
}

const HRESULT CIT35Specials::Decode(BYTE *pBuf, DWORD dwSize)
{
	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::GetSignalStrength(float *fVal)
{
	return E_NOINTERFACE;
}

const HRESULT CIT35Specials::PreLockChannel(TuningParam *pTuningParam)
{
	if (m_nPrivateSetTSID == enumPrivateSetTSID::ePrivateSetTSIDSpecial)
		return S_OK;

	if (pTuningParam->Modulation.Modulation == BDA_MOD_ISDB_S_TMCC) {
		// ���g���̑ш敝��9�ɐݒ�
		long bw = pTuningParam->Modulation.BandWidth;
		if (pTuningParam->Modulation.BandWidth == -1L) {
			pTuningParam->Modulation.BandWidth = 9L;
		}

		// IF���g���ɕϊ�
		if (m_bRewriteIFFreq) {
			long freq = pTuningParam->Frequency;
			if (pTuningParam->Antenna.LNBSwitch != -1) {
				if (pTuningParam->Frequency < pTuningParam->Antenna.LNBSwitch) {
					if (pTuningParam->Frequency > pTuningParam->Antenna.LowOscillator && pTuningParam->Antenna.HighOscillator != -1)
						pTuningParam->Frequency -= pTuningParam->Antenna.LowOscillator;
				}
				else {
					if (pTuningParam->Frequency > pTuningParam->Antenna.HighOscillator && pTuningParam->Antenna.HighOscillator != -1)
						pTuningParam->Frequency -= pTuningParam->Antenna.HighOscillator;
				}
			}
			else {
				if (pTuningParam->Antenna.Tone == 0) {
					if (pTuningParam->Frequency > pTuningParam->Antenna.LowOscillator && pTuningParam->Antenna.HighOscillator != -1)
						pTuningParam->Frequency -= pTuningParam->Antenna.LowOscillator;
				}
				else {
					if (pTuningParam->Frequency > pTuningParam->Antenna.HighOscillator && pTuningParam->Antenna.HighOscillator != -1)
						pTuningParam->Frequency -= pTuningParam->Antenna.HighOscillator;
				}
			}
		}
	}

	return S_OK;
}

const HRESULT CIT35Specials::PreTuneRequest(const TuningParam *pTuningParam, ITuneRequest *pITuneRequest)
{
	if (m_nPrivateSetTSID == enumPrivateSetTSID::ePrivateSetTSIDSpecial)
		return S_OK;

	if (!m_pIKsPropertySet)
		return E_FAIL;

	HRESULT hr;

	// Dual Mode ISDB Tuner�̏ꍇ�̓f���W�����[�^�[�̕���Mode��ݒ�
	if (m_bDualModeISDB) {
		switch (pTuningParam->Modulation.Modulation) {
		case BDA_MOD_ISDB_T_TMCC:
			::EnterCriticalSection(&m_CriticalSection);
			hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_DEMOD_OFDM);
			::LeaveCriticalSection(&m_CriticalSection);
			break;
		case BDA_MOD_ISDB_S_TMCC:
			::EnterCriticalSection(&m_CriticalSection);
			hr = it35_DigibestPrivateIoControl(m_pIKsPropertySet, PRIVATE_IO_CTL_FUNC_DEMOD_PSK);
			::LeaveCriticalSection(&m_CriticalSection);
			break;
		}
	}

	// TSID��Set����
	if (m_nPrivateSetTSID == enumPrivateSetTSID::ePrivateSetTSIDPreTR && pTuningParam->Modulation.Modulation == BDA_MOD_ISDB_S_TMCC) {
		::EnterCriticalSection(&m_CriticalSection);
		hr = it35_PutISDBIoCtl(m_pIKsPropertySet, pTuningParam->TSID == 0 ? (DWORD)-1 : (DWORD)pTuningParam->TSID);
		::LeaveCriticalSection(&m_CriticalSection);
	}
	return S_OK;
}

const HRESULT CIT35Specials::PostTuneRequest(const TuningParam * pTuningParam)
{
	if (m_nPrivateSetTSID == enumPrivateSetTSID::ePrivateSetTSIDSpecial)
		return S_OK;

	if (!m_pIKsPropertySet)
		return E_FAIL;

	HRESULT hr;

	if (m_nPrivateSetTSID == enumPrivateSetTSID::ePrivateSetTSIDPostTR) {
		// TSID��Set����
		if (pTuningParam->Modulation.Modulation == BDA_MOD_ISDB_S_TMCC) {
			::EnterCriticalSection(&m_CriticalSection);
			hr = it35_PutISDBIoCtl(m_pIKsPropertySet, pTuningParam->TSID == 0 ? (DWORD)-1 : (DWORD)pTuningParam->TSID);
			::LeaveCriticalSection(&m_CriticalSection);
		}

		// PID off
		::EnterCriticalSection(&m_CriticalSection);
		hr = it35_PutPidFilterOnOff(m_pIKsPropertySet, FALSE);
		::LeaveCriticalSection(&m_CriticalSection);

		// PID Map
		BDA_PID_MAP pidMap = { MEDIA_SAMPLE_CONTENT::MEDIA_TRANSPORT_PACKET, 1, { 0x1fff } };
		::EnterCriticalSection(&m_CriticalSection);
		hr = m_pIKsPropertySet->Set(KSPROPSETID_BdaPIDFilter, KSPROPERTY_BDA_PIDFILTER_MAP_PIDS, &pidMap, sizeof(pidMap), &pidMap, sizeof(pidMap));
		::LeaveCriticalSection(&m_CriticalSection);
	}

	return S_OK;
}

const HRESULT CIT35Specials::PostLockChannel(const TuningParam *pTuningParam)
{
	return S_OK;
}

void CIT35Specials::Release(void)
{
	delete this;
}
