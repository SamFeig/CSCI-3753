#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include<syslog.h>

int main(void) {
	int retVal;
	int res;
	int a = 10;
	int b = 15;
	retVal = syscall(334, a, b, &res);
	printf("cs3753_add computes %d + %d, which is %d \n", a, b, res);

	return 0;
}