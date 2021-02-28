#pragma once
#include <ctime>
#include "Ocx.h"
#define  PORT_START 8850
#define  PORT_END  9870
#define SAFE_DELETE(x) if(x){delete x; x = nullptr;}

class XLTool {
public:
	static void getAvailableListenPort(int & port);
	static bool getPortIsAvailable(int & port);
	static std::string TimetoString(time_t* t);
	static CString GetConfigureFilePath();
	static void Tchar2Char(const TCHAR* szSrc, char* szDes);
	static void Char2TChar(const char* szSrc, TCHAR* szDes);
	static time_t String2Time(CString strTime);
	static const CString Time2String(const time_t tTime);
	static const wstring Char2TChar(const char* szSrc);
	static BOOL SuperMkDir(CString P);
	static inline void Encode(const unsigned char* Data, long DataByte, std::string& str)
	{
		//编码表
		const char EncodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		//返回值
		unsigned char Tmp[4] = { 0 };
		int LineLength = 0;
		str = "";
		//#pragma omp parallel for 
		for (int i = 0; i<(int)(DataByte / 3); i++)
		{
			Tmp[1] = *Data++;
			Tmp[2] = *Data++;
			Tmp[3] = *Data++;
			str += EncodeTable[Tmp[1] >> 2];
			str += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
			str += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
			str += EncodeTable[Tmp[3] & 0x3F];
			if (LineLength += 4, LineLength == 76)
			{
				str += "\r\n"; LineLength = 0;
			}
		}
		//对剩余数据进行编码
		int Mod = DataByte % 3;
		if (Mod == 1)
		{
			Tmp[1] = *Data++;
			str += EncodeTable[(Tmp[1] & 0xFC) >> 2];
			str += EncodeTable[((Tmp[1] & 0x03) << 4)];
			str += "==";
		}
		else if (Mod == 2)
		{
			Tmp[1] = *Data++;
			Tmp[2] = *Data++;
			str += EncodeTable[(Tmp[1] & 0xFC) >> 2];
			str += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
			str += EncodeTable[((Tmp[2] & 0x0F) << 2)];
			str += "=";
		}
	}

	static bool UnicodeToChinese(string str, CString&  cstr);
	static std::wstring s2ws(const std::string& s);
	static const std::wstring s2wsForUtf8(const std::string& s);
	static const string  UnicodeToAnsi(CString unicode);
	static const string  Tchar2Char(const TCHAR* szSrc);
private:
	static CString m_local_cfg_file_name;
};


std::string format(char* fmt, ...);
#define Trace(fmt, ...) OutputDebugStringA(format(fmt, ##__VA_ARGS__).c_str());