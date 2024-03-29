// ReceiveDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ftpserver.h"
#include "ReceiveDlg.h"
#include "afxdialogex.h"


// ReceiveDlg 对话框

IMPLEMENT_DYNAMIC(ReceiveDlg, CDialogEx)

ReceiveDlg::ReceiveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOGRECEIVE, pParent)
	, m_ip(_T(""))
	, m_port(0)
	, m_filename(_T(""))
{

	srcIP = _T("");
	srcPort = 0;
}

ReceiveDlg::~ReceiveDlg()
{
}

void ReceiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ip);
	DDX_Text(pDX, IDC_EDIT2, m_port);
	DDX_Text(pDX, IDC_EDIT3, m_filename);
}


BEGIN_MESSAGE_MAP(ReceiveDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// ReceiveDlg 消息处理程序


BOOL ReceiveDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//创建套接字
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
	//给套接字传相关数据
	socket.ip = srcIP;
	socket.port = srcPort;
	socket.dlg = (void*)this;
	socket.isReceive = isReceive;
	//判断发送还是接收文件
	if (isReceive) {
		//更新窗口
		SetWindowTextW(L"接收窗口");
		UpdateData(false);
		//发送
		char *s = "410";
		socket.SendTo(s, 4, srcPort, srcIP);
		//写日志
		ofstream out0("log.txt", ios::out | ios::app);
		out0 << "S:410\n";
		out0.close();
	}
	else {
		//更新窗口
		SetWindowTextW(L"发送窗口");
		m_filename.Format(L"%S", socket.filename.c_str());
		UpdateData(false);

		socket.MySend();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void ReceiveDlg::OnTimer(UINT_PTR nIDEvent)//超时重传
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//销毁计时器
	KillTimer(1);
	socket.sendagain();
	CDialogEx::OnTimer(nIDEvent);
}
