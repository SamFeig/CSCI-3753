#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

asmlinkage long sys_cs3753_add(int num1, int num2, int* res) {
	int tmp;
	printk(KERN_ALERT "Number 1 is %d\n", num1);
	printk(KERN_ALERT "Number 2 is %d\n", num2);

	tmp = num1 + num2;

	copy_to_user(res, &tmp, sizeof(tmp));
	printk(KERN_ALERT "Result of number 1 + number 2 is %d\n",tmp);
	return 0;
}
