#include "kernel/types.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/param.h"

/*
编写一个简化版UNIX的xargs程序:
它从标准输入中按行读取，并且为每一行执行一个命令，将行作为参数提供给命令。
    echo hello three | xargs echo two | xargs echo one 
输出： one. two , hello three. 
*/
#define MAXN 1024

int main(int argc,char* argv[]){
    if(argc<2){
        fprintf(2,"usage: xargs command\n");
        exit(1);
    }
    // 建立一个字符数组，存储从标准输入获取的命令行参数
    char* argvs[MAXARG];
    int index=0;
    // 将 argv[0]= xargs 覆盖掉，直接存 参数
    for(int i=1;i<argc;i++){
        argvs[index++]=argv[i];
    }
    //从标准输入中读取一行
    int n;
    char buf[MAXN]={"\0"};
    while((n=read(0,buf,sizeof(buf)))>0){
        // 因为可能有多行输入，所以这里需要 将temp定义到while循环中。
        char temp[MAXN]={"\0"};
        //将标准输入中 获得的数据 作为xargs的参数。 
        argvs[index++]=temp;
        for(int i=0;i<n;i++){
            if(buf[i]=='\n'){
                // 读取到换行符，就该执行 xargs 这个程序了。
                if(fork()==0){
                    exec(argv[1],argvs);
                }
                wait((int *)0);
            }else{
                temp[i] = buf[i];
            }
        }
    }
    exit(0);
}