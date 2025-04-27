
// PauseGameDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "PauseGame.h"
#include "PauseGameDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h> // �����M�̿������P�ĺ�ʽ
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
	m_PauseHotkeyCtrl.SetRules(0, HOTKEYF_CONTROL | HOTKEYF_ALT | HOTKEYF_SHIFT | HOTKEYF_EXT);  // �����ȼ��ؼ��Ĺ���
	m_Started = false;
	LoadConfig();  // ��ȡ�����ļ�

	m_StopButtonCtrl.EnableWindow(FALSE);  // ����ֹͣ��ť

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*
*	��PauseGame.ini�ж�ȡ����
*	ProcessName�ֶ� ���浽 m_ProcessEditCtrl��
*	HotKey�ֶ� ���浽 m_PauseHotkeyCtrl��
*/
void CPauseGameDlg::LoadConfig() {
	// ��ȡ�����ļ�
	CWinApp* pApp = AfxGetApp();
	CString strProcessName = pApp->GetProfileString(_T("Settings"), _T("ProcessName"), _T(""));
	CString strHotKey = pApp->GetProfileString(_T("Settings"), _T("HotKey"), _T(""));
	// ���ñ༭������
	if (strProcessName.IsEmpty()) {
		strProcessName = _T("");
	}
	m_ProcessEditCtrl.SetWindowText(strProcessName);
	// �����ȼ��ؼ�����
	if (strHotKey.IsEmpty()) {
		m_PauseHotkeyCtrl.SetHotKey(0, 0);
	}
	else {
		// ���ַ���ת��Ϊ�ȼ�
		UINT32 nHotKey = _tcstoul(strHotKey, nullptr, 16);
		// ��λ�ֽ�Ϊ���μ�����λ�ֽ�Ϊ�����
		UINT32 nModifier = (nHotKey >> 8) & 0xFF;
		UINT32 nKeyCode = nHotKey & 0xFF;
		m_PauseHotkeyCtrl.SetHotKey(nKeyCode, nModifier);
	}
}

/*
* 	�������õ�PauseGame.ini
*	m_ProcessEditCtrl�� ���浽 ProcessName�ֶ�
*	m_PauseHotkeyCtrl�� ���浽 HotKey�ֶ�
*/
void CPauseGameDlg::SaveConfig()
{
	// ���浽�����ļ�
	CWinApp* pApp = AfxGetApp();
	CString strProcessName;
	m_ProcessEditCtrl.GetWindowText(strProcessName);
	if (strProcessName.IsEmpty()) {
		strProcessName = _T("");
	}
	pApp->WriteProfileString(_T("Settings"), _T("ProcessName"), strProcessName);

	// �����ȼ��ؼ�����
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
	// ��ȡ�༭������
	CString strProcessName;
	m_ProcessEditCtrl.GetWindowText(strProcessName);
	if (strProcessName.IsEmpty()) {
		m_AppInfoCtrl.SetWindowTextW(_T("�����������"));
		return;
	}

	// ��ȡ�ȼ�
	UINT32 nHotKey = m_PauseHotkeyCtrl.GetHotKey();
	if (nHotKey == 0) {
		m_AppInfoCtrl.SetWindowTextW(_T("�������ȼ�"));
		return;
	}

	// ��������
	SaveConfig();

	// ��ȡ��λ�ֽں͵�λ�ֽ�
	UINT32 nModifier = (nHotKey >> 8) & 0xFF;
	UINT32 nKeyCode = nHotKey & 0xFF;

	if ((nModifier & (MOD_CONTROL | MOD_ALT | MOD_SHIFT)) == 0 && (nModifier & MOD_WIN))
	{
		// û�� Ctrl/Alt/Shift��ֻ��Win��������η�
		nModifier = 0;
	}

	if (nKeyCode == 0) {
		m_AppInfoCtrl.SetWindowTextW(_T("û��������Ч�ȼ�"));
		return;
	}

	// ע��ȫ���ȼ�
	if (!RegisterHotKey(m_hWnd, m_nHotKeyId, nModifier, nKeyCode)) {
		m_AppInfoCtrl.SetWindowTextW(_T("ע���ȼ�ʧ��"));
		return;
	}
	m_Paused = false;
	m_Started = true; // ������������־
	m_nKeyCode = nKeyCode; // �����ȼ��������
	m_nModifier = nModifier; // �����ȼ����μ�
	m_strProcessName = strProcessName; // ���������

	// ����ֹͣ��ť
	m_StopButtonCtrl.EnableWindow(TRUE);
	// ���ÿ�ʼ��ť
	m_StartButtonCtrl.EnableWindow(FALSE);
	// �����ȼ��ؼ�
	m_PauseHotkeyCtrl.EnableWindow(FALSE);
	// ���ñ༭��
	m_ProcessEditCtrl.EnableWindow(FALSE);

	m_AppInfoCtrl.SetWindowTextW(_T("�ȼ���ע�ᣬ�����ȼ���ͣ��Ϸ"));
}

void CPauseGameDlg::OnBnClickedStopButton()
{
	// TODO: Add your control notification handler code here
	// ����ֹͣ��ť
	m_StopButtonCtrl.EnableWindow(FALSE);
	// ���ÿ�ʼ��ť
	m_StartButtonCtrl.EnableWindow(TRUE);
	// �����ȼ��ؼ�
	m_PauseHotkeyCtrl.EnableWindow(TRUE);
	// ���ñ༭��
	m_ProcessEditCtrl.EnableWindow(TRUE);

	// ȡ��ע���ȼ�
	UnregisterHotKey(m_hWnd, m_nHotKeyId);

	m_Started = false; // ����δ������־

	if (m_Paused) {
		// �����ǰ������ͣ״̬���ָ�����
		ResumeProcess();
		m_Paused = false;
	}
}

