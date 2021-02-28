#coding=gbk
import os
import shutil
import subprocess

#src_dir="D:\\Melineye_VNPS\\Product\\"
#pkg_dir="D:\\Melineye_VNPS打包工具\\客户端\\files\\"

src_dir="../../"
pkg_dir="./files/"

def cpy(srcfile,dstfile):
    srcfile = src_dir + srcfile
    dstfile = pkg_dir + dstfile
    if not os.path.isfile(srcfile):
        print "%s not exist!"%(srcfile)
    else:
        fpath,fname=os.path.split(dstfile)    #分离文件名和路径
        if not os.path.exists(fpath):
            os.makedirs(fpath)                #创建路径
        shutil.copyfile(srcfile,dstfile)      #复制文件
        print "copy %s -> %s"%( srcfile,dstfile)

def cpyfile():
    print u"正在复制文件..."
    cpy("XLWebControl\\Bin2\\Demo.html", "JS_Code\\Demo.html")
    cpy("XLWebControl\\Bin2\\DemoIframe.html", "JS_Code\\DemoIframe.html")
    cpy("XLWebControl\\Bin2\\demo多窗口9路播放.html", "JS_Code\\demo多窗口9路播放.html")
    cpy("XLWebControl\\Bin2\\demo多窗口9路播放-演示.html", "JS_Code\\demo多窗口9路播放-演示.html")
    cpy("XLWebControl\\Bin2\\Demo多个窗口独立播放.html", "JS_Code\\Demo多个窗口独立播放.html")
    cpy("XLWebControl\\Bin2\\Demo多个窗口精简版4个.html", "JS_Code\\Demo多个窗口精简版4个.html")
    cpy("XLWebControl\\Bin2\\Demo模拟12路轮询.html", "JS_Code\\Demo模拟12路轮询.html")
    cpy("XLWebControl\\Bin2\\dist\\eventBus.js", "JS_Code\\dist\\eventBus.js")
    cpy("XLWebControl\\Bin2\\dist\\webPlugin.js", "JS_Code\\dist\\webPlugin.js")
    cpy("XLWebControl\\Bin2\\EasyLinkVideo.exe", "Bin\\EasyLinkVideo.exe")
    cpy("XLWebControl\\Bin2\\mk_api.dll", "Bin\\mk_api.dll")
    cpy("XLWebControl\\Bin2\\stream_addin.dll", "Bin\\stream_addin.dll")
    cpy("XLEasyLinkVideoMonitor\\Bin\\XLMonitor.exe", "Bin\\ELMonitor.exe")
    cpy("XLWebControl\\Bin2\\VideoConfig.ini", "Bin\\VideoConfig.ini")

if __name__ == "__main__":
    cpyfile()
