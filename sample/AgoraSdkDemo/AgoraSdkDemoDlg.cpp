
// AgoraSdkDemoDlg.cpp : implementation file
//
#include "JoinDlg.h"
#include "stdafx.h"
#include "AgoraSdkDemo.h"
#include "AgoraSdkDemoDlg.h"
#include <AgoraAudioSDK.h>
#include <set>
#include "afxdialogex.h"
#include <fstream>
#include <string>
#include <vector>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_AGORA_LOAD_ENGINE_SUCCESS WM_USER+1
#define WM_AGORA_JOIN_SUCCESS	WM_USER+2
#define WM_AGORA_ERROR			WM_USER+3
#define WM_AGORA_LOG_EVENT		WM_USER+4
#define WM_AGORA_QUALITY		WM_USER+5

class AgoraAudioEventHandler : public IAgoraAudioEventHandler
{
private:
	CWnd* m_pWnd;
	std::set<std::string*> m_lstPendingMessages;
	CRITICAL_SECTION m_cs;
public:
	AgoraAudioEventHandler(CWnd* wnd)
		:m_pWnd(wnd)
	{
		::InitializeCriticalSection(&m_cs);
	}
	~AgoraAudioEventHandler()
	{
		::DeleteCriticalSection(&m_cs);
		std::set<std::string*>::iterator it = m_lstPendingMessages.begin();
		for (; it != m_lstPendingMessages.end(); ++it)
			delete *it;
	}
	std::string* insertMessage(const char* msg)
	{
		if (!msg)
			return NULL;
		std::string* str = new std::string(msg);
		::EnterCriticalSection(&m_cs);
		m_lstPendingMessages.insert(str);
		::LeaveCriticalSection(&m_cs);
		return str;
	}
	void removeMessage(std::string* msg)
	{
		::EnterCriticalSection(&m_cs);
		m_lstPendingMessages.erase(msg);
		::LeaveCriticalSection(&m_cs);
		delete msg;
	}

	virtual void onLoadAudioEngineSuccess()
	{
		// dont block the callback thread
		if (!m_pWnd->GetSafeHwnd())
			return;
		m_pWnd->PostMessage(WM_AGORA_LOAD_ENGINE_SUCCESS, 0, 0);
	}
    virtual void onGetAudioSvrAddrSuccess(const char* msg)
	{
	}
    virtual void onJoinSuccess(const char* msg)
	{
		// dont block the callback thread
		if (!m_pWnd->GetSafeHwnd())
			return;
		m_pWnd->PostMessage(WM_AGORA_JOIN_SUCCESS, 0, 0);
	}
    virtual void onError(int rescode, const char* msg)
	{
		// dont block the callback thread
		if (!m_pWnd->GetSafeHwnd())
			return;
		m_pWnd->PostMessage(WM_AGORA_ERROR, 0, rescode);
	}
    virtual void onLogEvent(const char* msg)
	{
		// dont block the callback thread
		if (!m_pWnd->GetSafeHwnd())
			return;
		m_pWnd->PostMessage(WM_AGORA_LOG_EVENT, 0, (LPARAM)insertMessage(msg));
	}
	virtual void onQuality(unsigned int uid, unsigned short delay, unsigned short lost, unsigned short jitter)
	{
		if (!m_pWnd->GetSafeHwnd())
			return;

		// dont block the callback thread
		char buf[256];
		sprintf(buf, "user %u delay %d lost %d jitter %d", uid, delay, lost, jitter);
		m_pWnd->PostMessage(WM_AGORA_QUALITY, 0, (LPARAM)insertMessage(buf));
	}
};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAgoraSdkDemoDlg dialog




CAgoraSdkDemoDlg::CAgoraSdkDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAgoraSdkDemoDlg::IDD, pParent)
	,dlg(new CJoinDlg())
	, m_pAgoraAudio(NULL)
	, m_agoraEventHandler(new AgoraAudioEventHandler(this))
	, m_bMute(false)
	, m_strVendorKey(_T("6D7A26A1D3554A54A9F43BE6797FE3E2"))
	, m_strChannelName(_T(""))
	, m_nUserId(1124)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAgoraSdkDemoDlg::~CAgoraSdkDemoDlg()
{
	// Hwnd is not valid
	ReleaseAgoraAudio();
	if (m_agoraEventHandler)
	{
		delete m_agoraEventHandler;
		m_agoraEventHandler = NULL;
	}
	finalDestroyAgoraAudioInstance();
}

