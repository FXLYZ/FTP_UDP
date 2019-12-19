#include "stdafx.h"
#include "ReceiveSocket.h"
#include "ReceiveDlg.h"

ReceiveSocket::ReceiveSocket()
{
	ip = _T("");
	port = 0;
	filesize = 0;
	nextnumb = 0;
	//  sumnumb = 0;
	recfilesize = 0;
}


ReceiveSocket::~ReceiveSocket()
{
}


void ReceiveSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	ReceiveDlg* recdlg = (ReceiveDlg*)dlg;
	memset(buffer, 0, 1200);
	int n = ReceiveFrom(buffer, 1200, ip, port);
	buffer[3] = '\0';
	buffer[n] = '\0';
	if (isReceive) {
		if (strcmp(buffer, "420") == 0) {  //420\n�ļ���С\n�ļ���
			//��ȡ�ļ������ļ���С
			filesize = *(int*)(buffer + 4);
			char* filename = buffer + 9;
			CString name(filename);
			//���´���
			recdlg->m_ip = ip;
			recdlg->m_port = port;
			recdlg->m_filename = name;
			recdlg->UpdateData(false);
			//�����ļ������
			out.open(filename, ios::out | ios::binary | ios::trunc); //�Զ�����д�뷽ʽ���ļ������ļ���������ɾ���ļ�
			//����
			memset(buffer, 0, 1200);
			buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
			*(int*)(buffer + 4) = 0;
			nextnumb = 0;
			SendTo(buffer, 8, port, ip);
			//���ü�ʱ��
			recdlg->SetTimer(1, 50, NULL);
			//д��־
			ofstream out1("log.txt", ios::out | ios::app);
			out1 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):420\\n" << filesize << "\\n" << filename << "\n";
			out1 << "S:430\\n" << nextnumb << "\n";
			out1.close();
		}
		else if (strcmp(buffer, "440") == 0) {
			//���ټ�ʱ��
			recdlg->KillTimer(1);
			//д��־
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):420\n";
			out0.close();
			//�ж��Ƿ�����Ҫ�����ݶ�
			if (*(int*)(buffer + 4) == nextnumb) {//����д���ļ�
				nextnumb++;
				out.write(buffer + 8, n - 8);
				recfilesize += n - 8;
				//�ж��ļ��Ƿ�������
				if (recfilesize >= filesize) {
					//����
					out.close();
					buffer[0] = '4'; buffer[1] = '5'; buffer[2] = '0'; buffer[3] = '\n';  //450  ��������ȡ�ļ����
					SendTo(buffer, 4, port, ip);
					//д��־
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "S:450\n";
					out0.close();
					//�ر��׽��֣��رնԻ��򣨶Ի���رպ����̹߳رգ�
					this->Close();
					recdlg-> EndDialog(0);
				}
				else {
					//����
					buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
					*(int*)(buffer + 4) = nextnumb;
					SendTo(buffer, 8, port, ip);
					//���ü�ʱ��
					recdlg->SetTimer(1, 50, NULL);
					//д��־
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "S:430\\n" << nextnumb << "\n";
					out0.close();
				}
			}
			else {//�����������ݣ����·���ACK
				//����
				buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
				*(int*)(buffer + 4) = nextnumb;
				SendTo(buffer, 8, port, ip);
				//���ü�ʱ��
				recdlg->SetTimer(1, 50, NULL);
				//д��־
				ofstream out0("log.txt", ios::out | ios::app);
				out0 << "S:430\\n" << nextnumb << "\n";
				out0.close();
			}
		}
	}
	else {
		if (strcmp(buffer, "330") == 0) {  
			//�ͻ��˷���ACK��ȷ����Ҫ���͵����ݱ����  
			//330\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
			int n = *(int*)(buffer + 4);  //n*1024 --- (n+1)*1024-1
			//��ȡ��Ӧ���ݶ����ݲ�����
			memset(buffer, 0, 1200);    //340\n���ݶκ�(int)����
			buffer[0] = '3'; buffer[1] = '4'; buffer[2] = '0'; buffer[3] = '\n';
			*(int*)(buffer + 4) = n;
			in.seekg(n * 1024, ios::beg);
			in.read(buffer + 8, 1024);
			if ((n + 1) * 1024 - 1 >= filesize) {
				SendTo(buffer, filesize - n * 1024 + 8, port, ip);
			}
			else {
				SendTo(buffer, 1024 + 8, port, ip);
			}
			//д��־
			ofstream out1("log.txt", ios::out | ios::app);
			out1 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):330\\n" << n << "\n";
			out1 << "S:340\n";
			out1.close();
		}
		else if (strcmp(buffer, "350") == 0) {  //�����������ļ����
			//д��־
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):350\n";
			out0.close();
			//�����ļ����ر��׽��֣��رնԻ���
			in.close();
			this->Close();
			recdlg->EndDialog(0);
		}
	}
}



void ReceiveSocket::MySend()
{
	// TODO: �ڴ����ר�ô����/����û���
	//�����ļ������
	in.open(filename, ios::in | ios::binary);
	in.seekg(0, ios::end);
	filesize = (int)in.tellg();   //��ȡ�ļ���С
	//���´���
	ReceiveDlg* recdlg = (ReceiveDlg*)dlg;
	recdlg->m_ip = ip;
	recdlg->m_port = port;
	recdlg->m_filename.Format(L"%S",filename.c_str());
	recdlg->UpdateData(false);

	//310\n�ļ���С��int��\n�ļ���
	string send = "310\n0000\n" + filename;   
	int *a = (int*)&send[4];
	*a = filesize;
	//����
	SendTo(send.c_str(), send.length(), port, ip);
	//д��־
	ofstream out0("log.txt", ios::out | ios::app);
	out0 << "S:310\\n" << filesize << "\\n" << filename.c_str() << ")\n";
	out0.close();
}


void ReceiveSocket::sendagain()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	//����ACK
	buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
	*(int*)(buffer + 4) = nextnumb;
	SendTo(buffer, 8, port, ip);
	//���ü�ʱ��
	ReceiveDlg* recdlg = (ReceiveDlg*)dlg;
	recdlg->SetTimer(1, 50, NULL);
	//д��־
	ofstream out0("log.txt", ios::out | ios::app);
	out0 << "S:430\\n" << nextnumb << "\n";
	out0.close();

	
}
