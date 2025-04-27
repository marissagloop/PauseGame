
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
	UINT32 m_nHotKeyId; // �ȼ�ID
	bool m_Paused; // �Ƿ���ͣ
	bool m_Started; // �Ƿ�������
	UINT32 m_nKeyCode; // �ȼ��������
	UINT32 m_nModifier; // �ȼ����μ�
	CString m_strProcessName; // ������

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
	// ��ͣ�ȼ��ؼ�
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