void CAgoraSdkDemoDlg::OnOK()
{
	ReleaseAgoraAudio();
	CDialog::OnOK();
}

void CAgoraSdkDemoDlg::OnCancel()
{
	ReleaseAgoraAudio();
	CDialog::OnCancel();
}

void CAgoraSdkDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_CREATE, m_btnCreate);
	DDX_Control(pDX, IDC_BTN_JOIN, m_btnJoin);
	DDX_Control(pDX, IDC_BTN_MUTE, m_btnMute);
	DDX_Text(pDX, IDC_EDIT_KEY, m_strVendorKey);
	DDX_Text(pDX, IDC_EDIT_CHANNEL, m_strChannelName);
	DDX_Control(pDX, IDC_EDIT_CHANNEL, m_ChannelName);
	DDX_Text(pDX, IDC_EDIT_USER_ID, m_nUserId);
	DDX_Control(pDX, IDC_REDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_BTN_PASTE, m_btnPaste);
	DDX_Control(pDX, IDOK, m_btnExit);
	//DDX_Control(pDX, IDC_MENU_AGORA, m_Menu);
}

BEGIN_MESSAGE_MAP(CAgoraSdkDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CREATE, &CAgoraSdkDemoDlg::OnBnClickedBtCreate)
	ON_BN_CLICKED(IDC_BTN_JOIN, &CAgoraSdkDemoDlg::OnBnClickedBtnJoin)
	ON_BN_CLICKED(IDC_BTN_MUTE, &CAgoraSdkDemoDlg::OnBnClickedBtnMute)
	ON_BN_CLICKED(IDOK, &CAgoraSdkDemoDlg::OnBnClickedOk)
	ON_MESSAGE(WM_AGORA_LOAD_ENGINE_SUCCESS, &CAgoraSdkDemoDlg::OnLoadEngineSuccess)
	ON_MESSAGE(WM_AGORA_JOIN_SUCCESS, &CAgoraSdkDemoDlg::OnJoinSuccess)
	ON_MESSAGE(WM_AGORA_ERROR, &CAgoraSdkDemoDlg::OnError)
	ON_MESSAGE(WM_AGORA_LOG_EVENT, &CAgoraSdkDemoDlg::OnLogEvent)
	ON_MESSAGE(WM_AGORA_QUALITY, &CAgoraSdkDemoDlg::OnQuality)
	ON_BN_CLICKED(IDC_BTN_PASTE, &CAgoraSdkDemoDlg::OnBnClickedBtnPaste)
	ON_BN_CLICKED(IDOK, &CAgoraSdkDemoDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_CHANNEL, &CAgoraSdkDemoDlg::OnEnChangeEditChannel)
	//ON_BN_CLICKED(IDC_MENU_AGORA, &CAgoraSdkDemoDlg::OnBnClickedMfcMume)
END_MESSAGE_MAP()


// CAgoraSdkDemoDlg message handlers

