// JoinDlg.cpp : ʵ���ļ�
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

// CAddDlg �Ի���

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


// CAddDlg ��Ϣ�������


void CJoinDlg::OnClickedBtnok()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString room_id;
	GetDlgItem(IDC_EDIT_ROOMID)->GetWindowText(room_id);//��ȡ����
	if(room_id.IsEmpty())//���ַ���Ϊ��
	{
		MessageBox(MB_OK);
		return;
	}
	CAgoraSdkDemoDlg *pWnd=(CAgoraSdkDemoDlg*)GetParent();//ȡ�ø����ڶ���ָ��
	pWnd->JoinRoom(room_id.GetBuffer());//���ú��������Ϣ
	OnCancel();//����Join Session�Ի���
}


void CJoinDlg::OnClickedBtncancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();//������ӳ�����Ϣ�Ի���
}
