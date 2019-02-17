#pragma once

/*
//
// Broadcast Driver Architecture �Œ�`����Ă��� Property set / Method set
//
// KSNODE_BDA_RF_TUNER Input pin, id:0
static constexpr GUID KSPROPSETID_BdaFrequencyFilter = { 0x71985f47, 0x1ca1, 0x11d3,{ 0x9c, 0xc8, 0x0, 0xc0, 0x4f, 0x79, 0x71, 0xe0 } };

enum KSPROPERTY_BDA_FREQUENCY_FILTER {
	KSPROPERTY_BDA_RF_TUNER_FREQUENCY = 0,				// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_RF_TUNER_POLARITY,					// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_RF_TUNER_RANGE,						// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_RF_TUNER_TRANSPONDER,				// not supported
	KSPROPERTY_BDA_RF_TUNER_BANDWIDTH,					// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_RF_TUNER_FREQUENCY_MULTIPLIER,		// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_RF_TUNER_CAPS,						// not supported
	KSPROPERTY_BDA_RF_TUNER_SCAN_STATUS,				// not supported
	KSPROPERTY_BDA_RF_TUNER_STANDARD,					// not supported
	KSPROPERTY_BDA_RF_TUNER_STANDARD_MODE,				// not supported
};

// KSNODE_BDA_RF_TUNER Input pin, id:0 / KSNODE_BDA_*_DEMODULATOR Output pin, id:1
static constexpr GUID KSPROPSETID_BdaSignalStats = { 0x1347d106, 0xcf3a, 0x428a,{ 0xa5, 0xcb, 0xac, 0xd, 0x9a, 0x2a, 0x43, 0x38 } };

enum KSPROPERTY_BDA_SIGNAL_STATS {
	KSPROPERTY_BDA_SIGNAL_STRENGTH = 0,					// get only			MinProperty=32		MinData=4
	KSPROPERTY_BDA_SIGNAL_QUALITY,						// get only			MinProperty=32		MinData=4
	KSPROPERTY_BDA_SIGNAL_PRESENT,						// get only			MinProperty=32		MinData=4
	KSPROPERTY_BDA_SIGNAL_LOCKED,						// get only			MinProperty=32		MinData=4
	KSPROPERTY_BDA_SAMPLE_TIME,							// get only			MinProperty=32		MinData=4
	KSPROPERTY_BDA_SIGNAL_LOCK_CAPS,					// not supported
	KSPROPERTY_BDA_SIGNAL_LOCK_TYPE,					// not supported
};

// KSNODE_BDA_RF_TUNER Input pin, id:0
static constexpr GUID KSPROPSETID_BdaLNBInfo = { 0x992cf102, 0x49f9, 0x4719,{ 0xa6, 0x64, 0xc4, 0xf2, 0x3e, 0x24, 0x8, 0xf4 } };

enum KSPROPERTY_BDA_LNB_INFO {
	KSPROPERTY_BDA_LNB_LOF_LOW_BAND = 0,				// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_LNB_LOF_HIGH_BAND,					// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_LNB_SWITCH_FREQUENCY,				// get/set			MinProperty=32		MinData=4
};

// KSNODE_BDA_*_DEMODULATOR Output pin, id:1
static constexpr GUID KSPROPSETID_BdaDigitalDemodulator = { 0xef30f379, 0x985b, 0x4d10,{ 0xb6, 0x40, 0xa7, 0x9d, 0x5e, 0x4, 0xe1, 0xe0 } };

enum KSPROPERTY_BDA_DIGITAL_DEMODULATOR {
	KSPROPERTY_BDA_MODULATION_TYPE = 0,					// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_INNER_FEC_TYPE,						// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_INNER_FEC_RATE,						// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_OUTER_FEC_TYPE,						// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_OUTER_FEC_RATE,						// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_SYMBOL_RATE,							// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_SPECTRAL_INVERSION,					// get/set			MinProperty=32		MinData=4
	KSPROPERTY_BDA_GUARD_INTERVAL,						// not supported
	KSPROPERTY_BDA_TRANSMISSION_MODE,					// not supported
	KSPROPERTY_BDA_ROLL_OFF,							// not supported
	KSPROPERTY_BDA_PILOT,								// not supported
	KSPROPERTY_BDA_SIGNALTIMEOUTS,						// not supported
	KSPROPERTY_BDA_PLP_NUMBER,							// not supported
};

// KSCATEGORY_BDA_NETWORK_TUNER Tuner filter, id:0
static constexpr GUID KSPROPSETID_BdaPIDFilter = { 0xd0a67d65, 0x8df, 0x4fec,{ 0x85, 0x33, 0xe5, 0xb5, 0x50, 0x41, 0xb, 0x85 } };

enum KSPROPERTY_BDA_PIDFILTER {
	KSPROPERTY_BDA_PIDFILTER_MAP_PIDS = 0,				// set only			MinProperty=32		MinData=12
	KSPROPERTY_BDA_PIDFILTER_UNMAP_PIDS,				// set only			MinProperty=32		MinData=8
	KSPROPERTY_BDA_PIDFILTER_LIST_PIDS,					// set pnly			MinProperty=32		MinData=0
};

static constexpr GUID KSMETHODSETID_BdaChangeSync = { 0xfd0a5af3, 0xb41d, 0x11d2,{ 0x9c, 0x95, 0x0, 0xc0, 0x4f, 0x79, 0x71, 0xe0 } };

enum KSMETHOD_BDA_CHANGE_SYNC {
	KSMETHOD_BDA_START_CHANGES = 0,						// none				MinMethod=24		MinData=0
	KSMETHOD_BDA_CHECK_CHANGES,							// none				MinMethod=24		MinData=0
	KSMETHOD_BDA_COMMIT_CHANGES,						// none				MinMethod=24		MinData=0
	KSMETHOD_BDA_GET_CHANGE_STATE,						// read				MinMethod=24		MinData=0
};

// KSCATEGORY_BDA_NETWORK_TUNER Tuner filter, id:0
static constexpr GUID KSMETHODSETID_BdaDeviceConfiguration = { 0x71985f45, 0x1ca1, 0x11d3,{ 0x9c, 0xc8, 0x0, 0xc0, 0x4f, 0x79, 0x71, 0xe0 } };

enum KSMETHOD_BDA_DEVICE_CONFIGURATION {
	KSMETHOD_BDA_CREATE_PIN_FACTORY = 0,				// not supported
	KSMETHOD_BDA_DELETE_PIN_FACTORY,					// not supported
	KSMETHOD_BDA_CREATE_TOPOLOGY,						// modify			MinMethod=32		MinData=0
};
*/

