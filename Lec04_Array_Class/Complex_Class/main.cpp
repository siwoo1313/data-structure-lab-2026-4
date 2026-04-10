#include "Complex.h"			//복소수 클래스 헤더파일 불러오기
int main()
{
	Complex a, b, c;
	printf("---복소수 A 입력---\n");		
	a.readReal("A의 실수부 : ");			//복소수 A의 실수부를 입력받기
	a.readImag("A의 허수부 : ");			//복소수 A의 허수부를 입력받기
	printf("---복소수 B 입력---\n");
	b.readReal("B의 실수부 : ");			//복소수 B의 실수부를 입력받기
	b.readImag("B의 허수부 : ");			//복소수 B의 허수부를 입력받기
	printf("---입력받은 A와 B---\n");
	a.print("입력된 A = ");					//입력된 복소수 A를 출력하기
	b.print("입력된 B = ");					//입력된 복소수 B를 출력하기
	printf("---A와 B의 합---\n");
	c.add(a, b);							//A와B의 합을 C에 넣기
	c.print("A+B=");						//A와B의 합을 출력하기
}