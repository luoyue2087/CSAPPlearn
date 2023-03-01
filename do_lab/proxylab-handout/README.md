## 自己编写的readme
### 概览
> 实现proxy服务器，有三个任务
1. 基本代理服务器
2. 进一步优化，使代理支持多线程（生产者-消费者模型）
3. 使用 LRU 策略缓存 Web 中的对象（读者-写者模型）


### 任务一-完成 40/40
* 接收客户端的GET方法，修改请求头后发送到tiny服务器上
* 接收tiny服务器返回的数据，转发给客户端

### 任务二-粗略通过 15/15
* 消费者-生产者问题
* 只实现了proxy服务器的多线程

### 任务三-未完成
* LRU缓存，读写者模型，读优先
* 

### 评分结果
```bash
ly@ubuntu:proxylab-handout$ ./driver.sh 
*** Basic ***
Starting tiny on 15191
Starting proxy on 32043
1: home.html
   Fetching ./tiny/home.html into ./.proxy using the proxy
   Fetching ./tiny/home.html into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
2: csapp.c
   Fetching ./tiny/csapp.c into ./.proxy using the proxy
   Fetching ./tiny/csapp.c into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
3: tiny.c
   Fetching ./tiny/tiny.c into ./.proxy using the proxy
   Fetching ./tiny/tiny.c into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
4: godzilla.jpg
   Fetching ./tiny/godzilla.jpg into ./.proxy using the proxy
   Fetching ./tiny/godzilla.jpg into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
5: tiny
   Fetching ./tiny/tiny into ./.proxy using the proxy
   Fetching ./tiny/tiny into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
Killing tiny and proxy
basicScore: 40/40

*** Concurrency ***
Starting tiny on port 33452
Starting proxy on port 19265
Starting the blocking NOP server on port 22772
Trying to fetch a file from the blocking nop-server
Fetching ./tiny/home.html into ./.noproxy directly from Tiny
Fetching ./tiny/home.html into ./.proxy using the proxy
Checking whether the proxy fetch succeeded
Success: Was able to fetch tiny/home.html from the proxy.
Killing tiny, proxy, and nop-server
concurrencyScore: 15/15

*** Cache ***
Starting tiny on port 16539
Starting proxy on port 26345
Fetching ./tiny/tiny.c into ./.proxy using the proxy
Fetching ./tiny/home.html into ./.proxy using the proxy
Fetching ./tiny/csapp.c into ./.proxy using the proxy
Killing tiny
Fetching a cached copy of ./tiny/home.html into ./.noproxy
Failure: Was not able to fetch tiny/home.html from the proxy cache.
Killing proxy
cacheScore: 0/15

totalScore: 55/70
```

### 注意
1. driver.sh 是测试文件,运行后会全部测试三个功能，并打分

2. 不要在tiny服务器中使用unix_error输出调试信息，似乎判定文件会对比

3. 如果运行时遇到下面的问题：
Timeout waiting for the server to grab the port reserved for it
> 作者写了一个nop-server.py脚本模拟一个永远不会响应的服务器，来测试我们 Proxy 的并发性能。301 行这里应该改为：
```sh
python3 nop-server.py ${nop_port} &> /dev/null &
```




## 原始readme
####################################################################
# CS:APP Proxy Lab
#
# Student Source Files
####################################################################

This directory contains the files you will need for the CS:APP Proxy
Lab.

proxy.c
csapp.h
csapp.c
    These are starter files.  csapp.c and csapp.h are described in
    your textbook. 

    You may make any changes you like to these files.  And you may
    create and handin any additional files you like.

    Please use `port-for-user.pl' or 'free-port.sh' to generate
    unique ports for your proxy or tiny server. 

Makefile
    This is the makefile that builds the proxy program.  Type "make"
    to build your solution, or "make clean" followed by "make" for a
    fresh build. 

    Type "make handin" to create the tarfile that you will be handing
    in. You can modify it any way you like. Your instructor will use your
    Makefile to build your proxy from source.

port-for-user.pl
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>

free-port.sh
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

driver.sh
    The autograder for Basic, Concurrency, and Cache.        
    usage: ./driver.sh

nop-server.py
     helper for the autograder.         

tiny
    Tiny Web server from the CS:APP text

