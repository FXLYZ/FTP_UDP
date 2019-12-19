#pragma once
#include <afxsock.h>
class MySocket :
	public CAsyncSocket
{
public:
	MySocket();
	~MySocket();
	virtual void OnReceive(int nErrorCode);
};

