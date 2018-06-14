//
//  CharToLong.cpp
//  tools
//
//  Created by gaol on 2018/6/14.
//  Copyright © 2018年 navinfo. All rights reserved.
//

#include "CharToLongLong.h"

static union LongValue{
    char val[8];
    long long l;
} ml_t;

//C/C++ char* 转8字节整型
//java 转成 linux下的C/C++ src[0]是高端，src[7]是低端
long long charArrayToLongLong1(char src[8]) {

    int i=0,j=0;
    
    for(i=0;i<8;++i) {
        j = 8 - 1 - i;
        ml_t.val[i] = src[j];
    }
    printf("charToLongLong ml_t.l = %lld \r\n",ml_t.l);
    return ml_t.l;
}

//java 转成 linux下的C/C++ src[0]是高端，src[7]是低端
long long charArrayToLongLong2(char src[8]) {
    
    long long index = 0;
    
    index |= (((long long) src[0] & 0xff) << 56);
    index |= (((long long) src[1] & 0xff) << 48);
    index |= (((long long) src[2] & 0xff) << 40);
    index |= (((long long) src[3] & 0xff) << 32);
    index |= (((long long) src[4] & 0xff) << 24);
    index |= (((long long) src[5] & 0xff) << 16);
    index |= (((long long) src[6] & 0xff) << 8);
    index |= ((long long) src[7] & 0xff);
    
    return index;
}

//该接口src[0]是低端，src[7]是高端,
long long charArrayToLongLong3(char src[8]) {
    
    long long index = 0;
    index = (long long)*src;

    return index;
}

//该接口src[0]是低端，src[7]是高端,
long long charArrayToLongLong4(char src[8]) {
    
    long long index = 0;
    memcpy(&index,src,8);
    return index;
}

