#include "stdafx.h"
#include "XLTool.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <ws2tcpip.h>

#include <Windows.h>
#include <psapi.h>//GetModuleFileNameEx
#include <afxwin.h>

#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define CONFIG_FILE	TEXT("VideoConfig.ini")

std::string format(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int size = _vsnprintf(nullptr, 0, fmt, args) + 1;
	std::unique_ptr<char> buf(new char[size]);
	size = _vsnprintf(buf.get(), size, fmt, args);
	va_end(args);
	return buf.get();
}

const string  XLTool::Tchar2Char(const TCHAR* szSrc)
{
	int len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szSrc, -1, NULL, 0, NULL, NULL);
	char *szStr = new char[len + 1];
	memset(szStr, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szSrc, -1, szStr, len, NULL, NULL);
	szStr[len] = '\0';
	string str(szStr);
	delete[] szStr;
	szStr = NULL;
	return str;

}
const string XLTool::UnicodeToAnsi(CString unicode)
{

	int len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)unicode, -1, szUtf8, len, NULL, NULL);
	szUtf8[len] = '\0';
	string str(szUtf8);
	delete[]szUtf8;
	szUtf8 = NULL;
	return str;
}

std::wstring XLTool::s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

const std::wstring XLTool::s2wsForUtf8(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

bool XLTool::UnicodeToChinese(string str, CString&  cstr)
{
	int i = 0;
	int j = 0;
	int len = 0;

	len = str.length();
	if (len <= 0)
	{
		return false;
	}

	int nValue = 0;
	WCHAR * pWchar;
	wchar_t* szHex;
	char strchar[6] = { '0','x','\0' };
	char tmp[1];
	for (i = 0; i < len; i++){
		if (str[i] == '\\' && str[i+1] == 'u') {
			continue;
		}

		if (str[i] == 'u') {
			for (j = 2; j < 6; j++) {
				i++;
				strchar[j] = str[i];
			}

			USES_CONVERSION;
			szHex = A2W(strchar);

			StrToIntExW(szHex, STIF_SUPPORT_HEX, &nValue);
			pWchar = (WCHAR*)& nValue;

			cstr += pWchar;
		}
		else {
			tmp[0] = str[i];
			USES_CONVERSION;
			cstr += A2W(tmp);
		}
	}
	return true;
}

// 判断目录是否存在
BOOL FolderExists(CString s)
{
	DWORD attr;
	attr = GetFileAttributes(s);
	return (attr != (DWORD)(-1)) &&
		(attr & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL XLTool::SuperMkDir(CString P)
{
	int len = P.GetLength();
	if (len <2)
	{
		return false;
	}

	if ('\\' == P[len - 1])
	{
		P = P.Left(len - 1);
		len = P.GetLength();
	}

	if (len <= 0)
	{
		return false;
	}

	if (len <= 3)
	{
		if (FolderExists(P))return true;
		else return false;
	}

	if (FolderExists(P))
	{
		return true;
	}

	CString Parent;
	Parent = P.Left(P.ReverseFind('\\'));

	if (Parent.GetLength() <= 0)
	{
		return false;
	}

	bool Ret = SuperMkDir(Parent);

	if (Ret)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = 0;
		Ret = (CreateDirectory(P, &sa) == TRUE);
		return Ret;
	}
	else
	{
		return false;
	}
}

const wstring XLTool::Char2TChar(const char* szSrc)
{
	int len = 0;
	len = strlen(szSrc);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		szSrc,
		-1,
		NULL,
		0);
	TCHAR* szDes = new TCHAR[unicodeLen + 1];
	memset(szDes, 0, unicodeLen * sizeof(TCHAR));
	::MultiByteToWideChar(CP_ACP,
		0,
		szSrc,
		-1,
		szDes,
		unicodeLen);
	wstring str(szDes);
	return str;
}

const CString XLTool::Time2String(const time_t tTime)
{
	CTime time(tTime);
	CString str = time.Format(_T("%Y-%m-%d %H:%M:%S"));
	return str;
}

time_t XLTool::String2Time(CString strTime)
{
	struct tm time;
	CString strTemp = strTime;
	if (strTemp.GetLength() < 18)
		return 0;
	strTemp.Remove('-');
	strTemp.Remove(':');
	strTemp.Remove('.');
	strTemp.Remove(',');
	strTemp.Remove(' ');

	time.tm_year = _ttoi(strTemp.Mid(0, 4)) - 1900;
	time.tm_mon = _ttoi(strTemp.Mid(4, 2)) - 1;
	time.tm_mday = _ttoi(strTemp.Mid(6, 2));
	time.tm_hour = _ttoi(strTemp.Mid(8, 2));
	time.tm_min = _ttoi(strTemp.Mid(10, 2));
	time.tm_sec = _ttoi(strTemp.Mid(12, 2));
	time_t tim = mktime(&time);
	return tim;
}

void XLTool::Char2TChar(const char* szSrc, TCHAR* szDes)
{
	int len = 0;
	len = strlen(szSrc);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		szSrc,
		-1,
		NULL,
		0);
	::MultiByteToWideChar(CP_ACP,
		0,
		szSrc,
		-1,
		szDes,
		unicodeLen);
}

