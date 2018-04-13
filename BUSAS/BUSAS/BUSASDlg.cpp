
// BUSASDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BUSAS.h"
#include "BUSASDlg.h"
#include "afxdialogex.h"
#include "MyCommFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinThread *g_pThread = NULL; //�߳�ָ��
CWinThread *g_pThreadshow = NULL; //�߳�ָ�� //�߳�ָ��

BOOL g_threadflag = FALSE;
BOOL g_pause = FALSE;
BOOL g_showtype = FALSE;
CGroup<LISTENBUS> g_listendata;

BOOL g_clearglisten = FALSE;
BOOL g_threadshowisstop = FALSE;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBUSASDlg �Ի���



CBUSASDlg::CBUSASDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBUSASDlg::IDD, pParent)
	, m_bytetime(5)
	, m_frametime(35)
	, m_waitanstime(55)
	, m_ecutimeouts(500)
	, m_edit_idlist(_T(""))
	, m_edit_masklist(_T(""))
	, m_richedit_str(_T(""))
	, m_busframetime(50)
	, m_bustimes(1)
	, m_strmonifile(_T("ReqAns.txt"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBUSASDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SERIL, m_combo_serial);
	DDX_Control(pDX, IDC_COMBO_PATTERN, m_combo_pettern);
	DDX_Control(pDX, IDC_COMBO_PROTOCOL, m_combo_potocol);
	DDX_Control(pDX, IDC_COMBO_BRATE, m_combo_baute);
	DDX_Text(pDX, IDC_EDIT_IDLIST, m_edit_idlist);
	DDX_Text(pDX, IDC_EDIT_MASKLIST, m_edit_masklist);
	DDX_Text(pDX, IDC_EDIT_BYTETIME, m_bytetime);
	DDX_Text(pDX, IDC_EDIT_FRAMETIME, m_frametime);
	DDX_Text(pDX, IDC_EDIT_ANSTIME, m_waitanstime);
	DDX_Text(pDX, IDC_EDIT_OUTTIME, m_ecutimeouts);
	DDX_Control(pDX, IDC_LIST_SHOW, m_listctrl_show);
	DDX_Control(pDX, IDC_LIST_ANS, m_listctrl_ans);
	DDX_Text(pDX, IDC_RICHEDIT21, m_richedit_str);
	DDX_Text(pDX, IDC_EDIT7, m_busframetime);
	DDX_Text(pDX, IDC_EDIT8, m_bustimes);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlprogress);
}

BEGIN_MESSAGE_MAP(CBUSASDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ONOFF, &CBUSASDlg::OnBnClickedButtonOnoff)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CBUSASDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CBUSASDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_SHOW, &CBUSASDlg::OnBnClickedButtonChangeShow)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SHOW, &CBUSASDlg::OnCustomdrawListShow)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CBUSASDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_MONIFILE, &CBUSASDlg::OnBnClickedButtonMonifile)
	ON_BN_CLICKED(IDC_BUTTON_FFMONI, &CBUSASDlg::OnBnClickedButtonFfmoni)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_ANS, &CBUSASDlg::OnCustomdrawListAns)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ANS, &CBUSASDlg::OnClickListAns)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SHOW, &CBUSASDlg::OnRclickListShow)
	ON_COMMAND(ID_32771, &CBUSASDlg::On32771)
	ON_CBN_SELCHANGE(IDC_COMBO_PROTOCOL, &CBUSASDlg::OnSelchangeComboProtocol)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CBUSASDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_BUSCLR, &CBUSASDlg::OnBnClickedButtonBusclr)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CBUSASDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CBUSASDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_UPDATA, &CBUSASDlg::OnBnClickedButtonUpdata)
END_MESSAGE_MAP()


// CBUSASDlg ��Ϣ�������

BOOL CBUSASDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	Enum_Serial_Port();//�г�����

	m_combo_pettern.AddString(_T("�ɼ�"));
	m_combo_pettern.AddString(_T("ģ��"));
	m_combo_pettern.SetCurSel(0);


	m_combo_potocol.AddString(_T("CAN1"));
	m_combo_potocol.AddString(_T("CAN2"));
	m_combo_potocol.AddString(_T("KLINE"));
	m_combo_potocol.SetCurSel(0);

	m_combo_baute.AddString(_T("500K"));
	m_combo_baute.AddString(_T("250K"));
	m_combo_baute.AddString(_T("125K"));
	m_combo_baute.AddString(_T("100K"));
	m_combo_baute.AddString(_T("1M"));

	m_combo_baute.AddString(_T("83.3K"));
	m_combo_baute.AddString(_T("50K"));
	//m_combo_baute.AddString(_T("33.3K"));
	m_combo_baute.AddString(_T("10400"));
	m_combo_baute.AddString(_T("9600"));
	m_combo_baute.SetCurSel(0);

	CRect rect;
	// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
	m_listctrl_show.GetClientRect(&rect);

	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
	m_listctrl_show.SetExtendedStyle(m_listctrl_show.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Ϊ�б���ͼ�ؼ��������   
	m_listctrl_show.InsertColumn(0, _T("Time"), LVCFMT_LEFT, rect.Width() / 5);
	m_listctrl_show.InsertColumn(1, _T("ID"), LVCFMT_CENTER, rect.Width() / 5);
	m_listctrl_show.InsertColumn(2, _T("D0"), LVCFMT_CENTER, rect.Width() * 3 / 40);
	m_listctrl_show.InsertColumn(3, _T("D1"), LVCFMT_CENTER, rect.Width() * 3 / 40);
	m_listctrl_show.InsertColumn(4, _T("D2"), LVCFMT_CENTER, rect.Width() * 3 / 40);
	m_listctrl_show.InsertColumn(5, _T("D3"), LVCFMT_CENTER, rect.Width() * 3 / 40);
	m_listctrl_show.InsertColumn(6, _T("D4"), LVCFMT_CENTER, rect.Width() * 3 / 40);
	m_listctrl_show.InsertColumn(7, _T("D5"), LVCFMT_CENTER, rect.Width() * 3 / 40);
	m_listctrl_show.InsertColumn(8, _T("D6"), LVCFMT_CENTER, rect.Width() * 3 / 40);
	m_listctrl_show.InsertColumn(9, _T("D7"), LVCFMT_CENTER, rect.Width() * 3 / 40);

	m_listctrl_ans.SetExtendedStyle(m_listctrl_ans.GetExtendedStyle());
	m_listctrl_ans.InsertColumn(0, _T("Type"), LVCFMT_CENTER, 60);
	for (int i = 0; i != 255; i++)
	{
		CString strtemp;
		strtemp.Format(_T("%02X"), i + 1);
		m_listctrl_ans.InsertColumn(i + 1, strtemp, LVCFMT_CENTER, 28);
	}
	m_ctlprogress.ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->EnableWindow(FALSE);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CBUSASDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBUSASDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBUSASDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CBUSASDlg::Enum_Serial_Port() //ö�ٴ���
{
	HKEY hKey;
	LPCTSTR data_Set = _T("HARDWARE\\DEVICEMAP\\SERIALCOMM\\");
	LONG ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey);
	if (ret0 != ERROR_SUCCESS)
	{
		return FALSE;
	}

	DWORD dwIndex = 0;
	while (1)
	{
		LONG Status;
		TCHAR Name[256] = { 0 };
		UCHAR szPortName[80] = { 0 };
		DWORD dwName;
		DWORD dwSizeofPortName;
		DWORD Type;
		dwName = sizeof(Name);
		dwSizeofPortName = sizeof(szPortName);
		Status = RegEnumValue(hKey, dwIndex++, Name, &dwName, NULL, &Type,
			szPortName, &dwSizeofPortName);

		if ((Status == ERROR_SUCCESS) || (Status == ERROR_MORE_DATA))
		{
			CString str;
			str.Format(_T("%s"), szPortName);
			m_combo_serial.AddString(str);
		}
		else
		{
			break;
		}
	}
	RegCloseKey(hKey);
	m_combo_serial.SetCurSel(m_combo_serial.GetCount() - 1);
	return(m_combo_serial.GetCount() ? TRUE : FALSE);
}



