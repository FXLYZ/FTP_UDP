#pragma once
#include "UpSocket.h"

// DialogUP 对话框

class DialogUP : public CDialogEx
{
	DECLARE_DYNAMIC(DialogUP)

public:
	DialogUP(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DialogUP();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString desIP;
	UINT desPort;
	virtual BOOL OnInitDialog();
//	CString m_ip;
//	UINT m_port;
	CString m_name;
	CString m_path;
	UpSocket socket;
	CStatic m_finish;
	bool isUP;
	CStatic m_down;
	CProgressCtrl m_ctrlpro;
//	int m_pro;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
