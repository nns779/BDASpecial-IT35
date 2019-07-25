#pragma once

#include <Windows.h>
#include <string>

class CCOMProc {
private:
	enum enumCOMRequest {
		eCOMReqDetectCard = 1,
		eCOMReqIsUARTReady,
		eCOMReqSendUART,
		eCOMReqGetUARTData,
		eCOMReqSetUARTBaudRate,
		eCOMReqResetCard,
		eCOMReqGetATRData,
		eCOMReqPrivateIOControl,
	};

	union COMReqParam {
		struct COMReqParamDetectCard {
			BOOL *pbPresent;
		} DetectCard;
		struct COMReqParamIsUARTReady {
			BOOL *pbReady;
		} IsUARTReady;
		struct COMReqParamSendUART {
			const BYTE *pcbySendBuffer;
			DWORD dwSendLength;
		} SendUART;
		struct COMReqParamGetUARTData {
			BYTE *pbyRecvBuffer;
			DWORD *pdwRecvLength;
		} GetUARTData;
		struct COMReqParamSetUARTBaudRate {
			WORD wBaudRate;
		} SetUARTBaudRate;
		struct COMReqParamResetCard {
		} ResetCard;
		struct COMReqParamGetATRData {
			BYTE *pbyBuffer;
		} GetATRData;
		struct COMReqParamPrivateIOControl {
			DWORD dwCode;
		} PrivateIOControl;
	};

private:
	HANDLE hThread;					// �X���b�h�n���h��
	HANDLE hThreadInitComp;			// �X���b�h�����������ʒm
	HANDLE hReqEvent;				// COMProc�X���b�h�ւ̃R�}���h���s�v��
	HANDLE hEndEvent;				// COMProc�X���b�h����̃R�}���h�����ʒm
	HANDLE hTerminateRequest;		// �X���b�h�I���v��
	enumCOMRequest Request;			// ���N�G�X�g
	COMReqParam Param;				// �p�����[�^
	HRESULT RetCode;				// �I���R�[�h
	CRITICAL_SECTION csLock;		// �r���p
	std::wstring TunerFriendlyName;	// �`���[�i�[���ʗpFriendlyName
	std::wstring TunerInstancePath; // �`���[�i�[���ʗp�f�o�C�X�C���X�^���X�p�X
	std::wstring TunerDisplayName;  // �`���[�i�[����擾����DisplayName

public:
	CCOMProc(void);
	~CCOMProc(void);
	void SetTunerFriendlyName(std::wstring friendlyName, std::wstring instancePath);
	std::wstring GetTunerDisplayName(void);
	BOOL CreateThread(void);
	void TerminateThread(void);
	HRESULT DetectCard(BOOL *Present);
	HRESULT IsUARTReady(BOOL *Ready);
	HRESULT SendUART(const BYTE *SendBuffer, DWORD SendLength);
	HRESULT GetUARTData(BYTE *RecvBuffer, DWORD *RecvLength);
	HRESULT SetUARTBaudRate(WORD BaudRate);
	HRESULT ResetCard(void);
	HRESULT GetATRData(BYTE *Buffer);
	HRESULT PrivateIOControl(DWORD Code);

private:
	void CloseThreadHandle(void);
	static DWORD WINAPI COMProcThread(LPVOID lpParameter);
};
