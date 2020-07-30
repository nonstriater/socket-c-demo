//
//  fd_set.c
//  socket-mac-cmd
//
//  Created by ranwenjie on 2020/7/2.
//  Copyright © 2020年 didi. All rights reserved.
//

#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

void test1();
void test2();
void test3();

int main(int argc, char **argv){
    
    //test1();
    //test2();
    test3();
    
}

void test1(){
    fd_set fdset;
    FD_ZERO (&fdset);                          /*清空集合中所有的元素*/
    
    FD_SET(STDOUT_FILENO,&fdset);//0x0000 0010              /*设置stdout（=1），使集合中包含stdout*/
    FD_SET(STDERR_FILENO,&fdset);//0x0000 0110              /*设置stderr（=2），使集合中包含stdout*/
    
    if(FD_ISSET(STDOUT_FILENO,&fdset)!=0)      /*测试stdout是否包含在集合中*/
        printf("stdout has been set\n");
    else
        printf("stdout has not been set\n");
    
    FD_CLR(STDOUT_FILENO,&fdset); // 0x0000 0100             /*从位向量中清除stdout*/
    
    if(FD_ISSET(STDOUT_FILENO,&fdset)!=0)      /*再次测试*/
        printf("stdout has been set\n");
    else
        printf("stdout has not been set\n");
    
}

void test2(){
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(0, &fdset); //1
    FD_SET(1, &fdset); //3 (0x0011)
    FD_SET(2, &fdset); //5 (0x0111)
    FD_SET(3, &fdset); //15 (0x1111)
    FD_SET(7, &fdset); //143 (0x10001111)
    int isset = FD_ISSET(3, &fdset);  //返回8
    printf("isset = %d\n", isset);
    FD_CLR(3, &fdset); //135 (0x1000111)
    isset = FD_ISSET(3, &fdset);
    printf("isset = %d\n", isset);
}

void test3(){
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(31, &fdset); //1
    FD_SET(63, &fdset); //1
    FD_SET(1023, &fdset); //1
}