void CBUSASDlg::OnBnClickedButtonOnoff()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString stronoff = _T("");
	GetDlgItemText(IDC_BUTTON_ONOFF, stronoff);
	if (stronoff == _T("�򿪴���"))
	{
		GetDlgItem(IDC_BUTTON_ONOFF)->EnableWindow(FALSE);
		CString strselect;
		BYTE siport = 0;
		GetDlgItemText(IDC_COMBO_SERIL, strselect);
		if (strselect.Find(_T("COM")) == 0)
		{
			strselect.Delete(0, 3);
			siport = _ttoi(strselect);
		}
		if (!m_currport.InitSerial(siport))
		{
			MessageBox(_T("���ڴ�ʧ��,���鴮���Ƿ�ռ��!"));
			GetDlgItem(IDC_BUTTON_ONOFF)->SetWindowTextW(_T("�򿪴���"));
			GetDlgItem(IDC_BUTTON_ONOFF)->EnableWindow(TRUE);
			return;
		}
		//��ʼ����λ��
		if (InitComBox())
		{
			//����ͨѶ�߳�
			g_threadflag = TRUE;
			g_pThread = AfxBeginThread(MyThread, 0, 0, 0, 0, NULL);
			CString strstatus = _T("");
			GetDlgItemText(IDC_BUTTON_CHANGE_SHOW, strstatus);
			GetDlgItem(IDC_BUTTON_ONOFF)->SetWindowTextW(_T("�رմ���"));
			GetDlgItem(IDC_BUTTON_ONOFF)->EnableWindow(TRUE);
		}
		else
		{
			//if (IDOK == MessageBox(_T("���ȷ����ť������λ��BIN�ļ�,������������Y07����"), _T("��λ������Ӧ��"), MB_OKCANCEL))
			//{
			//	//m_ctlprogress.ShowWindow(TRUE);
			//	//UpDataBinFile();
			//	//m_ctlprogress.ShowWindow(FALSE);
			//}
			MessageBox(_T("û�м�⵽�̼���"), _T("��λ������Ӧ��"));
			GetDlgItem(IDC_BUTTON_ONOFF)->EnableWindow(TRUE);
			m_currport.CloseComPort();
		}
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->EnableWindow(TRUE);
	}
	else
	{
		//��ֹ�̹߳رմ���
		g_threadflag = FALSE;
		TerminateThread(g_pThread->m_hThread, 0);
		GetDlgItem(IDC_BUTTON_ONOFF)->EnableWindow(FALSE);
		WaitForSingleObject(g_pThread->m_hThread, INFINITE);
		g_pThread = NULL;
		if (g_pThreadshow != NULL)
		{
			TerminateThread(g_pThreadshow->m_hThread, 0);
			WaitForSingleObject(g_pThreadshow->m_hThread, INFINITE);
			g_pThreadshow = NULL;
		}
		GetDlgItem(IDC_BUTTON_ONOFF)->SetWindowTextW(_T("�򿪴���"));
		GetDlgItem(IDC_BUTTON_ONOFF)->EnableWindow(TRUE);
		m_currport.CloseComPort();
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->EnableWindow(FALSE);
	}
}

