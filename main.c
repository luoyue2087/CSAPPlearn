#include <iostream>
#include <bitset>
#include <assert.h>
using namespace std;

void fun2_59(){
    size_t mask = 0xff;
    size_t x = 0x89ABCDEF;
    size_t y = 0x76543210;
    size_t res = (x & mask) | (y & ~mask);
    assert(res == 0x765432EF);
}

//移动一个字节在二进制中是移动八位！
unsigned replace_byte (unsigned x, int i, unsigned char b){
    unsigned char mask = 0xff;
    x = x & (~(mask << i*8));
    x = x | (b<<(i*8));

    return x;
}


int main(){
    // fun2_59();
    // printf("%X\n",replace_byte(0x12345678,2,0xAB));
    // printf("%X\n",replace_byte(0x12345678,0,0xAB));
    
    return 0;
}
//g++ main.c -o a && ./a