BOOL CAgoraSdkDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CWnd *pChannel = GetDlgItem(IDC_EDIT_CHANNEL);
	if (pChannel)
		pChannel->SetFocus();

	m_nUserId = 0;
	m_bMute = false;

	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CAgoraSdkDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAgoraSdkDemoDlg::OnPaint()
{
	//Button icon sets
	//((CButton*)GetDlgItem(IDC_BTN_CREATE))->SetIcon(::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CANCEL)));
	//((CButton*)GetDlgItem(IDOK))->SetIcon(::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CANCEL)));
	//((CButton*)GetDlgItem(IDC_BTN_MUTE))->SetIcon(::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_MICROPHONE)));
	//((CButton*)GetDlgItem(IDC_BTN))->SetBitmap((HBITMAP)::LoadImage(AfxGetApp()->m_hInstance, "J:\\xx.bmp",IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTCOLOR));
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		 //background sets
		 CDialog::OnPaint();
		 /*
		 CDialog::OnPaint();//要禁止这个调用   
         CPaintDC   dc(this);   
         CRect   rect;   
         GetClientRect(&rect);   
         CDC   dcMem;   
         dcMem.CreateCompatibleDC(&dc);   
         CBitmap   bmpBackground;   
         bmpBackground.LoadBitmap(IDB_BITMAP1);   
                 //IDB_BITMAP是你自己的图对应的ID   
         BITMAP   bitmap;   
         bmpBackground.GetBitmap(&bitmap);   
         CBitmap   *pbmpOld=dcMem.SelectObject(&bmpBackground);   
         dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,   
        bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
		*/
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAgoraSdkDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//Join Button
void CAgoraSdkDemoDlg::OnBnClickedBtnJoin()
{
	// TODO: 在此添加控件通知处理程序代码
	dlg->DoModal();//显示Join Session对话框
}

//Mute Button
void CAgoraSdkDemoDlg::OnBnClickedBtnMute()
{
	// TODO: Add your control notification handler code here
	if (!m_pAgoraAudio)
		return;
	if (m_bMute)
		m_btnMute.SetWindowText(_T("Mute"));
	else
		m_btnMute.SetWindowText(_T("Unmute"));
	m_bMute = !m_bMute;
	m_pAgoraAudio->mute(m_bMute);
}

//Exit Button
void CAgoraSdkDemoDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	ReleaseAgoraAudio();
	m_btnPaste.ShowWindow(SW_HIDE);
	m_editLog.ShowWindow(SW_HIDE);
	m_btnMute.ShowWindow(SW_HIDE);
	m_btnExit.ShowWindow(SW_HIDE);
	m_btnCreate.ShowWindow(SW_SHOW);
	m_btnJoin.ShowWindow(SW_SHOW);
	((CEdit*) GetDlgItem(IDC_EDIT_CHANNEL))->ShowWindow(SW_HIDE);//隐藏
	//((CEdit*) GetDlgItem(IDC_EDIT_CHANNEL))->ShowWindow(SW_SHOW);//显示
	m_strChannelName = "";
	GetDlgItem(IDC_EDIT_CHANNEL)->SetWindowText(m_strChannelName);
	//OnOK();
}

void CAgoraSdkDemoDlg::ReleaseAgoraAudio()
{
	if (m_pAgoraAudio)
	{
		m_pAgoraAudio->leave();
		m_pAgoraAudio->release();
		m_pAgoraAudio = NULL;
	}
}

LRESULT CAgoraSdkDemoDlg::OnLoadEngineSuccess(WPARAM wParam, LPARAM lParam)
{
	AddLog("Agora audio engine loaded\n");
	return 0;
}

LRESULT CAgoraSdkDemoDlg::OnJoinSuccess(WPARAM wParam, LPARAM lParam)
{
	AddLog("Joined\n");
	return 0;
}

LRESULT CAgoraSdkDemoDlg::OnError(WPARAM wParam, LPARAM lParam)
{
	char buf[256];
	sprintf(buf, "Got error %d\n", lParam);
	AddLog(buf);
	return 0;
}

LRESULT CAgoraSdkDemoDlg::OnLogEvent(WPARAM wParam, LPARAM lParam)
{
	std::string* msg = (std::string*)lParam;
	if (msg)
	{
		AddLog(*msg+"\n");
		m_agoraEventHandler->removeMessage(msg);
	}
	return 0;
}

LRESULT CAgoraSdkDemoDlg::OnQuality(WPARAM wParam, LPARAM lParam)
{
	std::string* msg = (std::string*)lParam;
	if (msg)
	{
		AddLog(*msg+"\n");
		m_agoraEventHandler->removeMessage(msg);
	}
	return 0;
}

void CAgoraSdkDemoDlg::AddLog(const std::string& message)
{
	if (message.empty())
		return;

	int nLength = m_editLog.GetWindowTextLength();
   m_editLog.SetSel(nLength, nLength);
   USES_CONVERSION;
   LPCTSTR msg = A2CT(message.c_str());
   m_editLog.ReplaceSel(msg);
}