//
// IT9135 BDA driver �ŗL�� Property set
//

// KSCATEGORY_BDA_NETWORK_TUNER Tuner filter, id:0
// ITE �g���v���p�e�B�Z�b�g GUID
static constexpr GUID KSPROPSETID_IteExtension = { 0xc6efe5eb, 0x855a, 0x4f1b,{ 0xb7, 0xaa, 0x87, 0xb5, 0xe1, 0xdc, 0x41, 0x13 } };

// ITE �g���v���p�e�BID
enum KSPROPERTY_ITE_EXTENSION {
	KSPROPERTY_ITE_EX_BULK_DATA = 0,					// get/set			MinProperty=24		MinData=4
	KSPROPERTY_ITE_EX_BULK_DATA_NB,						// get/set			MinProperty=24		MinData=4
	KSPROPERTY_ITE_EX_PID_FILTER_ON_OFF,				// set only			MinProperty=24		MinData=4
	KSPROPERTY_ITE_EX_BAND_WIDTH,						// get/set			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_MERCURY_DRIVER_INFO,				// get only			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_MERCURY_DEVICE_INFO,				// get only			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_TS_DATA,							// get/set			MinProperty=24		MinData=4
	KSPROPERTY_ITE_EX_OVL_CNT,							// get only			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_FREQ,								// get/set			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_RESET_USB,						// set only			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_MERCURY_REG,						// get/set			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_MERCURY_PVBER,					// get/set			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_MERCURY_REC_LEN,					// get/set			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_MERCURY_EEPROM,					// get/set			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_MERCURY_IR,						// get only			MinProperty=24		MinData=1	(ERROR_NOT_SUPORTED)
	KSPROPERTY_ITE_EX_MERCURY_SIGNAL_STRENGTH,			// get only			MinProperty=24		MinData=1
	KSPROPERTY_ITE_EX_CHANNEL_MODULATION = 99,			// get only			MinProperty=24		MinData=36
};

