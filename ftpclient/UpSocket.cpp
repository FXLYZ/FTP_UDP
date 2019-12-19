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
	// TODO: 在此添加专用代码和/或调用基类

	//读输入缓冲区
	DialogUP* dialog = (DialogUP*)dlg;
	memset(command, 0, 20);
	ReceiveFrom(command, 20, ip, port,MSG_PEEK);
	memset(buffer, 0, 1200);
	int n = ReceiveFrom(buffer, 1200, ip, port);
	command[3] = '\0';
	buffer[3] = '\0';
	buffer[n] = '\0';

	//判断上传还是下载
	if (isUP) {
		if (strcmp(command, "410") == 0) {  //服务器准备好了
			//获取需要上传的文件
			CFileDialog filedlg(true);  
			filedlg.DoModal();
			//获取文件名，文件路径，并显示到对话框
			CString filename = filedlg.GetFileName();
			CString filepath = filedlg.GetFolderPath();
			filepath = filepath + "\\" + filename;
			dialog->m_name = filename;
			dialog->m_path = filepath;
			dialog->UpdateData(false);

			//打开文件并获得文件长度
			string path = (CStringA)filepath.GetBuffer();  
			string name = (CStringA)filename.GetBuffer();
			in.open(path, ios::in | ios::binary);
			in.seekg(0, ios::end);
			filelength = (int)in.tellg();

			//发送数据报
			string send = "420\n0000\n" + name;   //420\n文件大小（int）\n文件名
			int *a = (int*)&send[4];
			*a = filelength;
			SendTo(send.c_str(), send.length(), port, ip);

			//写日志
			ofstream out1("log.txt", ios::out | ios::app);
			out1 << "S:410\nC:420\\n"; out1 << filelength << "\\n";out1 << name.c_str() << "\n";
			out1.close();
		}
		else if (strcmp(command, "430") == 0) {  //服务器返回ACK，确定需要发送的数据报序号  430\n下一个请求的数据段号（从0开始，每段1024字节）
			//获取数据报序号
			int n = *(int*)(command + 4);  //n*1024 --- (n+1)*1024-1

			//根据接收进度更新进度条（每次10%）
			if (recfilesize < m_pro && (recfilesize + n - 8) >= m_pro) {
				dialog->m_ctrlpro.StepIt();
				m_pro += (filelength / 10);
			}

			//读文件，发送序号对应的数据段
			memset(buffer, 0, 1200);    //440\n数据段号(int)数据
			buffer[0] = '4'; buffer[1] = '4'; buffer[2] = '0'; buffer[3] = '\n';
			*(int*)(buffer + 4) = n;
			in.seekg(n * 1024, ios::beg);
			in.read(buffer + 8, 1024);
			if ((n + 1) * 1024 - 1 >= filelength) 
				SendTo(buffer, filelength - n * 1024 + 8, port, ip);
			else
				SendTo(buffer, 1024 + 8, port, ip);


			//写日志
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "S:430\\n"; out0 << n << "\t"; out0 << "C:440\t";
			out0.close();
		}
		else if (strcmp(command, "450") == 0) {  //服务器接收文件完毕
			//关闭文件
			in.close();

			//更改对话框显示状态
			dialog->m_finish.ShowWindow(SW_SHOW);
			dialog->m_ctrlpro.SetPos(10);

			//写日志
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "\nS:450\n";
			out0.close();

			//关闭套接字
			this->Close();
		}
		else {
			dialog->MessageBox(L"error 2");
		}
	}
	else {
		if (strcmp(buffer, "340") == 0) {
			//关闭计时器
			dialog->KillTimer(1);
			//写日志
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "S:340\t";
			out0.close();
			//判断是否是需要的下一个数据段
			if (*(int*)(buffer + 4) == nextnumb) {
				//根据接收文件大小更新进度条
				if (recfilesize < m_pro && (recfilesize + n - 8) >= m_pro) {
					dialog->m_ctrlpro.StepIt();
					m_pro += (filelength / 10);
				}
				//写文件
				nextnumb++;
				out.write(buffer + 8, n - 8);
				recfilesize += n - 8;
				//根据接收文件大小判断是否接收到整个文件
				if (recfilesize >= filelength) {
					//关闭写文件流
					out.close();
					//发送数据报
					//350  服务器收取文件完毕
					buffer[0] = '3'; buffer[1] = '5'; 
					buffer[2] = '0'; buffer[3] = '\n';  
					SendTo(buffer, 4, port, ip);
					//写日志
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "\nC:350\n";
					out0.close();
					//更改对话框显示状态
					dialog->m_down.ShowWindow(SW_SHOW);
					dialog->m_ctrlpro.SetPos(10);
					//关闭套接字
					this->Close();
				}
				else {//文件收取没有结束
					//继续请求下一个数据段
					//330\n下一个请求的数据段号（从0开始，每段1024字节）
					buffer[0] = '3'; buffer[1] = '3'; 
					buffer[2] = '0'; buffer[3] = '\n';  
					*(int*)(buffer + 4) = nextnumb;
					SendTo(buffer, 8, port, ip);
					//设置计时器
					dialog->SetTimer(1, 50, NULL);
					//写日志
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "C:330\\n"; out0 << nextnumb << "\t";
					out0.close();
				}
			}
			else {//没有收到预期数据段
				//继续请求当前数据段
				//430\n下一个请求的数据段号（从0开始，每段1024字节）
				buffer[0] = '3'; buffer[1] = '3'; 
				buffer[2] = '0'; buffer[3] = '\n';  
				*(int*)(buffer + 4) = nextnumb;
				SendTo(buffer, 8, port, ip);
				//设置计时器
				dialog->SetTimer(1, 50, NULL);
				//写日志
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
	//对话框控件赋值并更新
	DialogUP* dialog = (DialogUP*)dlg;
	dialog->m_name.Format(L"%S", filename.c_str());
	dialog->UpdateData(false);

	//新建将要接收的文件，发送数据报
	out.open(filename, ios::out | ios::binary | ios::trunc);
	string send = "330\n0000";
	*(int*)&(send[4]) = nextnumb;
	SendTo(send.c_str(), send.length(), port, ip);

	//设置计时器
	dialog->SetTimer(1, 50, NULL);
	
	//写日志
	ofstream out0("log.txt", ios::out | ios::app);
	out0 << "C:330\\n"; out0 << nextnumb << "\t";
	out0.close();
}


void UpSocket::sendagain()
{
	// TODO: 在此处添加实现代码.
	DialogUP* dialog = (DialogUP*)dlg;
	//再次发送需要的下一个数据段序号
	string send = "330\n0000";
	*(int*)&(send[4]) = nextnumb;
	SendTo(send.c_str(), send.length(), port, ip);
	//设置计时器
	dialog->SetTimer(1, 50, NULL);
}