void CAgoraSdkDemoDlg::OnBnClickedBtCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	char c[62] = {'0','1','2','3','4','5','6','7','8','9',
		'a','b','c','d','e','f','g','h','i','j','k','l',
		'm','n','o','p','q','r','s','t','u','v','w','x',
		'y','z','A','B','C','D','E','F','G','H','I','J',
		'K','L','M','N','O','P','Q','R','S','T','U','V',
		'W','X','Y','Z'};
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	CString urlStr;
	urlStr = "http://www.agora.io/";
	srand((int)time(0));
	for(int i = 0; i < 5; i++) {
		str += c[rand()%62];
	}
	urlStr += str;
	UpdateData(TRUE);
	if (!m_pAgoraAudio)
	{
	// Create channel
		m_strChannelName = urlStr;
		m_btnCreate.ShowWindow(SW_HIDE);
		m_btnJoin.ShowWindow(SW_HIDE);
		m_editLog.ShowWindow(SW_SHOW);
		m_btnExit.ShowWindow(SW_SHOW);
		m_btnPaste.ShowWindow(SW_SHOW);
		m_btnMute.ShowWindow(SW_SHOW);
		//((CEdit*) GetDlgItem(IDC_EDIT_CHANNEL))->ShowWindow(SW_HIDE);//隐藏
		((CEdit*) GetDlgItem(IDC_EDIT_CHANNEL))->ShowWindow(SW_SHOW);//显示
		GetDlgItem(IDC_EDIT_CHANNEL)->SetWindowText(m_strChannelName);
		m_pAgoraAudio = createAgoraAudioInstance(m_agoraEventHandler);
		USES_CONVERSION;
		LPCSTR key = T2CA((LPCTSTR)m_strVendorKey);
		//LPCSTR channelName = T2CA((LPCTSTR)m_strChannelName);
		LPCSTR channelName = T2CA((LPCTSTR)str);
		m_pAgoraAudio->joinChannel(key, channelName, "", m_nUserId);
	}
	else
	{
		ReleaseAgoraAudio();
		//OnOK();
	}
}


void CAgoraSdkDemoDlg::OnBnClickedBtnPaste()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strData;
	m_ChannelName.GetWindowTextW(strData);
	int len = strData.GetLength();
	if (len <= 0)
		return;
	if (!OpenClipboard())
		return;
	EmptyClipboard(); 
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (len + 1));
	if (hglbCopy == NULL) 
    { 
        CloseClipboard(); 
        return; 
    }
	char* lptstrCopy = (char*)GlobalLock(hglbCopy); 
	strcpy(lptstrCopy, (CStringA)strData);
	GlobalUnlock(hglbCopy); 
	SetClipboardData(CF_TEXT, hglbCopy); 
	CloseClipboard(); 
}

void CAgoraSdkDemoDlg::JoinRoom(CString room_id)
{
	UpdateData(TRUE);
	//if (m_strVendorKey.IsEmpty() || m_strChannelName.IsEmpty())
	//	return;

	if (!m_pAgoraAudio)
	{
		// Join channel
		m_btnCreate.ShowWindow(SW_HIDE);
		m_btnJoin.ShowWindow(SW_HIDE);
		m_editLog.ShowWindow(SW_SHOW);
		m_btnExit.ShowWindow(SW_SHOW);
		m_btnPaste.ShowWindow(SW_SHOW);
		m_btnMute.ShowWindow(SW_SHOW);
		//((CEdit*) GetDlgItem(IDC_EDIT_CHANNEL))->ShowWindow(SW_HIDE);//隐藏
		((CEdit*) GetDlgItem(IDC_EDIT_CHANNEL))->ShowWindow(SW_SHOW);//显示
		GetDlgItem(IDC_EDIT_CHANNEL)->SetWindowText(room_id);
		m_pAgoraAudio = createAgoraAudioInstance(m_agoraEventHandler);
		USES_CONVERSION;
		LPCSTR key = T2CA((LPCTSTR)m_strVendorKey);
		LPCSTR channelName = T2CA((LPCTSTR)room_id);
		//LPCSTR channelName = LPCTSTR(m_strChannelName);
		m_pAgoraAudio->joinChannel(key, channelName, "", m_nUserId);
	}
	else
	{
		//m_btnJoin.SetWindowText(_T("Join"));
		ReleaseAgoraAudio();
	}
}



void CAgoraSdkDemoDlg::OnEnChangeEditChannel()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

