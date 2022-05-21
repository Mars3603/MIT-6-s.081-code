#include "kernel/types.h"
#include "user/user.h"
/*
父进程应该向子进程发送一个字节;子进程应该打印“<pid>: received ping”，其中<pid>是进程ID，
并在管道中写入字节发送给父进程，然后退出;父级应该从读取从子进程而来的字节，打印“<pid>: received pong”，
然后退出。

tips1：
    C 库函数int sprintf(char *str, const char *format, ...)将格式化输出发送到str指向的字符串。
    示例：sprintf(buf, "%s %d %s\r\n", "HTTP/1.1", status, title);

tips2:
pid_t getpid(void); //返回调用进程的PID号
*/
int main(int argc,char* argv[]){

    // 管道的特性是单向流动，所以创建两个管道，一个从父进程向子进程写。一个从子进程向父进程写
    int fdc2p[2];
    int fdp2c[2];
    int pid=0;
    char buf[8];
    pipe(fdp2c);
    pipe(fdc2p);
    pid=fork();
    if(pid==0){
        close(fdp2c[1]);
        close(fdc2p[0]);
        read(fdp2c[0],buf,4); // 子进程 读取 fdp2c中的数据 
        printf("%d: received %s\n",getpid(),buf);
        write(fdc2p[1],"pong",4);
        exit(0);
    }else if(pid>0){
        close(fdp2c[0]);
        close(fdc2p[1]);
        write(fdp2c[1],"ping",strlen("ping"));
        wait((int *)0);        
        read(fdc2p[0],buf,sizeof(buf)); // read to standard output.
        printf("%d: received %s\n",getpid(),buf);
        exit(0);
    }else{
        fprintf(2,"fork() error");
        exit(1);
    }
}