#include "zuma.h"

void operateException() {//TODO:��Ϊ�������ã��ŷ���longjmp���ܿ��ļ���
	int errorCode = 0;
	const char exceptionHints[][30] = { "",
		"json file not found",
		"io exception",
		"json parsing exception",
		"unknow storing method",
		"malloc exception",//5
		"TODO:Fix this situation",
		"Points Index Out of Bounds",

	
	
	
	};
	if ((errorCode = setjmp(env)) != 0) {//�쳣����
		printf("%d\n", errorCode);
		//pmi->mpi.routeStoringMethod = -errorCode;
		printf("[Excption] TODO(add parts): %s.\n", exceptionHints[errorCode]);
		return;
	}
}