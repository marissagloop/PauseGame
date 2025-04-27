
// PauseGameDlg.h : header file
//

#pragma once


// CPauseGameDlg dialog
class CPauseGameDlg : public CDialogEx
{
// Construction
public:
	CPauseGameDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAUSEGAME_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	UINT32 m_nHotKeyId; // 热键ID
	bool m_Paused; // 是否暂停
	bool m_Started; // 是否已启动
	UINT32 m_nKeyCode; // 热键虚拟键码
	UINT32 m_nModifier; // 热键修饰键
	CString m_strProcessName; // 进程名

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void LoadConfig();
	void SaveConfig();

	bool PauseProcess();
	bool ResumeProcess();
public:
	// 暂停热键控件
	CHotKeyCtrl m_PauseHotkeyCtrl;
	CEdit m_ProcessEditCtrl;
	afx_msg void OnBnClickedStartButton();
	afx_msg void OnBnClickedStopButton();
	CButton m_StartButtonCtrl;
	CButton m_StopButtonCtrl;
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	CStatic m_AppInfoCtrl;
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
