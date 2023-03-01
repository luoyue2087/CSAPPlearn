#include "include/csapp.h"

void doit(int connfd);
void read_requesthdrs(rio_t *rp);
void serve_static(int fd, char *filename, int filesize);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void get_filetype(char *filename, char *filetype);
int parse_uri(char *uri, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv){
    //检查参数是否合理
    if(argc!=2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        // ./include/../tiny 输出：usage: ./include/../tiny <port>
        //argv[0] 会显示第一个参数的所有信息
        exit(1);
    }

    int listenfd, connfd, port;
    struct sockaddr_in clientaddr;
    port = atoi(argv[1]);
    listenfd = Open_listenfd(port);

    //listen
    
    /*
    The identifier true is not declared by default. 
    To use it, two solutions :
        Compile in C99 and include <stdbool.h>.
        Define this identifier by yourself.
    */
    for(;;){
        int addrlen = sizeof(clientaddr);
        //accept
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&addrlen);
        
        doit(connfd);

        //close
        Close(connfd);
    }

    return 0;
}

void doit(int fd){
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE],uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    //将客户端的信息存储到自定义的 rio_t 结构体中
    Rio_readinitb(&rio, fd);
    //读取一行数据到 rio_t 结构体中
    Rio_readlineb(&rio, buf, MAXLINE);

    sscanf(buf, "%s %s %s", method, uri, version);//GET / HTTP/ 1 .1
    // 检查请求的方法
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implement","Tiny does not implement this method");
        return;
    }
    // 继续读取数据
    read_requesthdrs(&rio);
    
    /* Parse URI from GET request */
    is_static = parse_uri(uri, filename, cgiargs);
    if (stat(filename, &sbuf) < 0) {
        // stat通过文件名filename获取文件信息，并保存在buf所指的结构体stat中
        clienterror(fd, filename, "404", "Not found", "Tiny couldn't read the file");
        return;
    }


    // 是执行加载静态网页还是动态网页
     if (is_static) {
        /* Server static content */
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
            return ;
        }
        serve_static(fd, filename, sbuf.st_size);
    }
    else {
         /* Server dynamci content */
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
            return;
        }
        serve_dynamic(fd, filename, cgiargs);
    }
}

/*找不到文件时，网页提示信息*/
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
     char buf[MAXLINE], body[MAXBUF];

     /* Build the HTTP response body */
     sprintf(body, "<html><title>Tiny Error</title>");
     sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
     sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
     sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
     sprintf(body, "%s<hr><em>The Tiny Web server </em>\r\n", body);

     /* Print the HTTP response */
     sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
     Rio_writen(fd, buf, strlen(buf));
     sprintf(buf, "Content-type: text/html\r\n");
     Rio_writen(fd, buf, strlen(buf));
     sprintf(buf, "Content-length: %d\r\n\r\n",(int)strlen(body));
     Rio_writen(fd, buf, strlen(buf));
     Rio_writen(fd, body, strlen(body));
}

/*
continue read line data
*/
void read_requesthdrs(rio_t *rp)
{
     char buf[MAXLINE];
      Rio_readlineb(rp ,buf, MAXLINE);
      while(strcmp(buf, "\r\n"))
          Rio_readlineb(rp, buf, MAXLINE);
      return ;
}

//将客户端请求的文件信息获取，并返回给指针参数
int parse_uri(char *uri, char *filename, char *cgiargs)
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) { /* Static content*/
        strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri)-1] == '/')
            strcat(filename, "home.html");
        return 1;
    }
    else { /* Dynamic content*/
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        }
        else
            strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}

void serve_static(int fd, char *filename, int filesize)
{
     int srcfd;
     char *srcp, filetype[MAXLINE], buf[MAXBUF];

     /* Send reponse headers to client */
     get_filetype(filename, filetype);
     sprintf(buf, "HTTP/1.0 200 OK\r\n");
     sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
     sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
     sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
     Rio_writen(fd, buf, strlen(buf));

     /* Send reqponse body to client */
     srcfd = Open(filename, O_RDONLY, 0);
     // Mmap将一个文件或者其它对象映射到进程的地址空间,实现不同进程间的文件共享
     // 完成了对文件的操作而不必再调用read,write等系统调用函数。
     srcp = Mmap(0, filesize,  PROT_READ, MAP_PRIVATE, srcfd, 0);
     Close(srcfd);
     Rio_writen(fd, srcp, filesize);
     Munmap(srcp, filesize);
}

void get_filetype(char *filename, char *filetype)
{
     if (strstr(filename, ".html"))
         strcpy(filetype, "text/html");
     else if (strstr(filename, ".gif"))
         strcpy(filetype, "image/gif");
     else if (strstr(filename, ".jpg"))
         strcpy(filetype, "image/jpeg");
     else
         strcpy(filetype, "text/plain");
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
     char buf[MAXLINE], *emptylist[] = { NULL };

     /* Return first part of HTTP response*/
     sprintf(buf, "HTTP/1.0 200 OK\r\n");
     Rio_writen(fd, buf, strlen(buf));
     sprintf(buf,"Server: Tiny Web Server\r\n");
     Rio_writen(fd, buf, strlen(buf));

     if (Fork() == 0) {
        setenv("QUERY_STRING", cgiargs, 1);//change or add an environment variable
        Dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, environ);
     }
     Wait(NULL);
}
// http://192.168.123.86:20876/cgi-bin/add?1&2