#ifndef _MACROS_H
#define _MACROS_H

#define likely(x)	__builtin_expect(!!(x), 1)
#define rarely(x)	__builtin_expect(!!(x), 0)
#define EPS 1e-8
/* return 1 if x > y, otherwise 0. */
#define fg(x, y)	((x) - (y) > EPS)
#define fl(x, y)	((x) - (y) < EPS)
#define feq(x, y)	(fabs((x) - (y)) < EPS)

#define COLOR_INIT	0x1B
#define COL_BLACK	0
#define COL_RED		1
#define COL_GREEN	2
#define COL_YELLOW	3
#define COL_BLUE	4
#define	COL_MAGENTA	5
#define COL_CYAN	6
#define	COL_WHITE	7

#define SET_COLOR(fp, fg) do { \
	fprintf(fp, "%c[%d;%dm", COLOR_INIT, 1, 30 + fg); \
} while(0)

#define RESET_COLOR(fp) do { \
	fprintf(fp, "%c[%dm", COLOR_INIT, 0); \
} while(0)

/* Following macros are for debug use. */

static char _buff[2048];

#ifndef NODEBUG

#define INFO(x) do { \
	SET_COLOR(stderr, COL_BLUE); \
	fprintf(stderr, "[" __FILE__ ":%d] [INFO] %s\n", __LINE__, x); \
	RESET_COLOR(stderr); \
} while(0)

#define WARN(x) do { \
	SET_COLOR(stderr, COL_YELLOW); \
	fprintf(stderr, "[" __FILE__ ":%d] [WARN] %s\n", __LINE__, x); \
	RESET_COLOR(stderr); \
} while(0)

#define ERROR(x) do { \
	SET_COLOR(stderr, COL_RED); \
	fprintf(stderr, "[" __FILE__ ":%d] [ERROR] %s\n", __LINE__, x); \
	RESET_COLOR(stderr); \
} while(0)

#else

#define INFO(x) (void)(x)
#define WARN(x) (void)(x)
#define ERROR(x) (void)(x)

#endif

#endif
