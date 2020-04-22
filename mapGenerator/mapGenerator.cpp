#include <stdio.h>
#include <math.h>

typedef struct _Point{
	double x,y;
}Point;

Point pointArray[100000];
Point ret[100000];

Point makePoint(double x,double y){
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

Point minus(Point p1,Point p2){
	return makePoint(p1.x-p2.x,p1.y-p2.y);
}

Point multiply(Point p,double d){
	return makePoint(p.x*d,p.y*d);
}


double getDistance(Point p1,Point p2){
	return sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2));
}

int main(){
	
	int cntOfPoints = 3000;
	
	
	int index = 0;
	double x,y,length=0,lenPerPoint,lenNow=0;
	for(double t=0;t<2500;t+=3/sqrt(pow(3+t/900,2)+pow(2+t/1800,2))*(1+0.7*fabs(cos((t/2000)*6.28)))){
		//printf("%lf\n",i);
		x = 600+cos((t/2000)*6.28)*(300+t/9);
		y = 320+sin((t/2000)*6.28)*(200+t/18);
		
		pointArray[index++] = makePoint(x,y);
//		if(index!=0){
//			length += getDistance(pointArray[index],pointArray[index-1]);
//			//printf("index=%d,length=%.2lf\n",index,length);
//		}
		//printf("%.4lf %.4lf\n",x,y);
	}
	for(int j=1;j<index;j++){
		length += getDistance(pointArray[j],pointArray[j-1]);
	}
	printf("\n\nlength\n\n");
	printf("length=%.4lf\n",length);
	
	for(int i=0,j;i<cntOfPoints;i++){
		j=1;
		lenNow=0;
		while(lenNow<length/cntOfPoints*i){
			//printf("i=%d,lenNow=%.2lf,j=%d\n",i,lenNow,j);
			lenNow+=getDistance(pointArray[j],pointArray[j-1]);
			j++;
		}
		ret[i] = minus(pointArray[j],multiply(minus(pointArray[j],pointArray[j-1]),(lenNow-length/cntOfPoints*i)/getDistance(pointArray[j],pointArray[j-1])));
		printf("%.4lf %.4lf\n",ret[i].x,ret[i].y);
	}
	//while(1);
	return 0;
}
