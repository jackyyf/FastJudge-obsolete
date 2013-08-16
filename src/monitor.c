#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/timeb.h>
#include "macros.h"
#include "monitor.h"

int TLE = 0;
struct itimerval clear = {
	{0, 0},
	{0, 0}
};

static pid_t child;

void getProgramStatus(pid_t who, const struct timeval *timeout, struct res_usage *result) {
	/* Setup Alarm */
	INFO("Monitor Called");
	child = who;
	static struct itimerval timer;
	static clockid_t clk;
	static struct timeb task_begin, task_end;
	static siginfo_t sig;
	static struct timespec cputime;
	ftime(&task_begin);
	if(rarely(signal(SIGALRM, alarmHandler) == SIG_ERR))
		ERROR("Set signal handler failed!");
	memset(&timer.it_interval, 0, sizeof(struct timeval));
	timer.it_value.tv_sec = timeout -> tv_sec, timer.it_value.tv_usec = timeout -> tv_usec;
	if(rarely(setitimer(ITIMER_REAL, &timer, NULL) < 0))
		ERROR("Timer set failed!");
	INFO("Begin monitoring child process.");
	int status = waitid(P_PID, who, &sig, WEXITED | WNOWAIT);
	if(rarely(status < 0)) /* WNOWAIT: For later pull resource info. */
		ERROR("waitid failed!");
	else {
		if (sig.si_code == CLD_EXITED) {
			result -> exitcode = sig.si_status;
		} else if(sig.si_code == CLD_KILLED || sig.si_code == CLD_DUMPED) {
			result -> exitcode = -sig.si_status;
		} else {
			WARN("Unknown child status!");
			result -> exitcode = 0x80000000;
		}
	}
	if(!TLE) {
		if(setitimer(ITIMER_REAL, &clear, NULL) < 0) ERROR("Timer clear failed!");
	}
	else {
		result -> exitcode = -9;
	}
	ftime(&task_end);
	int run_ms = (task_end.time - task_begin.time) * 1000 + (task_end.millitm - task_begin.millitm);
	result -> realtime.tv_sec = run_ms / 1000;
	result -> realtime.tv_nsec = (run_ms % 1000) * 1000000;
	if(rarely(clock_getcpuclockid(who, &clk)))
		ERROR("Can't get child process cpu clock id!");
	else {
		if(rarely(clock_gettime(clk, &cputime)))
			ERROR("Unable to retrive CPU time!");
		else {
			result -> cputime.tv_sec = cputime.tv_sec;
			result -> cputime.tv_nsec = cputime.tv_nsec;
		}
	}
}

void alarmHandler(int signal) {
	if (signal != SIGALRM) {
		sprintf(_buff, "Handling unexpected signal %d", signal);
		WARN(_buff);
	}
	TLE = 1;
	INFO("Alarm signal handled");
	if(rarely(kill(child, SIGKILL) < 0)) {
		ERROR("Can't terminate child process!");
	}
}

