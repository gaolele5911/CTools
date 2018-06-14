//
//  CharToLong.hpp
//  C/C++ char* 转8字节整型
//
//  Created by gaol on 2018/6/14.
//  Copyright © 2018年 navinfo. All rights reserved.
//

#ifndef CharToLongLong_hpp
#define CharToLongLong_hpp

#include <stdio.h>
#include <string.h>


//java 转成 linux下的C/C++ src[0]是高端，src[7]是低端
long long charArrayToLongLong1(char src[8]);
//java 转成 linux下的C/C++ src[0]是高端，src[7]是低端
long long charArrayToLongLong2(char src[8]);
//该接口src[0]是低端，src[7]是高端,
long long charArrayToLongLong3(char src[8]);
//该接口src[0]是低端，src[7]是高端,
long long charArrayToLongLong4(char src[8]);

#endif /* CharToLongLong_h */
