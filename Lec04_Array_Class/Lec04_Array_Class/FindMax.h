#pragma once	
#define HEIGHT 8			//배열의 크기를 정의
#define WIDTH 8

class FindMax{
public:
	int findMaxPixel(int img[][WIDTH], int h, int w) {	//함수의 형태를 선언
		int maxVal = img[0][0];				//첫 번째 화소 값을 임시 최대값으로 설정
		for (int i = 0; i < h; i++) {		//이중 for문을 돌며 2차원 배열의 모든 요소를 검사
			for (int j = 0; j < w; j++) {
				if (img[i][j] > maxVal) {	//현재 값이 기존 최댓값보다 크면
					maxVal = img[i][j];		//최댓값을 갱신
				}
			}
		}
		return maxVal;						//최댓값 반환
	}
};