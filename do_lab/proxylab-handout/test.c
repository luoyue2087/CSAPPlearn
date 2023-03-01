#include <pthread.h>
#include <stdio.h>

char ch;

void* test(void*argc){
    char arr[10];
    char ch = *((char*)argc);
    for(int i=0; i<10; ++i)arr[i] = ch;
    for(int i=0; i<10; ++i)printf("%c",arr[i]);printf("\n");
}

int main(){
    
    for(char ii='a'; ii<'z'; ++ii){
        pthread_t tid;
        ch = ii;
        pthread_create(&tid,NULL,test,&ch);
        pthread_detach(tid);
        // pthread_join(tid,NULL);
    }
    return 0;
}
//gcc test.c -pthread && ./a.out