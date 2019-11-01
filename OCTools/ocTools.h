//
//  ocTools.h
//  CTools
//
//  Created by gaol on 2019/11/1.
//

#import <Foundation/Foundation.h>

@interface ocTools : NSObject

-(NSData *)readFile;
-(void)writefile:(NSData *)data;
-(void) getCostTime;

@end
