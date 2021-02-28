#coding=gbk
import os
import subprocess
import shutil
import re
import time
print("Melineye_VNPS web插件正在编译")
rootdir = "../../../"
count_total = 0
count_success = 0
count_failed = 0

ok=[

]

slns=[
"XLEasyLinkVideoMonitor\\XLMonitor\\XLMonitor.sln",
"XLWebControl\\Merlineye\\Merlineye.sln"
]

def error(msg):
    print("***Build Error***")
    print(msg)
    time.sleep(60)

def build(arr):
    global count_total
    global count_success
    global count_failed
    for i in range(0, len(arr)):
        path = rootdir + "\\" + arr[i]
        if os.path.exists(path):
            is_success = True
            name = os.path.splitext(os.path.split(path)[1])[0]
            print("=>[" + str(i + 1) + "/" + str(len(arr)) + "]Start Build:" + name)
            print("Path:" + path)
            print("Building ...")
#            cmd = "cmd.exe /c" + " build_by_vs2010.bat " + path + " " + name \
#                if(os.path.splitext(path)[1] == ".sln") else \
#                "cmd.exe /c" + " build_by_vc6.bat " + path + " " + name
            cmd=""
            if(os.path.splitext(path)[1] == ".sln"):
                if name == "Merlineye":
                    cmd = "cmd.exe /c" + " build_by_vs2015.bat " + path + " " + name
                if name == "XLMonitor":
                    cmd = "cmd.exe /c" + " build_by_vs2015.bat " + path + " " + name
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            curline = p.stdout.readline()
            while(curline != b''):
                if (curline.find("error C") != -1) or (curline.find("Error:") != -1 ) or(curline.find("error LNK") != -1):
                    is_success = False
                    error(curline)
                m = re.search("\d+ error", curline)
                if m:
                    err = repr(m.group())
                    errcount = re.search("\d+", err).group()
                    if errcount != '0':
                        is_success = False
                        error(curline)
                    else:
                        print(curline)
                curline = p.stdout.readline()
            if is_success:
                count_success = count_success + 1
                print("Build Finish:" + name + " 总共：" + str(count_total) + " 成功：" + str(count_success) + " 失败：" + str(
                    count_failed))
                print(" ")
            else:
                count_failed = count_failed + 1
                print("Build Finish:" + name + " 总共：" + str(count_total) + " 成功：" + str(count_success) + " 失败：" + str(
                    count_failed))
                print(" ")
                exit(-1);
        else:
            count_failed = count_failed + 1
            print("File path not exists:" + path + " 总共：" + str(count_total) + " 成功：" + str(count_success) + " 失败：" + str(count_failed))
            time.sleep(60)

#统计解决方案
def countslns():
    for root, dirs, files in os.walk(rootdir):
        for name in files:
            if os.path.splitext(name)[1] == ".sln" or os.path.splitext(name)[1] == ".dsp":
                print(os.path.join(root, name))
    time.sleep(60)

if __name__ == '__main__':
    #countslns()
    #logpath = os.getcwd() + "\Log"
    #logpath="D:\\Melineye_VNPS\\Product\\Script\\Log"
    #logpath="../../../../../../Product/Script/Log"
    logpath="./Log"
    if os.path.exists(logpath):
        shutil.rmtree(logpath)
        time.sleep(1)
    os.mkdir(logpath)

    count_total += len(slns);
    print("参与编译项目总数：" + str(count_total))
    #build("Video_Client", test) # 对于编译不过的可以单独放到test里面测试
    build(slns)

    print("编译结束，总共：" + str(count_total) + " 成功：" + str(count_success) + " 失败：" + str(count_failed) )
    #time.sleep(60)
