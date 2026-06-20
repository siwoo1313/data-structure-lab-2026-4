#include <stdio.h>
#include "FindMax.h"					//FindMax 클래스 헤더파일 불러오기

int main() {
	int image[HEIGHT][WIDTH] = {					//ppt에 주어진 예제 이미지를 2차원 배열로 선언
	{ 10, 30,  55,  80, 120, 160, 200, 230 },
	{ 20, 45,  70, 100, 140, 180, 220, 210 },
	{ 35, 60,  90, 130, 170, 210, 240, 190 },
	{ 50, 80, 115, 150, 190, 230, 255, 170 },
	{ 40, 65, 100, 140, 175, 215, 235, 150 },
	{ 25, 50,  80, 115, 155, 195, 210, 130 },
	{ 15, 35,  60,  90, 130, 165, 185, 110 },
	{  5, 20,  40,  70, 105, 140, 160,  90 },
	};

	FindMax myFinder;											//FindMax 클래스의 객체 생성
	int maxPixel = myFinder.findMaxPixel(image, HEIGHT, WIDTH);	//최댓값 찾기
	printf("2차원 이미지의 최대 화소 밝기: %d\n", maxPixel);	//결과 출력

	return 0;
}