BOOL CBUSASDlg::InitComBox()
{
	BYTE setcmd[250] = { 0 };
	BYTE cmdlen = 0;
	UpdateData(TRUE);
	g_listendata.Empty();
	g_showtype = FALSE;
	//���ù���ģʽ
	if ((*(CComboBox*)GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 0){
		setcmd[0] = 0xAA;
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->SetWindowTextW(_T("������ʾ"));
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->EnableWindow(TRUE);
		SetWindowText(_T("ZXT-simulator �ɼ�"));
		
	}
	else{
		setcmd[0] = 0xBB;
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->SetWindowTextW(_T("������ʾ"));
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->EnableWindow(FALSE);
		//�������ģ���ļ�����
		CString strtitle = _T("ZXT-simulator  ģ���ļ�:");
		strtitle += m_strmonifile;
		SetWindowText(strtitle);
		LoadReqReq();
	}

	//����ͨ��
	setcmd[1] = (*(CComboBox*)GetDlgItem(IDC_COMBO_PROTOCOL)).GetCurSel();

	//���ò�����
	setcmd[2] = (*(CComboBox*)GetDlgItem(IDC_COMBO_BRATE)).GetCurSel();

	//����ʱ��
	setcmd[3] = m_bytetime;
	setcmd[4] = m_frametime;
	setcmd[5] = m_waitanstime;
	setcmd[6] = m_ecutimeouts;
	setcmd[7] = m_ecutimeouts >> 8;

	//����CAN�˲���
	BYTE fitlternum = 0;
	BYTE masknum = 0;
	DWORD idlist[14] = { 0 };
	DWORD masklist[14];
	memset(masklist, 0xFF, 14 * 4);
	//����������ID
	CString str_tempidlist = m_edit_idlist;
	if (str_tempidlist.GetLength() > 0 && str_tempidlist.GetAt(str_tempidlist.GetLength() - 1) != ',')
		str_tempidlist.Append(_T(","));
	while (str_tempidlist.Find(_T(",")) != -1)
	{
		CString stridtemp = str_tempidlist.Left(str_tempidlist.Find(_T(",")));
		int ll = str_tempidlist.Find(_T(",") + 1);
		str_tempidlist.Delete(0, str_tempidlist.Find(_T(",")) + 1);
		sscanf(TCHAR2char(stridtemp.GetBuffer(0)), "%x", &idlist[fitlternum++]);
	}
	setcmd[8] = fitlternum;
	//������MASK
	str_tempidlist = m_edit_masklist;
	if (str_tempidlist.GetLength() > 0 && str_tempidlist.GetAt(str_tempidlist.GetLength() - 1) != ',')
		str_tempidlist.Append(_T(","));
	while (str_tempidlist.Find(_T(",")) != -1)
	{
		CString stridtemp = str_tempidlist.Left(str_tempidlist.Find(_T(",")));
		str_tempidlist.Delete(0, str_tempidlist.Find(_T(",")) + 1);
		sscanf(TCHAR2char(stridtemp.GetBuffer(0)), "%x", &masklist[masknum++]);
	}
	for (int i = 0; i < fitlternum; i++)
	{
		memcpy(&setcmd[9 + i * 8], &idlist[i], 4);
		memcpy(&setcmd[13 + i * 8], &masklist[i], 4);
	}
	cmdlen = 9 + fitlternum * 8;

	//������������
	m_currport.SendDataWithPro(setcmd, cmdlen);
	BYTE pbuff[30] = "";
	int num = m_currport.RecvDataWithPro(pbuff);
	if (num > 0);// && pbuff[0] == 0x55)
		return TRUE;
    return FALSE;
}
void CBUSASDlg::LoadReqReq()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char *pcha = TCHARtoCHAR(m_strmonifile.GetBuffer());
	string filname = pcha;
	delete[]pcha;
	CString faultmsg = _T("��ģ���ļ� ") + m_strmonifile + _T("ʧ��");
	MyCommFile comfile;
	if (false == comfile.LoadFile(filname))
	{
		MessageBox(faultmsg, MB_OK);
		return;
	}
	m_listctrl_ans.DeleteAllItems();
	for (int i = 0; i != g_allreqans.GetSize(); i++)
	{
		if (g_allreqans.GetAt(i).flag == 0)
			m_listctrl_ans.InsertItem(i, _T("Req:"));
		else{
			CString strframepos;
			strframepos.Format(_T("%04d"), i);
			m_listctrl_ans.InsertItem(i, strframepos);
		}
		for (int j = 0; j != g_allreqans.GetAt(i).barray.GetSize(); j++)
		{
			CString strtemp;
			strtemp.Format(_T("%02X"), g_allreqans.GetAt(i).barray.GetAt(j));
			m_listctrl_ans.SetItemText(i, j + 1, strtemp);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void ListShowShortTime(CBUSASDlg *pMain, BYTE *precvbuffer, DWORD len)
{
	/*
	    0        1         5          6       7        11
	֡��ʽ0x88 + 4�ֽ�ʱ�� +1�ֽڷ��� + 1�ֽ�DLC +4�ֽ�ID + DLC�ֽ�����
	*/
	if (precvbuffer[6] > 8 )
	{
		AfxMessageBox(_T("error:DLC") , MB_OK);
	}
	CString strtime;
	//��ȡʱ�� 
	if ((*(CComboBox*)pMain->GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 0)
		strtime.Format(_T("%u Listen:"), *(DWORD*)(&precvbuffer[1]));
	else
		strtime.Format(_T("%u Req:"), *(DWORD*)(&precvbuffer[1]));
	int irow = pMain->m_listctrl_show.GetItemCount();
	pMain->m_listctrl_show.InsertItem(irow, strtime);
	//��ȡID
	TCHAR spbuffer[80] = _T("");
	for (int i = 0; i != 4; i++)
	{
		wsprintf(&spbuffer[i * 3], _T("%02X "), precvbuffer[i+7]);
	}
	spbuffer[11] = 0;
	pMain->m_listctrl_show.SetItemText(irow, 1, spbuffer);
	for (int i = 0; i != precvbuffer[6];i++)
	{
		wsprintf(spbuffer, _T("%02X"), precvbuffer[i+11]);
		pMain->m_listctrl_show.SetItemText(irow, 2 + i, spbuffer);
	}
	//����
	//if ((*(CComboBox*)pMain->GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 1)
	{
		pMain->m_listctrl_show.EnsureVisible(pMain->m_listctrl_show.GetItemCount() - 1, FALSE);
	}
}
void ListShowShortCanId(CBUSASDlg *pMain, BYTE *precvbuffer, DWORD len)
{
	/*
	0             1         5          6       7        11
	֡��ʽ0x88 + 4�ֽ�ʱ�� +1�ֽڷ��� + 1�ֽ�DLC +4�ֽ�ID + DLC�ֽ�����
	*/
	for (int i = 0; i != g_listendata.GetSize(); i++)
	{
		if (g_listendata[i].canid == *(DWORD*)&precvbuffer[7])//���ID������ж��ֽڱ仯
		{
			g_listendata[i].time = *(DWORD*)&precvbuffer[1];//����ʱ��		
			if (g_listendata[i].dlc > *(BYTE*)&precvbuffer[6])//8  7
			{
			}
			g_listendata[i].dlc = *(BYTE*)&precvbuffer[6];
			for (BYTE j = 0; j < g_listendata[i].dlc; j++)//���²��ұȽ�������
			{
				if (g_listendata[i].data[j] != precvbuffer[11 + j])//�ֽڱ任
				{
					g_listendata[i].changepos |= (BYTE)(1 << j);
					g_listendata[i].data[j] = precvbuffer[11 + j];
					if (g_listendata[i].changetimes[j]<22)//��Ƶ�仯����ˢ��
					{
						g_listendata[i].changetimes[j]++;
						g_listendata[i].fliji = TRUE;//����ˢ��
					}					
					g_listendata[i].fftime[j] = ::GetTickCount();				
				}
				else if ((::GetTickCount() - g_listendata[i].fftime[j]) > 1000 &&//������ɫ�仯
					(g_listendata[i].changepos & (BYTE)(1 << j)))
				{
					g_listendata[i].changepos &= (BYTE)~(1 << j);
					g_listendata[i].data[j] = precvbuffer[11 + j];
				}
			}
			return;
		}
	}
	LISTENBUS onelisten;
	onelisten.changepos = 0;
	onelisten.canid = *(DWORD*)&precvbuffer[7];
	onelisten.time = *(DWORD*)&precvbuffer[1];
	onelisten.dlc = *(BYTE*)&precvbuffer[6];
	onelisten.fidtime = 0;
	onelisten.fliji = TRUE;
	memset(onelisten.fftime, 0, 32);
	memset(onelisten.changetimes, 0, 32);
	memset(onelisten.data, 0, 8);
	memcpy(onelisten.data, &precvbuffer[11], 8);
	g_listendata.push(onelisten);
	return;
}
static int x = 0;
void CmpFrameCanDataAndRes(CBUSASDlg *pMain, BYTE *precvbuffer, DWORD len)
{
	/*
	   0        1         5          6      10        11
	֡��ʽ0x88 + 4�ֽ�ʱ�� +1�ֽڷ��� + 4�ֽ�ID +1�ֽ�DLC + DLC�ֽ�����
	*/
	for (int y = 0; y < g_allreqans.GetSize(); x++, y++)//yΪ���������� ,xΪ������ʼλ��
	{
		if (x == g_allreqans.GetSize())
			x = 0;
		if (g_allreqans.GetAt(x).flag == 0)
		{
			BYTE cmpret;
			if (precvbuffer[6] != (g_allreqans[x].barray.m_nSize - 4))	continue;;
			cmpret = memcmp(g_allreqans[x].barray.m_pData, &precvbuffer[7], precvbuffer[6]+4);

			if (!cmpret)
			{
				int irow = pMain->m_listctrl_show.GetItemCount();
				BYTE Sendbuf[15] = { 0 };
				while (g_allreqans.GetAt(++x).flag == 1)
				{
					CString strtemp;
					Sendbuf[0] = 0x88;
					Sendbuf[1] = (g_allreqans[x].barray.m_nSize - 4) < 8 ? g_allreqans[x].barray.m_nSize - 4 : 8;
					memcpy(&Sendbuf[2], g_allreqans[x].barray.m_pData, Sendbuf[1] + 4);

					pMain->m_currport.SendDataWithPro(Sendbuf, 14);

					strtemp.Format(_T("%u %04d:"), *(DWORD*)(&precvbuffer[1]), x);
					pMain->m_listctrl_show.InsertItem(irow, strtemp);
					TCHAR strbuff[50] = _T("");
					for (int i = 2; i != 6; i++)
					{
						wsprintf(&strbuff[(i - 2) * 3], _T("%02X "), Sendbuf[i]);
					}
					strbuff[11] = 0;
					pMain->m_listctrl_show.SetItemText(irow, 1, strbuff);

					for (int i = 6; i != Sendbuf[1] + 6; i++)
					{
						wsprintf(strbuff, _T("%02X"), Sendbuf[i]);
						pMain->m_listctrl_show.SetItemText(irow, 2 + i - 6, strbuff);
					}
					pMain->m_listctrl_show.EnsureVisible(pMain->m_listctrl_show.GetItemCount() - 1, FALSE);
					if (x == g_allreqans.GetSize() - 1)	break;
					irow++;
					Sleep(30);
				}
				break;
			}
		}
	}
}

void CmpFrameKlineDataAndRes(CBUSASDlg *pMain, BYTE *precvbuffer, DWORD len)
{
	static DWORD rtime = 0;
	rtime = ::GetTickCount();
	for (int y = 0; y < g_allreqans.GetSize(); x++, y++)//yΪ���������� ,xΪ������ʼλ��
	{
		if (x == g_allreqans.GetSize())
			x = 0;
		if (g_allreqans.GetAt(x).flag == 0)
		{
			BYTE cmpret;
			if ((len-5) != g_allreqans[x].barray.m_nSize)	continue;
			cmpret = memcmp(g_allreqans[x].barray.m_pData, &precvbuffer[5], len-5);

			if (!cmpret)
			{
				int irow = pMain->m_listctrl_show.GetItemCount();
				BYTE Sendbuf[300] = { 0 };
				Sendbuf[0] = 0x77;
				while (g_allreqans.GetAt(++x).flag == 1)
				{
					CString strtemp; BYTE crc = 0;
					memcpy(&Sendbuf[1], g_allreqans[x].barray.m_pData, g_allreqans[x].barray.m_nSize);
					for (int crn = 0; crn != g_allreqans[x].barray.m_nSize - 1; crn++)//�����У��
					{
						BYTE temp = g_allreqans[x].barray[crn];
						crc += temp;
					}
					Sendbuf[g_allreqans[x].barray.m_nSize] = crc;
					pMain->m_currport.SendData(Sendbuf, g_allreqans[x].barray.m_nSize + 1);
					strtemp.Format(_T("%u %04d:"), *(DWORD*)&precvbuffer[1] + ::GetTickCount() - rtime, x);
					pMain->m_listctrl_show.InsertItem(irow, strtemp);
					TCHAR strbuff[300] = _T("");
					for (int i = 1; i != g_allreqans[x].barray.m_nSize + 1; i++)
					{
						wsprintf(&strbuff[(i - 1) * 3], _T("%02X "), Sendbuf[i]);
					}
					pMain->m_listctrl_show.SetItemText(irow, 1, strbuff);
					pMain->m_listctrl_show.EnsureVisible(pMain->m_listctrl_show.GetItemCount() - 1, FALSE);
					if (x == g_allreqans.GetSize() - 1)	break;
					irow++;
				}
				break;
			}
		}
	}
	return;
}
UINT MyThread(LPVOID  parm)
{
	CBUSASDlg *pMain = (CBUSASDlg *)AfxGetApp()->m_pMainWnd;
	BYTE recvbuffer[4096];
	while (g_threadflag)
	{
		DWORD rlen = pMain->m_currport.RecvDataWithProQuick(recvbuffer);
		if (rlen == 0 || g_pause == TRUE) continue;

		if (recvbuffer[0] == 0x88 || recvbuffer[0] == 0x08)//CAN����֡
		{
			if (g_clearglisten && g_threadshowisstop)
			{
				g_listendata.Empty();
				pMain->m_listctrl_show.DeleteAllItems();
				g_clearglisten = FALSE;
				g_threadshowisstop = FALSE;
			}
			if (g_showtype == TRUE && (*(CComboBox*)pMain->GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 0)//�ɼ�ģʽIDȥ����ʾ
			{
				ListShowShortCanId(pMain, recvbuffer, rlen);
			}
			else//ʱ��������ʾ
			{
				ListShowShortTime(pMain, recvbuffer, rlen);
				if ((*(CComboBox*)pMain->GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 1)//ģ��ģʽ
				{
					CmpFrameCanDataAndRes(pMain, recvbuffer, rlen);//ģ���ļ����Ҳ��ظ�һ֡����
				}
			}
		}
		else if (recvbuffer[0] == 0x77)//K������֡
		{
			CString strtime;
			//��ȡʱ�� 
			if ((*(CComboBox*)pMain->GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 0)
				strtime.Format(_T("%u Listen:"), *(DWORD*)(&recvbuffer[1]));
			else
				strtime.Format(_T("%u Req:"), *(DWORD*)(&recvbuffer[1]));
			DWORD irow = pMain->m_listctrl_show.GetItemCount();
			pMain->m_listctrl_show.InsertItem(irow, strtime);
			TCHAR strbuff[300] = _T("");
			for (int i = 0; i != rlen - 5; i++)
			{
				wsprintf(&strbuff[i * 3], _T("%02X "), recvbuffer[i+5]);
			}
			pMain->m_listctrl_show.SetItemText(irow, 1, strbuff);
			pMain->m_listctrl_show.EnsureVisible(pMain->m_listctrl_show.GetItemCount() - 1, FALSE);
			if ((*(CComboBox*)pMain->GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 1)//ģ��ģʽ��Ҫ�ظ�
			{
				CmpFrameKlineDataAndRes(pMain, recvbuffer, rlen);
			}
		}

	}
	return 0;
}
UINT MyThreadShow(LPVOID  parm)
{
	CBUSASDlg *pMain = (CBUSASDlg *)AfxGetApp()->m_pMainWnd;
	while (1)
	{
		if (g_showtype == TRUE && (*(CComboBox*)pMain->GetDlgItem(IDC_COMBO_PATTERN)).GetCurSel() == 0)
		{
			for (int i = 0; i != g_listendata.GetSize(); i++)
			{
				if (g_clearglisten)
				{
					g_threadshowisstop = TRUE;//ȷ����ʾ�̲߳�ȥ����canid����
					break;
				}
				if (g_pause == TRUE)
					break;
				//if (g_listendata[i].fliji == FALSE && (::GetTickCount() - g_listendata[i].fidtime)<30)  continue;
				TCHAR strbuff[300] = _T("");
				wsprintf(strbuff, _T("%d Listen:"), g_listendata[i].time);
				CString strtemp = pMain->m_listctrl_show.GetItemText(i, 0);
				if (strtemp == "")
					pMain->m_listctrl_show.InsertItem(i, strbuff);
				else
					pMain->m_listctrl_show.SetItemText(i, 0, strbuff);
				wsprintf(strbuff, _T("%02X %02X %02X %02X"), BYTE(g_listendata[i].canid),
					BYTE(g_listendata[i].canid >> 8), BYTE(g_listendata[i].canid >> 16), BYTE(g_listendata[i].canid >> 24));
				pMain->m_listctrl_show.SetItemText(i, 1, strbuff);
				for (int j = 0; j != 8; j++)
				{
					if (j < g_listendata[i].dlc)
					{
						wsprintf(strbuff, _T("%02X"), g_listendata[i].data[j]);
						pMain->m_listctrl_show.SetItemText(i, j + 2, strbuff);
					}
					else
					{
						pMain->m_listctrl_show.SetItemText(i, j + 2, _T(""));
					}
				}
				g_listendata[i].fidtime = ::GetTickCount();
				g_listendata[i].fliji = FALSE;
			}
		}
	}
}
char* TCHAR2char(TCHAR* tchStr)
{
	int iLen = 2 * wcslen(tchStr);//CString,TCHAR������һ���ַ�����˲�����ͨ���㳤�� 
	char* chRtn = new char[iLen + 1];
	wcstombs(chRtn, tchStr, iLen + 1);//ת���ɹ�����Ϊ�Ǹ�ֵ 
	return chRtn;
}
char * TCHARtoCHAR(const TCHAR *tchar)
{
	int iLength;
	char *_char = new char[2 * wcslen(tchar) + 1];
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
	return _char;
}






void CBUSASDlg::OnBnClickedButtonClear()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_listctrl_show.DeleteAllItems();
	g_clearglisten = TRUE;
}


void CBUSASDlg::OnBnClickedButtonStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strstatus = _T("");
	GetDlgItemText(IDC_BUTTON_STOP, strstatus);
	if (strstatus == _T("��ͣ"))
	{
		g_pause = TRUE;
		GetDlgItem(IDC_BUTTON_STOP)->SetWindowTextW(_T("����"));
	}
	else
	{
		g_pause = FALSE;
		GetDlgItem(IDC_BUTTON_STOP)->SetWindowTextW(_T("��ͣ"));
	}
}


void CBUSASDlg::OnBnClickedButtonChangeShow()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strstatus = _T("");
	GetDlgItemText(IDC_BUTTON_CHANGE_SHOW, strstatus);
	if (strstatus == _T("������ʾ"))
	{
		g_pThreadshow = AfxBeginThread(MyThreadShow, 0, 0, 0, 0, NULL);
		m_listctrl_show.DeleteAllItems();
		g_showtype = TRUE;
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->SetWindowTextW(_T("ʱ����ʾ"));

	}
	else
	{
		g_showtype = FALSE;
		g_clearglisten = TRUE;
		g_threadshowisstop = TRUE;
		TerminateThread(g_pThreadshow->m_hThread, 0);
		WaitForSingleObject(g_pThreadshow->m_hThread, INFINITE);
		g_pThreadshow = NULL;
		GetDlgItem(IDC_BUTTON_CHANGE_SHOW)->SetWindowTextW(_T("������ʾ"));
	}
}


void CBUSASDlg::OnCustomdrawListShow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	switch (lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		if (lpLVCustomDraw->nmcd.dwItemSpec < g_listendata.GetSize())
		{
			if (lpLVCustomDraw->iSubItem <= 9 && lpLVCustomDraw->iSubItem >= 2)
			{
				if (g_listendata[(WORD)(lpLVCustomDraw->nmcd.dwItemSpec)].changepos & (1 << (lpLVCustomDraw->iSubItem - 2)))
				{
					if (g_listendata[(WORD)lpLVCustomDraw->nmcd.dwItemSpec].changetimes[lpLVCustomDraw->iSubItem - 2] > 20)
					{
						lpLVCustomDraw->clrText = RGB(255, 255, 255); // white text
						lpLVCustomDraw->clrTextBk = RGB(0, 0, 0); // black background��
					}
					else
					{
						lpLVCustomDraw->clrText = RGB(255, 255, 255); // white text
						lpLVCustomDraw->clrTextBk = RGB(255, 0, 0); // black background��
					}


				}
				else
				{
					lpLVCustomDraw->clrText = CLR_DEFAULT; //CLR_DEFAULT;
					lpLVCustomDraw->clrTextBk = CLR_DEFAULT;// CLR_DEFAULT;
				}
			}
		}
		break;
	default: break;
	}
	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}


void CBUSASDlg::OnBnClickedButtonSave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// ���ù�����   
	TCHAR szFilter[] = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
	// ������ļ��Ի���   
	CFileDialog fileDlg(FALSE, _T("txt"), NULL, 0, szFilter, this);
	CString strFilePath;

	// ��ʾ���ļ��Ի���   
	if (IDOK == fileDlg.DoModal())
	{
		strFilePath = fileDlg.GetPathName();
		CFile outfile;
		outfile.Open(strFilePath, CFile::modeCreate | CFile::modeWrite);

		for (int i = 0; i != m_listctrl_show.GetItemCount(); i++)
		{
			for (int j = 0; j != 10; j++)
			{
				char szStr[1024] = { 0 };
				CString stesave = m_listctrl_show.GetItemText(i, j);
				wcstombs(szStr, stesave, stesave.GetLength());
				outfile.Write(szStr, stesave.GetLength());
				if (j <= 1)
					outfile.Write("\t", 1);
				else
					outfile.Write(" ", 1);
			}
			outfile.Write("\r\n", 2);
		}
		outfile.Close();
	}
}


void CBUSASDlg::OnBnClickedButtonMonifile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Moni File(*.txt)|*.txt||"), NULL);
	if (dlgFile.DoModal())
	{
		m_strmonifile = dlgFile.GetPathName();
	}
	CString strtitle = _T("ZXT-simulator  ģ���ļ�:");
	strtitle += m_strmonifile;
	SetWindowText(strtitle);
	LoadReqReq();
}


void CBUSASDlg::OnBnClickedButtonFfmoni()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	LoadReqReq();
}


void CBUSASDlg::OnCustomdrawListAns(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	switch (lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		if (m_col == lpLVCustomDraw->iSubItem && m_row == lpLVCustomDraw->nmcd.dwItemSpec
			&& lpLVCustomDraw->iSubItem >= 1)

		{
			lpLVCustomDraw->clrText = RGB(255, 255, 255); // white text
			lpLVCustomDraw->clrTextBk = RGB(0, 0, 0); // black background
			//m_listctrl.SetItemState(lpLVCustomDraw->nmcd.dwItemSpec, 0XFF, LVIS_SELECTED);
		}
		else
		{
			lpLVCustomDraw->clrText = CLR_DEFAULT; //CLR_DEFAULT;
			lpLVCustomDraw->clrTextBk = CLR_DEFAULT;// CLR_DEFAULT;
		}
		break;
	default: break;
	}
	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}


void CBUSASDlg::OnClickListAns(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	LPNMITEMACTIVATE pia = (LPNMITEMACTIVATE)pNMHDR;;
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	m_listctrl_ans.RedrawItems(m_row, m_row);
	m_listctrl_ans.ScreenToClient(&point);

	LVHITTESTINFO lvinfo;
	lvinfo.pt = point;
	lvinfo.flags = LVHT_ABOVE;

	int nItem = m_listctrl_ans.SubItemHitTest(&lvinfo);
	if (nItem != -1)
	{
		CString strTemp;
		strTemp = m_listctrl_ans.GetItemText(lvinfo.iItem, lvinfo.iSubItem); // strTempΪѡ�ֵ�ĳ��ĳ�е����� 
	}
	CRect rect;
	m_listctrl_ans.GetSubItemRect(lvinfo.iItem, lvinfo.iSubItem, LVIR_BOUNDS, rect);
	m_col = lvinfo.iSubItem;
	m_row = lvinfo.iItem;

	m_listctrl_ans.RedrawItems(lvinfo.iItem, lvinfo.iItem);
	*pResult = 0;
}


BOOL CBUSASDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	static int highlow = 1;
	if (::GetFocus() == GetDlgItem(IDC_LIST_ANS)->GetSafeHwnd())
	{
		if (WM_CHAR == pMsg->message)
		{
			BYTE &posvalue = g_allreqans[m_row].barray[m_col - 1];
			if (pMsg->wParam <= 0x39 && pMsg->wParam >= 0x30)
			{
				if (highlow == 1)
				{
					posvalue = posvalue & 0x0F | ((pMsg->wParam - 0x30) << 4);
					highlow = 0;
				}
				else
				{
					posvalue = posvalue & 0xF0 | (pMsg->wParam - 0x30);
					highlow = 1;
				}
				CString strtemp;
				strtemp.Format(_T("%02X"), g_allreqans.GetAt(m_row).barray.GetAt(m_col - 1));
				m_listctrl_ans.SetItemText(m_row, m_col, strtemp);
			}
			if (pMsg->wParam <= 0x46 && pMsg->wParam >= 0x41)
			{
				if (highlow == 1)
				{
					posvalue = posvalue & 0x0F | ((pMsg->wParam - 0x37) << 4);
					highlow = 0;
				}
				else
				{
					posvalue = posvalue & 0xF0 | (pMsg->wParam - 0x37);
					highlow = 1;
				}
				CString strtemp;
				strtemp.Format(_T("%02X"), g_allreqans.GetAt(m_row).barray.GetAt(m_col - 1));
				m_listctrl_ans.SetItemText(m_row, m_col, strtemp);
			}
			if (pMsg->wParam <= 0x66 && pMsg->wParam >= 0x61)
			{
				if (highlow == 1)
				{
					posvalue = posvalue & 0x0F | ((pMsg->wParam - 0x57) << 4);
					highlow = 0;
				}
				else
				{
					posvalue = posvalue & 0xF0 | (pMsg->wParam - 0x57);
					highlow = 1;
				}
				CString strtemp;
				strtemp.Format(_T("%02X"), g_allreqans.GetAt(m_row).barray.GetAt(m_col - 1));
				m_listctrl_ans.SetItemText(m_row, m_col, strtemp);
			}

		}
		if (WM_MOUSEWHEEL == pMsg->message && (pMsg->wParam & MK_CONTROL ))
		{
			BYTE &posvalue = g_allreqans[m_row].barray[m_col - 1];
			if (int(pMsg->wParam) > 0)
			{
				posvalue++;
			}
			else if (int(pMsg->wParam) < 0)
			{
				posvalue--;
			}
			CString strtemp;
			strtemp.Format(_T("%02X"), g_allreqans.GetAt(m_row).barray.GetAt(m_col - 1));
			m_listctrl_ans.SetItemText(m_row, m_col, strtemp);
		}

	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && 
		GetDlgItem(IDC_EDIT_SEARCH) == GetFocus())
	{
		OnBnClickedButtonSearch();
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CBUSASDlg::OnRclickListShow(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//*pResult = 0;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//��ֹ�ڿհ�����������˵�
	if (m_listctrl_show.GetSelectedCount() <= 0)
	{
		return;
	}
	//�������δ���, ��������Ӧ��ListCtrl
	CMenu menu, *pPopup;
	menu.LoadMenu(IDR_MENU1);
	pPopup = menu.GetSubMenu(0);
	CPoint myPoint;
	ClientToScreen(&myPoint);
	GetCursorPos(&myPoint); //���λ��
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, myPoint.x, myPoint.y, this);
}


void CBUSASDlg::On32771()//�Ҽ����ƺ���
{
	// TODO:  �ڴ���������������
	int subitemnum = m_listctrl_show.GetHeaderCtrl()->GetItemCount();
	CString source = _T("");

	POSITION pos = m_listctrl_show.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int it = m_listctrl_show.GetNextSelectedItem(pos);
		for (int i = 1; i != subitemnum; i++)
		{
			source += m_listctrl_show.GetItemText(it, i);
			source += " ";
		}
		source += "\r\n";
	}

	//�ı����ݱ�����source������
	if (OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength() + 1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, TCHAR2char(source.GetBuffer()));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}
}


