#include <stdio.h>

int main() {
	static char buff[1024];
	int len;
	printf("Hello world!\n");
	while((len = fread(buff, 1, 1024, stdin)) > 0) {
		fwrite(buff, 1, len, stdout);
	}
	fflush(stdout);
	return 0;
}
