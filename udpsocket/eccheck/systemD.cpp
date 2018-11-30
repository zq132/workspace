#include <stdio.h>
// #include <iostream>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <thread>

#include "server.h"

int main(){
    int err,signo;
    sigset_t mask;

    std::thread serv(RunServer);
    serv.detach();

    syslog(LOG_INFO, "Starting service......");
    while(1) {
        // 捕捉信号
        err = sigwait(&mask,&signo);
        if( err != 0  ){
            syslog(LOG_INFO,"sigwait error : %d",err);
            exit(1);
        }
        switch (signo){
            default :
                syslog(LOG_INFO,"unexpected signal %d \n",signo);
                break;
            case SIGTERM:
                syslog(LOG_INFO,"Received SIGTERM. exit service......");
                CloseServer();
                serv.join();
                exit(0);
        }

    }

}

