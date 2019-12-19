#pragma once
#include <afxsock.h>
#include <fstream>
using namespace std;
class UpSocket :
	public CAsyncSocket
{
public:
	UpSocket();
	~UpSocket();
	virtual void OnReceive(int nErrorCode);
	char command[20];
	CString ip;
	UINT port;
	void* dlg;
	ifstream in;
	int filelength;
	char buffer[1200];
	virtual void MySend();
	ofstream out;
	string filename;
	int recfilesize;
	int nextnumb;
	bool isUP;
	int m_pro;
	void sendagain();
};