// KSCATEGORY_BDA_NETWORK_TUNER Tuner filter, id:0
// DVB-S IO �R���g���[�� �v���p�e�B�Z�b�g GUID
static constexpr GUID KSPROPSETID_DvbsIoCtl = { 0xf23fac2d, 0xe1af, 0x48e0,{ 0x8b, 0xbe, 0xa1, 0x40, 0x29, 0xc9, 0x2f, 0x21 } };

// DVB-S IO �R���g���[�� �v���p�e�BID
enum KSPROPERTY_DVBS_IO_CTL {
	KSPROPERTY_DVBS_IO_LNB_POWER = 0,					// get/set			MinProperty=24		MinData=1
	KSPROPERTY_DVBS_IO_DiseqcLoad,						// set only			MinProperty=24		MinData=1
};

// KSCATEGORY_BDA_NETWORK_TUNER Tuner filter, id:0
// �g�� IO �R���g���[�� �v���p�e�B�Z�b�g GUID
static constexpr GUID KSPROPSETID_ExtIoCtl = { 0xf23fac2d, 0xe1af, 0x48e0,{ 0x8b, 0xbe, 0xa1, 0x40, 0x29, 0xc9, 0x2f, 0x11 } };

// �g�� IO �R���g���[�� �v���p�e�B ID
enum KSPROPERTY_EXT_IO_CTL {
	KSPROPERTY_EXT_IO_DRV_DATA = 0,						// get/set			MinProperty=24		MinData=1
	KSPROPERTY_EXT_IO_DEV_IO_CTL,						// get/set			MinProperty=24		MinData=1
	KSPROPERTY_EXT_IO_UNUSED50 = 50,					// not used			MinProperty=24		MinData=1
	KSPROPERTY_EXT_IO_UNUSED51 = 51,					// not used			MinProperty=24		MinData=1
	KSPROPERTY_EXT_IO_ISDBT_IO_CTL = 200,				// set only			MinProperty=24		MinData=1
};

// �g�� IO �R���g���[�� KSPROPERTY_EXT_IO_DRV_DATA �p�t�@���N�V�����R�[�h
enum DRV_DATA_FUNC_CODE {
	DRV_DATA_FUNC_GET_DRIVER_INFO = 1,					// Read
};

// �g�� IO �R���g���[�� KSPROPERTY_EXT_IO_DRV_DATA �p�\����
#pragma pack(1)
struct DrvDataDataSet {
	struct {
		DWORD DriverPID;
		DWORD DriverVersion;
		DWORD FwVersion_LINK;
		DWORD FwVersion_OFDM;
		DWORD TunerID;
	} DriverInfo;
};
#pragma pack()

// �g�� IO �R���g���[�� KSPROPERTY_EXT_IO_DEV_IO_CTL �p�t�@���N�V�����R�[�h
enum DEV_IO_CTL_FUNC_CODE {
	DEV_IO_CTL_FUNC_READ_OFDM_REG = 0,					// Read
	DEV_IO_CTL_FUNC_WRITE_OFDM_REG,						// Write
	DEV_IO_CTL_FUNC_READ_LINK_REG,						// Read
	DEV_IO_CTL_FUNC_WRITE_LINK_REG,						// Write
	DEV_IO_CTL_FUNC_AP_CTRL,							// Write
	DEV_IO_CTL_FUNC_READ_RAW_IR = 7,					// Read
	DEV_IO_CTL_FUNC_GET_UART_DATA = 10,					// Read
	DEV_IO_CTL_FUNC_SENT_UART,							// Write
	DEV_IO_CTL_FUNC_SET_UART_BAUDRATE,					// Write
	DEV_IO_CTL_FUNC_CARD_DETECT,						// Read
	DEV_IO_CTL_FUNC_GET_ATR,							// Read
	DEV_IO_CTL_FUNC_AES_KEY,							// Write
	DEV_IO_CTL_FUNC_AES_ENABLE,							// Write
	DEV_IO_CTL_FUNC_RESET_SMART_CARD,					// Write
	DEV_IO_CTL_FUNC_IS_UART_READY,						// Read
	DEV_IO_CTL_FUNC_SET_ONE_SEG,						// Write
	DEV_IO_CTL_FUNC_GET_BOARD_INPUT_POWER = 25,			// Read
	DEV_IO_CTL_FUNC_SET_DECRYP,							// Write
	DEV_IO_CTL_FUNC_UNKNOWN99 = 99,						// Read
	DEV_IO_CTL_FUNC_GET_RX_DEVICE_ID,					// Read
	DEV_IO_CTL_FUNC_UNKNOWN101,							// Write
	DEV_IO_CTL_FUNC_IT930X_EEPROM_READ = 300,			// Read
	DEV_IO_CTL_FUNC_IT930X_EEPROM_WRITE,				// Write
	DEV_IO_CTL_FUNC_READ_GPIO,							// Read
	DEV_IO_CTL_FUNC_WRITE_GPIO,							// Write
};

