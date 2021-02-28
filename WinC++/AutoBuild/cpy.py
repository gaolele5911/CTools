#coding=gbk
import os
import shutil
import subprocess

#src_dir="D:\\Melineye_VNPS\\Product\\"
#pkg_dir="D:\\Melineye_VNPS�������\\�ͻ���\\files\\"

src_dir="../../"
pkg_dir="./files/"

def cpy(srcfile,dstfile):
    srcfile = src_dir + srcfile
    dstfile = pkg_dir + dstfile
    if not os.path.isfile(srcfile):
        print "%s not exist!"%(srcfile)
    else:
        fpath,fname=os.path.split(dstfile)    #�����ļ�����·��
        if not os.path.exists(fpath):
            os.makedirs(fpath)                #����·��
        shutil.copyfile(srcfile,dstfile)      #�����ļ�
        print "copy %s -> %s"%( srcfile,dstfile)

def cpyfile():
    print u"���ڸ����ļ�..."
    cpy("XLWebControl\\Bin2\\Demo.html", "JS_Code\\Demo.html")
    cpy("XLWebControl\\Bin2\\DemoIframe.html", "JS_Code\\DemoIframe.html")
    cpy("XLWebControl\\Bin2\\demo�ര��9·����.html", "JS_Code\\demo�ര��9·����.html")
    cpy("XLWebControl\\Bin2\\demo�ര��9·����-��ʾ.html", "JS_Code\\demo�ര��9·����-��ʾ.html")
    cpy("XLWebControl\\Bin2\\Demo������ڶ�������.html", "JS_Code\\Demo������ڶ�������.html")
    cpy("XLWebControl\\Bin2\\Demo������ھ����4��.html", "JS_Code\\Demo������ھ����4��.html")
    cpy("XLWebControl\\Bin2\\Demoģ��12·��ѯ.html", "JS_Code\\Demoģ��12·��ѯ.html")
    cpy("XLWebControl\\Bin2\\dist\\eventBus.js", "JS_Code\\dist\\eventBus.js")
    cpy("XLWebControl\\Bin2\\dist\\webPlugin.js", "JS_Code\\dist\\webPlugin.js")
    cpy("XLWebControl\\Bin2\\EasyLinkVideo.exe", "Bin\\EasyLinkVideo.exe")
    cpy("XLWebControl\\Bin2\\mk_api.dll", "Bin\\mk_api.dll")
    cpy("XLWebControl\\Bin2\\stream_addin.dll", "Bin\\stream_addin.dll")
    cpy("XLEasyLinkVideoMonitor\\Bin\\XLMonitor.exe", "Bin\\ELMonitor.exe")
    cpy("XLWebControl\\Bin2\\VideoConfig.ini", "Bin\\VideoConfig.ini")

if __name__ == "__main__":
    cpyfile()
