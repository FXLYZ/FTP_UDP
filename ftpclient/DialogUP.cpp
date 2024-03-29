// DialogUP.cpp: 实现文件
//

#include "stdafx.h"
#include "ftpclient.h"
#include "DialogUP.h"
#include "afxdialogex.h"


// DialogUP 对话框

IMPLEMENT_DYNAMIC(DialogUP, CDialogEx)

DialogUP::DialogUP(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_UP, pParent)
	, m_name(_T(""))
	, m_path(_T(""))
{

	desIP = _T("");
	desPort = 0;
	//  m_pro = 0;
}

DialogUP::~DialogUP()
{
}

void DialogUP::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT1, m_ip);
	//  DDX_Text(pDX, IDC_EDIT2, m_port);
	DDX_Text(pDX, IDC_EDIT1, m_name);
	DDX_Text(pDX, IDC_EDIT2, m_path);
	DDX_Control(pDX, IDC_FINISH, m_finish);
	DDX_Control(pDX, IDC_DOWN, m_down);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlpro);
}


BEGIN_MESSAGE_MAP(DialogUP, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DialogUP 消息处理程序


BOOL DialogUP::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//选择可用端口创建套接字
	BOOL bFlag;
	for (int i = minport; i <= maxport; i++) {  //从5556-5700端口范围内选择一个可用端口
		bFlag = socket.Create(i, SOCK_DGRAM);
		if (bFlag) {
			break;
		}
	}
	if (!bFlag) {
		MessageBox(L"socket create error");
	}

	//赋值
	socket.ip = desIP;
	socket.port = desPort;
	socket.dlg = (void*)this;
	socket.isUP = isUP;

	//设置进度条属性
	m_ctrlpro.SetRange(1, 10);
	m_ctrlpro.SetStep(1);

	//判断上传还是下载
	if (isUP) {
		//发送命令
		char *s = "400";
		socket.SendTo(s, 4, desPort, desIP);

		//写日志
		ofstream out("log.txt", ios::out | ios::app);
		out << "\nC:400\n";
		out.close();
	}
	else {
		SetWindowTextW(L"下载窗口");
		UpdateData(false);
		socket.MySend();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void DialogUP::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	KillTimer(1);
	socket.sendagain();
	CDialogEx::OnTimer(nIDEvent);
}
