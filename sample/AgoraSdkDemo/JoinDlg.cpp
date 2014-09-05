// JoinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraSdkDemo.h"
#include "JoinDlg.h"
#include "afxdialogex.h"
#include "AgoraSdkDemoDlg.h"
#include <AgoraAudioSDK.h>
#include <set>
#include <fstream>
#include <string>
#include <vector>

// CAddDlg 对话框

IMPLEMENT_DYNAMIC(CJoinDlg, CDialog)

CJoinDlg::CJoinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJoinDlg::IDD, pParent)
{

}

CJoinDlg::~CJoinDlg()
{
}

void CJoinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ROOMID, m_RoomID);
	DDX_Control(pDX, IDC_BTNOK, m_BTN_OK);
}


BEGIN_MESSAGE_MAP(CJoinDlg, CDialog)
	ON_BN_CLICKED(IDC_BTNOK, &CJoinDlg::OnClickedBtnok)
	ON_BN_CLICKED(IDC_BTNCANCEL, &CJoinDlg::OnClickedBtncancel)
END_MESSAGE_MAP()


// CAddDlg 消息处理程序


void CJoinDlg::OnClickedBtnok()
{
	// TODO: 在此添加控件通知处理程序代码
	CString room_id;
	GetDlgItem(IDC_EDIT_ROOMID)->GetWindowText(room_id);//获取输入
	if(room_id.IsEmpty())//若字符串为空
	{
		MessageBox(MB_OK);
		return;
	}
	CAgoraSdkDemoDlg *pWnd=(CAgoraSdkDemoDlg*)GetParent();//取得父窗口对象指针
	pWnd->JoinRoom(room_id.GetBuffer());//调用函数添加信息
	OnCancel();//销毁Join Session对话框
}


void CJoinDlg::OnClickedBtncancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();//销毁添加城市信息对话框
}
