struct res_usage {
	struct timespec cputime, realtime;
	/* in bytes */
	long memusage;
	/**
	 * exitcode == 0, exit normally.
	 * exitcode > 0, exit normally, but exitcode is not zero (represented by exitcode).
	 * exitcode < 0, terminated by signal -exitcode
	 * exitcode == 0x80000000, unknown status.
	 */
	long exitcode;
};

void getProgramStatus(pid_t who, const struct timeval *timeout, struct res_usage *result);
void alarmHandler(int signal);
