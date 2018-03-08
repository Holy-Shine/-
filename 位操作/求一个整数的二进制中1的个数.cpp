/*
求一个整数的二进制表示中1的个数。

1. 移位求解注意右移符号位问题
2. 
    对于1个整数n,n-1可以视作：将n的二进制表示中最右边的1变为0，再右边的0全部变为1.
    例如：10011000-1=10010111，所以 n&n-1可以表示为将n最右边的1归0
*/
#include <iostream>
using namespace std;
int countOne(int n){
    int counter = 0;
    while(n){
        counter++;
        n = n & (n-1);
    }
    return counter;
}

int main(){
    int test = 0x0000FE00;  // 7个1
    cout<<countOne(test);
}