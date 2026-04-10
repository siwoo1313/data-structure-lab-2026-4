#pragma once
#include <cstdio>
#define MAX_DEGREE 80		//다항식이 가질 수 있는 최대 항의 개수를 80으로 설정

class Polynomial {				//다항식 클래스
	int degree;					//다항식의 최고 차수를 저장할 변수
	float coef[MAX_DEGREE];		//다항식의 각 항의 계수들을 저장할 배열
public:
	Polynomial() : degree(0), coef{ 0.0f } {	//생성자함수
	}
	void read() {											//다항식의 차수와 계수를 입력받는 함수
		printf("다항식의 최고 차수를 입력하시오 : ");
		scanf_s("%d", &degree );
		printf("각 항의 계수를 입력하시오 (총 %d개): \n", degree + 1);
		for (int i = 0; i <= degree; i++) {					//for문을 돌면서 최고 차수부터 상수항까지 계수를 입력받음
			if (degree - i == 0) {
				printf("상수항 : ");
			}
			else {
				printf("%d차 : ", degree-i );
			}
			scanf_s("%f", coef + i);						//입력받은 계수 값을 배열의 i번째 칸에 저장
		}
	}
	void display(const char* str = "Poly = ") {				//다항식을 출력하는 함수
		printf("%s", str);
		for (int i = 0; i < degree; i++)
			printf("%5.1f x^%d +", coef[i], degree - i);
		printf("%4.1f\n", coef[degree]);
	}
	void add(Polynomial a, Polynomial b) {					//두 다항식을 더하는 함수
		if (a.degree > b.degree) {							//a의 차수가 더 크면
			*this = a;										//a를 복사
			for (int i = 0; i <= b.degree; i++)				//for문을 돌면서 b의 계수들을 더한다
				coef[i + (degree - b.degree) ]+= b.coef[i];
		}
		else {												//a의 차수가 더 크지 않으면
			*this = b;										//b를 복사
			for (int i = 0; i <= a.degree; i++)				//for문을 돌면서 a의 계수들을 더한다
				coef[i + (degree - a.degree)] += a.coef[i];
		}
	}
	bool isZero() { return degree == 0; }					//다항식이 상수항밖에 안 남았는지 알려주는 함수
	void negate() {
		for (int i = 0; i <= degree; i++) {					//모든 계수에 -를 붙여 부호를 바꿔주는 함수
			coef[i] = -coef[i];
		}
	}
};