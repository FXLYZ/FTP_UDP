#pragma once
#include <afxsock.h>
#include <fstream>
using namespace std;
class ReceiveSocket :
	public CAsyncSocket
{
public:
	ReceiveSocket();
	~ReceiveSocket();
	CString ip;
	UINT port;
	void* dlg;
	virtual void OnReceive(int nErrorCode);
	ofstream out;
	int filesize;
	int nextnumb;
//	int sumnumb;
	int recfilesize;
	char buffer[1200];
	bool isReceive;
	virtual void MySend();
	string filename;
	ifstream in;
	void sendagain();
};

