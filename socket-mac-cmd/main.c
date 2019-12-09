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
#define MAXLINE 4096
#define SERVER_IP "127.0.0.1"
//#include <mach/error.h>

void socket_client(){
    
    int s;
    struct sockaddr_in addr;
    char buffer[256];
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
        //printf("connect：%s", strerror(errno));
        exit(1);
    }
    
    /* 接收由server端传来的信息*/
    recv(s,buffer,sizeof(buffer),0);
    printf("%s\n",buffer);
    while(1){
        bzero(buffer,sizeof(buffer));
        /* 从标准输入设备取得字符串*/
        read(STDIN_FILENO,buffer,sizeof(buffer));
        /* 将字符串传给server端*/
        if(send(s,buffer,sizeof(buffer),0)<0){
            printf("send");
            exit(1);
        }
    }
}

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

//void create_thread(){
//    while(1){
//        recv()
//    }
//}


void server_socket_multiplexing(){
    
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int result;
    fd_set readfds, testfds;
    
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);//建立服务器端socket
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(PORT);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    
    listen(server_sockfd, 5);
    
    //将服务器端socket加入到FD集合中
    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);
    
    while(1)
    {
        char ch;
        int fd;
        int nread;
        testfds = readfds;
        printf("server waiting/n");
        
        /*无限期阻塞，并测试文件描述符变动 */
        result = select(FD_SETSIZE, &testfds, (fd_set *)0,(fd_set *)0, (struct timeval *) 0);
        if(result < 1)
        {
            printf("select error/n");
            exit(1);
        }
        
        /*扫描所有的文件描述符*/
        for(fd = 0; fd < FD_SETSIZE; fd++)
        {
            /*找到相关文件描述符*/
            if(FD_ISSET(fd,&testfds))
            {
                /*判断是否为服务器套接字，是则表示为客户请求连接。*/
                if(fd == server_sockfd)
                {
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd,
                                           (struct sockaddr *)&client_address, &client_len);
                    FD_SET(client_sockfd, &readfds);//将客户端socket加入到集合中
                    printf("adding client on fd %d/n", client_sockfd);
                }
                
                /*客户端socket中有数据请求时*/
                else
                {
            
                    printf("data");
                    
                    //
//                    ioctl(fd, FIONREAD, &nread);//取得数据量交给nread
//
//                    /*客户数据请求完毕，关闭套接字，从集合中清除相应描述符 */
//                    if(nread == 0)
//                    {
//                        close(fd);
//                        FD_CLR(fd, &readfds);
//                        printf("removing client on fd %d/n", fd);
//                    }
//
//                    /*处理客户数据请求*/
//                    else
//                    {
//                        read(fd, &ch, 1);
//                        sleep(5);
//                        printf("serving client on fd %d/n", fd);
//                        ch++;
//                        write(fd, &ch, 1);
//                    }
                }
            }
        }
    }
}



int main(int argc, const char * argv[]) {
    
    //server_socket_block();
    
    //server_socket_nonblock();
    server_socket_multiplexing();
    
    //socket_client();
    
    return 0;
}

