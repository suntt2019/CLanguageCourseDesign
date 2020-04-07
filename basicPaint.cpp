#include "zuma.h"

void rotateAndPaint(IMAGE* img, IMAGE* imgMask, double angle, Point position, bool highQuality) {
	IMAGE rotatedImg, rotatedImgMask;
	rotateimage(&rotatedImg, img, angle, BLACK, true, highQuality);
	rotateimage(&rotatedImgMask, imgMask, angle, WHITE, true, highQuality);
	double startingPositionX, startingPositionY;
	startingPositionX = position.x - rotatedImg.getwidth() / 2;
	startingPositionY = position.y - rotatedImg.getheight() / 2;
	putimage(startingPositionX, startingPositionY, &rotatedImgMask, SRCAND);
	putimage(startingPositionX, startingPositionY, &rotatedImg, SRCINVERT);
	return;
}

void justPaint(IMAGE* img, IMAGE* imgMask, Point position) {
	putimage(position.x - imgMask->getwidth() / 2, position.y - imgMask->getheight() / 2, imgMask, SRCAND);
	putimage(position.x - img->getwidth() / 2, position.y - img->getheight() / 2, img, SRCINVERT);
	return;
}

void outtextxy(Point p, LPCTSTR str) {
	outtextxy(p.x, p.y, str);
	return;
}