#include<stdio.h>
#include<stdlib.h>
//#include<unistd.h>

int main(){

int a=20;

while(a--){
	sleep(1);
	printf("%d\n",a);
}
exit(0);
}
