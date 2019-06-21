#pragma once

#include "IBdaSpecials2.h"

class CIT35Specials : public IBdaSpecials2b3
{
public:
	CIT35Specials(HMODULE hMySelf, CComPtr<IBaseFilter> pTunerDevice);
	virtual ~CIT35Specials(void);

	const HRESULT InitializeHook(void);
	const HRESULT Set22KHz(bool bActive);
	const HRESULT FinalizeHook(void);

	const HRESULT GetSignalState(int *pnStrength, int *pnQuality, int *pnLock);
	const HRESULT LockChannel(BYTE bySatellite, BOOL bHorizontal, unsigned long ulFrequency, BOOL bDvbS2);

	const HRESULT SetLNBPower(bool bActive);

	const HRESULT Set22KHz(long nTone);
	const HRESULT LockChannel(const TuningParam *pTuningParam);

	const HRESULT ReadIniFile(const WCHAR *szIniFilePath);
	const HRESULT IsDecodingNeeded(BOOL *pbAns);
	const HRESULT Decode(BYTE *pBuf, DWORD dwSize);
	const HRESULT GetSignalStrength(float *fVal);
	const HRESULT PreLockChannel(TuningParam *pTuningParam);
	const HRESULT PreTuneRequest(const TuningParam *pTuningParam, ITuneRequest *pITuneRequest);
	const HRESULT PostTuneRequest(const TuningParam *pTuningParam);
	const HRESULT PostLockChannel(const TuningParam *pTuningParam);

	virtual void Release(void);

private:
	HMODULE m_hMySelf;
	CComPtr<IBaseFilter> m_pTunerDevice;
	CComQIPtr<IKsPropertySet> m_pIKsPropertySet;
	CComPtr<IBDA_FrequencyFilter> m_pIBDA_FrequencyFilter;				// IBDA_FrequencyFilter (Input Pin, Node 0)
	CComPtr<IBDA_SignalStatistics> m_pIBDA_SignalStatistics;			// IBDA_SignalStatistics(Input Pin, Node 0)
	CComPtr<IBDA_LNBInfo> m_pIBDA_LNBInfo;								// IBDA_LNBInfo (Input Pin, Node 0)
	CComPtr<IBDA_DigitalDemodulator> m_pIBDA_DigitalDemodulator;		// IBDA_DigitalDemodulator (Output Pin, Node 1)
	CComPtr<IBDA_DeviceControl> m_pIBDA_DeviceControl;					// IBDA_DeviceControl (Tuner)
	CRITICAL_SECTION m_CriticalSection;

	// �ŗL�� Property set ���g�p����TSID�̏����݂��s�����[�h
	enum enumPrivateSetTSID {
		ePrivateSetTSIDNone = 0,				// �s��Ȃ�
		ePrivateSetTSIDPreTR,					// PreTuneRequest�ōs��
		ePrivateSetTSIDPostTR,					// PostTuneRequest�ōs��
		ePrivateSetTSIDSpecial = 100,			// �S�Ẵ`���[�j���O�����LockChannel�ōs��
	};

	// Tuner Power �������䃂�[�h
	enum enumTunerPowerMode {
		eTunerPowerModeNoTouch = 0,				// Tuner Power �������䃂�[�h�̕ύX���s��Ȃ�
		eTunerPowerModeAuto,					// Tuner Power �̎��������L���ɂ���
		eTunerPowerModeAlwaysOn,				// Tuner Power �̎�������𖳌��ɂ��A�펞ON�ɂ���
	};

	BOOL m_bRewriteIFFreq;						// IF���g���� put_CarrierFrequency() ���s��
	enumPrivateSetTSID m_nPrivateSetTSID;		// �ŗL�� Property set ���g�p����TSID�̏����݂��s�����[�h
	BOOL m_bLNBPowerON;							// LNB�d���̋�����ON����
	BOOL m_bDualModeISDB;						// Dual Mode ISDB Tuner
	unsigned int m_nSpecialLockConfirmTime;		// BDASpecial�ŗL��LockChannel���g�p����ꍇ��Lock�����m�F����
	unsigned int m_nSpecialLockSetTSIDInterval;	// BDASpecial�ŗL��LockChannel���g�p����ꍇ��Lock�����҂�����TSID / PID map�̍ăZ�b�g���s���C���^�[�o������
	enumTunerPowerMode m_nTunerPowerMode;		// Tuner Power �������䃂�[�h
};
