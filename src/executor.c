#include<unistd.h>
#include<sys/wait.h>
#include<sys/resource.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/types.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include "executor.h"

/**
 * note: use bytes with softlim/hardlim if limit with file size/memory size.
 */

int setlimit(int type, long softlim, long hardlim) {
	static struct rlimit lim;
	static long pagesize = 0;
	if(__builtin_expect(!pagesize, 1)) pagesize = sysconf(_SC_PAGESIZE);
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
