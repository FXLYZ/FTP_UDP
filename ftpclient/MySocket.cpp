#include "stdafx.h"
#include "MySocket.h"
#include "ftpclientDlg.h"
#include "UpThread.h"
#include "DialogUP.h"


MySocket::MySocket()
{
	buffer = new char[60000];
	command = new char[50];
	ip = _T("");
	port = 0;
	bufferall = "";
}


MySocket::~MySocket()
{
}


void MySocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CftpclientDlg* dlg = (CftpclientDlg*)AfxGetMainWnd();
	memset(command, 0, 50);
	ReceiveFrom(command, 50, ip, port, MSG_PEEK);
	command[3] = '\0';
	if (strcmp(command,"210") == 0 || strcmp(command, "320") == 0) {  //接收目录
		
		//接收
		memset(buffer, 0, 60000);
		ReceiveFrom(buffer, 60000, ip, port);
		
		//解析数据报，将目录显示到listbox
		CString str(buffer);
		str.Delete(0, str.Find(_T("\n")) + 1);  //删除210\n
		dlg->m_ctrldir.ResetContent();
		while (str.Find(_T("\n")) + 1) {
			dlg->m_ctrldir.AddString(str.Left(str.Find(_T("\n"))));
			str.Delete(0, str.Find(_T("\n")) + 1);
		}

		//提示窗口
		if (strcmp(command, "210") == 0)
			dlg->MessageBox(L"获取目录成功");
		else
			dlg->MessageBox(L"远程文件可能被移动，更新目录成功");

		//写日志
		ofstream out("log.txt", ios::out | ios::app);
		out << "S:"; out << buffer; out << "\n";
		out.close();
	}
	else if (strcmp(command, "310") == 0) {  //310\n文件大小\n文件名
		//接收
		memset(buffer, 0, 60000);
		int n = ReceiveFrom(buffer, 60000, ip, port);
		buffer[3] = '\0';
		buffer[n] = '\0';
		//获得文件大小及文件名
		int length = *(int*)(buffer + 4);
		char* filename = buffer + 9;

		//写日志
		ofstream out("log.txt", ios::out | ios::app);
		out << "S:310\\n"; out << length << "\\n" << filename << "\n";
		out.close();

		//模态打开对话框,创建套接字接收文件
		DialogUP dialog;
		dialog.desIP = ip;
		dialog.desPort = port;
		dialog.socket.filelength = length;
		dialog.socket.m_pro = length / 10;
		dialog.socket.filename = filename;
		dialog.m_path = L"程序所在目录";
		dialog.isUP = false;
		dialog.DoModal();

	}
	else {
		//dlg->MessageBox(L"error 3");
	}
	CAsyncSocket::OnReceive(nErrorCode);
}


void MySocket::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnSend(nErrorCode);
}
