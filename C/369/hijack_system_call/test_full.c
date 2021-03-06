#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "interceptor.h"


static int last_child;


int vsyscall_arg(int sno, int n, ...) {

	va_list va;
	long args[6];
	int i, ret;

	va_start(va, n);
	for(i = 0; i < n; i++) {
		args[i] = va_arg(va, long);
	}
	va_end(va);

	ret = syscall(sno, args[0], args[1], args[2]);
	if(ret) ret = -errno;
	return ret;
}

#define test(s, a, t) \
({\
	int i;\
	static int line = 1;\
	char dummy[1024];\
	\
	sprintf(dummy, s, a);\
	printf("test @(%d): %s", line,dummy); \
	line++;\
	for(i=0; i<60-strlen(dummy); i++)\
		putchar('.');\
	if (!(t))\
		printf("failed\n");\
	else\
		printf("passed\n");\
	fflush(stdout);\
})


void clear_log() {
	system("dmesg -c &> /dev/null");
}

/**
 * Check if the log contains what is expected - if log_message was done properly
 */
int find_log(long pid, long sno, long *args, long ret) {
	char message[1024], command[1024], output[1024];
	FILE *fp;

	sprintf(message, "[%lx]%lx(%lx,%lx,%lx,%lx,%lx,%lx)",
	               (long)getpid(), sno, args[0], args[1], args[2], args[3], args[4], args[5]);
	sprintf(command, "dmesg | grep \"\\[%lx\\]%lx(%lx,%lx,%lx,%lx,%lx,%lx)\" 2>&1",
	               (long)getpid(), sno, args[0], args[1], args[2], args[3], args[4], args[5]);

	fp = popen(command, "r");
	if(!fp)  return -1;

	while(fgets(output, sizeof(output)-1, fp) != NULL) {
		if(strstr(output, message)) {
			pclose(fp);
			return 0;
		}
	}

	pclose(fp);
	return -1;
}

/**
 * Check if a syscall gets logged properly when it's been already intercepted
 */
int do_monitor(int sysno) {
	int sno, ret, i;
	long args[6];

	sno = sysno;
	for(i = 0; i < 6; i++) {
		args[i] = rand();
	}

	ret = syscall(sno, args[0], args[1], args[2], args[3], args[4], args[5]);
	if(ret) ret = -errno;

	//printf("[%x]%lx(%lx,%lx,%lx,%lx,%lx,%lx)\n", getpid(), (long)sysno,
	//	args[0], args[1], args[2], args[3], args[4], args[5]);

	test("%d nonroot monitor", sysno, find_log(getpid(), (long)sno, args, (long)ret) == 0);
	return 0;
}


int do_intercept(int syscall, int status) {
	test("%d intercept", syscall, vsyscall_arg(MY_CUSTOM_SYSCALL, 3, REQUEST_SYSCALL_INTERCEPT, syscall, getpid()) == status);
	return 0;
}


int do_release(int syscall, int status) {
	test("%d release", syscall, vsyscall_arg(MY_CUSTOM_SYSCALL, 3, REQUEST_SYSCALL_RELEASE, syscall, getpid()) == status);
	return 0;
}


int do_start(int syscall, int pid, int status) {
	if (pid == -1)
		pid=getpid();
	test("%d start", syscall, vsyscall_arg(MY_CUSTOM_SYSCALL, 3, REQUEST_START_MONITORING, syscall, pid) == status);
	return 0;
}

int do_stop(int syscall, int pid, int status) {
	if (pid == -1)
		pid=getpid();
	test("%d stop", syscall, vsyscall_arg(MY_CUSTOM_SYSCALL, 3, REQUEST_STOP_MONITORING, syscall, pid) == status);
	return 0;
}



/**
 * Run the tester as a non-root user, and basically run do_nonroot
 */
void do_as_guest(const char *str, int args1, int args2) {

	char cmd[1024];
	char cmd2[1024];
	char* exec[]={"bash", "-c", cmd2, NULL};

	sprintf(cmd, str, args1, args2);
	sprintf(cmd2, "su nobody -c '%s' ", cmd);
	switch ((last_child = fork()))  {
		case -1:
			assert(0);
		case 0:
			execvp("/bin/bash", exec);
			assert(0);
		default:
			waitpid(last_child, NULL, 0);
	}
}

int do_nonroot(int syscall) {
	// (11) when already intercepted by root
	do_intercept(syscall, -EPERM);
	// (12) no permission to release it if you are not root
	do_release(syscall, -EPERM);
	// (13) no permission to monitor all as nonroot
	do_start(syscall, 0, -EPERM);
	// (14) no permission to stop monitor all as nonroot
	do_stop(syscall, 0, -EPERM);
	// (15) no perm to monitor a pid as a nonroot
	do_start(syscall, 1, -EPERM);
	// (16) no perm to stop for same reason
	do_stop(syscall, 1, -EPERM);
	// (17) able to monitor itself
	do_start(syscall, getpid(), 0);
	// (18) busy if you do it twice
	do_start(syscall, getpid(), -EBUSY);
	// (19) since intercepted and monitored , should log message when call syscall
	do_monitor(syscall);
	// (20) stop monitor success
	do_stop(syscall, getpid(), 0);
	// (21) stop twice raise error
	do_stop(syscall, getpid(), -EINVAL);
	return 0;
}

