#include <iostream>
#include <cstring>
using namespace std;
typedef unsigned char *byte_pointer;
void show_bytes(byte_pointer start, size_t len) {
    size_t i;
    for (i = 0; i < len; i++)
        printf("%p\t0x%.2x\n", &start[i], start[i]);
    printf("\n");
}

int main(){
    const char *s = "abcdef";
    show_bytes((byte_pointer) s, strlen(s));
    return 0;
}
//g++ -o a cs_ch02_2_7.cpp && ./a