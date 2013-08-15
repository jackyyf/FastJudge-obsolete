#ifndef _EXECUTOR_H
#define _EXECUTOR_H
#include <time.h>

#define LIM_MEMORY	0
#define LIM_CPU		1
#define LIM_FSIZE	2
#define LIM_FNO		3
#define LIM_STACK	4
#define LIM_PROCESS	5

struct res_lim {

	char *filein, *fileout; /* Input File and Output File */

	long mem_cur, mem_max, /* Memory limit for program. */
		 fs_cur, fs_max, /* File size limit for program. */
		 fn_cur, fn_max, /* Total amount of opened file limit for program. */
		 st_cur, st_max, /* Stack size limit for program. */
		 ps_cur, ps_max; /* Process/Thread limit for program. 1 is suggested. */
		 
	struct timespec cpu_cur, cpu_max; /* CPU time limit for program. */
	uid_t uid; /* Which uid should be used to execute the program. */
	gid_t gid; /* Which gid should be used to execute the program. */
	char *root; /* If not null, judge will chroot to new root. */
};

int setlimit(int type, long softlim, long hardlim);
pid_t execute(char *const args[], const struct res_lim *lim);

#endif
