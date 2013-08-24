#ifndef _FILTER_H
#define _FILTER_H

#ifndef HAVE_LINUX_SECCOMP_H
	#warning "Header linux/seccomp.h not found. Seccomp-BPF syscall filter won't work."
#else
#endif

#endif
