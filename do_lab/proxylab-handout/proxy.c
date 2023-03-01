#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
const int MAX_CACHE_SIZE = 1049000;
const int MAX_OBJECT_SIZE = 102400;

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
void *doit(void *connfd);
void read_requesthdrs(rio_t *rp);
void parse_uri(char *, char *, char *, char *);
void build_header(char *uri, char *hostname);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    if (argc != 2)
    {
        // proxy服务器的端口
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    listenfd = Open_listenfd(argv[1]);
    pthread_t tid;

    // 不断接收客户的连接请求
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // line:netp:tiny:accept
        printf("proxy: new client\n");
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
        printf("Proxy Sever Accepted connection from (%s, %s)\n", hostname, port);

        Pthread_create(&tid, NULL, doit, &connfd);
        Pthread_detach(tid);
    }
    return 0;
}
/*
void *doit(void *vargp)
{
    int serverfd = *((int*)vargp);
    rio_t rio_server;  //make proxy as server
    rio_t rio_client;  //make proxy as client
    char buf[MAXLINE];
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], filepath[MAXLINE], server_port[MAXLINE];
    int clientfd;
    char reqhd[MAXLINE];

    Rio_readinitb(&rio_server, serverfd);

    //handle HTTP request line
    if (!Rio_readlineb(&rio_server, buf, MAXLINE)) {

        return NULL;
    }
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")) {   // only deal with "GET"
        return NULL;
    }
    if (!strcmp(version, "HTTP/1.1")) {
        strcpy(version, "HTTP/1.0");
    }

    parse_uri(uri, hostname, server_port, filepath);
    sprintf(uri, "%s %s %s\r\n", method, filepath, version);
    // build connect to web server
    clientfd = Open_clientfd(hostname, server_port);
    Rio_readinitb(&rio_client, clientfd);

    //read HTTP request from browser
    strcpy(reqhd, uri);
    build_header(reqhd, hostname);
    // build_request_header(&rio_server, reqhd, hostname);
    //write HTTP headers to web server
    Rio_writen(clientfd, reqhd, strlen(reqhd));
    // send HTTP response to client browser
    ssize_t n;
    while ((n = Rio_readlineb(&rio_client, buf, MAXLINE))) {
        Rio_writen(serverfd, buf, n);
    }
    Close(serverfd);
    Close(clientfd);
}
*/

// 处理客户端和服务端的数据交互
void *doit(void *vargp)
{
    // Pthread_detach(pthread_self());
    int is_static;
    struct stat sbuf;
    rio_t rio_client, rio_server;
    int connfd = *((int *)vargp);
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];

    // 解析客户端的数据，获取目标服务器的地址等
    Rio_readinitb(&rio_client, connfd);
    if (!Rio_readlineb(&rio_client, buf, MAXLINE)) // line:netp:doit:readrequest
        return NULL;

    printf("%s", buf); // GET /favicon.ico HTTP/1.1
    sscanf(buf, "%s %s %s", method, uri, version);

    if (strcasecmp(method, "GET"))
    {
        printf("Proxy does not implement the method");
        return NULL;
    }

    // 将客户端发送的数据全部读取出来
    printf("proxy recieve client data:\n");
    read_requesthdrs(&rio_client);

    if (strcmp(version, "HTTP/1.1") == 0)
        strcpy(version, "HTTP/1.0");

    // 解析出目标服务器的地址,端口号,路径
    char hostname[20], portnum[10], filepath[MAXLINE];
    parse_uri(uri, hostname, portnum, filepath);
    sprintf(uri, "%s %s %s\r\n", method, filepath, version);

    // 连接上目标服务器，并开始发送数据
    int serverfd;
    if ((serverfd = Open_clientfd(hostname, portnum)) < 0)
    {
        return NULL;
    }
    Rio_readinitb(&rio_server, serverfd);

    char reqhd[MAXLINE];
    // memset(reqhd, 0, sizeof reqhd);
    // TODO 为什么需要加上这一行？？？？，不加的话为啥数组中还记录着之前的结果,线程的原因
    // printf("-----uri: %s\n",uri);
    // printf("-----reqhd: %s\n",reqhd);
    strcpy(reqhd, uri);// 不要用strcat
    build_header(reqhd, hostname);
    // printf("proxy reqhd: \n%s\n", reqhd);
    Rio_writen(serverfd, reqhd, strlen(reqhd));

    // 将数据发送到客户端
    ssize_t n;    
    while ((n = Rio_readlineb(&rio_server, buf, MAXLINE)))
    {
        Rio_writen(connfd, buf, n);
    }

    Close(connfd);
    Close(serverfd);
}


/**
 * uri = "http://192.168.123.86:20876/"
 * hostname = "192.168.123.86"
 * portnum = "20876"
 * filepath = "/"
 */

void parse_uri(char *uri, char *hostname, char *portnum, char *filepath)
{
    char *ptr1, *ptr2;
    uri += 7;
    ptr1 = index(uri, ':'); // default port is 80
    *ptr1 = '\0';
    ptr1++;
    strcpy(hostname, uri);

    ptr2 = index(ptr1, '/');
    *ptr2 = '\0';
    strcpy(portnum, ptr1);
    strcpy(filepath, "/");
    strcat(filepath, ptr2 + 1);
}

// 之前得分较低，原因是driver.sh需要获取其他文件，但逻辑代码里没有写，只写了home.html处理逻辑
/*
void parse_uri(char *uri, char*hostname, char*portnum, char*filepath){
 char *hostpose = strstr(uri, "//");
    //默认端口为80
    if (hostpose == NULL)
    {
        char *pathpose = strstr(uri, "/");
        if (pathpose != NULL)
            strcpy(filepath, pathpose);
        strcpy(portnum, "80");
        return;
    }
    else
    {
        char *portpose = strstr(hostpose + 2, ":");
        if (portpose != NULL)
        {
            int tmp;
            sscanf(portpose + 1, "%d%s", &tmp, filepath);
            sprintf(portnum, "%d", tmp);
            *portpose = '\0';
        }
        else
        {
            char *pathpose = strstr(hostpose + 2, "/");
            if (pathpose != NULL)
            {
                strcpy(filepath, pathpose);
                strcpy(portnum, "80");
                *pathpose = '\0';
            }
        }
        strcpy(hostname, hostpose + 2);
    }
    return;
}
*/

/*
代理服务器发送给目标服务器的头的格式是这样的, 少一行都不对
*/
void build_header(char *reqhd, char *hostname)
{
    char buf[MAXLINE];
    memset(buf, 0, sizeof buf);

    sprintf(buf, "Host: %s\r\n", hostname);
    strcat(reqhd, buf);
    sprintf(buf, "%s", user_agent_hdr);
    strcat(reqhd, buf);

    strcat(reqhd, "Connection: close\r\n");
    strcat(reqhd, "Proxy-Connection: close\r\n");
    strcat(reqhd, "\r\n"); // 注意这一行也不能少，否则tiny服务器会一直输出
}

void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while (strcmp(buf, "\r\n"))
    {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}
/*
作为代理服务器使用，转发客户端和服务器之间的数据
代理服务器使用多线程编程实现

gcc -o proxy proxy.c csapp.c -pthread -g
./proxy 20878
curl -v --proxy 192.168.123.86:20878 192.168.123.86:20876
*/