#include "stdafx.h"
#include "ReceiveThread.h"

IMPLEMENT_DYNCREATE(ReceiveThread, CWinThread)

ReceiveThread::ReceiveThread()
{
	srcIP = _T("");
	srcPort = 0;
}


ReceiveThread::~ReceiveThread()
{
}


int ReceiveThread::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWinThread::ExitInstance();
}


BOOL ReceiveThread::InitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	dlg.srcIP = srcIP;
	dlg.srcPort = srcPort;
	dlg.DoModal();
	return CWinThread::InitInstance();
}


BEGIN_MESSAGE_MAP(ReceiveThread, CWinThread)//消息
END_MESSAGE_MAP()