//
//  main.c
//  socket-mac-cmd
//
//  Created by ranwenjie on 2019/9/12.
//  Copyright © 2019年 didi. All rights reserved.
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
//#include <mach/error.h>

#import "select.h"

int main(int argc, const char * argv[]) {
    
    //socket_client();
    
    //server_socket_block();
    //server_socket_nonblock();
    server_socket_multiplexing();
    
    
    return 0;
}

//服务端，阻塞IO
void server_socket_block(){
    
    int socket_fd, connect_fd;
    struct sockaddr_in servaddr;
    char buff[4096];
    int n;
    
    //初始化Socket
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: \n");
        exit(0);
    }
    //初始化 sockaddr_in
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(PORT);      //设置的端口为DEFAULT_PORT
    
    //将本地地址绑定到所创建的套接字上
    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error\n");
        exit(0);
    }
    //开始监听是否有客户端连接
    if( listen(socket_fd, 10) == -1){
        printf("listen socket error\n");
        exit(0);
    }
    
    printf("======waiting for client's request======\n");
    while(1){
        //阻塞直到有客户端连接，不然多浪费CPU资源。
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error");
            continue;
        }
        
        //接受客户端传过来的数据
        n = recv(connect_fd, buff, MAXLINE, 0);
        //向客户端发送回应数据
        if(!fork()){
            if(send(connect_fd, "Hello,you are connected!\n", 26,0) == -1)
                printf("send error");
            close(connect_fd);
            exit(0);
        }
        buff[n] = '\0';
        printf("recv msg from client: %s\n", buff);
        close(connect_fd);
    }
    close(socket_fd);
}


void set_nonblock(int socket) {
    int flags;
    flags = fcntl(socket,F_GETFL,0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

//void* readThread(void* threadid) {
//    while(1) {
//        numRead = recv(new_sd, in, 255, 0);
//
//        if(numRead > 0) {
//            cout<<"\n"<<in;
//            for(int i=0;i<strlen(in);i++)
//                in[i] = '\0';
//        }   //end if
//        else if(numRead < 0) {
//            printf("\nError reading %m", errno);
//            exit(1);
//        }
//
//    }   //end while
//}   //END READTHREAD

//服务端，非阻塞IO
void server_socket_nonblock(){
    
    int socket_fd, connect_fd;
    struct sockaddr_in servaddr;
    char buff[4096];
    int n;
    
    //初始化Socket
    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: \n");
        exit(0);
    }
    //初始化 sockaddr_in
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP); //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(PORT);      //设置的端口为DEFAULT_PORT
    
    //将本地地址绑定到所创建的套接字上
    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error\n");
        exit(0);
    }
    //开始监听是否有客户端连接
    if( listen(socket_fd, 10) == -1){
        printf("listen socket error\n");
        exit(0);
    }
    
    printf("======waiting for client's request======\n");
    while(1){
        //阻塞直到有客户端连接，不然多浪费CPU资源。
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error");
            continue;
        }
        
        //设置 nonblock IO fd
        set_nonblock(connect_fd);
        
        //create_thread();
        
        //接受客户端传过来的数据
        n = recv(connect_fd, buff, MAXLINE, 0);//设置过nonblock后，就不会阻塞在这里了，需要轮训
       
        buff[n] = '\0';
        printf("recv msg from client: %s\n", buff);
        close(connect_fd);
    }
    close(socket_fd);
}




