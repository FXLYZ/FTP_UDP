#pragma once
#include <afxwin.h>
#include "ReceiveDlg.h"
class ReceiveThread :
	public CWinThread
{
	DECLARE_DYNCREATE(ReceiveThread)
public:
	ReceiveThread();
	~ReceiveThread();


protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int ExitInstance();
	virtual BOOL InitInstance();
	ReceiveDlg dlg;
	CString srcIP;
	UINT srcPort;
};

