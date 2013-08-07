#define LIM_MEMORY	0
#define LIM_CPU		1
#define LIM_FSIZE	2

struct res_lim {
	long mem_cur, mem_max,
		 fs_cur, fs_max;
	double cpu_cur, cpu_max;
};

int setlimit(int type, long softlim, long hardlim);
pid_t execute(const char *args[], const struct res_lim *lim);
