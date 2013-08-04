#define LIM_MEMORY	0
#define LIM_CPU		1
#define LIM_FSIZE	2

int setlimit(int type, long softlim, long hardlim);

