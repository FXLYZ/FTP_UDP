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
	// TODO: �ڴ����ר�ô����/����û���

	return CWinThread::ExitInstance();
}


BOOL ReceiveThread::InitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	dlg.srcIP = srcIP;
	dlg.srcPort = srcPort;
	dlg.DoModal();
	return CWinThread::InitInstance();
}


BEGIN_MESSAGE_MAP(ReceiveThread, CWinThread)//��Ϣ
END_MESSAGE_MAP()