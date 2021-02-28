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



std::string XLTool::cmdExec(std::string cmd)
{
    std::string s;
    std::vector<std::string> args;
    std::string cmdSh = "sh";
    args.push_back("-c");
    args.push_back(cmd);
    Poco::Pipe outPipe;
    Poco::ProcessHandle ph = Poco::Process::launch(cmdSh, args, 0, &outPipe, 0);
    Poco::PipeInputStream istr(outPipe);
    int c = istr.get();
    while (c != -1) { s += (char) c; c = istr.get(); }
    return s;
}
