#pragma once

class IAgoraAudio;
class AgoraAudioEventHandler;
// CAddDlg �Ի���

class CJoinDlg : public CDialog
{
	DECLARE_DYNAMIC(CJoinDlg)

public:
	CJoinDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CJoinDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_JOIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	

// Implementation
protected:
	afx_msg void OnClickedBtnok();
	afx_msg void OnClickedBtncancel();
public:
	CEdit m_RoomID;
	CButton m_BTN_OK;
};
