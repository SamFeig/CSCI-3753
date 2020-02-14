#include <unistd.h>
#include <sys/syscall.h>
int main(void) {
	int retVal;
	retVal = syscall(333);
	return 0;
}