#pragma once
#include <afxsock.h>
#include <string>
#include <fstream>
using namespace std;
class MySocket :
	public CAsyncSocket
{
public:
	MySocket();
	~MySocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	char* buffer;
	CString ip;
	UINT port;
//	char* command;
//	void* dlg;
	string bufferall;
	char* command;
	ifstream file;
};

