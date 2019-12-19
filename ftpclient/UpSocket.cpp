#include "stdafx.h"
#include "UpSocket.h"
#include "DialogUP.h"
//#include <fstream>

using namespace std;


UpSocket::UpSocket()
{
	ip = _T("");
	port = 0;
	filelength = 0;
	recfilesize = 0;
	nextnumb = 0;
	m_pro = 0;
}


UpSocket::~UpSocket()
{
}


void UpSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	//�����뻺����
	DialogUP* dialog = (DialogUP*)dlg;
	memset(command, 0, 20);
	ReceiveFrom(command, 20, ip, port,MSG_PEEK);
	memset(buffer, 0, 1200);
	int n = ReceiveFrom(buffer, 1200, ip, port);
	command[3] = '\0';
	buffer[3] = '\0';
	buffer[n] = '\0';

	//�ж��ϴ���������
	if (isUP) {
		if (strcmp(command, "410") == 0) {  //������׼������
			//��ȡ��Ҫ�ϴ����ļ�
			CFileDialog filedlg(true);  
			filedlg.DoModal();
			//��ȡ�ļ������ļ�·��������ʾ���Ի���
			CString filename = filedlg.GetFileName();
			CString filepath = filedlg.GetFolderPath();
			filepath = filepath + "\\" + filename;
			dialog->m_name = filename;
			dialog->m_path = filepath;
			dialog->UpdateData(false);

			//���ļ�������ļ�����
			string path = (CStringA)filepath.GetBuffer();  
			string name = (CStringA)filename.GetBuffer();
			in.open(path, ios::in | ios::binary);
			in.seekg(0, ios::end);
			filelength = (int)in.tellg();

			//�������ݱ�
			string send = "420\n0000\n" + name;   //420\n�ļ���С��int��\n�ļ���
			int *a = (int*)&send[4];
			*a = filelength;
			SendTo(send.c_str(), send.length(), port, ip);

			//д��־
			ofstream out1("log.txt", ios::out | ios::app);
			out1 << "S:410\nC:420\\n"; out1 << filelength << "\\n";out1 << name.c_str() << "\n";
			out1.close();
		}
		else if (strcmp(command, "430") == 0) {  //����������ACK��ȷ����Ҫ���͵����ݱ����  430\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
			//��ȡ���ݱ����
			int n = *(int*)(command + 4);  //n*1024 --- (n+1)*1024-1

			//���ݽ��ս��ȸ��½�������ÿ��10%��
			if (recfilesize < m_pro && (recfilesize + n - 8) >= m_pro) {
				dialog->m_ctrlpro.StepIt();
				m_pro += (filelength / 10);
			}

			//���ļ���������Ŷ�Ӧ�����ݶ�
			memset(buffer, 0, 1200);    //440\n���ݶκ�(int)����
			buffer[0] = '4'; buffer[1] = '4'; buffer[2] = '0'; buffer[3] = '\n';
			*(int*)(buffer + 4) = n;
			in.seekg(n * 1024, ios::beg);
			in.read(buffer + 8, 1024);
			if ((n + 1) * 1024 - 1 >= filelength) 
				SendTo(buffer, filelength - n * 1024 + 8, port, ip);
			else
				SendTo(buffer, 1024 + 8, port, ip);


			//д��־
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "S:430\\n"; out0 << n << "\t"; out0 << "C:440\t";
			out0.close();
		}
		else if (strcmp(command, "450") == 0) {  //�����������ļ����
			//�ر��ļ�
			in.close();

			//���ĶԻ�����ʾ״̬
			dialog->m_finish.ShowWindow(SW_SHOW);
			dialog->m_ctrlpro.SetPos(10);

			//д��־
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "\nS:450\n";
			out0.close();

			//�ر��׽���
			this->Close();
		}
		else {
			dialog->MessageBox(L"error 2");
		}
	}
	else {
		if (strcmp(buffer, "340") == 0) {
			//�رռ�ʱ��
			dialog->KillTimer(1);
			//д��־
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "S:340\t";
			out0.close();
			//�ж��Ƿ�����Ҫ����һ�����ݶ�
			if (*(int*)(buffer + 4) == nextnumb) {
				//���ݽ����ļ���С���½�����
				if (recfilesize < m_pro && (recfilesize + n - 8) >= m_pro) {
					dialog->m_ctrlpro.StepIt();
					m_pro += (filelength / 10);
				}
				//д�ļ�
				nextnumb++;
				out.write(buffer + 8, n - 8);
				recfilesize += n - 8;
				//���ݽ����ļ���С�ж��Ƿ���յ������ļ�
				if (recfilesize >= filelength) {
					//�ر�д�ļ���
					out.close();
					//�������ݱ�
					//350  ��������ȡ�ļ����
					buffer[0] = '3'; buffer[1] = '5'; 
					buffer[2] = '0'; buffer[3] = '\n';  
					SendTo(buffer, 4, port, ip);
					//д��־
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "\nC:350\n";
					out0.close();
					//���ĶԻ�����ʾ״̬
					dialog->m_down.ShowWindow(SW_SHOW);
					dialog->m_ctrlpro.SetPos(10);
					//�ر��׽���
					this->Close();
				}
				else {//�ļ���ȡû�н���
					//����������һ�����ݶ�
					//330\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
					buffer[0] = '3'; buffer[1] = '3'; 
					buffer[2] = '0'; buffer[3] = '\n';  
					*(int*)(buffer + 4) = nextnumb;
					SendTo(buffer, 8, port, ip);
					//���ü�ʱ��
					dialog->SetTimer(1, 50, NULL);
					//д��־
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "C:330\\n"; out0 << nextnumb << "\t";
					out0.close();
				}
			}
			else {//û���յ�Ԥ�����ݶ�
				//��������ǰ���ݶ�
				//430\n��һ����������ݶκţ���0��ʼ��ÿ��1024�ֽڣ�
				buffer[0] = '3'; buffer[1] = '3'; 
				buffer[2] = '0'; buffer[3] = '\n';  
				*(int*)(buffer + 4) = nextnumb;
				SendTo(buffer, 8, port, ip);
				//���ü�ʱ��
				dialog->SetTimer(1, 50, NULL);
				//д��־
				ofstream out0("log.txt", ios::out | ios::app);
				out0 << "C:330\\n"; out0 << nextnumb << "\t";
				out0.close();
			}
		}
		else {
			dialog->MessageBox(L"error 1");
		}
	}
	CAsyncSocket::OnReceive(nErrorCode);
}


void UpSocket::MySend()
{
	//�Ի���ؼ���ֵ������
	DialogUP* dialog = (DialogUP*)dlg;
	dialog->m_name.Format(L"%S", filename.c_str());
	dialog->UpdateData(false);

	//�½���Ҫ���յ��ļ����������ݱ�
	out.open(filename, ios::out | ios::binary | ios::trunc);
	string send = "330\n0000";
	*(int*)&(send[4]) = nextnumb;
	SendTo(send.c_str(), send.length(), port, ip);

	//���ü�ʱ��
	dialog->SetTimer(1, 50, NULL);
	
	//д��־
	ofstream out0("log.txt", ios::out | ios::app);
	out0 << "C:330\\n"; out0 << nextnumb << "\t";
	out0.close();
}


void UpSocket::sendagain()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	DialogUP* dialog = (DialogUP*)dlg;
	//�ٴη�����Ҫ����һ�����ݶ����
	string send = "330\n0000";
	*(int*)&(send[4]) = nextnumb;
	SendTo(send.c_str(), send.length(), port, ip);
	//���ü�ʱ��
	dialog->SetTimer(1, 50, NULL);
}
