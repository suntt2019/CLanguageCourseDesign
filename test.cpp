#include "zuma.h"

void test() {
	printf("game  before exception\n");
	handleException(2);
	printf("game  after exception\n");
	return;
}