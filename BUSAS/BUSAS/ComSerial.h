#pragma once

//自定义协议函数集合
namespace MYPRO
{
	DWORD Check7D7E7FNum(BYTE *data, DWORD len);
	DWORD PackData(BYTE* dataSrc, DWORD lenSrc, BYTE* dataDest);
	BOOL CheckFrame(BYTE* dataSrc, DWORD lenSrc);
	DWORD UnPackData(BYTE* dataSrc, DWORD lenSrc, BYTE* dataDest);
}
//串口通讯类
class ComSerial
{
public://不带协议的串口字节收发
	ComSerial();
	virtual ~ComSerial();
	BOOL InitSerial(BYTE iport);
	DWORD RecvData(BYTE *pbuffer, DWORD rlen);
	DWORD RecvDataQuick(BYTE *pbuffer);
	void SendData(BYTE *pbuffer, DWORD size);
	void CloseComPort(){ CloseHandle(m_hCom); };
	char* WaitAString(char *src, DWORD wtime);
	char* WaitAString(char *src, DWORD wtime, char *radd);
public:
	//带通讯协议的收发函数
	friend DWORD MYPRO::Check7D7E7FNum(BYTE *data, DWORD len);
	friend DWORD MYPRO::PackData(BYTE* dataSrc, DWORD lenSrc, BYTE* dataDest);
	friend BOOL MYPRO::CheckFrame(BYTE* dataSrc, DWORD lenSrc);
	friend DWORD MYPRO::UnPackData(BYTE* dataSrc, DWORD lenSrc, BYTE* dataDest);
	DWORD RecvDataWithPro(BYTE *pbuffer);
	DWORD RecvDataWithProQuick(BYTE *pbuffer);
	void SendDataWithPro(BYTE *pbuffer, DWORD size);
protected:
	char * RecvCharStr();
	HANDLE m_hCom;
};

