#include "zuma.h"

void demo1() {
	IMAGE back;
	IMAGE p1;
	IMAGE p2;
	int i;

	initgraph(WIDTH, HEIGHT);

	loadimage(&back, "image\\background.jpg");
	loadimage(&p1, "image\\fish02.jpg");
	loadimage(&p2, "image\\fish01.jpg");
	i = 10;
	while (1) {
		BeginBatchDraw();//��ʼ������ͼ
		putimage(0, 0, &back);
		//	putimage(i, 100, &p2);
		putimage(i, 100, &p1, SRCAND);
		putimage(i, 100, &p2, SRCINVERT);
		EndBatchDraw();//����������ͼ�������ƺõ�ͼƬͳһ������Ļ�ϡ�	
		Sleep(12);	//��ͣ
		i++;
		if (i > WIDTH - 10) i = 10;
	}
}