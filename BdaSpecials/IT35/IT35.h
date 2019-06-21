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
	CRITICAL_SECTION m_CriticalSection;

	// �ŗL�� Property set ���g�p����TSID�̏����݂��s�����[�h
	enum enumPrivateSetTSID {
		ePrivateSetTSIDNone = 0,			// �s��Ȃ�
		ePrivateSetTSIDPreTR,				// PreTuneRequest�ōs��
		ePrivateSetTSIDPostTR,				// PostTuneRequest�ōs��
	};

	BOOL m_bRewriteIFFreq;					// IF���g���� put_CarrierFrequency() ���s��
	enumPrivateSetTSID m_nPrivateSetTSID;	// �ŗL�� Property set ���g�p����TSID�̏����݂��s�����[�h
	BOOL m_bLNBPowerON;						// LNB�d���̋�����ON����
	BOOL m_bDualModeISDB;					// Dual Mode ISDB Tuner
};