void XLTool::Tchar2Char(const TCHAR* szSrc, char* szDes)
{
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		szSrc,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	::WideCharToMultiByte(CP_ACP,
		0,
		szSrc,
		-1,
		szDes,
		iTextLen,
		NULL,
		NULL);
}

CString XLTool::m_local_cfg_file_name;
CString XLTool::GetConfigureFilePath()
{
	if (XLTool::m_local_cfg_file_name.IsEmpty()) {
		// 配置文件路径
		TCHAR szFilePath[MAX_PATH] = { 0 };
		TCHAR szDrive[_MAX_DRIVE] = { 0 };
		TCHAR szDir[_MAX_DIR] = { 0 };
		TCHAR szFname[_MAX_FNAME] = { 0 };
		TCHAR szExt[_MAX_EXT] = { 0 };
		TCHAR szConfPath[MAX_PATH] = { 0 };

		::GetModuleFileName(NULL, szFilePath, MAX_PATH);
		_wsplitpath_s(szFilePath, szDrive, szDir, szFname, szExt);
		wsprintf(szConfPath, _T("%s%s%s"), szDrive, szDir, CONFIG_FILE);
		XLTool::m_local_cfg_file_name = szConfPath;
	}
	
	return XLTool::m_local_cfg_file_name;
}

/**
* 检测UDP端口是否被占用
*
* @param [in]  port    待检测端口号
*
* @return bool: 0 未占用，1 占用
*/
bool CheckUdpPort(unsigned int port)
{
	if (port < 1024 || port > 65535)
		return TRUE;

	PMIB_UDPTABLE pudptable = NULL;
	DWORD dwSize = 0;

	HINSTANCE hInst;		//动态链接库模块句柄
	hInst = LoadLibraryA("iphlpapi.dll");		//动态加载iphlpapi.dll

												//定义函数指针类型
	typedef long(__stdcall *ADDPROC) (PMIB_UDPTABLE pUdpTable, PDWORD pdwSize, BOOL bOrder);

	//获取iphlpapi.dll导出函数
	ADDPROC GetUdpTable = (ADDPROC)GetProcAddress(hInst, "GetUdpTable");

	if (GetUdpTable(pudptable, &dwSize, TRUE) == ERROR_INSUFFICIENT_BUFFER)		//pTcpTable空间不足
	{
		pudptable = new MIB_UDPTABLE[dwSize];		//为pTcpTable申请足够的空间

		if (GetUdpTable(pudptable, &dwSize, TRUE) == NO_ERROR)     //GetTcpTable调用成功
		{
			//检测端口nPort是否在
			for (UINT i = 0; i < pudptable->dwNumEntries; i++)
			{
				if (port != ntohs(pudptable->table[i].dwLocalPort))
					continue;

				//释放资源
				delete[] pudptable;
				FreeLibrary((HMODULE)hInst);

				return TRUE;
			}
		}
	}
	//释放资源
	delete[] pudptable;
	FreeLibrary((HMODULE)hInst);

	return FALSE;
}

