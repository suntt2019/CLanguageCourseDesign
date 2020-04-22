#include "zuma.h"
#pragma comment(lib,"Winmm.lib")

void test() {
	mciSendString("open \"Data\\1.mp3\" alias gbtc", NULL, 0, NULL);
	mciSendString("play gbtc", NULL, 0, NULL);
	mciSendString("setaudio gbtc volume to 0", NULL, 0, 0);
	Sleep(50000);
	return;
}