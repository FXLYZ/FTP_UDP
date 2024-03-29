
// ftpclientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ftpclient.h"
#include "ftpclientDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include "UpThread.h"
#include "DialogUP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CftpclientDlg 对话框



CftpclientDlg::CftpclientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FTPCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	desIP = _T("");
}

void CftpclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_ctrlcommand);
	DDX_Control(pDX, IDC_LISTDIR, m_ctrldir);
	DDX_Control(pDX, IDC_IPADDRESS1, ServerIP);
}

BEGIN_MESSAGE_MAP(CftpclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON, &CftpclientDlg::OnBnClickedButton)
END_MESSAGE_MAP()


// CftpclientDlg 消息处理程序

BOOL CftpclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	ofstream out("log.txt",ios::out | ios::trunc);
	out.close();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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

	m_ctrlcommand.SetCurSel(0);

	desIP = L"127.0.0.1";
	ServerIP.SetAddress(127, 0, 0, 1);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CftpclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CftpclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CftpclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CftpclientDlg::OnBnClickedButton()
{
	// TODO: 在此添加控件通知处理程序代码
	ServerIP.GetWindowTextW(desIP);
	CString str;
	m_ctrlcommand.GetLBText(m_ctrlcommand.GetCurSel(), str);  //获取当前选中命令
	char* s;//根据选中命令发送指令
	if (L"200" == str.Left(3)) {
		//发数据报
		s = "200";
		socket.SendTo(s, 4, desport, desIP);

		//写日志
		ofstream out("log.txt", ios::out | ios::app);
		out << "\nC:200\n";
		out.close();
		
	}
	else if (L"300" == str.Left(3)) {
		if (m_ctrldir.GetCount() == 0) {
			MessageBox(L"请先更新目录");
			return;
		}
		if (m_ctrldir.GetCurSel() == -1) {
			MessageBox(L"请选中文件");
			return;
		}

		//发数据报
		CString filename;
		m_ctrldir.GetText(m_ctrldir.GetCurSel(), filename);
		int n = filename.GetLength();
		string send = "300\n" + (CStringA)filename.GetBuffer();
		socket.SendTo(send.c_str(), send.length(), desport, desIP);

		//写日志
		ofstream out("log.txt", ios::out | ios::app);
		out << "\nC:300\\n";
		out << ((string)(CStringA)filename.GetBuffer()).c_str();
		out << "\n";
		out.close();
	}
	else if (L"400" == str.Left(3)) {

		//模态打开对话框
		DialogUP dlg;
		dlg.desIP = desIP;
		dlg.desPort = desport;
		dlg.isUP = true;
		dlg.DoModal();

	}
	else {
		MessageBox(L"错误命令");
	}

}


