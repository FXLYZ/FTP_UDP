
// ftpclientDlg.h: 头文件
//

#pragma once
#include "MySocket.h"


// CftpclientDlg 对话框
class CftpclientDlg : public CDialogEx
{
// 构造
public:
	CftpclientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTPCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	MySocket socket;
	afx_msg void OnBnClickedButton();
	CComboBox m_ctrlcommand;
	CString desIP;
	CListBox m_ctrldir;
	CIPAddressCtrl ServerIP;
};