// �������Ʋ��ҽ���
bool CPauseGameDlg::PauseProcess() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		m_AppInfoCtrl.SetWindowTextW(_T("�o�������M�̿��գ�"));
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
			// ���^�M�����Q�����^�ִ�С��
			if (strCurrentProcessName.CompareNoCase(m_strProcessName) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					// ��ͣ�M���е����Ј��оw
					NTSTATUS(NTAPI * NtSuspendProcess)(IN HANDLE ProcessHandle) =
						(NTSTATUS(NTAPI*)(IN HANDLE))GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtSuspendProcess");

					if (NtSuspendProcess != NULL)
					{
						NTSTATUS status = NtSuspendProcess(hProcess);
						if (status == 0) // STATUS_SUCCESS
						{
							CString strMessage;
							strMessage.Format(_T("�ѳɹ���ͣ�M��: %s (PID: %u)"), strCurrentProcessName, pe32.th32ProcessID);
							m_AppInfoCtrl.SetWindowText(strMessage);
							bFound = TRUE;
						}
						else
						{
							CString strError;
							strError.Format(_T("��ͣ�M�� %s ʧ�����e�`�a: 0x%X"), strCurrentProcessName, status);
							m_AppInfoCtrl.SetWindowText(strError);
						}
					}
					else
					{
						m_AppInfoCtrl.SetWindowText(_T("�o���ҵ� NtSuspendProcess ������"));
					}
					CloseHandle(hProcess);
				}
				else
				{
					CString strError;
					strError.Format(_T("�o�����_�M�� %s (PID: %u)�����ܛ]�����ę��ޣ��e�`�a: %d"), strCurrentProcessName, pe32.th32ProcessID, GetLastError());
					m_AppInfoCtrl.SetWindowText(strError);
				}
				break; // �ҵ�Ŀ���M����ֹͣ�ь� (���Oֻ��ͣ��һ��ƥ����M��)
			}
		} while (Process32Next(hSnapshot, &pe32));
	}
	else
	{
		m_AppInfoCtrl.SetWindowText(_T("�o����v�M���б�"));
	}

	CloseHandle(hSnapshot);

	if (!bFound) {
		CString strError;
		strError.Format(_T("δ�ҵ��M��: %s"), m_strProcessName);
		m_AppInfoCtrl.SetWindowText(strError);
	}

	return bFound;
}

bool CPauseGameDlg::ResumeProcess()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		m_AppInfoCtrl.SetWindowTextW(_T("�o�������M�̿��գ�"));
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
			// ���^�M�����Q�����^�ִ�С��
			if (strCurrentProcessName.CompareNoCase(m_strProcessName) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					// �֏��M���е����Ј��оw
					NTSTATUS(NTAPI * NtResumeProcess)(IN HANDLE ProcessHandle) =
						(NTSTATUS(NTAPI*)(IN HANDLE))GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtResumeProcess");

					if (NtResumeProcess != NULL)
					{
						NTSTATUS status = NtResumeProcess(hProcess);
						if (status == 0) // STATUS_SUCCESS
						{
							CString strMessage;
							strMessage.Format(_T("�ѳɹ��֏��M��: %s (PID: %u)"), strCurrentProcessName, pe32.th32ProcessID);
							m_AppInfoCtrl.SetWindowText(strMessage);
							bFound = TRUE;
							CloseHandle(hProcess); // �ɹ��֏����P�]�M�̾��
							break; // �ҵ��K�֏���ֹͣ�ь� (���Oֻ�֏͵�һ��ƥ����M��)
						}
						else
						{
							CString strError;
							strError.Format(_T("�֏��M�� %s ʧ�����e�`�a: 0x%X"), strCurrentProcessName, status);
							m_AppInfoCtrl.SetWindowText(strError);
							CloseHandle(hProcess); // �֏�ʧ�����P�]�M�̾��
							break;
						}
					}
					else
					{
						m_AppInfoCtrl.SetWindowText(_T("�o���ҵ� NtResumeProcess ������"));
						CloseHandle(hProcess); // �o���ҵ������r�P�]�M�̾��
						break;
					}
				}
				else
				{
					CString strError;
					strError.Format(_T("�o�����_�M�� %s (PID: %u)�����ܛ]�����ę��ޣ��e�`�a: %d"), strCurrentProcessName, pe32.th32ProcessID, GetLastError());
					m_AppInfoCtrl.SetWindowText(strError);
					break;
				}
			}
		} while (Process32Next(hSnapshot, &pe32));
	}
	else
	{
		m_AppInfoCtrl.SetWindowText(_T("�o����v�M���б�"));
	}

	CloseHandle(hSnapshot);

	if (!bFound)
	{
		CString strError;
		strError.Format(_T("δ�ҵ��M��: %s"), m_strProcessName);
		m_AppInfoCtrl.SetWindowText(strError);
	}

	return bFound; // �����Ƿ�ɹ��ҵ��K�֏��M��
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
		OnBnClickedStopButton();  // ����ֹͣ��ť�Ĵ�����
	}

	CDialogEx::OnClose();
}

BOOL CPauseGameDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN) {
		// ����������ȼ��ؼ��ϣ�����
		HWND hFocus = ::GetFocus();
		if (hFocus == m_PauseHotkeyCtrl.GetSafeHwnd()) {
			// �ȼ��ؼ������н��㣬�����أ���������
			return FALSE;
		}

		// ������µ��� ESC ����س�������ֹĬ����Ϊ
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) {
			return TRUE;  // �����ϢΪ�Ѵ�����Ϣ���ᴫ�ݸ��ؼ��򴰿ڵ�Ĭ�ϴ���
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
