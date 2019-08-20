/*
// PX-W3U4/PXQ3U4�����R���p TVTest �v���O�C��
// TVTest �v���O�C���T���v�� HDUSRemocon.cpp�̃\�[�X�R�[�h���x�[�X�ɂ��Ă��܂�
*/
#include <windows.h>
#include <tchar.h>
#define TVTEST_PLUGIN_CLASS_IMPLEMENT

#pragma warning (push)
#pragma warning (disable: 4100)
#include "TVTestPlugin.h"
#pragma warning (pop)

#include "common.h"
#include "CCOMProc-x3U4Remocon.h"
#include "CIniFileAccess.h"

// �R���g���[����
#define X3U4_REMOCON_NAME L"PX-x3U4 Remocon"

FILE *g_fpLog = NULL;

// �{�^���̃��X�g
static const struct {
	TVTest::ControllerButtonInfo Info;
	DWORD KeyCode;
} g_ButtonList[] = {
	{ { L"�d��",			L"Close",			{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201020df },
	{ { L"�~���[�g",		L"Mute",			{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20102ad5 },
	{ { L"REC",				L"SaveImage",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201004fb },
	{ { L"EPG",				L"ProgramGuide",	{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201025da },
	{ { L"TV",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201006f9 },
	{ { L"DVD",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201007f8 },
	{ { L"��",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201008f7 },
	{ { L"CC�i�����j",		NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201009f6 },
	{ { L"�v���[���[ ON",	NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201002fd },
	{ { L"�v���[���[ OFF",	NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201003fc },
	{ { L"�����ؑ�",		L"SwitchAudio",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20100cf3 },
	{ { L"�S���",			L"Fullscreen",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20100ef1 },
	{ { L"�J�[�\�� ��",		NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20100df2 },
	{ { L"�J�[�\�� ��",		NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201013ec },
	{ { L"�J�[�\�� ��",		NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20100ff0 },
	{ { L"�J�[�\�� �E",		NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201011ee },
	{ { L"OK",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201030cf },
	{ { L"i",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20100bf4 },
	{ { L"�^��",			L"RecordStart",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201018e7 },
	{ { L"�ꎞ��~",		L"RecordPause",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201016e9 },
	{ { L"�Đ�",			NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201035ca },
	{ { L"��~",			L"RecordStop",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201017e8 },
	{ { L"<<",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201019e6 },
	{ { L">>",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20103ac5 },
	{ { L"|<",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20101be4 },
	{ { L">|",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20101ce3 },
	{ { L"1",				L"Channel1",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20101de2 },
	{ { L"2",				L"Channel2",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20101ee1 },
	{ { L"3",				L"Channel3",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20101fe0 },
	{ { L"4",				L"Channel4",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201021de },
	{ { L"5",				L"Channel5",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201042bd },
	{ { L"6",				L"Channel6",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201043bc },
	{ { L"7",				L"Channel7",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201045ba },
	{ { L"8",				L"Channel8",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201026d9 },
	{ { L"9",				L"Channel9",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201047b8 },
	{ { L"0",				L"Channel10",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20104ab5 },
	{ { L"*/.",				L"Channel11",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201049b6 },
	{ { L"#",				L"Channel12",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20104bb4 },
	{ { L"�`�����l�� +",	L"ChannelUp",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201040bf },
	{ { L"�`�����l�� -",	L"ChannelDown",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201044bb },
	{ { L"���� +",			L"VolumeUp",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201048b7 },
	{ { L"���� -",			L"VolumeDown",		{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20104cb3 },
	{ { L"�����ؑ�",		NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20104db2 },
	{ { L"�N���A",			NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20104eb1 },
	{ { L"����",			NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20102fd0 },
	{ { L"�f�[�^����",		NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x20104fb0 },
	{ { L"��",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201050af },
	{ { L"��",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201051ae },
	{ { L"��",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201052ad },
	{ { L"��",				NULL,				{ 0, 0, 0, 0 },			{ 0, 0 } },			0x201022dd },
};

#define NUM_BUTTONS (sizeof(g_ButtonList)/sizeof(g_ButtonList[0]))

static CCOMProc COMProc;

// �v���O�C���N���X
class CRemocon : public TVTest::CTVTestPlugin
{
	bool m_fInitialized;	// �������ς݂�?

	bool InitializePlugin();
	void OnError(LPCWSTR pszMessage);

	static void CALLBACK OnRemoconDown(DWORD Data, LPARAM pParam);

	static LRESULT CALLBACK EventCallback(UINT Event,LPARAM lParam1,LPARAM lParam2,void *pClientData);

public:
	CRemocon();
	virtual bool GetPluginInfo(TVTest::PluginInfo *pInfo);
	virtual bool Initialize();
	virtual bool Finalize();
};


CRemocon::CRemocon()
	: m_fInitialized(false)
{
}


bool CRemocon::GetPluginInfo(TVTest::PluginInfo *pInfo)
{
	// �v���O�C���̏���Ԃ�
	pInfo->Type           = TVTest::PLUGIN_TYPE_NORMAL;
	pInfo->Flags          = 0;
	pInfo->pszPluginName  = L"PX-W3U4/PX-Q3U4�����R��";
	pInfo->pszCopyright   = L"radi_sh";
	pInfo->pszDescription = L"PX-W3U4/PX-Q3U4�̃����R���ɑΉ����܂��B";
	return true;
}


bool CRemocon::Initialize()
{
	// ����������

	// �{�^���̃��X�g���쐬
	TVTest::ControllerButtonInfo ButtonList[NUM_BUTTONS];
	for (int i=0;i<NUM_BUTTONS;i++)
		ButtonList[i]=g_ButtonList[i].Info;

	// �R���g���[���̓o�^
	TVTest::ControllerInfo Info;
	Info.Flags             = 0;
	Info.pszName           = X3U4_REMOCON_NAME;
	Info.pszText           = L"PX-W3U4/PX-Q3U4�����R��";
	Info.NumButtons        = NUM_BUTTONS;
	Info.pButtonList       = ButtonList;
	Info.pszIniFileName    = NULL;
	Info.pszSectionName    = L"x3U4Controller";
	Info.ControllerImageID = NULL;
	Info.SelButtonsImageID = NULL;
	Info.pTranslateMessage = NULL;
	Info.pClientData       = this;
	if (!m_pApp->RegisterController(&Info)) {
		m_pApp->AddLog(L"�R���g���[����o�^�ł��܂���B",TVTest::LOG_TYPE_ERROR);
		return false;
	}

	// �C�x���g�R�[���o�b�N�֐���o�^
	m_pApp->SetEventCallback(EventCallback,this);

	return true;
}


// �v���O�C�����L���ɂ��ꂽ���̏���������
bool CRemocon::InitializePlugin()
{
	if (!m_fInitialized) {
		// ini�t�@�C����path�擾
		std::wstring tempPath = common::GetModuleName(g_hinstDLL);
		CIniFileAccess IniFileAccess(tempPath + L"ini");
		IniFileAccess.SetSectionName(L"Generic");

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

		// �����R���X�L�����Ԋu
		DWORD interval = IniFileAccess.ReadKeyI(L"PollingInterval", 100);
		COMProc.SetPollingInterval(interval);

		if (COMProc.CreateThread() != TRUE)
			return false;

		// �R�[���o�b�N�֐��o�^
		COMProc.SetCallback(OnRemoconDown, (LPARAM)this);

		m_fInitialized = TRUE;
	}

	return true;
}


bool CRemocon::Finalize()
{
	// �I������
	COMProc.TerminateThread();
	m_fInitialized = FALSE;

	return true;
}


void CRemocon::OnError(LPCWSTR pszMessage)
{
	// �G���[�������̃��b�Z�[�W�\��
	m_pApp->AddLog(pszMessage,TVTest::LOG_TYPE_ERROR);
	if (!m_pApp->GetSilentMode()) {
		::MessageBoxW(m_pApp->GetAppWindow(),pszMessage,L"PX-W3U4/PX-Q3U4�����R��",
					  MB_OK | MB_ICONEXCLAMATION);
	}
}


void CRemocon::OnRemoconDown(DWORD Data, LPARAM pParam) {
	// �����ꂽ�{�^����T��
	for (int i = 0; i<NUM_BUTTONS; i++) {
		if (g_ButtonList[i].KeyCode == Data) {
			// �{�^���������ꂽ���Ƃ�ʒm����
			CRemocon* pThis = (CRemocon*)pParam;
			pThis->m_pApp->OnControllerButtonDown(X3U4_REMOCON_NAME, i);
			break;
		}
	}
}


// �C�x���g�R�[���o�b�N�֐�
// �����C�x���g���N����ƌĂ΂��
LRESULT CALLBACK CRemocon::EventCallback(UINT Event, LPARAM lParam1, LPARAM /*lParam2*/, void *pClientData)
{
	CRemocon *pThis=static_cast<CRemocon*>(pClientData);

	switch (Event) {
	case TVTest::EVENT_PLUGINENABLE:
		// �v���O�C���̗L����Ԃ��ω�����
		if (lParam1!=0) {
			if (!pThis->InitializePlugin()) {
				pThis->OnError(L"�������ŃG���[���������܂����B");
				return FALSE;
			}
		} else {
		}
		return TRUE;

	case TVTest::EVENT_CONTROLLERFOCUS:
		// ���̃v���Z�X������̑ΏۂɂȂ���
		return TRUE;
	}

	return 0;
}


// �v���O�C���N���X�̃C���X�^���X�𐶐�����
TVTest::CTVTestPlugin *CreatePluginClass()
{
	return new CRemocon;
}
