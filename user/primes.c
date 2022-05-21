#include "kernel/types.h"
#include "user/user.h"

/*
    使用管道编写prime sieve(筛选素数)的并发版本 (多进程)
您的目标是使用pipe和fork来设置管道。第一个进程将数字2到35输入管道。
对于每个素数，您将安排创建一个进程，该进程通过一个管道从其左邻居读取数据，
并通过另一个管道向其右邻居写入数据。由于xv6的文件描述符和进程数量有限，因此第一个进程可以在35处停止。

tips： 使用 dup(fd) + close(fd) ,可以避免文件描述符耗尽。
*/

// 文件描述符重定向
void mapping(int n,int pfd[]){
    // 文件描述符n(0,1)指向管道的 读 or 写端。pfd[0] or pfd[1]
    close(n);
    dup(pfd[n]);

    // 关闭管道中的描述符，节省资源。
    close(pfd[0]);
    close(pfd[1]);
}

void primes(){
    int previous,next;
    int fd[2];
    // read(0,&previous,sizeof(int))!=sizeof(int)
    if(read(0,&previous,sizeof(int))){
        printf("prime %d\n",previous);
        // 创建管道
        pipe(fd);
        if(fork()==0){
            // 子进程将管道的写端口映射到 描述符 1 上
            mapping(1,fd);
            while(read(0,&next,sizeof(int))){
                if(next%previous!=0){
                    write(1,&next,sizeof(int));
                }
            }
            exit(0); // 子进程退出
        }else{
            wait((int*)0);
            mapping(0,fd);
            primes();

            exit(0); // 父进程退出
        }
    }
}

int main(int argc,char* argv[]){
    int fd[2];
    pipe(fd);
    if(fork()==0){
        // 子进程将管道的写端口映射到 标准输入上----> close(1);dup(fd[1]);
        mapping(1,fd);
        for(int i=2;i<=35;i++){
            // write(1,...)将标准输出中的数据写入管道
            write(1,&i,sizeof(int));
        }
        exit(0);
    }else{
        wait((int *)0);
        mapping(0,fd); // fd[0]=0 标准输入
        primes();
    }
    exit(0);
}