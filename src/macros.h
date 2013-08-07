#define likely(x) __builtin_expect(!!(x), 1)
#define rarely(x) __builtin_expect(!!(x), 0)
#define EPS 1e-8
/* return 1 if x > y, otherwise 0. */
#define fg(x, y) ((x) - (y) > EPS)
#define fl(x, y) ((x) - (y) < EPS)
#define feq(x, y) (fabs((x) - (y)) < EPS)
