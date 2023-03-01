#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(){

    struct stat sbuf;
    printf("%d\n", stat("./test.c", &sbuf));//0
    printf("%d\n", stat("./test.cp", &sbuf));//-1

    return 0;
}
//gcc -o main test.c && ./main