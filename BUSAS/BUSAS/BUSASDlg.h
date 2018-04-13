
// BUSASDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ComSerial.h"

// CBUSASDlg 对话框
class CBUSASDlg : public CDialogEx
{
// 构造
public:
	CBUSASDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BUSAS_DIALOG };

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
	CComboBox m_combo_serial;//串口
	CComboBox m_combo_pettern;//模式
	CComboBox m_combo_potocol;//通道
	CComboBox m_combo_baute;//波特率
	CString m_edit_idlist;//ID列表
	CString m_edit_masklist;//掩码列表
	UINT m_bytetime;//字节间隔
	UINT m_frametime;//帧间隔
	UINT m_waitanstime;//等待时间
	UINT m_ecutimeouts;//通讯超市
	CListCtrl m_listctrl_show;//显示控件
	CListCtrl m_listctrl_ans;//回复控件
	CString m_richedit_str;//总线数据
	CString m_strmonifile;//模拟文件路径

	ComSerial m_currport;
	UINT m_busframetime;//总线默认帧间隔
	UINT m_bustimes;//总线便利次数

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