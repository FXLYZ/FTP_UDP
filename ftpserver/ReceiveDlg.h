#pragma once
#include "ReceiveSocket.h"

// ReceiveDlg 对话框

class ReceiveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ReceiveDlg)

public:
	ReceiveDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ReceiveDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGRECEIVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString srcIP;
	UINT srcPort;
	ReceiveSocket socket;
	virtual BOOL OnInitDialog();
	CString m_ip;
	int m_port;
	CString m_filename;
	bool isReceive;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
