#include "stdio.h"

int main(void){

int a[10]={1,2,3,4,5,6,7,8,9,10},*p=a+2,b;
b=p[5];
printf("%d",b);
}