#include <stdio.h>

int delete(int num){
    return num + 1;
}

int main(){
    auto int x = 2; 
    printf("%d", delete(x));
    return 0;
}