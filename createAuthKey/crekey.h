#ifndef _CREKEY_H_
#define _CREKEY_H_

#include <iostream>
/**
 * 返回值改为int
 *     0:注册码文件生成
 *     1:序列号文件不存在或路径不对！
 *     2:类别代码错误！
*/
extern "C"{
    int CreateKey(char* src, char* dst, int type);
};


#endif
