#include "stdafx.h"
#include "ComSerial.h"
using namespace MYPRO;

ComSerial::ComSerial()
{
}


ComSerial::~ComSerial()
{
	m_hCom = NULL;
}

BOOL ComSerial::InitSerial(BYTE iport)
{
	CString szCom;
	szCom.Format(_T("\\\\.\\COM%d"), iport);
	m_hCom = CreateFile(szCom.GetBuffer(50), //打开串口 
		GENERIC_READ | GENERIC_WRITE, //允许读和写操作 
		0, //独占方式 
		NULL,
		OPEN_EXISTING, //打开存在的串口,必须是OPEN_EXISTING,文件还可以CREATE_NEW,串口不能创建 
		0,//FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //异步方式打开 
		NULL);
	if (INVALID_HANDLE_VALUE == m_hCom) {
		return FALSE;
	}

	SetupComm(
		m_hCom, //串口句柄,CreateFile的有效返回值 
		20480, //输入缓冲区字节数 
		40960);//输出缓冲区字节数

	COMMTIMEOUTS TimeOuts;
	//TimeOuts.ReadIntervalTimeout = 1000;
	//TimeOuts.ReadTotalTimeoutMultiplier = 500;
	//TimeOuts.ReadTotalTimeoutConstant = 5000;
	//TimeOuts.WriteTotalTimeoutMultiplier = 500;
	//TimeOuts.WriteTotalTimeoutConstant = 2000;
	TimeOuts.ReadIntervalTimeout = 50;
	TimeOuts.ReadTotalTimeoutConstant = 5;
	TimeOuts.ReadTotalTimeoutMultiplier = 50;
	TimeOuts.WriteTotalTimeoutConstant = 0;
	TimeOuts.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(m_hCom, &TimeOuts);

	DCB dcb;
	GetCommState(m_hCom, &dcb);
	//dcb.BaudRate = 115200;
	//dcb.ByteSize = 8;
	//dcb.Parity = NOPARITY;
	//dcb.StopBits = ONESTOPBIT;
	dcb.BaudRate =  256000;//115200;//
	dcb.fParity = TRUE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = TRUE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ErrorChar = '?';
	SetCommState(m_hCom, &dcb);
	SetCommMask(m_hCom, EV_RXCHAR);
	//PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	//EscapeCommFunction(m_hCom, SETDTR);
	return TRUE;
}
DWORD ComSerial::RecvData(BYTE *pbuffer, DWORD rlen)
{
	DWORD len = 0; 
	DWORD dwErrorFlags = 0;
	DWORD offset = 0;
	COMSTAT ComStat;	
	//DWORD dwLength;
	LONG lfirsttime = ::GetTickCount();

	while (1){
		if (::GetTickCount() - lfirsttime > 1500)break;
		ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
		if (ComStat.cbInQue == 0)continue;
		ReadFile(m_hCom, &pbuffer[offset], rlen - len, (LPDWORD)&len, NULL);
		offset += len;
		if (offset == rlen)break;
	}
	return offset;
}
///////////////////////////////////////////快速缓冲区数据处理//////////////////////////
static BYTE alldorecvbuff[4096];
static WORD undooffset = 0;
static WORD dooffset = 0;
DWORD ComSerial::RecvDataQuick(BYTE *pbuffer)
{
	DWORD len = 0;
	DWORD dwErrorFlags = 0;
	DWORD offset = 0;
	COMSTAT ComStat;

	ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
	if (ComStat.cbInQue == 0) return 0;
	ReadFile(m_hCom, &pbuffer[offset], 4000, (LPDWORD)&len, NULL);
	undooffset += len;
	return len;
}
DWORD ComSerial::RecvDataWithProQuick(BYTE *pbuffer)
{
	BYTE recvbuff[2048];
	DWORD i = 0;
	BOOL flag = FALSE;
	while (dooffset<undooffset)
	{
		recvbuff[i] = alldorecvbuff[dooffset];
		dooffset++;
		if (flag == FALSE && recvbuff[0] == 0x7d)//收到帧头
			flag = TRUE;
		if (flag && recvbuff[i] == 0x7e)//收到帧尾
			break;
		if (i >= 2000)//防止协议不匹配乱发数据被阻塞
			break;
		if (flag)
			i++;
	}
	if ((i>=1 && recvbuff[i] != 0x7E) || i==0)
	{
		memset(alldorecvbuff, 0, 4096);
		memcpy(alldorecvbuff, recvbuff, i);
		undooffset = i;
		dooffset = 0;
		RecvDataQuick(&alldorecvbuff[i]);
	}
	else
	{
		DWORD iret = UnPackData(recvbuff, i + 1, pbuffer);//解包协议帧
		return iret;
	}


}
////////////////////////////////////////////
void ComSerial::SendData(BYTE *pbuffer, DWORD size)
{
	DWORD len = 0;
	WriteFile(m_hCom, pbuffer, size, &len, NULL);
}
char *ComSerial::RecvCharStr()
{
	static char pbuff[1024] = "";
	DWORD len = 0;
	DWORD dwErrorFlags = 0;
	DWORD offset = 0;
	COMSTAT ComStat;

	ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
	memset(pbuff,0,1024);
	pbuff[ComStat.cbInQue] = 0;
	ReadFile(m_hCom, pbuff, ComStat.cbInQue, (LPDWORD)&len, NULL);
	return pbuff;
}
char *ComSerial::WaitAString(char *src, DWORD wtime)
{
	static char precv[1024] = "";
	memset(precv, 0, 1024);
	LONG fstime = ::GetTickCount();
	while (strstr(precv, src) == NULL)
	{
		char *temp = RecvCharStr();
		if (strcmp(temp, ""))
			strcat(precv, temp);
		if ((::GetTickCount() - fstime) > wtime)
			return NULL;
	}
	return precv;
}
char *ComSerial::WaitAString(char *src, DWORD wtime ,char *radd)
{
	static char precv[1024] = "";
	memset(precv, 0, 1024);
	LONG fstime = ::GetTickCount();
	while (strstr(precv, src) == NULL)
	{
		char *temp = RecvCharStr();
		if (strcmp(temp, ""))
			strcat(precv, temp);
		if ((::GetTickCount() - fstime) > wtime)
		{
			radd = precv; return NULL;
		}		
	}
	radd = precv;
	return precv;
}
DWORD ComSerial::RecvDataWithPro(BYTE *pbuffer)
{
	BYTE recvbuff[2048];
	DWORD i = 0;
	BOOL flag = FALSE;
	while (RecvData(recvbuff + i, 1) == 1)
	{
		if (flag == FALSE && recvbuff[0] == 0x7d)//收到帧头
			flag = TRUE;
		if (flag && recvbuff[i]==0x7e)//收到帧尾
			break;
		if (i>=2000)//防止协议不匹配乱发数据被阻塞
			break;
		if (flag)
			i++;
	}
	DWORD iret = UnPackData(recvbuff, i+1, pbuffer);//解包协议帧
	return iret;
}
void ComSerial::SendDataWithPro(BYTE *pbuffer, DWORD size)
{
	BYTE *psendbuf = new BYTE[Check7D7E7FNum(pbuffer,size) + 6 + size];
	DWORD slen = PackData(pbuffer, size, psendbuf);
	DWORD len = 0;
	WriteFile(m_hCom, psendbuf, slen, &len, NULL);
	delete[]psendbuf;
}
/*****************************************************************************************/
//////////////////////////////自定义协议函数集合////////////////////////////////////////////
/*****************************************************************************************/
DWORD MYPRO::Check7D7E7FNum(BYTE *data, DWORD len)
{
	DWORD iret = 0;
	for (int i = 0; i != len; i++)
	{
		if (data[i] == 0x7F || data[i] == 0x7D || data[i] == 0x7E)
			iret++;
	}
	return iret;
}
/*
数据域组包为 帧头1+帧长2+DATA+帧尾
*/
DWORD MYPRO::PackData(BYTE* dataSrc, DWORD lenSrc, BYTE* dataDest)
{
	DWORD ipos = 3;
	BYTE sum = 0;
	for (int i = 0; i != lenSrc; i++)//求出校验和
	{
		sum += dataSrc[i];
	}
	//赋值数据域 包括转义字符
	for (int i = 0; i != lenSrc; i++)
	{
		if (dataSrc[i] == 0x7D)
		{
			dataDest[ipos++] = 0x7F;
			dataDest[ipos++] = 0x00;
		}
		else if (dataSrc[i] == 0x7E)
		{
			dataDest[ipos++] = 0x7F;
			dataDest[ipos++] = 0x01;
		}
		else if (dataSrc[i] == 0x7F)
		{
			dataDest[ipos++] = 0x7F;
			dataDest[ipos++] = 0x02;
		}
		else
			dataDest[ipos++] = dataSrc[i];
	}
	//赋值校验和
	if (sum == 0x7D)
	{
		dataDest[ipos++] = 0x7F;
		dataDest[ipos++] = 0x00;
	}
	else if (sum == 0x7E)
	{
		dataDest[ipos++] = 0x7F;
		dataDest[ipos++] = 0x01;
	}
	else if (sum == 0x7F)
	{
		dataDest[ipos++] = 0x7F;
		dataDest[ipos++] = 0x02;
	}
	else
		dataDest[ipos++] = sum;
	dataDest[0] = 0x7D;//赋值帧头
	dataDest[1] = BYTE(((ipos - 3) >> 2) & 0xF0);//帧长
	dataDest[2] = BYTE(ipos - 3 & 0x3F);//帧长
	dataDest[ipos] = 0x7E;
	return ipos + 1;
}
BOOL MYPRO::CheckFrame(BYTE* dataSrc, DWORD lenSrc)
{
	DWORD len = 0;
	if (lenSrc <= 4)	return FALSE;
	if (dataSrc[0] == 0x7D && dataSrc[lenSrc - 1] == 0x7E)
	{
		len = ((WORD)(dataSrc[1] & 0xF0) << 2) | (dataSrc[2] & 0x003F);
	}
	if (lenSrc == (len + 4))
		return TRUE;
	return FALSE;
}
DWORD MYPRO::UnPackData(BYTE* dataSrc, DWORD lenSrc, BYTE* dataDest)
{
	if (CheckFrame(dataSrc, lenSrc) == FALSE) return 0;
	dataSrc += 3;
	lenSrc -= 4;
	DWORD ipos = 0;
	for (int i = 0; i != lenSrc; i++)
	{
		if (dataSrc[i] == 0x7F && dataSrc[i + 1] == 0x00)
		{
			dataDest[ipos++] = 0x7D; i++;
		}
		else if (dataSrc[i] == 0x7F && dataSrc[i + 1] == 0x01)
		{
			dataDest[ipos++] = 0x7E; i++;
		}
		else if (dataSrc[i] == 0x7F && dataSrc[i + 1] == 0x02)
		{
			dataDest[ipos++] = 0x7F; i++;
		}
		else
			dataDest[ipos++] = dataSrc[i];

	}
	dataSrc -= 3;
	BYTE sum = 0;
	for (int j = 0; j != ipos - 1; j++)
	{
		sum += dataDest[j];
	}
	if (sum != dataDest[ipos - 1])
		return 0;
	return ipos - 1;
}