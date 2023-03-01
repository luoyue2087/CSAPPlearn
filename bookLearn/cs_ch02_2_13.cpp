#include <iostream>
#include <bitset>
using namespace std;

/*
bic(int x, int m)
m上的某一位为1，对x的对应位置0
bis(int x, int m)
m上的某一位为1，对x的对应位置1
*/
int bis(int x, int m){
    int i=0;
    while(m){
        if(m&1){
            x|=(1<<i);//如果m右移i位后的低位为1，则将x的对应位置1
        }
        ++i;
        m >>= 1;
    }
    return x;
}
int bic(int x, int m){
    int i=0;
    while(m){
        if(m&1) x&=(~(1<<i));
        m >>= 1;
        ++i;
    }
    return x;
}


/*
思路：y的某一位是1，x|y会让x的对应位置1
*/
int bool_or(int x, int y){
    return bis(x,y);
}

/*
思路：
bic(x,y) 会让y中为1的位置让x置0
x最后没有被置0的位置就是原来x是1且y为0
bic(y,x) 会让x中为1的位置让y置0
y最后没有被置0的位置就是原来y是1且x为0
刚好体现出x和y不同的位置
*/
int bool_xor(int x, int y){
    return bis(bic(x,y),bic(y,x));
}

int main(){
    bitset<32>bi("101010101"), m("11000101");
    cout <<"       x: "<< bi<<endl;
    cout <<"       m: "<<m<<endl;
    cout <<"     bis: "<< bitset<32>(bis((int)(bi.to_ulong()), (int)(m.to_ulong()))) <<endl;
    cout <<"     bic: "<< bitset<32>(bic((int)(bi.to_ulong()), (int)(m.to_ulong()))) <<endl;
    cout << endl;
    cout <<" bool_or: "<< bitset<32>(bool_or((int)(bi.to_ulong()), (int)(m.to_ulong()))) <<endl;
    cout <<"bool_xor: "<< bitset<32>(bool_xor((int)(bi.to_ulong()), (int)(m.to_ulong()))) <<endl;

    return 0;
}
//g++ main.c -o a && ./a