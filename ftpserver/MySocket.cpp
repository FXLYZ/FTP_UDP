#include "stdafx.h"
#include "MySocket.h"
#include "ReceiveThread.h"
#include "ftpserverDlg.h"
#include <io.h>
#include <string>
#include <fstream>
using namespace std;

MySocket::MySocket()
{
}


MySocket::~MySocket()
{
}


void MySocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CftpserverDlg* dlg = (CftpserverDlg*)AfxGetMainWnd();
	char str[100];
	CString ip;
	UINT port;
	int a = ReceiveFrom(str, 100, ip, port);
	str[3] = '\0';
	if (strcmp(str,"200") == 0) {
		string send = "210\n";

		//��ȡ�����ļ�Ŀ¼���������ļ���
		string p = "";
		long hFile = 0;
		_finddata_t fileinfo;
		if ((hFile = _findfirst(p.append("*.*").c_str(), &fileinfo)) != -1) {
			do {
				if (!(fileinfo.attrib & _A_SUBDIR)) {
					send.append(fileinfo.name);
					send += "\n";
				}
			} while (_findnext(hFile, &fileinfo) == 0);
		}
		_findclose(hFile);
		//����
		SendTo(send.c_str(),send.length() , port, ip);
		//д��־
		ofstream out0("log.txt", ios::out | ios::app);
		out0 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):200\n";
		out0 << "S:" << send.c_str() << "\n";
		out0.close();
	}
	else if (strcmp(str, "400") == 0) {
		//д��־
		ofstream out0("log.txt", ios::out | ios::app);
		out0 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):400\n";
		out0.close();
		//�������̣߳�׼�������ļ�
		ReceiveThread* thread = (ReceiveThread*)AfxBeginThread(RUNTIME_CLASS(ReceiveThread), 0, 0, CREATE_SUSPENDED);
		thread->srcIP = ip;
		thread->srcPort = port;
		thread->dlg.isReceive = true;
		ResumeThread(thread->m_hThread);
	}
	else if (strcmp(str, "300") == 0) {
		//��ȡ�ļ���
		str[a] = '\0';
		string filename(str + 4);
		ifstream in(filename);
		//д��־
		ofstream out0("log.txt", ios::out | ios::app);
		out0 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):300\n";
		out0.close();
		if (!in) {  //�ļ������ڣ���ȡ��Ŀ¼��������
			string send = "320\n";
			string p = "";
			long hFile = 0;
			_finddata_t fileinfo;
			if ((hFile = _findfirst(p.append("*").c_str(), &fileinfo)) != -1) {
				do {
					if (!(fileinfo.attrib & _A_SUBDIR)) {
						send.append(fileinfo.name);
						send += "\n";
					}
				} while (_findnext(hFile, &fileinfo) == 0);
			}
			_findclose(hFile);
			//����
			SendTo(send.c_str(), send.length(), port, ip);
			//д��־
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "S:" << send.c_str() << "\n";
			out0.close();
		}
		else {//�ļ����ڣ������̷߳����ļ�
			in.close();
			ReceiveThread* thread = (ReceiveThread*)AfxBeginThread(
										RUNTIME_CLASS(ReceiveThread), 
										0, 0, CREATE_SUSPENDED);
			thread->srcIP = ip;
			thread->srcPort = port;
			thread->dlg.isReceive = false;
			thread->dlg.socket.filename = filename;
			ResumeThread(thread->m_hThread);
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}
