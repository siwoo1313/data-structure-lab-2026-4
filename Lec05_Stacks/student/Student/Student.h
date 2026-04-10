#pragma once
#include <cstdio>
#include <cstring>
#include <cstdlib>
#define MAX_STRING 500
#define MAX_STACK_SIZE 500

class Student {
	int id;							// 학번
	char name[MAX_STRING];			// 이름
	char dept[MAX_STRING];			// 소속 학과
public:
	Student(int i = 0, const char* n = "", const char* d = "") {
		set(i, n, d);
	}
	void set(int i, const char* n, const char* d) {
		id = i;
		strcpy_s(name, n);			// 문자열 복사 함수
		strcpy_s(dept, d);			// 문자열 복사 함수
	}
	void display() {
		printf(" 학번:%-15d 성명:%-10s 학과:%-20s\n", id, name, dept);
	}
};

class ArrayStack
{
	int top;								 // 요소의 개수
	Student data[MAX_STACK_SIZE];
public:
	ArrayStack() { top = -1; }
	bool isEmpty() { return top == -1; }
	bool isFull() { return top == MAX_STACK_SIZE  -1; }

	void error(const char* msg) {
		printf("스택 에러 : %s\n", msg);
		exit(1);
	}
	void push(Student e) {
		if (isFull()) error("스택포화 상태입니다!");
		top++;
		data[top] = e;
	}
	Student pop() {
		if (isEmpty()) error("스택 공백 상태입니다!");
		Student e = data[top];
		top--;
		return e;
	}
	Student peek() {
		if (isEmpty()) error("스택 공백 상태입니다!");
		return data[top];
	}
	void display() {
		printf("[스택 항목의 수 = %2d] ==> \n", top + 1);
		for (int i = 0; i <= top; i++) {
			data[i].display();
		}
		printf("\n");
	}
};