#pragma once
#include "DialogUP.h"
class UpThread :
	public CWinThread
{
	DECLARE_DYNCREATE(UpThread)
public:
	UpThread();
	~UpThread();
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()

public:
	//DialogUP dlg;
	CString desIP;
	UINT desPort;
};

