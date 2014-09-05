
// ClanVoiceDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <string>
#include "afxcmn.h"
#include "JoinDlg.h"

class IAgoraAudio;
class AgoraAudioEventHandler;

// CAgoraSdkDemoDlg dialog
class CAgoraSdkDemoDlg : public CDialog
{
// Construction
public:
	CAgoraSdkDemoDlg(CWnd* pParent = NULL);	// standard constructor
	~CAgoraSdkDemoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CLANVOICE };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
	HICON m_hIcon;
	CButton m_btnJoin;
	CButton m_btnMute;
	CButton m_btnPaste;
	CButton m_btnCreate;
	CButton m_btnExit;
	CEdit m_ChannelName;
	CMFCMenuButton m_Menu;
	CString m_strVendorKey;
	CString m_strChannelName;
	CRichEditCtrl m_editLog;
	UINT m_nUserId;
	IAgoraAudio* m_pAgoraAudio;
	AgoraAudioEventHandler* m_agoraEventHandler;
	bool m_bMute;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnJoin();
	afx_msg void OnBnClickedBtnMute();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtCreate();
	afx_msg void OnBnClickedBtnPaste();
	afx_msg void OnEnChangeEditChannel();
	afx_msg void OnBnClickedMfcMume();
	afx_msg LRESULT OnLoadEngineSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJoinSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLogEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQuality(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	void ReleaseAgoraAudio();
	void AddLog(const std::string& message);
	CJoinDlg *dlg;
public:
	void JoinRoom(CString room_id);
};
