### Programming Assignment 2:
##### CSCI 3753 Operating Systems Spring 2020
##### Sam Feig samuel.feig@colorado.edu

### Loadable Kernel Modules
* helloModule.c is a "hello world" LKM that just prints in the init and exit functions
* pa2_char_driver.c is a character driver LKM that implements open, read, write, and seek to write text data to a simulated device file at '/dev/pa2_character_device' using major number 240.

###Building and Running
To build the LKMs and install them, run:
```
make
```
to clean up all installed and created files (including unload the LKM), run:
```
make clean
```

### Test Programs
Test code to use the driver is in:
```
tester.c
```
This tester program is compiled as part of the make process and can be run after running 'make' by calling
```
./tester
```

### System Logs
```
/var/log/syslog
```
View quickly using:
'dmesg' or 'tail -f /var/log/syslog'