#include "zuma.h"

jmp_buf env;

int main(){
	MajorData md;
	loadMap(&md.mi, "maps","test_points");//TODO����ͼѡ����涯̬����ѡ����ͼ
	coreGaming(md);
	while (1);
	return 0;
}