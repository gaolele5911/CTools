
// Merlineye.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Merlineye.h"
#include "ws.h"
#include "wss.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool g_is_https = false;
CMerlineyeApp theApp;

BEGIN_MESSAGE_MAP(CMerlineyeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CMerlineyeApp::CMerlineyeApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

//生成dmp文件
LONG WINAPI ExcepthionHandler(EXCEPTION_POINTERS* pExceptionPointers)
{
	BOOL bMiniDumpSuccessful;
	TCHAR szFileName[MAX_PATH];
	TCHAR szFileDate[MAX_PATH];
	HINSTANCE   hOcx = ::GetModuleHandle(_T(""));
	GetModuleFileName(hOcx, szFileName, MAX_PATH);
	TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	_tsplitpath(szFileName, drive, dir, fname, ext);

	_tcscpy(szFileName, drive);
	_tcscat(szFileName, dir);

	HANDLE hDumpFile;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
	SYSTEMTIME		sys_time;
	GetLocalTime(&sys_time);
	wsprintf(szFileDate, _T("Melineye_%04d%02d%02d%02d%02d%02d.dmp"),
		sys_time.wYear, sys_time.wMonth, sys_time.wDay, sys_time.wHour,
		sys_time.wMinute, sys_time.wSecond);
	_tcscat(szFileName, szFileDate);

	hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	if (hDumpFile != INVALID_HANDLE_VALUE)
	{
		ExpParam.ThreadId = GetCurrentThreadId();
		ExpParam.ExceptionPointers = pExceptionPointers;
		ExpParam.ClientPointers = TRUE;
		bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			hDumpFile, MiniDumpNormal,
			&ExpParam, NULL, NULL);
		CloseHandle(hDumpFile);
	}
	return  EXCEPTION_EXECUTE_HANDLER;
}

/***************************************************************************** /
/* 函数名：GetProcessCnt()   根据进程名获取进程个信息                          */
/* 参数：                                                                    */
/*     string ProcName , 进程名，输入参数                                    */
/*     int &o_ProcessID, 进程名对应的进程ID，输出参数                        */
/* 返回值：                                                                  */
/*     >=0,找到的进程个数，成功      -1：未找到。                            */
/*****************************************************************************/
int GetProcessCnt(string ProcName, int &o_ProcessID)
{
	int iFindCnt = 0;
	o_ProcessID = 0;
	//取得进程名
	//string PName=ProcessNames[PNameIndex]; 
	std::transform(ProcName.begin(), ProcName.end(), ProcName.begin(), ::tolower);
	PROCESSENTRY32 process;
	process.dwSize = sizeof(process);
	//获取本机的所有进程快照
	HANDLE hToolHelp = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hToolHelp == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot调用失败!");
		return 0;
	}
	BOOL bMore = ::Process32First(hToolHelp, &process);

	while (bMore)
	{
		int i = 0;
		while (process.szExeFile[i])
		{
			process.szExeFile[i] = tolower(process.szExeFile[i]);
			i++;
		}
		//循环查找进程中是否有待找进程
		USES_CONVERSION;
		if (0 == strcmp(W2A(process.szExeFile), ProcName.c_str()))
		{
			//获得进程ID
			o_ProcessID = process.th32ProcessID;
			iFindCnt++;
			//break;
		}
		bMore = ::Process32Next(hToolHelp, &process);
	}
	::CloseHandle(hToolHelp);
	if (0 == o_ProcessID)
	{
		//printf("未找到进程%s!\n",ProcName.c_str());
		return 0;
	}


	return iFindCnt;
}

BOOL CMerlineyeApp::InitInstance()
{
	//生成dmp文件
	LPTOP_LEVEL_EXCEPTION_FILTER cb = SetUnhandledExceptionFilter(&ExcepthionHandler);

	//判断进程是否存在
	int processid;
	int count = 0;
	count += GetProcessCnt("EasyLinkVideo.exe", processid);	
	count += GetProcessCnt("EASYLI~1.EXE", processid); 	//win7下可能会出现简写进程名称
	if (count > 1)
	{
		getchar();
		return 0;
	}

	//判断进程是否已经存在（windows常用方法）
	HANDLE hMutex = CreateMutex(NULL, TRUE, L"MelieyePlayerMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		logerr("����Melineye Playerʧ�ܣ� err=%d", hMutex);
		return 0;
	}

	//获取可用接口
	int port=8899
	XLTool::getAvailableListenPort(port);

	//判断端口是否可用
	if (!XLTool::getPortIsAvailable(port)){
		return 0;
	}

	bool g_is_https = false;
	//https
	if (g_is_https) {
		TSockets::instance()->run(port);
		TSockets::instance()->m_thread_messsage_loop = websocketpp::lib::make_shared<websocketpp::lib::thread>(bind(&broadcast_servers::process_messages, TSockets::instance()));
	}
	else {
		//http
		TSocket::instance()->run(port);
		TSocket::instance()->m_thread_messsage_loop = websocketpp::lib::make_shared<websocketpp::lib::thread>(bind(&broadcast_server::process_messages, TSocket::instance()));
	}
	
	//3 time_t 转 string
	time_t timep;
	time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
	string strTime = XLTool::TimetoString(&timep)

	//string 转time
	USES_CONVERSION;
	m_start_time = XLTool::String2Time(A2W(strTime.c_str()));

	//
	time_t timep1;
	time(&timep1); //获取从1970至今过了多少秒，存入time_t类型的timep
	CString start_time = XLTool::Time2String(timep1);

	//输出到visual studio
	Trace("<<<【消息发送失败】%s, id = %d\n", a.data.c_str(), it->second);

	//读ini文件字段
	TCHAR	mStreamType[MAX_PATH];
	GetPrivateProfileString(_T("openFlag"), _T("openFlag"), _T("udp"), mStreamType, sizeof(mStreamType), XLTool::GetConfigureFilePath());
	m_stream_type = CT2A(mStreamType);

	//
	char szWholePath[MAX_PATH] = { 0 };
	CString strWholePath = "jkjhhiuhh";
	XLTool::Tchar2Char(strWholePath, szWholePath);

	//
	char szPicName[MAX_PATH] = { 0 };
	sprintf(szPicName, "%s\\%s.jpg", "aa", "bbb").c_str(),
	TCHAR wszPicName[256] = { 0 };
	XLTool::Char2TChar(szPicName, wszPicName);
	CString str = wszPicName;

	//
	char szPicName[MAX_PATH] = { 0 };
	TCHAR wszPicName[256] = { 0 };
	XLTool::Char2TChar(szPicName, wszPicName);

	//
	CString strWholePath = "D:\\" + "TVRecord" + "\\";
	if (!XLTool::SuperMkDir(strWholePath)) //  检查路径是否存在，不存在则创建   by:hanyl
	{
		//路径配置有误，请重新配置
	}

	//
	string tmp = "ppoop";
	CString name;
	XLTool::UnicodeToChinese(tmp, name);

	//
	string str = "llll";
	wstring wStr = XLTool::s2ws((*itor)->GetFileName())

	//
	char szPicName[MAX_PATH] = { 0 };
	TCHAR wszPicName[256] = { 0 };
	XLTool::Char2TChar(szPicName, wszPicName);
	CString str = wszPicName;
	str.Replace(_T("\\\\"), _T("\\"));
	str.Replace(_T("\\"), _T("/"));
	XLTool::Tchar2Char(str.GetBuffer(0), szPicName);

	return TRUE;
}

