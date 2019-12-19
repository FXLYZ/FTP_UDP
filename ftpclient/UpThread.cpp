#include "stdafx.h"
#include "UpThread.h"

IMPLEMENT_DYNCREATE(UpThread, CWinThread)

UpThread::UpThread()
{
	desIP = _T("");
	desPort = 0;
}


UpThread::~UpThread()
{
}


BOOL UpThread::InitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	/*dlg.desIP = desIP;
	dlg.desPort = desPort;
	dlg.DoModal();*/
	return CWinThread::InitInstance();
}


int UpThread::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWinThread::ExitInstance();
}


BEGIN_MESSAGE_MAP(UpThread, CWinThread)//消息
END_MESSAGE_MAP()