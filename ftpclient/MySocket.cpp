#include "stdafx.h"
#include "MySocket.h"
#include "ftpclientDlg.h"
#include "UpThread.h"
#include "DialogUP.h"


MySocket::MySocket()
{
	buffer = new char[60000];
	command = new char[50];
	ip = _T("");
	port = 0;
	bufferall = "";
}


MySocket::~MySocket()
{
}


void MySocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CftpclientDlg* dlg = (CftpclientDlg*)AfxGetMainWnd();
	memset(command, 0, 50);
	ReceiveFrom(command, 50, ip, port, MSG_PEEK);
	command[3] = '\0';
	if (strcmp(command,"210") == 0 || strcmp(command, "320") == 0) {  //����Ŀ¼
		
		//����
		memset(buffer, 0, 60000);
		ReceiveFrom(buffer, 60000, ip, port);
		
		//�������ݱ�����Ŀ¼��ʾ��listbox
		CString str(buffer);
		str.Delete(0, str.Find(_T("\n")) + 1);  //ɾ��210\n
		dlg->m_ctrldir.ResetContent();
		while (str.Find(_T("\n")) + 1) {
			dlg->m_ctrldir.AddString(str.Left(str.Find(_T("\n"))));
			str.Delete(0, str.Find(_T("\n")) + 1);
		}

		//��ʾ����
		if (strcmp(command, "210") == 0)
			dlg->MessageBox(L"��ȡĿ¼�ɹ�");
		else
			dlg->MessageBox(L"Զ���ļ����ܱ��ƶ�������Ŀ¼�ɹ�");

		//д��־
		ofstream out("log.txt", ios::out | ios::app);
		out << "S:"; out << buffer; out << "\n";
		out.close();
	}
	else if (strcmp(command, "310") == 0) {  //310\n�ļ���С\n�ļ���
		//����
		memset(buffer, 0, 60000);
		int n = ReceiveFrom(buffer, 60000, ip, port);
		buffer[3] = '\0';
		buffer[n] = '\0';
		//����ļ���С���ļ���
		int length = *(int*)(buffer + 4);
		char* filename = buffer + 9;

		//д��־
		ofstream out("log.txt", ios::out | ios::app);
		out << "S:310\\n"; out << length << "\\n" << filename << "\n";
		out.close();

		//ģ̬�򿪶Ի���,�����׽��ֽ����ļ�
		DialogUP dialog;
		dialog.desIP = ip;
		dialog.desPort = port;
		dialog.socket.filelength = length;
		dialog.socket.m_pro = length / 10;
		dialog.socket.filename = filename;
		dialog.m_path = L"��������Ŀ¼";
		dialog.isUP = false;
		dialog.DoModal();

	}
	else {
		//dlg->MessageBox(L"error 3");
	}
	CAsyncSocket::OnReceive(nErrorCode);
}


void MySocket::OnSend(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnSend(nErrorCode);
}
