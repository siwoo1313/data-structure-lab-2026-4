#pragma once
#include <cstdio>

class Complex
{								//private 생략됨
	double real;				//실수부
	double imag;				//허수부
public:
	void set(double r, double i) {	
		real = r;
		imag = i;
	}
	void readReal(const char* msg = "실수부 =") {		//실수부를 입력 받는 함수
		printf("%s", msg);		
		scanf_s("%lf", &real);
	}
	void readImag(const char* msg = "허수부 =") {		//허수부를 입력 받는 함수
		printf("%s", msg);
		scanf_s("%lf", &imag);
	}
	void print(const char* msg = "복소수=") {			//복소수를 출력하는 함수
		printf("%s%4.2f + %4.2fi\n", msg, real, imag);
	}
	void add(Complex a, Complex b) {					//두 개의 복소수의 합을 구하는 함수
		real = a.real + b.real;							//실수부의 합
		imag = a.imag + b.imag;							//허수부의 합
	}
};