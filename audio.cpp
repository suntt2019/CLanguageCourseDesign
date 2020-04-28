#include "zuma.h"

void loadAudio() {
	char commandBuffer[400];
	char audioName[][50] = { "focus","clicked","1" ,"2","3"};
	if (DEBUG_OUTPUT)
		printf("[DEBUG_OUTPUT]loadAudio()\n");
	//mciSendString("open \"audio\\1.mp3\" alias gbtc", NULL, 0, NULL);
	for (int i = 0; i < 5; i++) {
		sprintf(commandBuffer, "open \"audio\\%s.mp3\" alias %s", audioName[i], audioName[i]);
		mciSendString(commandBuffer, NULL, 0, NULL);
		if (DEBUG_OUTPUT)
			printf("  MCI: %s\n", commandBuffer);

		//sprintf(commandBuffer, "play %s", audioName[i]);
		//mciSendString(commandBuffer, NULL, 0, NULL);
		//if (DEBUG_OUTPUT)
		//	printf("  MCI: %s\n", commandBuffer);

		//sprintf(commandBuffer, "pause %s", audioName[i]);
		//mciSendString(commandBuffer, NULL, 0, NULL);
		//if (DEBUG_OUTPUT)
		//	printf("  MCI: %s\n", commandBuffer);
	}
	//mciSendString("open \"audio\\focus.mp3\" alias focus", NULL, 0, NULL);
}

void playAudio(char* name,int volume) {
	char commandBuffer[400];
	if (DEBUG_OUTPUT)
		printf("[DEBUG_OUTPUT]playAudio()\n");

	sprintf(commandBuffer, "seek %s to start", name);
	mciSendString(commandBuffer, NULL, 0, NULL);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", commandBuffer);

	//sprintf(commandBuffer, "pause %s", name);
	//mciSendString(commandBuffer, NULL, 0, NULL);
	//if (DEBUG_OUTPUT)
	//	printf("  MCI: %s\n", commandBuffer);
	//Sleep(100);
	sprintf(commandBuffer, "play %s",name);
	mciSendString(commandBuffer, NULL, 0, NULL);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", commandBuffer);
	sprintf(commandBuffer, "setaudio %s volume to %d", name,volume);
	mciSendString(commandBuffer, NULL, 0, 0);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", commandBuffer);
	return;
}