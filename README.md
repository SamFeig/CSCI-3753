### Programming Assignment 1:
##### CSCI 3753 Operating Systems Spring 2020
##### Sam Feig samuel.feig@colorado.edu

### Kernal Files
* helloworld.c is a system call (#333) that prints "hello world" from the kernel
* cs3753_add.c is a system call (#334) that takes in 2 numbers and adds them together and returns the result as a pointer to the user space

Place each file in the respective path listed bellow
```
arch/x86/kernel/helloworld.c
arch/x86/kernel/cs3753_add.c
arch/x86/kernel/Makefile
arch/x86/entry/syscalls/syscall_64.tbl
include/linux/syscalls.h
```

### Test Programs
Drivers to call the two system calls
```
helloWorldDriver.c
cs3753AddDriver.c
```
Compile with:
'gcc helloWorldDriver.c -o helloWorldDriver'
'gcc cs3753AddDriver.c -o cs3753AddDriver'

### System Logs
```
/var/log/syslog
```
View quickly using:
'tail -f /var/log/syslog'
