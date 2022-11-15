#include <iostream>
using namespace std;
typedef unsigned char *byte_pointer;
void show_bytes(byte_pointer start, size_t len) {
    size_t i;
    for (i = 0; i < len; i++)
        printf("%p\t0x%.2x\n", &start[i], start[i]);
    printf("\n");
}
void show_int(int x) {
    show_bytes((byte_pointer) &x, sizeof(int));
}
void show_float(float x) {
    show_bytes((byte_pointer) &x, sizeof(float));
}
void show_pointer(void *x) {
    show_bytes((byte_pointer) &x, sizeof(void *)); 
}
void test_show_bytes(int val) {
    int ival = val;
    double fval = (double) ival;
    int *pval = &ival;
    printf("Stack variable ival = %d\n", ival);
    printf("(int)ival:\n");
    show_int(ival);
    printf("(float)ival:\n");
    show_float(fval);
    printf("&ival:\n");
    show_pointer(pval);
}
int main(){
    int val = 0x87654321;
    byte_pointer valp = (byte_pointer)&val;
    show_bytes(valp, 2);//输出valp存储在内存中的前两个字节 test ok
    // show_bytes(valp, 1);//输出valp存储在内存中的前1个字节 test ok //0x21
    // test_show_bytes(val);
    return 0;
}
//g++ -o a cs_ch02_2_5.cpp && ./a