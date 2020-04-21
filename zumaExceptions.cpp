#include "zuma.h"

//handleException(0)为初始化异常处理系统，只允许在main()中调用一次
//handleException(x)，x>0，为处理异常，x为错误代码，出现异常时游戏整体结束
int handleException(int exceptionCode) {
	static jmp_buf env;
	const char exceptionHints[][100] = { "",
		"json file not found",
		"io exception",
		"json parsing exception",
		"unknow storing method",
		"malloc exception",//5
		"TODO:Fix this situation",
		"Points Index Out of Bounds",
		"Fail to get ball color",
		"Unknow generating ball method",
		"sameBallProbability can't be 0 (because it's a denominator)",//10
		"zorder file not found",
		"Fixed ball order index out of bounds",
		"Can't get file handle: image//*",//13
		//TODO:错误代码改进，改进为分部分的



	};
	if (exceptionCode) {//遇到exception，调用该函数时先到此处
		longjmp(env, exceptionCode);
	}else if (exceptionCode = setjmp(env)) {//初始化时路过这里setjmp，遇异常时由上一行的longjmp到此处
		if (exceptionCode < 0) {
			printf("\n\n[[Zuma Excption]] TODO(add parts): TODO-number unknown exception code\n");
		}else {
			printf("\n\n[[Zuma Excption]] TODO(add parts): %d %s.\n", exceptionCode, exceptionHints[exceptionCode]);
			printf("[[Zuma Excption]] ended due to exception.\n");
		}
		closegraph();
		system("pause");
		return -exceptionCode;
	}else {//初始化完成后到达此处，游戏本体开始
		int returnValue = gameMain();
		printf("[[Zuma Excption]] ended without exception.\n");
		if(DEBUG_OUTPUT)
			system("pause");
		else
			Sleep(10000);
		return returnValue;
	}
}