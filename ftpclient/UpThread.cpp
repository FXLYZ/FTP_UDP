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
	// TODO: �ڴ����ר�ô����/����û���
	/*dlg.desIP = desIP;
	dlg.desPort = desPort;
	dlg.DoModal();*/
	return CWinThread::InitInstance();
}


int UpThread::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���

	return CWinThread::ExitInstance();
}


BEGIN_MESSAGE_MAP(UpThread, CWinThread)//��Ϣ
END_MESSAGE_MAP()