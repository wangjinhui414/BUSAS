
// BUSASDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ComSerial.h"

// CBUSASDlg �Ի���
class CBUSASDlg : public CDialogEx
{
// ����
public:
	CBUSASDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BUSAS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_combo_serial;//����
	CComboBox m_combo_pettern;//ģʽ
	CComboBox m_combo_potocol;//ͨ��
	CComboBox m_combo_baute;//������
	CString m_edit_idlist;//ID�б�
	CString m_edit_masklist;//�����б�
	UINT m_bytetime;//�ֽڼ��
	UINT m_frametime;//֡���
	UINT m_waitanstime;//�ȴ�ʱ��
	UINT m_ecutimeouts;//ͨѶ����
	CListCtrl m_listctrl_show;//��ʾ�ؼ�
	CListCtrl m_listctrl_ans;//�ظ��ؼ�
	CString m_richedit_str;//��������
	CString m_strmonifile;//ģ���ļ�·��

	ComSerial m_currport;
	UINT m_busframetime;//����Ĭ��֡���
	UINT m_bustimes;//���߱�������

public:
	BOOL Enum_Serial_Port();
	afx_msg void OnBnClickedButtonOnoff();

	BOOL InitComBox();
	void LoadReqReq();

	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonChangeShow();
	afx_msg void OnCustomdrawListShow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonMonifile();
	afx_msg void OnBnClickedButtonFfmoni();
	int m_col;
	int m_row;
	afx_msg void OnCustomdrawListAns(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickListAns(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRclickListShow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void On32771();
	afx_msg void OnSelchangeComboProtocol();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonBusclr();
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonNext();
	CProgressCtrl m_ctlprogress;
	void  UpDataBinFile();
	afx_msg void OnBnClickedButtonUpdata();
};
//////////////////////////////////////////////////////////////////////////
static UINT  MyThread(LPVOID  parm);
static UINT  MyThreadShow(LPVOID  parm);
char* TCHAR2char(TCHAR* tchStr);
char * TCHARtoCHAR(const TCHAR *tchar);

struct LISTENBUS
{
	//DWORD row;
	//DWORD col;
	DWORD time;
	DWORD canid;
	BYTE  dlc;
	BYTE  data[8];
	BYTE  changepos;
	DWORD changetimes[8];
	LONG fftime[8];
	BOOL fliji;
	LONG fidtime;

};