#include <cstdio>
#include "Polynomial.h"						//다항식 클래스 헤더파일 불러오기

int main() {
	Polynomial a, b, c;
	printf("---다항식 A 입력---\n");
	a.read();								//다항식 A 입력받기
	printf("---다항식 B 입력---\n");
	b.read();								//다항식 B 입력받기
	c.add(a, b);							//다항식 A와B의 합 구하기
	a.display("A =");						//다항식 A 출력하기
	b.display("B =");						//다항식 B 출력하기
	c.display("A+B =");						//다항식 B와B의 합 출력하기
}