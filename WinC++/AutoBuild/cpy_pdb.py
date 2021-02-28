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
        fpath,fname=os.path.split(dstfile)    #�����ļ�����·��
        if not os.path.exists(fpath):
            os.makedirs(fpath)                #����·��
        shutil.copyfile(srcfile,dstfile)      #�����ļ�
        print "copy %s -> %s"%( srcfile,dstfile)

def cpyfile(pdb_path):
    cpy("XLWebControl\\Bin2\\EasyLinkVideo.pdb", pdb_path + "EasyLinkVideo.pdb")
    cpy("XLWebControl\\Bin2\\stream_addin.pdb", pdb_path + "stream_addin.pdb")
    cpy("XLEasyLinkVideoMonitor\\Bin\\XLMonitor.pdb", pdb_path + "XLMonitor.pdb")

	
if __name__ == "__main__":
    #pdb_path = os.path.split(os.path.realpath(__file__))[0] + "\\PDB\\" + sys.argv[1] + "\\";
    #print "pdb���Ŀ¼��" + pdb_path;
    pdb_path = os.path.split(os.path.realpath(__file__))[0] + "/PDB/" + sys.argv[1] + "/";
    os.mkdir(pdb_path)
    cpyfile(pdb_path)
