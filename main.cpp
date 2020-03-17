#include "zuma.h"

jmp_buf env;

int main(){
	//demo1();
	MajorData md;
	//md.mapDir = "maps\\test_function-classic.zmap";

	loadMap(&md.mi, "maps","test_points");
	//coreGaming(md);
	while (1);
	return 0;
}