// �g�� IO �R���g���[�� KSPROPERTY_EXT_IO_DEV_IO_CTL �p�\����
#pragma pack(1)
struct DevIoCtlDataSet {
	union {
		struct {
			DWORD Addr;
			BYTE WriteData;
		} Reg;
		struct {
			DWORD Key;
			BYTE Enable;
		} Decryp;
		struct {
			BYTE Length;
			BYTE Buffer[3 + 254 + 2];
		} UartData;
		DWORD DeviceId;
		DWORD PowerOn;
	};
	union {
		DWORD CardDetected;
		DWORD UartReady;
		DWORD AesEnable;
	};
	BYTE Reserved1[2];							// -28h (-40)
	WORD UartBaudRate;							// -26h (-38)
	BYTE ATR[13];								// -24h (-36)
	BYTE AesKey[16];							// -17h (-23)
	BYTE Reserved2[7];							// -07h (-7)
	DevIoCtlDataSet(void) {
		memset(this, 0, sizeof(*this));
	};
};
#pragma pack()

// KSCATEGORY_BDA_NETWORK_TUNER Tuner filter, id:0
// �v���C�x�[�g IO �R���g���[�� �v���p�e�B�Z�b�g GUID
static constexpr GUID KSPROPSETID_PrivateIoCtl = { 0xede22531, 0x92e8, 0x4957,{ 0x9d, 0x5, 0x6f, 0x30, 0x33, 0x73, 0xe8, 0x37 } };

// �v���C�x�[�g IO �R���g���[�� �v���p�e�B ID
enum KSPROPERTY_PRIVATE_IO_CTL {
	KSPROPERTY_PRIVATE_IO_DIGIBEST_TUNER = 0,			// set only			MinProperty=24		MinData=1
	KSPROPERTY_PRIVATE_IO_DIGIBEST_TUNER_PARAMETER,		// get/set			MinProperty=24		MinData=1
};

// �v���C�x�[�g IO �R���g���[�� KSPROPERTY_PRIVATE_IO_DIGIBEST_TUNER �p�t�@���N�V�����R�[�h
enum PRIVATE_IO_CTL_FUNC_CODE {
	PRIVATE_IO_CTL_FUNC_PROTECT_TUNER_POWER = 0,
	PRIVATE_IO_CTL_FUNC_UNPROTECT_TUNER_POWER,
	PRIVATE_IO_CTL_FUNC_SET_TUNER_POWER_ON,
	PRIVATE_IO_CTL_FUNC_SET_TUNER_POWER_OFF,
	PRIVATE_IO_CTL_FUNC_DEMOD_OFDM = 7,
	PRIVATE_IO_CTL_FUNC_DEMOD_PSK,
};

//
// ITE �g���v���p�e�B�Z�b�g�p�֐�
//
// �`���[�j���O�ш敝�擾
static inline HRESULT it35_GetBandWidth(IKsPropertySet *pIKsPropertySet, WORD *pwData)
{
	HRESULT hr = S_OK;
	DWORD dwBytes;
	BYTE buf[sizeof(*pwData)];
	if (FAILED(hr = pIKsPropertySet->Get(KSPROPSETID_IteExtension, KSPROPERTY_ITE_EX_BAND_WIDTH, NULL, 0, buf, sizeof(buf), &dwBytes))) {
		return hr;
	}

	if (pwData)
		*pwData = *(WORD*)buf;

	return hr;
}

// �`���[�j���O�ш敝�ݒ�
static inline HRESULT it35_PutBandWidth(IKsPropertySet *pIKsPropertySet, WORD wData)
{
	return pIKsPropertySet->Set(KSPROPSETID_IteExtension, KSPROPERTY_ITE_EX_BAND_WIDTH, NULL, 0, &wData, sizeof(wData));
}

