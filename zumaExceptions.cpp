#include "zuma.h"

void operateException() {
	int errorCode = 0;
	const char exceptionHints[][30] = { "",
		"json file not found",
		"io exception",
		"json parsing exception",
		"unknow storing method",
		"malloc exception",//5
	
	
	
	
	};
	if ((errorCode = setjmp(env)) != 0) {//Òì³£´¦Àí
		printf("%d\n", errorCode);
		//pmi->mpi.routeStoringMethod = -errorCode;
		printf("[Excption] TODO(add parts): %s.\n", exceptionHints[errorCode]);
		return;
	}
}