#pragma once
#include "Car.h"				//자동차 클래스 헤더파일 불러오기

class SportsCar : public Car	//스포츠카 클래스가 자동차 클래스의 자식 클래스라고 선언
{
public:
	bool bTurbo;									//터보 장치가 켜져 있는지 꺼져 있는지 기억하는 변수
	void setTurbo(bool bTur) { bTurbo = bTur; }
	void speedUp() {								//터보가 on이 되어 있으면 가속이 빨리 된다
		if (bTurbo) speed += 20;					//터보가 켜져 있으면 속도를 20 올린다
		else Car::speedUp();						//터보가 켜져 있지 않으면 속도를 5 올린다
	}
};