// �`���[�j���O���g���擾
static inline HRESULT it35_GetFreq(IKsPropertySet *pIKsPropertySet, WORD *pwData)
{
	HRESULT hr = S_OK;
	DWORD dwBytes;
	BYTE buf[sizeof(*pwData)];
	if (FAILED(hr = pIKsPropertySet->Get(KSPROPSETID_IteExtension, KSPROPERTY_ITE_EX_FREQ, NULL, 0, buf, sizeof(buf), &dwBytes))) {
		return hr;
	}

	if (pwData)
		*pwData = *(WORD*)buf;

	return hr;
}

// �`���[�j���O���g���ݒ�
static inline HRESULT it35_PutFreq(IKsPropertySet *pIKsPropertySet, WORD wData)
{
	return pIKsPropertySet->Set(KSPROPSETID_IteExtension, KSPROPERTY_ITE_EX_FREQ, NULL, 0, &wData, sizeof(wData));
}

//
// DVB-S IO �R���g���[�� �v���p�e�B�Z�b�g�p�֐�
//
// LNBPower �̐ݒ��Ԏ擾
static inline HRESULT it35_GetLNBPower(IKsPropertySet *pIKsPropertySet, BYTE *pbyData)
{
	HRESULT hr = S_OK;
	DWORD dwBytes;
	BYTE buf[sizeof(*pbyData)];
	if (FAILED(hr = pIKsPropertySet->Get(KSPROPSETID_DvbsIoCtl, KSPROPERTY_DVBS_IO_LNB_POWER, NULL, 0, buf, sizeof(buf), &dwBytes))) {
		return hr;
	}

	if (pbyData)
		*pbyData = *(BYTE*)buf;

	return hr;
}

// LNBPower �̐ݒ�
static inline HRESULT it35_PutLNBPower(IKsPropertySet *pIKsPropertySet, BYTE byData)
{
	return pIKsPropertySet->Set(KSPROPSETID_DvbsIoCtl, KSPROPERTY_DVBS_IO_LNB_POWER, NULL, 0, &byData, sizeof(byData));
}

//
// �g�� IO �R���g���[�� KSPROPERTY_EXT_IO_DRV_DATA �p�֐�
//
// KSPROPERTY_EXT_IO_DRV_DATA ���ʊ֐�
static inline HRESULT it35_GetDrvData(IKsPropertySet *pIKsPropertySet, DWORD dwCode, DrvDataDataSet *pData)
{
	HRESULT hr = S_OK;
	DWORD dwBytes;
	BYTE buf[sizeof(*pData)];

	if FAILED(hr = pIKsPropertySet->Set(KSPROPSETID_ExtIoCtl, KSPROPERTY_EXT_IO_DRV_DATA, NULL, 0, &dwCode, sizeof(dwCode))) {
		return hr;
	}

	if (FAILED(hr = pIKsPropertySet->Get(KSPROPSETID_ExtIoCtl, KSPROPERTY_EXT_IO_DRV_DATA, NULL, 0, buf, sizeof(buf), &dwBytes))) {
		return hr;
	}

	if (pData)
		*pData = *(DrvDataDataSet*)buf;

	return hr;
}

// �h���C�o�[�o�[�W�������擾
static inline HRESULT it35_GetDriverInfo(IKsPropertySet *pIKsPropertySet, DrvDataDataSet *pData)
{
	return it35_GetDrvData(pIKsPropertySet, DRV_DATA_FUNC_GET_DRIVER_INFO, pData);
}

//
// �g�� IO �R���g���[�� KSPROPERTY_EXT_IO_DEV_IO_CTL �p�֐�
//
// KSPROPERTY_EXT_IO_DEV_IO_CTL ���ʊ֐�
static inline HRESULT it35_GetDevIoCtl(IKsPropertySet *pIKsPropertySet, BOOL bNeedGet, DWORD dwCode, DWORD *dwRetVal, DevIoCtlDataSet *pDataSet)
{
	HRESULT hr = S_OK;
	DWORD dwBytes;
#pragma pack(1)
	struct {
		DWORD dwData;
		DevIoCtlDataSet DataSet;
	} putGetData;
#pragma pack()

	putGetData.dwData = dwCode;
	if (pDataSet)
		putGetData.DataSet = *pDataSet;

	if FAILED(hr = pIKsPropertySet->Set(KSPROPSETID_ExtIoCtl, KSPROPERTY_EXT_IO_DEV_IO_CTL, NULL, 0, &putGetData, sizeof(putGetData))) {
		return hr;
	}

	if (!bNeedGet)
		return hr;

	if (FAILED(hr = pIKsPropertySet->Get(KSPROPSETID_ExtIoCtl, KSPROPERTY_EXT_IO_DEV_IO_CTL, NULL, 0, &putGetData, sizeof(putGetData), &dwBytes))) {
		return hr;
	}

	if (pDataSet)
		*pDataSet = putGetData.DataSet;

	if (dwRetVal)
		*dwRetVal = putGetData.dwData;

	return hr;
}