/**
* 检测TCP端口是否被占用
*
* @param [in]  port    待检测端口号
*
* @return bool: 0 未占用，1 占用
*/
bool CheckTcpPort(unsigned int port)
{
	if (port < 1024 || port > 65535)
	{
		return TRUE;
	}

	PMIB_TCPTABLE ptcptable = NULL;
	DWORD dwSize = 0;
	HINSTANCE hInst;		//动态链接库模块句柄
	hInst = LoadLibraryA("iphlpapi.dll");		//动态加载iphlpapi.dll

												//定义函数指针类型
	typedef long(__stdcall *ADDPROC) (PMIB_TCPTABLE pTcpTable, PDWORD pdwSize, BOOL bOrder);

	//获取iphlpapi.dll导出函数
	ADDPROC GetTcpTable = (ADDPROC)GetProcAddress(hInst, "GetTcpTable");

	if (GetTcpTable(ptcptable, &dwSize, TRUE) == ERROR_INSUFFICIENT_BUFFER)		//pTcpTable空间不足
	{
		ptcptable = new MIB_TCPTABLE[dwSize];		//为pTcpTable申请足够的空间

		if (GetTcpTable(ptcptable, &dwSize, TRUE) == NO_ERROR)     //GetTcpTable调用成功
		{
			//检测端口nPort是否在
			for (UINT i = 0; i < ptcptable->dwNumEntries; i++)
			{
				if (port != ntohs(ptcptable->table[i].dwLocalPort))
					continue;

				//释放资源
				delete[] ptcptable;
				FreeLibrary((HMODULE)hInst);

				return TRUE;
			}
		}
	}
	//释放资源
	delete[] ptcptable;
	FreeLibrary((HMODULE)hInst);

	return FALSE;
}

void XLTool::getAvailableListenPort(int & port)
{
	for (port = PORT_START; port <= PORT_END; port++) {

		if (CheckTcpPort(port) | CheckUdpPort(port)) {
			continue;
		}
		else {
			break;
		}
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int _System(const char * cmd, char *pRetMsg, int msg_len)
{
	FILE * fp;
	char * p = NULL;
	int res = -1;
	if (cmd == NULL || pRetMsg == NULL || msg_len < 0)
	{
		printf("Param Error!\n");
		return -1;
	}
	if ((fp = _popen(cmd, "r")) == NULL)
	{
		printf("Popen Error!\n");
		return -2;
	}
	else
	{
		memset(pRetMsg, 0, msg_len);
		//get lastest result  
		while (fgets(pRetMsg, msg_len, fp) != NULL)
		{
			printf("Msg:%s", pRetMsg); //print all info  
		}

		if ((res = _pclose(fp)) == -1)
		{
			printf("close popenerror!\n");
			return -3;
		}
		pRetMsg[strlen(pRetMsg) - 1] = '\0';
		return 0;
	}
}


bool XLTool::getPortIsAvailable(int & port)
{
	string cmd = format("netstat -aon|findstr \"%d\"", port);
	char a8Result[128] = { 0 };
	int ret = 0;
	ret = _System(cmd.c_str(), a8Result, sizeof(a8Result));

	string resultStr = a8Result;
	return resultStr.empty();
	//return !(CheckTcpPort(port) | CheckUdpPort(port));
}

std::string XLTool::TimetoString(time_t* t)
{
	char ch[32];
	struct tm *tt;
	tt = localtime(t);

	sprintf(ch, "%d-%02d-%02d %02d:%02d:%02d", tt->tm_year + 1900, tt->tm_mon + 1, tt->tm_mday, tt->tm_hour, tt->tm_min, tt->tm_sec);
	return std::string(ch);
}