// shan
int do_nonroot_single(syscall){
	do_start(syscall,-1,0);
	do_start(syscall,0,-EPERM);
	do_monitor(syscall);
	do_stop(syscall,0,-EPERM);
	do_stop(syscall,-1,0);
	return 0;
}

// shan
int do_bonus_after_addall(syscall){
	do_start(syscall,-1,0);
	do_start(syscall,0,-EPERM);
	do_monitor(syscall);
	do_stop(syscall,0,-EPERM);
	do_stop(syscall,-1,0);
	return 0;
}


void test_syscall(int syscall) {

	//clear_log();
	// (9) intercept that sys call
	do_intercept(syscall, 0);
	// (10) intercept again should return EBUSY
	do_intercept(syscall, -EBUSY);

	// just run as visitor
	do_as_guest("./test_full nonroot %d", syscall, 0);
	// (22) root : invalid pid
	do_start(syscall, -2, -EINVAL);
	// (23) root : monitor all pid
	do_start(syscall, 0, 0);

	// =============== can test here =========================
	printf("----------------when start for pid = 0-----------\n");
	do_monitor(syscall);
	do_as_guest("./test_full monitor %d", syscall, 0);
	printf("-------------------------------------------------\n");
	//(24) remove all pid
	do_stop(syscall, 0, 0);
	do_start(syscall, 1, 0);
	do_as_guest("./test_full stop %d 1 %d", syscall, -EPERM);
	do_stop(syscall, 1, 0);
	do_as_guest("./test_full start %d -1 %d", syscall, 0);
	do_stop(syscall, last_child, -EINVAL);
	do_release(syscall, 0);

	printf("----------------cross test-------------------\n");
	// root intercept syscall
	do_intercept(syscall,0);
	// do all stuff in a via a single nonroot user
	do_as_guest("./test_full single %d", syscall, 0);

	// root monitor all process but a single process want to stop itself
	do_start(syscall,0,0);
	printf("~~~~\n");
	do_as_guest("./test_full stop %d -1 %d", syscall, 0); // currently
	do_stop(syscall,0,0);

	// root monitor all but a single process want to add it self
	do_start(syscall,0,0);
	do_as_guest("./test_full start %d -1 %d", syscall, -EBUSY);
	do_stop(syscall,0,0);
	
	// root monitor all but a single process want to stop and then add it self 
	do_start(syscall,0,0);
	// ???
	do_stop(syscall,0,0);

	// nonroot add itself and root add all
	do_as_guest("./test_full start %d -1 %d", syscall, 0);
	do_start(syscall,0,0);
	do_stop(syscall,0,0);

	// non root add itself and root stop all
	do_as_guest("./test_full start %d -1 %d", syscall, 0);
	do_stop(syscall,0,0);









	// root release syscall
	do_release(syscall,0);
	printf("-------------------------------------------------\n");
}


int main(int argc, char **argv) {

	srand(time(NULL));

	if (argc>1 && strcmp(argv[1], "intercept") == 0)
		return do_intercept(atoi(argv[2]), atoi(argv[3]));

	if (argc>1 && strcmp(argv[1], "release") == 0)
		return do_release(atoi(argv[2]), atoi(argv[3]));

	if (argc>1 && strcmp(argv[1], "start") == 0)
		return do_start(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));

	if (argc>1 && strcmp(argv[1], "stop") == 0)
		return do_stop(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));

	if (argc>1 && strcmp(argv[1], "monitor") == 0)
		return do_monitor(atoi(argv[2]));

	if (argc>1 && strcmp(argv[1], "nonroot") == 0)
		return do_nonroot(atoi(argv[2]));
	
	if (argc>1 && strcmp(argv[1], "single") == 0)
		return do_nonroot_single(atoi(argv[2]));
		
	// (1) load the insmod
	test("insmod interceptor.ko %s", "", system("insmod interceptor.ko") == 0);
	// (2) test when the syscall number is MY_CUSTOM_SYSCALL iteself, should return -EINVAL
	test("bad MY_SYSCALL args%s", "",  vsyscall_arg(MY_CUSTOM_SYSCALL, 3, 100, 0, 0) == -EINVAL);
	// (3) intercept MY_CUSTOM_SYSCALL, should fail with -EINVAL
	do_intercept(MY_CUSTOM_SYSCALL, -EINVAL);
	// (4) release MY_CUSTOM_SYSCALL, should fail
	do_release(MY_CUSTOM_SYSCALL, -EINVAL);
	// (5)
	do_intercept(-1, -EINVAL);
	// (6)
	do_release(-1, -EINVAL);

	// note that __NR_exit = 1
	// (7)
	do_intercept(__NR_exit, 0);
	// (8)
	do_release(__NR_exit, 0);


	test_syscall(SYS_open);
	/* The above line of code tests SYS_open.
	   Feel free to add more tests here for other system calls,
	   once you get everything to work; check Linux documentation
	   for other syscall number definitions.  */

	test("rmmod interceptor.ko %s", "", system("rmmod interceptor") == 0);
	return 0;
}
