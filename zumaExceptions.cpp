#include "zuma.h"

//handleException(0)Ϊ��ʼ���쳣����ϵͳ��ֻ������main()�е���һ��
//handleException(x)��x>0��Ϊ�����쳣��xΪ������룬�����쳣ʱ��Ϸ�������
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
		//TODO:�������Ľ����Ľ�Ϊ�ֲ��ֵ�



	};
	if (exceptionCode) {//����exception�����øú���ʱ�ȵ��˴�
		longjmp(env, exceptionCode);
	}else if (exceptionCode = setjmp(env)) {//��ʼ��ʱ·������setjmp�����쳣ʱ����һ�е�longjmp���˴�
		if (exceptionCode < 0) {
			printf("\n\n[[Zuma Excption]] TODO(add parts): TODO-number unknown exception code\n");
		}else {
			printf("\n\n[[Zuma Excption]] TODO(add parts): %d %s.\n", exceptionCode, exceptionHints[exceptionCode]);
			printf("[[Zuma Excption]] ended due to exception.\n");
		}
		closegraph();
		system("pause");
		return -exceptionCode;
	}else {//��ʼ����ɺ󵽴�˴�����Ϸ���忪ʼ
		int returnValue = gameMain();
		printf("[[Zuma Excption]] ended without exception.\n");
		if(DEBUG_OUTPUT)
			system("pause");
		else
			Sleep(10000);
		return returnValue;
	}
}