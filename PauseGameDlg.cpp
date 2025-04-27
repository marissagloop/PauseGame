
// PauseGameDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "PauseGame.h"
#include "PauseGameDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h> // 包含進程快照相關的函式
#include <ntstatus.h>
#include <winternl.h> // Include winternl.h for NtSuspendProcess declaration  

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPauseGameDlg dialog



CPauseGameDlg::CPauseGameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PAUSEGAME_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nHotKeyId = 1;
}

void CPauseGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PAUSE_HOTKEY, m_PauseHotkeyCtrl);
	DDX_Control(pDX, IDC_PROCESS_EDIT, m_ProcessEditCtrl);
	DDX_Control(pDX, IDC_START_BUTTON, m_StartButtonCtrl);
	DDX_Control(pDX, IDC_STOP_BUTTON, m_StopButtonCtrl);
	DDX_Control(pDX, IDC_APP_INFO, m_AppInfoCtrl);
}

BEGIN_MESSAGE_MAP(CPauseGameDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_BUTTON, &CPauseGameDlg::OnBnClickedStartButton)
	ON_BN_CLICKED(IDC_STOP_BUTTON, &CPauseGameDlg::OnBnClickedStopButton)
	ON_WM_HOTKEY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPauseGameDlg message handlers

BOOL CPauseGameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_PauseHotkeyCtrl.SetRules(0, HOTKEYF_CONTROL | HOTKEYF_ALT | HOTKEYF_SHIFT | HOTKEYF_EXT);  // 设置热键控件的规则
	m_Started = false;
	LoadConfig();  // 读取配置文件

	m_StopButtonCtrl.EnableWindow(FALSE);  // 禁用停止按钮

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*
*	从PauseGame.ini中读取配置
*	ProcessName字段 保存到 m_ProcessEditCtrl中
*	HotKey字段 保存到 m_PauseHotkeyCtrl中
*/
void CPauseGameDlg::LoadConfig() {
	// 读取配置文件
	CWinApp* pApp = AfxGetApp();
	CString strProcessName = pApp->GetProfileString(_T("Settings"), _T("ProcessName"), _T(""));
	CString strHotKey = pApp->GetProfileString(_T("Settings"), _T("HotKey"), _T(""));
	// 设置编辑框内容
	if (strProcessName.IsEmpty()) {
		strProcessName = _T("");
	}
	m_ProcessEditCtrl.SetWindowText(strProcessName);
	// 设置热键控件内容
	if (strHotKey.IsEmpty()) {
		m_PauseHotkeyCtrl.SetHotKey(0, 0);
	}
	else {
		// 将字符串转换为热键
		UINT32 nHotKey = _tcstoul(strHotKey, nullptr, 16);
		// 高位字节为修饰键，低位字节为虚拟键
		UINT32 nModifier = (nHotKey >> 8) & 0xFF;
		UINT32 nKeyCode = nHotKey & 0xFF;
		m_PauseHotkeyCtrl.SetHotKey(nKeyCode, nModifier);
	}
}

/*
* 	保存配置到PauseGame.ini
*	m_ProcessEditCtrl中 保存到 ProcessName字段
*	m_PauseHotkeyCtrl中 保存到 HotKey字段
*/
void CPauseGameDlg::SaveConfig()
{
	// 保存到配置文件
	CWinApp* pApp = AfxGetApp();
	CString strProcessName;
	m_ProcessEditCtrl.GetWindowText(strProcessName);
	if (strProcessName.IsEmpty()) {
		strProcessName = _T("");
	}
	pApp->WriteProfileString(_T("Settings"), _T("ProcessName"), strProcessName);

	// 保存热键控件内容
	UINT32 nHotKey = m_PauseHotkeyCtrl.GetHotKey();

	CString strHotKey;
	strHotKey.Format(_T("%08X"), nHotKey);
	pApp->WriteProfileString(_T("Settings"), _T("HotKey"), strHotKey);
}

void CPauseGameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPauseGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPauseGameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPauseGameDlg::OnBnClickedStartButton()
{
	// TODO: Add your control notification handler code here
	// 获取编辑框内容
	CString strProcessName;
	m_ProcessEditCtrl.GetWindowText(strProcessName);
	if (strProcessName.IsEmpty()) {
		m_AppInfoCtrl.SetWindowTextW(_T("请输入进程名"));
		return;
	}

	// 获取热键
	UINT32 nHotKey = m_PauseHotkeyCtrl.GetHotKey();
	if (nHotKey == 0) {
		m_AppInfoCtrl.SetWindowTextW(_T("请设置热键"));
		return;
	}

	// 保存配置
	SaveConfig();

	// 获取高位字节和低位字节
	UINT32 nModifier = (nHotKey >> 8) & 0xFF;
	UINT32 nKeyCode = nHotKey & 0xFF;

	if ((nModifier & (MOD_CONTROL | MOD_ALT | MOD_SHIFT)) == 0 && (nModifier & MOD_WIN))
	{
		// 没有 Ctrl/Alt/Shift，只有Win，清掉修饰符
		nModifier = 0;
	}

	if (nKeyCode == 0) {
		m_AppInfoCtrl.SetWindowTextW(_T("没有设置有效热键"));
		return;
	}

	// 注册全局热键
	if (!RegisterHotKey(m_hWnd, m_nHotKeyId, nModifier, nKeyCode)) {
		m_AppInfoCtrl.SetWindowTextW(_T("注册热键失败"));
		return;
	}
	m_Paused = false;
	m_Started = true; // 设置已启动标志
	m_nKeyCode = nKeyCode; // 保存热键虚拟键码
	m_nModifier = nModifier; // 保存热键修饰键
	m_strProcessName = strProcessName; // 保存进程名

	// 启用停止按钮
	m_StopButtonCtrl.EnableWindow(TRUE);
	// 禁用开始按钮
	m_StartButtonCtrl.EnableWindow(FALSE);
	// 禁用热键控件
	m_PauseHotkeyCtrl.EnableWindow(FALSE);
	// 禁用编辑框
	m_ProcessEditCtrl.EnableWindow(FALSE);

	m_AppInfoCtrl.SetWindowTextW(_T("热键已注册，按下热键暂停游戏"));
}

void CPauseGameDlg::OnBnClickedStopButton()
{
	// TODO: Add your control notification handler code here
	// 禁用停止按钮
	m_StopButtonCtrl.EnableWindow(FALSE);
	// 启用开始按钮
	m_StartButtonCtrl.EnableWindow(TRUE);
	// 启用热键控件
	m_PauseHotkeyCtrl.EnableWindow(TRUE);
	// 启用编辑框
	m_ProcessEditCtrl.EnableWindow(TRUE);

	// 取消注册热键
	UnregisterHotKey(m_hWnd, m_nHotKeyId);

	m_Started = false; // 设置未启动标志

	if (m_Paused) {
		// 如果当前处于暂停状态，恢复进程
		ResumeProcess();
		m_Paused = false;
	}
}

// 根据名称查找进程
bool CPauseGameDlg::PauseProcess() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		m_AppInfoCtrl.SetWindowTextW(_T("無法創建進程快照！"));
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bFound = FALSE;
	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			CString strCurrentProcessName(pe32.szExeFile);
			// 比較進程名稱，不區分大小寫
			if (strCurrentProcessName.CompareNoCase(m_strProcessName) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					// 暫停進程中的所有執行緒
					NTSTATUS(NTAPI * NtSuspendProcess)(IN HANDLE ProcessHandle) =
						(NTSTATUS(NTAPI*)(IN HANDLE))GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtSuspendProcess");

					if (NtSuspendProcess != NULL)
					{
						NTSTATUS status = NtSuspendProcess(hProcess);
						if (status == 0) // STATUS_SUCCESS
						{
							CString strMessage;
							strMessage.Format(_T("已成功暫停進程: %s (PID: %u)"), strCurrentProcessName, pe32.th32ProcessID);
							m_AppInfoCtrl.SetWindowText(strMessage);
							bFound = TRUE;
						}
						else
						{
							CString strError;
							strError.Format(_T("暫停進程 %s 失敗！錯誤碼: 0x%X"), strCurrentProcessName, status);
							m_AppInfoCtrl.SetWindowText(strError);
						}
					}
					else
					{
						m_AppInfoCtrl.SetWindowText(_T("無法找到 NtSuspendProcess 函數！"));
					}
					CloseHandle(hProcess);
				}
				else
				{
					CString strError;
					strError.Format(_T("無法打開進程 %s (PID: %u)，可能沒有足夠的權限！錯誤碼: %d"), strCurrentProcessName, pe32.th32ProcessID, GetLastError());
					m_AppInfoCtrl.SetWindowText(strError);
				}
				break; // 找到目標進程後停止搜尋 (假設只暫停第一個匹配的進程)
			}
		} while (Process32Next(hSnapshot, &pe32));
	}
	else
	{
		m_AppInfoCtrl.SetWindowText(_T("無法遍歷進程列表！"));
	}

	CloseHandle(hSnapshot);

	if (!bFound) {
		CString strError;
		strError.Format(_T("未找到進程: %s"), m_strProcessName);
		m_AppInfoCtrl.SetWindowText(strError);
	}

	return bFound;
}

