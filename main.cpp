#include "zuma.h"

int main(){
	return handleException(0);	
}

int gameMain() {
	//test();
	//return 0;
	MajorPanels mp;
	initPanels(&mp);
	mainMenu(&mp);
	return 0;
}