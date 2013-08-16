#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "macros.h"
#include "executor.h"
#include "monitor.h"

/**
 * note: use bytes with softlim/hardlim if limit with file size/memory size.
 */

int setlimit(int type, long softlim, long hardlim) {
	static struct rlimit lim;
	//static long pagesize = sysconf(_SC_PAGESIZE);
	lim.rlim_cur = (softlim <= 0 ? RLIM_INFINITY : softlim), lim.rlim_max = (hardlim <= 0 ? RLIM_INFINITY : hardlim);
	switch(type) {
		case LIM_MEMORY:
			return setrlimit(RLIMIT_AS, &lim);
		case LIM_CPU:
			return setrlimit(RLIMIT_CPU, &lim);
		case LIM_FSIZE:
			return setrlimit(RLIMIT_FSIZE, &lim);
		case LIM_FNO:
			return setrlimit(RLIMIT_NOFILE, &lim);
		case LIM_STACK:
			return setrlimit(RLIMIT_STACK, &lim);
		case LIM_PROCESS:
			return setrlimit(RLIMIT_NPROC, &lim);
		default:
			return -1;
	}
}

pid_t execute(char *const args[], const struct res_lim *lim) {
	pid_t child_pid;
	long cpu_cur, cpu_max;
	int child_status;
	int hasError = 0;
	child_status = 0;
	child_pid = vfork(); // Fork a new process to run.
	if(rarely(child_pid < 0)) { // Error!
		ERROR("Error at vfork!");
		return -1;
	}
	if(child_pid == 0) { // We are now child.
		sprintf(_buff, "Child PID = %d", getpid());
		INFO(_buff);
		if(likely(lim -> filein != NULL)) {
			int fd = open(lim -> filein, O_RDONLY);
			if (rarely(fd < 0)) {
				WARN("Open Input File failed!");
			} else{
				dup2(fd, 0);
			}
		}
		if(likely(lim -> fileout != NULL)) {
			int fd = open(lim -> fileout, O_CREAT | O_WRONLY | O_TRUNC, 0777);
			if (rarely(fd < 0)) {
				WARN("Open Output File failed!");
			} else{
				dup2(fd, 1);
			}
		}
		if(rarely(getuid())) {
			WARN("Judge is not running as root. All restriction will not work.");
		} else if(rarely(lim == NULL)) {
			WARN("No Limitation will be applied to child!");
		} else {
			if(rarely(setlimit(LIM_MEMORY, lim -> mem_cur, lim -> mem_max) < 0)) {
				WARN("Memory limit failed!");
				hasError = 1;
			}
			if(rarely(setlimit(LIM_FSIZE, lim -> fs_cur, lim -> fs_max) < 0)) {
				WARN("File output limit failed!");
				hasError = 1;
			}
			if(rarely(setlimit(LIM_FNO, lim -> fn_cur, lim -> fn_max) < 0)) {
				WARN("File number limit failed!");
				hasError = 1;
			}
			if(rarely(lim -> cpu_cur.tv_sec < 0)) cpu_cur = RLIM_INFINITY;
			else cpu_cur = (long)(lim -> cpu_cur.tv_sec + (!!lim -> cpu_cur.tv_nsec));
			if(rarely(lim -> cpu_max.tv_sec < 0)) cpu_max = RLIM_INFINITY;
			else cpu_max = (long)(lim -> cpu_max.tv_sec + (!!lim -> cpu_max.tv_nsec));
			if(rarely(setlimit(LIM_CPU, cpu_cur, cpu_max))) {
				WARN("CPU time limit failed!");
				hasError = 1;
			}
			if(rarely(setlimit(LIM_STACK, lim -> st_cur, lim -> st_max) < 0)) {
				WARN("Stack limit failed!");
				hasError = 1;
			}
			if(rarely(setlimit(LIM_PROCESS, lim -> ps_cur, lim -> ps_max) < 0)) {
				WARN("Process limit failed!");
				hasError = 1;
			}
			if(lim -> root != NULL)
				if(rarely(chroot(lim -> root) < 0)) {
					WARN("Chroot failed!");
					hasError = 1;
				}
			if(lim -> gid > 0)
				if(rarely(setgid(lim -> gid) < 0)) {
					WARN("Set GID failed!");
					hasError = 1;
				}
			if(lim -> uid > 0)
				if(rarely(setuid(lim -> uid) < 0)) {
					WARN("Set UID failed!");
					hasError = 1;
				}
			if(!hasError) INFO("Limitation applied successfully.");
			//if(err >= 0) dup2(err, 2);
		}
		for(register int i = 3; i < 1024; ++ i) close(i);

		if(execvp(args[0], args) < 0) {
			child_status = -1;
			ERROR("vfork ok, but execvp failed!");
			_exit(-1);
		}
	}
	if (child_status < 0) {
		return -1;
	}
	sprintf(_buff, "Child vfork and execvp OK. PID = %d", child_pid);
	INFO(_buff);
	return child_pid;
}