bool CPauseGameDlg::ResumeProcess()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		m_AppInfoCtrl.SetWindowTextW(_T("無法創建進程快照！"));
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bFound = FALSE;
	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			CString strCurrentProcessName(pe32.szExeFile);
			// 比較進程名稱，不區分大小寫
			if (strCurrentProcessName.CompareNoCase(m_strProcessName) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					// 恢復進程中的所有執行緒
					NTSTATUS(NTAPI * NtResumeProcess)(IN HANDLE ProcessHandle) =
						(NTSTATUS(NTAPI*)(IN HANDLE))GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtResumeProcess");

					if (NtResumeProcess != NULL)
					{
						NTSTATUS status = NtResumeProcess(hProcess);
						if (status == 0) // STATUS_SUCCESS
						{
							CString strMessage;
							strMessage.Format(_T("已成功恢復進程: %s (PID: %u)"), strCurrentProcessName, pe32.th32ProcessID);
							m_AppInfoCtrl.SetWindowText(strMessage);
							bFound = TRUE;
							CloseHandle(hProcess); // 成功恢復後關閉進程句柄
							break; // 找到並恢復後停止搜尋 (假設只恢復第一個匹配的進程)
						}
						else
						{
							CString strError;
							strError.Format(_T("恢復進程 %s 失敗！錯誤碼: 0x%X"), strCurrentProcessName, status);
							m_AppInfoCtrl.SetWindowText(strError);
							CloseHandle(hProcess); // 恢復失敗後關閉進程句柄
							break;
						}
					}
					else
					{
						m_AppInfoCtrl.SetWindowText(_T("無法找到 NtResumeProcess 函數！"));
						CloseHandle(hProcess); // 無法找到函數時關閉進程句柄
						break;
					}
				}
				else
				{
					CString strError;
					strError.Format(_T("無法打開進程 %s (PID: %u)，可能沒有足夠的權限！錯誤碼: %d"), strCurrentProcessName, pe32.th32ProcessID, GetLastError());
					m_AppInfoCtrl.SetWindowText(strError);
					break;
				}
			}
		} while (Process32Next(hSnapshot, &pe32));
	}
	else
	{
		m_AppInfoCtrl.SetWindowText(_T("無法遍歷進程列表！"));
	}

	CloseHandle(hSnapshot);

	if (!bFound)
	{
		CString strError;
		strError.Format(_T("未找到進程: %s"), m_strProcessName);
		m_AppInfoCtrl.SetWindowText(strError);
	}

	return bFound; // 返回是否成功找到並恢復進程
}

void CPauseGameDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default
	if (nHotKeyId == m_nHotKeyId) {
		CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);

		if (m_Paused) {
			if (!ResumeProcess()) {
				return;
			}
			m_Paused = false;
		}
		else {
			if (!PauseProcess()) {
				return;
			}
			m_Paused = true;
		}

		return;
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

void CPauseGameDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_Started) {
		OnBnClickedStopButton();  // 调用停止按钮的处理函数
	}

	CDialogEx::OnClose();
}

BOOL CPauseGameDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN) {
		// 如果焦点在热键控件上，放行
		HWND hFocus = ::GetFocus();
		if (hFocus == m_PauseHotkeyCtrl.GetSafeHwnd()) {
			// 热键控件正在有焦点，不拦截，让它接收
			return FALSE;
		}

		// 如果按下的是 ESC 键或回车键，阻止默认行为
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) {
			return TRUE;  // 标记消息为已处理，消息不会传递给控件或窗口的默认处理
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