// OFDM Register �l�擾
static inline HRESULT it35_ReadOfdmReg(IKsPropertySet *pIKsPropertySet, DWORD dwAddr, BYTE *pbyData)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	dataset.Reg.Addr = dwAddr;
	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_READ_OFDM_REG, &dwResult, &dataset))) {
		return hr;
	}

	if (pbyData)
		*pbyData = (BYTE)dwResult;

	return hr;
}

// OFDM Register �l����
static inline HRESULT it35_WriteOfdmReg(IKsPropertySet *pIKsPropertySet, DWORD dwAddr, BYTE byData)
{
	DevIoCtlDataSet dataset;

	dataset.Reg.Addr = dwAddr;
	dataset.Reg.WriteData = byData;
	return it35_GetDevIoCtl(pIKsPropertySet, FALSE, DEV_IO_CTL_FUNC_WRITE_OFDM_REG, NULL, &dataset);
}

// Link Register �l�擾
static inline HRESULT it35_ReadLinkReg(IKsPropertySet *pIKsPropertySet, DWORD dwAddr, BYTE *pbyData)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	dataset.Reg.Addr = dwAddr;
	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_READ_LINK_REG, &dwResult, &dataset))) {
		return hr;
	}

	if (pbyData)
		*pbyData = (BYTE)dwResult;

	return hr;
}

// Link Register �l����
static inline HRESULT it35_WriteLinkReg(IKsPropertySet *pIKsPropertySet, DWORD dwAddr, BYTE byData)
{
	DevIoCtlDataSet dataset;

	dataset.Reg.Addr = dwAddr;
	dataset.Reg.WriteData = byData;
	return it35_GetDevIoCtl(pIKsPropertySet, FALSE, DEV_IO_CTL_FUNC_WRITE_LINK_REG, NULL, &dataset);
}

// Tuner �p���[�R���g���[��
static inline HRESULT it35_ApCtrl(IKsPropertySet *pIKsPropertySet, BOOL bPowerOn)
{
	DevIoCtlDataSet dataset;

	dataset.DeviceId = (DWORD)bPowerOn;

	return it35_GetDevIoCtl(pIKsPropertySet, FALSE, DEV_IO_CTL_FUNC_AP_CTRL, NULL, &dataset);
}

// �����R����M�f�[�^�擾
static inline HRESULT it35_ReadRawIR(IKsPropertySet *pIKsPropertySet, DWORD *pdwData)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_READ_RAW_IR, &dwResult, &dataset))) {
		return hr;
	}

	if (pdwData)
		*pdwData = dwResult;

	return hr;
}

// UART ��M�f�[�^�擾
static inline HRESULT it35_GetUartData(IKsPropertySet *pIKsPropertySet, BYTE *pRcvBuff, DWORD *pdwLength)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	dataset.UartData.Length = 255;		// ������0��ݒ肵�ČĂяo����BSoD BAD_POOL_HEADER ����������(Ver. Beta �h���C�o)

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_GET_UART_DATA, &dwResult, &dataset))) {
		return hr;
	}

	if (pRcvBuff)
		memcpy(pRcvBuff, dataset.UartData.Buffer, min((DWORD)min(dataset.UartData.Length, 255), *pdwLength));

	if (pdwLength)
		*pdwLength = dataset.UartData.Length;

	return hr;
}

// UART ���M�f�[�^�ݒ�
static inline HRESULT it35_SentUart(IKsPropertySet *pIKsPropertySet, const BYTE *pSendBuff, DWORD dwLength)
{
	DevIoCtlDataSet dataset;

	if (pSendBuff) {
		dataset.UartData.Length = (BYTE)min(255, dwLength);
		memcpy(dataset.UartData.Buffer, pSendBuff, dataset.UartData.Length);
	}

	return it35_GetDevIoCtl(pIKsPropertySet, FALSE, DEV_IO_CTL_FUNC_SENT_UART, NULL, &dataset);
}

