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

int main(int argc, char *argv[]) {
	static char pwd[1024];
	if(argc < 2) {
		WARN("No Command Specified.");
		_exit(1);
	}
	INFO("Current Working Directory:");
	INFO((char *)getcwd(pwd, 1024));
	struct res_lim *lim = (struct res_lim*) malloc(sizeof(struct res_lim));
	memset(lim, 0, sizeof(struct res_lim));
	lim -> mem_cur = lim -> mem_max = 16777216;
	lim -> fs_cur = lim -> fs_max = 16777216;
	lim -> fn_cur = 3, lim -> fn_max = 3;
	lim -> cpu_cur.tv_sec = 1, lim -> cpu_cur.tv_nsec = 0;
	lim -> cpu_max.tv_sec = 2, lim -> cpu_max.tv_nsec = 0;
	lim -> st_cur = lim -> st_max = 16777216;
	lim -> ps_cur = 1, lim -> ps_max = 1;
	lim -> uid = 9998, lim -> gid = 9998;
	lim -> root = pwd;
	lim -> filein = "/tmp/input.txt", lim -> fileout = "/tmp/output.txt";
	//lim -> root = NULL;
	static char* args[1024];
	register int i;
	for(i = 1; i < argc; ++ i) args[i - 1] = argv[i];
	args[argc - 1] = NULL;
	pid_t child = execute(args, lim);
	if(child < 0) {
		ERROR("execute failed!");
		exit(1);
	}
	static struct res_usage result;
	static struct timeval timeout;
	timeout.tv_sec = 0, timeout.tv_usec = 500000;
	getProgramStatus(child, &timeout, &result);
	sprintf(_buff, "Child exit status code: %ld", result.exitcode);
	INFO(_buff);
	sprintf(_buff, "CPU Time: %ld s, %ld ns", result.cputime.tv_sec, result.cputime.tv_nsec);
	INFO(_buff);
	sprintf(_buff, "Real Time: %ld s, %ld ns", result.realtime.tv_sec, result.realtime.tv_nsec);
	INFO(_buff);
	sprintf(_buff, "Memory Usage: %ld", result.memusage);
	INFO(_buff);
	return 0;
}
