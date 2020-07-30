//
//  select.c
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

#include "select.h"

#define PORT 8886
#define SERVER_IP "127.0.0.1"

//select 多路复用
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
    
    //服务器套接字fd
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    
    listen(server_sockfd, 5);
    
    //将服务器端socket加入到FD集合中
    FD_ZERO(&readfds);
    //把server 监听socket 加入 fdset, 当有客户端连接请求时，select()能检查到
    FD_SET(server_sockfd, &readfds);
    
    while(1)
    {
        char ch;
        int nread;
        int fd;
        
        testfds = readfds; //copy 到  testfds

        printf("server waiting...\n");
        printf("fd_setsize=%d\n",FD_SETSIZE);//1024
        
        /*无限期阻塞，并测试文件描述符变动 */
        //可读，可写，异常等事件的描述符集合
        //timeout 为 0 表示立即返回
        //timeout NULL 表示等待无限长时间
        result = select(FD_SETSIZE, &testfds, (fd_set *)0,(fd_set *)0, (struct timeval *) 0);
        //result = select(FD_SETSIZE, &testfds, (fd_set *)0,(fd_set *)0, (struct timeval *) 3);
        //result = select(FD_SETSIZE, &testfds, (fd_set *)0,(fd_set *)0, (struct timeval *) NULL);
        
        if(result < 1)
        {
            printf("select error\n");
            exit(1);
        }
        
        /*扫描所有的文件描述符*/
        for(fd = 0; fd < FD_SETSIZE; fd++)
        {
//            if(FD_ISSET(fd,&writefds))
//            {
//                
//            }
            
            /*找到相关文件描述符*/
            if(FD_ISSET(fd,&testfds))
            {
                /*判断是否为服务器套接字，是则表示为客户请求连接。*/
                if(fd == server_sockfd)
                {
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd,
                                           (struct sockaddr *)&client_address,
                                           &client_len);//[0~1024]
                    FD_SET(client_sockfd, &readfds);//将客户端socket加入到集合中
                    printf("adding client on fd %d\n", client_sockfd);
                }
               
                /*客户端socket中有数据请求时, 读取数据*/
                else
                {
                    printf("read data\n");
                    
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

