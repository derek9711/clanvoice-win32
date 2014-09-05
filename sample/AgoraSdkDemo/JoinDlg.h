#pragma once

class IAgoraAudio;
class AgoraAudioEventHandler;
// CAddDlg 对话框

class CJoinDlg : public CDialog
{
	DECLARE_DYNAMIC(CJoinDlg)

public:
	CJoinDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CJoinDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_JOIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	

// Implementation
protected:
	afx_msg void OnClickedBtnok();
	afx_msg void OnClickedBtncancel();
public:
	CEdit m_RoomID;
	CButton m_BTN_OK;
};
