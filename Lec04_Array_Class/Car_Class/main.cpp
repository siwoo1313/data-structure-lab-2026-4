#include "Car.h"			//자동차 클래스 헤더파일 불러오기
#include "SportsCar.h"		//스포츠카 클래스 헤더파일 불러오기		

int main() {	
	SportsCar myCar;		//스포츠카 객체 생성
	printf("---초기 상태---\n");
	myCar.display();		//속도 출력

	myCar.changeGear(5);	//기어를 5단으로 설정
	myCar.setTurbo(true);	//터보 on
	myCar.speedUp();		//가속

	printf("---주행 후 상태---\n");
	myCar.display();		//속도 출력
	myCar.whereAmI();		//this가 가리키는 메모리 주소 출력

	return 0;
}