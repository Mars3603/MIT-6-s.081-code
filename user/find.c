#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*
    strlen()：获取字符串的实际长度。
    strcmp(str1,str2) if equals ,return 0;
*/
void find(char* dir,char* file){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd=open(dir,0))<0){
        fprintf(2,"find: cannot open %s\n",dir);
        return;
    }
    /* 
        int fstat(int fd, struct stat *statbuf);
        返回文件信息，存储在statbuf指针中。
    */
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", dir);
        close(fd);
        return;
    }

    if(st.type !=T_DIR){
        fprintf(2,"find %s is not a directory\n",dir);
        close(fd);
        return;
    }

    if (strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2,"find: path too long\n");
        close(fd);
        return;
    }
    strcpy(buf, dir);
    p = buf+strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      // 如果是 . he .. 不递归执行。也不打印出来
      if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
          continue;
      // 添加路径名称
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;    // 字符串结束标志

      if(stat(buf, &st) < 0){
        fprintf(2,"find: cannot stat %s\n", buf);
        continue;
      }
      // 不要在 . he ..中递归执行， strcmp(x,x) 相等为0
      if(st.type == T_DIR ){
         find(buf,file);
      }
      else if(st.type==T_FILE && strcmp(de.name,file)==0){
          printf("%s\n",buf);
      }
    }
    //close(fd);
}

int main(int argc,char* argv[]){
    if(argc !=3){
        fprintf(2,"usage: find <dirName> <fileName>\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}