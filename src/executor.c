#include<unistd.h>
#include<sys/wait.h>
#include<sys/resource.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/types.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "macros.h"
#include "executor.h"

/**
 * note: use bytes with softlim/hardlim if limit with file size/memory size.
 */

int setlimit(int type, long softlim, long hardlim) {
	static struct rlimit lim;
	static long pagesize = 0;
	if(rarely(!pagesize)) pagesize = sysconf(_SC_PAGESIZE);
	//static long pagesize = sysconf(_SC_PAGESIZE);
	lim.rlim_cur = softlim, lim.rlim_max = hardlim;
	switch(type) {
		case LIM_MEMORY:
			lim.rlim_cur = (lim.rlim_cur + pagesize - 1) / pagesize, lim.rlim_max = (lim.rlim_max + pagesize - 1) / pagesize; // Round Up
			return setrlimit(RLIMIT_AS, &lim);
		case LIM_CPU:
			return setrlimit(RLIMIT_CPU, &lim);
		case LIM_FSIZE:
			return setrlimit(RLIMIT_FSIZE, &lim);
		default:
			return -1;
	}
}

pid_t execute(const char *args[], const struct res_lim *lim) {
	pid_t child_pid;
	double cputime;
	long cpu_cur, cpu_max;
	int child_status;
	child_status = 0;
	child_pid = vfork(); // Fork a new process to run.
	if(rarely(child_pid < 0)) { // Error!
		return -1;
	}
	if(child_pid == 0) { // We are now child.
		setlimit(LIM_MEMORY, lim -> mem_cur, lim -> mem_max);
		setlimit(LIM_FSIZE, lim -> fs_cur, lim -> fs_max);
		if(rarely(fl(lim -> cpu_cur, 0.0))) cpu_cur = RLIM_INFINITY;
		else cpu_cur = (long)ceil(lim -> cpu_cur);
		if(rarely(fl(lim -> cpu_max, 0.0))) cpu_max = RLIM_INFINITY;
		else cpu_max = (long)ceil(lim -> cpu_max);
		setlimit(LIM_CPU, cpu_cur, cpu_max);
		if(execvp(args[0], args)) {
			child_status = -1;
			_exit(1);
		}
	}
	// We are parent.
	if(child_status) return -1; // execvp failed.
	return child_pid;
}
