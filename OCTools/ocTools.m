//
//  ocTools.m
//  CTools
//
//  Created by gaol on 2019/11/1.
//

#import "ocTools.h"

@implementation ocTools

-(NSData *)readFile {

    //文件路径
    NSString *path = [[NSBundle mainBundle] pathForResource:@"kkmklm" ofType:nil];
    NSData *reader=[NSData dataWithContentsOfFile:path];
    
    //读 0~100 字节数据
    NSData *netPCMData = [reader subdataWithRange:NSMakeRange(0, 100)];

    return netPCMData;
}

-(void)writefile:(NSData *)data
{
    NSArray *paths  = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask,YES);
    NSString *homePath = [paths objectAtIndex:0];
    
    NSString *filePath = [homePath stringByAppendingPathComponent:@"export-pcm.caf"];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    if(![fileManager fileExistsAtPath:filePath]) //如果不存在
    {
        [data writeToFile:filePath atomically:YES];
    }
    else//追加写入文件，而不是覆盖原来的文件
    {
        //        NSLog(@"-------文件存在，追加文件----------");
        NSFileHandle *fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:filePath];
        
        [fileHandle seekToEndOfFile];  //将节点跳到文件的末尾
        
        [fileHandle writeData:data]; //追加写入数据
        
        [fileHandle closeFile];
    }
}


-(void) getCostTime {
    struct timeval t_start,t_end;
    long cost_time = 0;
    
    //get start time
    gettimeofday(&t_start, NULL);
    long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;

    //get end time
    gettimeofday(&t_end, NULL);
    long end = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;

    cost_time = end - start;
    
    NSLog(@"AAAAASD ***** costTime = %ld",cost_time);
}

@end
