#include "zuma.h"

#define WIDTH 640
#define HEIGHT 480

typedef struct {
	char name[20];
	int score;
}USER;

void init(USER& user);

void demo2()
{
	USER user;

	user.score = 0;
	init(user);   //��ʼ��

	printf("%s", user.name);    //������
	getchar();


	closegraph();
	return;
}

void init(USER& user)
{
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);  //�����С,����SHOWCONSOLE���Ϳ�����printf��
	//setbkcolor(WHITE);         //����ɫ
	setbkmode(TRANSPARENT);
	cleardevice();             //����
	setcolor(RED);             //����ɫ
	setlinestyle(PS_SOLID, 2);
	rectangle(1, 1, WIDTH - 2, HEIGHT - 2);  //�������

	InputBox(user.name, 20, "�������û�����");
	IMAGE mainBackground;
	loadimage(&mainBackground, "image\\main_background3.jpg", WIDTH, HEIGHT, true);
	putimage(0, 0, &mainBackground);

	outtextxy(WIDTH / 2 + 10, HEIGHT / 3 + 10, "Name: ");
	outtextxy(WIDTH / 2 + 60, HEIGHT / 3 + 10, user.name);

	char buf[10];
	sprintf_s(buf, "%d", user.score);   //user.scoreΪ���ͣ�outtextxyҪ��������� char[]
	outtextxy(WIDTH / 2 + 100, HEIGHT / 3 + 10, buf);
}
