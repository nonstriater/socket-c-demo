//
//  client.c
//  socket-mac-cmd
//
//  Created by ranwenjie on 2020/6/18.
//  Copyright © 2020年 didi. All rights reserved.
//

#include <sys/socket.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8886
#define SERVER_IP "127.0.0.1"
#define MAXLINE 4096

//客户端，阻塞IO
void socket_client(){
    
    int s;
    struct sockaddr_in addr;
    if((s = socket(AF_INET,SOCK_STREAM,0))<0){
        printf("socket");
        exit(1);
    }
    /* 填写sockaddr_in结构*/
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    /* 尝试连线*/
    if(connect(s,&addr,sizeof(addr)) < 0){
        // int i = errno;
        printf("connect err\n");
        exit(1);
    }
    
    while (1) {
        
        //发送数据
        char buffer[256] = {"hello,world"};
        int ret = send(s, buffer, sizeof(buffer), 0);
        printf("ret=%d\n",ret);
        
        sleep(10);
    }
    
    
    /* 接收由server端传来的信息*/
//    recv(s,buffer,sizeof(buffer),0);
//    printf("%s\n",buffer);
//    while(1){
//        bzero(buffer,sizeof(buffer));
//        /* 从标准输入设备取得字符串*/
//        read(STDIN_FILENO,buffer,sizeof(buffer));
//        /* 将字符串传给server端*/
//        if(send(s,buffer,sizeof(buffer),0)<0){
//            printf("send");
//            exit(1);
//        }
//    }
}

int main(int argc, const char * argv[]) {
    
    socket_client();

    return 0;
}