void CBUSASDlg::OnSelchangeComboProtocol()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if ((*(CComboBox*)GetDlgItem(IDC_COMBO_PROTOCOL)).GetCurSel() < 2)
	{
		CRect rect;
		// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
		m_listctrl_show.GetClientRect(&rect);

		// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
		m_listctrl_show.SetExtendedStyle(m_listctrl_show.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		while (m_listctrl_show.DeleteColumn(0));
		// Ϊ�б���ͼ�ؼ��������   
		m_listctrl_show.InsertColumn(0, _T("Time"), LVCFMT_LEFT, rect.Width() / 5);
		m_listctrl_show.InsertColumn(1, _T("ID"), LVCFMT_CENTER, rect.Width() / 5);
		m_listctrl_show.InsertColumn(2, _T("D0"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_listctrl_show.InsertColumn(3, _T("D1"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_listctrl_show.InsertColumn(4, _T("D2"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_listctrl_show.InsertColumn(5, _T("D3"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_listctrl_show.InsertColumn(6, _T("D4"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_listctrl_show.InsertColumn(7, _T("D5"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_listctrl_show.InsertColumn(8, _T("D6"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_listctrl_show.InsertColumn(9, _T("D7"), LVCFMT_CENTER, rect.Width() * 3 / 40);
		m_combo_baute.SetCurSel(0);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BUSCLR)->EnableWindow(TRUE);
	}
	else if ((*(CComboBox*)GetDlgItem(IDC_COMBO_PROTOCOL)).GetCurSel() > 1)
	{
		CRect rect;
		// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
		m_listctrl_show.GetClientRect(&rect);

		// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
		m_listctrl_show.SetExtendedStyle(m_listctrl_show.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		while (m_listctrl_show.DeleteColumn(0));
		m_listctrl_show.InsertColumn(0, _T("Time"), LVCFMT_LEFT, rect.Width() / 5);
		m_listctrl_show.InsertColumn(1, _T("Data"), LVCFMT_LEFT, rect.Width() * 4 / 5);
		m_combo_baute.SetCurSel(7);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BUSCLR)->EnableWindow(FALSE);
	}


}

int __fastcall strtoHex(int result, int a2)
{
	char* src = (char*)result;
	char* des = (char*)a2;
	int v5 = 0;
	BYTE val = 0;
	while (v5 < 250 && *src)
	{
		BYTE temp = *src++;
		temp = toupper(temp);
		if (temp <= 0x39 && temp >= 0x30)
		{
			temp = temp - 0x30;
		}
		else if (temp >= 0x41 && temp <= 0x46)
		{
			temp = temp - 0x41 + 10;
		}
		if (v5 % 2 == 0)
			val = temp * 0x10;
		else{
			val = val + temp;
			*des++ = val;
		}
		v5++;
	}
	return 1;
}
void CBUSASDlg::OnBnClickedButtonSend()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_richedit_str.Replace(_T(" "), _T(""));
	if (m_richedit_str.GetAt(m_richedit_str.GetLength() - 1) != '\n')
		m_richedit_str.Append(_T("\r\n"));
	BYTE num = 0;
	CString mstrtemp = m_richedit_str;
	while (mstrtemp.Find(_T("\r\n")) != -1)
	{
		mstrtemp.Delete(0, mstrtemp.Find(_T("\r\n")) + 2);
		num++;
	}
	BYTE *psendbuf = new BYTE[8 + num * 15];
	psendbuf[0] = 0xCC;
	memcpy(&psendbuf[1], &m_busframetime, 2);
	memcpy(&psendbuf[3], &m_bustimes, 4);
	psendbuf[7] = num;
	BYTE cmdtemp[15];
	BYTE i = 0;
	while (m_richedit_str.Find(_T("\r\n")) != -1)
	{
		int pos1 = m_richedit_str.Find(_T("\r\n"));
		DWORD dlytime = m_busframetime;
		CString temp = m_richedit_str.Left(m_richedit_str.Find(_T("\r\n")));
		int pos2 = temp.Find(_T("/"));
		if (pos2 > 0)
		{
			CString strdelay = temp.Mid(pos2+1, pos1 - pos2-1);
			dlytime = _ttoi(strdelay);
			temp.Delete(pos2, pos1 - pos2);
		}	
		BYTE dlc = temp.GetLength() / 2 - 4;
		m_richedit_str.Delete(0, m_richedit_str.Find(_T("\r\n")) + 2);
		memset(cmdtemp, 0, 15);
		char *pstr = TCHAR2char(temp.GetBuffer());
		strtoHex((int)pstr, (int)cmdtemp);
		cmdtemp[12] = (dlc <= 8 ? dlc : 8);
		memcpy(&cmdtemp[13], &dlytime, 2);
		memcpy(&psendbuf[8 + 15 * i++], cmdtemp, 15);

	}
	m_currport.SendDataWithPro(psendbuf, 8 + num * 15);
	delete[] psendbuf;
}


void CBUSASDlg::OnBnClickedButtonBusclr()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BYTE cmd[2] = { 0xDD, 0xDD };
	m_currport.SendDataWithPro(cmd, 2);
}

void CBUSASDlg::OnBnClickedButtonSearch()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strsearch = _T("");
	GetDlgItemText(IDC_EDIT_SEARCH, strsearch);
	strsearch.MakeUpper();
	for (int i = 0; i != m_listctrl_show.GetItemCount(); i++)
	{
		CString stesave = _T("");
		for (int j = 0; j != 10; j++)
		{
			
			stesave += m_listctrl_show.GetItemText(i, j);
			if (j <= 1)
				stesave += "\t";
			else
				stesave += " ";
		}
		if (stesave.Find(strsearch) > 0)
		{
			m_listctrl_show.SetFocus();
			m_listctrl_show.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
		else
		{
			m_listctrl_show.SetItemState(i, 0, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}

	return;
}
int __fastcall CheckCrc(int a1, int a2)
{
	int v2; // r4@1
	int v3; // r1@1
	int i; // r2@1
	signed int j; // r5@2

	v2 = a2;
	v3 = 0;
	for (i = 0; i < v2; i = (unsigned __int16)(i + 1))
	{
		v3 ^= *(BYTE *)(a1 + i) << 8;
		for (j = 0; j < 8; j = (unsigned __int16)(j + 1))
		{
			if (v3 & 0x8000)
				v3 = (unsigned __int16)(2 * v3 ^ 0x1021);
			else
				v3 = 2 * v3 & 0xFFFF;
		}
	}
	return (unsigned __int16)v3;
}
#define CMD_SOH  0X01
#define CMD_STX  0X02
#define CMD_EOT  0X04
#define CMD_ACK  0X06
#define CMD_NCK  0X15
#define CMD_CAN  0X18
void  CBUSASDlg::UpDataBinFile()
{
	FILE *fpin;
	fpin = fopen("sim.bin", "rb");
	if (fpin == NULL)
	{
		MessageBox(_T("�Ҳ��������ļ�sim.bin,��ȷ�������ļ���Ӧ�ó���Ŀ¼��"));
		return;
	}
	fseek(fpin, 0, SEEK_END);
	DWORD fsize = ftell(fpin);
	BYTE *fpbuff = new BYTE[fsize];
	BYTE *delfp = fpbuff;
	fseek(fpin, 0, SEEK_SET);
	fread(fpbuff, 1, fsize, fpin);

	//��$HIDE_ALL
	char *pcrecv = NULL;
	char *hideall = "$HIDE_ALL\r\n";
	m_currport.SendData((BYTE*)hideall, strlen(hideall));
	m_currport.SendData((BYTE*)hideall, strlen(hideall));
	pcrecv = m_currport.WaitAString("HIDE_ALL(OK!)", 2000);
	if (pcrecv == NULL)
	{
		MessageBox(_T("����ʧ��$HIDE_ALL����Ӧ��")); delete[]delfp; return;
	}
	//��$S_UPDATE
	pcrecv = NULL;
	char *updatall = "$S_UPDATE\r\n";
	m_currport.SendData((BYTE*)updatall, strlen(updatall));
	m_currport.SendData((BYTE*)updatall, strlen(updatall));
	pcrecv = m_currport.WaitAString("S_UPDATE=(OK!)", 2000);
	if (pcrecv == NULL)
	{
		MessageBox(_T("����ʧ��$S_UPDATE����Ӧ��")); delete[]delfp; return;
	}
	//���IAP��ʼ�ź�
	pcrecv = NULL;
	pcrecv = m_currport.WaitAString("$IAP:Start!", 500);
	if (pcrecv == NULL)
	{
		MessageBox(_T("����ʧ��û�м�⵽$IAP:Start!")); delete[]delfp; return;
	}
	//��IAPд��ģʽ
	char *cmode1 = "1\r\n";
	m_currport.SendData((BYTE*)cmode1, strlen(cmode1));
	pcrecv = m_currport.WaitAString("$IAP:Ymodem start", 2000);
	if (pcrecv == NULL)
	{
		MessageBox(_T("����ʧ��û�м�⵽$IAP:Ymodem start")); delete[]delfp; return;
	}
	//���Ymodem��ʼ�ź�
	pcrecv = NULL;
	pcrecv = m_currport.WaitAString("C", 1000);
	if (pcrecv == NULL)
	{
		MessageBox(_T("����ʧ��û�м�⵽Ymodem��ʼ�ź�'C'")); delete[]delfp; return;
	}

	BYTE recvbuf[1050];
	pcrecv = NULL;
	memset(recvbuf, 0, 1050);
	BYTE packnum = 0;
	recvbuf[0] = CMD_SOH;
	recvbuf[1] = packnum;
	recvbuf[2] = 255 - packnum;
	packnum++;
	memcpy(&recvbuf[3], "99.99.b", strlen("99.99.b"));
	char strsize[50] = "";
	sprintf(strsize, "%d", fsize);
	memcpy(&recvbuf[strlen("99.99.b") + 4], strsize, strlen(strsize));
	DWORD len = strlen("99.99.b") + 2;
	WORD ncrc = CheckCrc((int)&recvbuf[3], 128);
	recvbuf[128 + 3] = BYTE(ncrc >> 8);
	recvbuf[128 + 4] = BYTE(ncrc);
	m_currport.SendData(recvbuf, 128 + 5);
	char *pret = NULL;
	pcrecv = m_currport.WaitAString("\x06\x43", 5000, pret);
	if (pcrecv == NULL)
	{
		MessageBox(_T("����ʧ�ܣ�ERROR:CMD_SOH")); delete[]delfp; return;
	}
	BYTE *psdata = &recvbuf[3];
	int ssize = fsize;
	BYTE brecv[5] = { 0 };
	m_ctlprogress.SetRange(0, short(fsize / 1024 + 1));
	m_ctlprogress.SetStep(1);
	while (1)
	{
		recvbuf[0] = CMD_STX;
		recvbuf[1] = packnum;
		recvbuf[2] = 0xFF - packnum;
		memcpy(psdata, fpbuff, ssize >= 1024 ? 1024 : ssize);
		WORD crc = CheckCrc((int)&recvbuf[3], 1024);
		recvbuf[1024 + 3] = BYTE(crc >> 8);
		recvbuf[1024 + 4] = BYTE(crc);
		m_currport.SendData(recvbuf, 1024 + 5);
		pcrecv = NULL;
		pcrecv = m_currport.WaitAString("\x06", 100);
		if (pcrecv == NULL)
		{
			MessageBox(_T("����ʧ�ܣ�ERROR:CMD_STX")); delete[]delfp; return;
		}
		m_ctlprogress.StepIt();
		memset(recvbuf, 0, 1024 + 5);
		ssize = ssize - 1024;
		if (ssize < 0)
			break;
		packnum++;
		fpbuff += 1024;
	}
	recvbuf[0] = CMD_EOT;
	m_currport.SendData(recvbuf, 1);
	pcrecv = NULL;
	pcrecv = m_currport.WaitAString("\x06\x43", 1000);
	if (pcrecv != NULL)
	{
		memset(recvbuf, 0, 1024 + 5);
		recvbuf[0] = CMD_SOH;
		recvbuf[1] = 0;
		recvbuf[2] = 0xff;
		m_currport.SendData(recvbuf, 133);
	}
	else
	{
		MessageBox(_T("����ʧ�ܣ�ERROR:CMD_EOT")); delete[]delfp; return;
	}
	pcrecv = NULL;
	pcrecv = m_currport.WaitAString("$IAP:Burn success!", 20000);
	//"$IAP:Burn success!"
	if (pcrecv == NULL)
	{
		MessageBox(_T("����ʧ��,û�м�⵽�����ɹ��źţ�")); delete[]delfp; return;
	}
	char *setcar = "$S_CARINFO:V,1,1\r\n";
	m_currport.SendData((BYTE*)setcar, strlen(setcar));
	pcrecv = NULL;
	pcrecv = m_currport.WaitAString("S_CARINFO", 500);

	char *tenter = "$T_ENTER\r\n";
	m_currport.SendData((BYTE*)tenter, strlen(tenter));
	pcrecv = NULL;
	pcrecv = m_currport.WaitAString("T_ENTER", 500);
	delete[]delfp;
	fclose(fpin);
	MessageBox(_T("�����ɹ������´򿪴���"));
	return;
}

void CBUSASDlg::OnBnClickedButtonNext()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


void CBUSASDlg::OnBnClickedButtonUpdata()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpDataBinFile();
}
