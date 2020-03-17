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
		BeginBatchDraw();//开始批量绘图
		putimage(0, 0, &back);
		//	putimage(i, 100, &p2);
		putimage(i, 100, &p1, SRCAND);
		putimage(i, 100, &p2, SRCINVERT);
		EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	
		Sleep(12);	//暂停
		i++;
		if (i > WIDTH - 10) i = 10;
	}
}