// UART �{�[���[�g�ݒ�
static inline HRESULT it35_SetUartBaudRate(IKsPropertySet *pIKsPropertySet, WORD wBaudRate)
{
	DevIoCtlDataSet dataset;

	dataset.UartBaudRate = wBaudRate;
	return it35_GetDevIoCtl(pIKsPropertySet, FALSE, DEV_IO_CTL_FUNC_SET_UART_BAUDRATE, NULL, &dataset);
}

// CARD ���o
static inline HRESULT it35_CardDetect(IKsPropertySet *pIKsPropertySet, BOOL *pbDetect)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_CARD_DETECT, &dwResult, &dataset))) {
		return hr;
	}

	if (pbDetect)
		*pbDetect = (BOOL)dataset.CardDetected;

	return hr;
}

// CARD ATR �f�[�^�擾
static inline HRESULT it35_GetATR(IKsPropertySet *pIKsPropertySet, BYTE *pATR)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_GET_ATR, &dwResult, &dataset))) {
		return hr;
	}

	if (pATR)
		memcpy(pATR, dataset.ATR, sizeof(dataset.ATR));

	return hr;
}

// CARD ���Z�b�g
static inline HRESULT it35_ResetSmartCard(IKsPropertySet *pIKsPropertySet)
{
	return it35_GetDevIoCtl(pIKsPropertySet, FALSE, DEV_IO_CTL_FUNC_RESET_SMART_CARD, NULL, NULL);
}

// UART Ready ��Ԏ擾
static inline HRESULT it35_IsUartReady(IKsPropertySet *pIKsPropertySet, BOOL *pbReady)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_IS_UART_READY, &dwResult, &dataset))) {
		return hr;
	}

	if (pbReady)
		*pbReady = (BOOL)dataset.UartReady;

	return hr;
}

// ����d���擾?
static inline HRESULT it35_GetBoardInputPower(IKsPropertySet *pIKsPropertySet, DWORD *pdwPower)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_GET_BOARD_INPUT_POWER, &dwResult, &dataset))) {
		return hr;
	}

	if (pdwPower)
		*pdwPower = dwResult;

	return hr;
}

// 
static inline HRESULT it35_Unk99(IKsPropertySet *pIKsPropertySet, BOOL *pbDetect)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_UNKNOWN99, &dwResult, &dataset))) {
		return hr;
	}

	if (pbDetect)
		*pbDetect = (BOOL)dataset.CardDetected;

	return hr;
}

// 
static inline HRESULT it35_GetRxDeviceId(IKsPropertySet *pIKsPropertySet, DWORD *pdwDevId)
{
	HRESULT hr = S_OK;
	DWORD dwResult;
	DevIoCtlDataSet dataset;

	if (FAILED(hr = it35_GetDevIoCtl(pIKsPropertySet, TRUE, DEV_IO_CTL_FUNC_GET_RX_DEVICE_ID, &dwResult, &dataset))) {
		return hr;
	}

	if (pdwDevId)
		*pdwDevId = dataset.DeviceId;

	return hr;
}

// 
static inline HRESULT it35_Unk101(IKsPropertySet *pIKsPropertySet)
{
	return it35_GetDevIoCtl(pIKsPropertySet, FALSE, DEV_IO_CTL_FUNC_UNKNOWN101, NULL, NULL);
}

//
// �g�� IO �R���g���[�� KSPROPERTY_EXT_IO_ISDBT_IO_CTL �p�֐�
//
// TSID ���Z�b�g
static inline HRESULT it35_PutISDBIoCtl(IKsPropertySet *pIKsPropertySet, DWORD dwData)
{
	return pIKsPropertySet->Set(KSPROPSETID_ExtIoCtl, KSPROPERTY_EXT_IO_ISDBT_IO_CTL, NULL, 0, &dwData, sizeof(dwData));
}

//
// �v���C�x�[�g IO �R���g���[�� KSPROPERTY_PRIVATE_IO_DIGIBEST_TUNER �p�֐�
//
// DigiBest Tuner �p�v���C�x�[�g�t�@���N�V����
static inline HRESULT it35_DigibestPrivateIoControl(IKsPropertySet *pIKsPropertySet, DWORD dwCode)
{
	return pIKsPropertySet->Set(KSPROPSETID_PrivateIoCtl, KSPROPERTY_PRIVATE_IO_DIGIBEST_TUNER, NULL, 0, &dwCode, sizeof(dwCode));
}
