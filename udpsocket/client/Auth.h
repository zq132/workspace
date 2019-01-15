#ifndef _AUTH_H_
#define _AUTH_H_

#include <iostream>

/**
 * 回调函数参数：
 *     0:代表注册码验证失败或监听到8800端口的数据包从本机发出,程序需要退出
 *     1:代表程序正常运行
 *     2:代表长时间未收到服务端发出的心跳包
 *     3:读取注册码失败
*/

enum class ExitSign{
    Listening_port,//监听到端口退出
    RUN,//正常运行
    UNRegistered,//未注册
    Disconnect_serv//与服务断开连接
};

typedef int (*ClientBack)(ExitSign sign);
class Auth
{
public:
    static Auth& GetInstance();
    void Destroy();
    /**
     * 启动线程
     * 参数： 回调函数
    */
    void RunClient(ClientBack ptr);
    void SetTime(struct timeval *tmsec);
};

#endif