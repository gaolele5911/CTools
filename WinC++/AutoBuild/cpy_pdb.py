#coding=gbk
import os
import shutil
import subprocess
import sys

src_dir="../../"
def cpy(srcfile,dstfile):
    srcfile = src_dir + srcfile
    if not os.path.isfile(srcfile):
        print "%s not exist!"%(srcfile)
    else:
        fpath,fname=os.path.split(dstfile)    #分离文件名和路径
        if not os.path.exists(fpath):
            os.makedirs(fpath)                #创建路径
        shutil.copyfile(srcfile,dstfile)      #复制文件
        print "copy %s -> %s"%( srcfile,dstfile)

def cpyfile(pdb_path):
    cpy("XLWebControl\\Bin2\\EasyLinkVideo.pdb", pdb_path + "EasyLinkVideo.pdb")
    cpy("XLWebControl\\Bin2\\stream_addin.pdb", pdb_path + "stream_addin.pdb")
    cpy("XLEasyLinkVideoMonitor\\Bin\\XLMonitor.pdb", pdb_path + "XLMonitor.pdb")

	
if __name__ == "__main__":
    #pdb_path = os.path.split(os.path.realpath(__file__))[0] + "\\PDB\\" + sys.argv[1] + "\\";
    #print "pdb输出目录：" + pdb_path;
    pdb_path = os.path.split(os.path.realpath(__file__))[0] + "/PDB/" + sys.argv[1] + "/";
    os.mkdir(pdb_path)
    cpyfile(pdb_path)
