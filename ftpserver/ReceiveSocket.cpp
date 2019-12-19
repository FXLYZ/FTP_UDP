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
	// TODO: 在此添加专用代码和/或调用基类
	ReceiveDlg* recdlg = (ReceiveDlg*)dlg;
	memset(buffer, 0, 1200);
	int n = ReceiveFrom(buffer, 1200, ip, port);
	buffer[3] = '\0';
	buffer[n] = '\0';
	if (isReceive) {
		if (strcmp(buffer, "420") == 0) {  //420\n文件大小\n文件名
			//获取文件名，文件大小
			filesize = *(int*)(buffer + 4);
			char* filename = buffer + 9;
			CString name(filename);
			//更新窗口
			recdlg->m_ip = ip;
			recdlg->m_port = port;
			recdlg->m_filename = name;
			recdlg->UpdateData(false);
			//创建文件输出流
			out.open(filename, ios::out | ios::binary | ios::trunc); //以二进制写入方式打开文件，若文件存在则先删除文件
			//发送
			memset(buffer, 0, 1200);
			buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n下一个请求的数据段号（从0开始，每段1024字节）
			*(int*)(buffer + 4) = 0;
			nextnumb = 0;
			SendTo(buffer, 8, port, ip);
			//设置计时器
			recdlg->SetTimer(1, 50, NULL);
			//写日志
			ofstream out1("log.txt", ios::out | ios::app);
			out1 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):420\\n" << filesize << "\\n" << filename << "\n";
			out1 << "S:430\\n" << nextnumb << "\n";
			out1.close();
		}
		else if (strcmp(buffer, "440") == 0) {
			//销毁计时器
			recdlg->KillTimer(1);
			//写日志
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):420\n";
			out0.close();
			//判断是否是需要的数据段
			if (*(int*)(buffer + 4) == nextnumb) {//是则写入文件
				nextnumb++;
				out.write(buffer + 8, n - 8);
				recfilesize += n - 8;
				//判断文件是否接收完毕
				if (recfilesize >= filesize) {
					//发送
					out.close();
					buffer[0] = '4'; buffer[1] = '5'; buffer[2] = '0'; buffer[3] = '\n';  //450  服务器收取文件完毕
					SendTo(buffer, 4, port, ip);
					//写日志
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "S:450\n";
					out0.close();
					//关闭套接字，关闭对话框（对话框关闭后，子线程关闭）
					this->Close();
					recdlg-> EndDialog(0);
				}
				else {
					//发送
					buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n下一个请求的数据段号（从0开始，每段1024字节）
					*(int*)(buffer + 4) = nextnumb;
					SendTo(buffer, 8, port, ip);
					//设置计时器
					recdlg->SetTimer(1, 50, NULL);
					//写日志
					ofstream out0("log.txt", ios::out | ios::app);
					out0 << "S:430\\n" << nextnumb << "\n";
					out0.close();
				}
			}
			else {//不是则丢弃数据，重新发送ACK
				//发送
				buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n下一个请求的数据段号（从0开始，每段1024字节）
				*(int*)(buffer + 4) = nextnumb;
				SendTo(buffer, 8, port, ip);
				//设置计时器
				recdlg->SetTimer(1, 50, NULL);
				//写日志
				ofstream out0("log.txt", ios::out | ios::app);
				out0 << "S:430\\n" << nextnumb << "\n";
				out0.close();
			}
		}
	}
	else {
		if (strcmp(buffer, "330") == 0) {  
			//客户端返回ACK，确定需要发送的数据报序号  
			//330\n下一个请求的数据段号（从0开始，每段1024字节）
			int n = *(int*)(buffer + 4);  //n*1024 --- (n+1)*1024-1
			//读取相应数据段数据并发送
			memset(buffer, 0, 1200);    //340\n数据段号(int)数据
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
			//写日志
			ofstream out1("log.txt", ios::out | ios::app);
			out1 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):330\\n" << n << "\n";
			out1 << "S:340\n";
			out1.close();
		}
		else if (strcmp(buffer, "350") == 0) {  //服务器接收文件完毕
			//写日志
			ofstream out0("log.txt", ios::out | ios::app);
			out0 << "C(" << ((string)(CStringA)ip.GetBuffer()).c_str() << ":" << port << "):350\n";
			out0.close();
			//保存文件，关闭套接字，关闭对话框
			in.close();
			this->Close();
			recdlg->EndDialog(0);
		}
	}
}



void ReceiveSocket::MySend()
{
	// TODO: 在此添加专用代码和/或调用基类
	//创建文件输出流
	in.open(filename, ios::in | ios::binary);
	in.seekg(0, ios::end);
	filesize = (int)in.tellg();   //获取文件大小
	//更新窗口
	ReceiveDlg* recdlg = (ReceiveDlg*)dlg;
	recdlg->m_ip = ip;
	recdlg->m_port = port;
	recdlg->m_filename.Format(L"%S",filename.c_str());
	recdlg->UpdateData(false);

	//310\n文件大小（int）\n文件名
	string send = "310\n0000\n" + filename;   
	int *a = (int*)&send[4];
	*a = filesize;
	//发送
	SendTo(send.c_str(), send.length(), port, ip);
	//写日志
	ofstream out0("log.txt", ios::out | ios::app);
	out0 << "S:310\\n" << filesize << "\\n" << filename.c_str() << ")\n";
	out0.close();
}


void ReceiveSocket::sendagain()
{
	// TODO: 在此处添加实现代码.
	//发送ACK
	buffer[0] = '4'; buffer[1] = '3'; buffer[2] = '0'; buffer[3] = '\n';  //430\n下一个请求的数据段号（从0开始，每段1024字节）
	*(int*)(buffer + 4) = nextnumb;
	SendTo(buffer, 8, port, ip);
	//设置计时器
	ReceiveDlg* recdlg = (ReceiveDlg*)dlg;
	recdlg->SetTimer(1, 50, NULL);
	//写日志
	ofstream out0("log.txt", ios::out | ios::app);
	out0 << "S:430\\n" << nextnumb << "\n";
	out0.close();

	
}
