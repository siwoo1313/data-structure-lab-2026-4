#include "Location2D.h"
#include <stack>
#include <iostream>
#include <deque>
#include <queue>

using namespace std;
#define MAZE_SIZE 6

char originalMap[MAZE_SIZE][MAZE_SIZE] = {
	{'1', '1', '1', '1', '1', '1'},
	{'e', '0', '1', '0', '0', '1'},
	{'1', '0', '0', '0', '1', '1'},
	{'1', '0', '1', '0', '1', '1'},
	{'1', '0', '1', '0', '0', 'x'},
	{'1', '1', '1', '1', '1', '1'},
};

char map[MAZE_SIZE][MAZE_SIZE];

// 맵 초기화 함수
void resetMap() {
	for (int i = 0; i < MAZE_SIZE; i++) {
		for (int j = 0; j < MAZE_SIZE; j++) {
			map[i][j] = originalMap[i][j];
		}
	}
}

// 이동 가능한 좌표인지 확인하는 함수
bool isValidLoc(int r, int c) {
	if (r < 0 || c < 0 || r >= MAZE_SIZE || c >= MAZE_SIZE) return false;
	return map[r][c] == '0' || map[r][c] == 'x' || map[r][c] == 'e';
}

int main() {
	int ds_num;
	::printf("DFS 데이터 구조 라이브러리 선택 : 1) stack, 2) deque \n");
	::printf("입력 : ");
	scanf_s("%d", &ds_num);

	Location2D entry(1, 0); // 출발점 설정 (e의 위치)
	resetMap();             // 맵 초기화
	bool dfs_success = false;

	// 1단계: 선택한 데이터 구조에 따라 DFS 실행
	switch (ds_num) {
	case 1: {
		// 1. Stack을 이용한 DFS (깊이 우선 탐색)
		::printf("-> Stack(DFS) 미로 탐색을 시작합니다.\n");
		stack<Location2D> locStack;

		locStack.push(entry);
		map[entry.row][entry.col] = '.';

		while (!locStack.empty()) {
			Location2D here = locStack.top();
			locStack.pop();

			int r = here.row;
			int c = here.col;

			printf("(%d,%d) ", r, c);

			if (originalMap[r][c] == 'x') {
				printf("\nDFS 성공\n");
				break;
			}

			// 역순 push
			if (isValidLoc(r, c + 1)) {
				map[r][c + 1] = '.';
				locStack.push(Location2D(r, c + 1));
			}

			if (isValidLoc(r, c - 1)) {
				map[r][c - 1] = '.';
				locStack.push(Location2D(r, c - 1));
			}

			if (isValidLoc(r + 1, c)) {
				map[r + 1][c] = '.';
				locStack.push(Location2D(r + 1, c));
			}

			if (isValidLoc(r - 1, c)) {
				map[r - 1][c] = '.';
				locStack.push(Location2D(r - 1, c));
			}
		}
		break;
	}

	case 2: {
		// 2. Deque를 이용한 DFS
		::printf("-> Deque(DFS) 미로 탐색을 시작합니다.\n");
		deque<Location2D> locDeque;

		locDeque.push_back(entry);
		map[entry.row][entry.col] = '.'; // 시작점 방문 처리

		while (!locDeque.empty()) {

			Location2D here = locDeque.back();
			locDeque.pop_back();

			int r = here.row;
			int c = here.col;

			::printf("(%d, %d) ", r, c);

			// 도착점 검사
			if (originalMap[r][c] == 'x') {
				::printf("\n[DFS] 미로 탐색 성공\n");
				dfs_success = true;
				break;
			}

			// DFS는 Stack 방식이므로
			// 원하는 탐색 순서의 역순으로 push

			// 우
			if (isValidLoc(r, c + 1)) {
				map[r][c + 1] = '.';
				locDeque.push_back(Location2D(r, c + 1));
			}

			// 좌
			if (isValidLoc(r, c - 1)) {
				map[r][c - 1] = '.';
				locDeque.push_back(Location2D(r, c - 1));
			}

			// 하
			if (isValidLoc(r + 1, c)) {
				map[r + 1][c] = '.';
				locDeque.push_back(Location2D(r + 1, c));
			}

			// 상
			if (isValidLoc(r - 1, c)) {
				map[r - 1][c] = '.';
				locDeque.push_back(Location2D(r - 1, c));
			}
		}

		if (!dfs_success)
		break;
	}

	default:
		break;
	} // switch 끝

	// 2단계: DFS가 끝난 후 이어서 BFS 자동 실행
	::printf("\n------------------------------------\n");
	::printf("-> Queue(BFS) 미로 탐색을 시작합니다.\n");

	resetMap(); // DFS가 지나간 흔적을 지우고 깨끗한 상태로 리셋
	bool bfs_success = false;

	queue<Location2D> locQueue;

	locQueue.push(entry);
	map[entry.row][entry.col] = '.';

	while (!locQueue.empty()) {
		Location2D here = locQueue.front();
		locQueue.pop();

		int r = here.row;
		int c = here.col;

		printf("(%d,%d) ", r, c);

		if (originalMap[r][c] == 'x') {
			printf("\nBFS 성공\n");
			break;
		}

		// BFS는 원하는 순서대로 enqueue
		if (isValidLoc(r - 1, c)) {
			map[r - 1][c] = '.';
			locQueue.push(Location2D(r - 1, c));
		}

		if (isValidLoc(r + 1, c)) {
			map[r + 1][c] = '.';
			locQueue.push(Location2D(r + 1, c));
		}

		if (isValidLoc(r, c - 1)) {
			map[r][c - 1] = '.';
			locQueue.push(Location2D(r, c - 1));
		}

		if (isValidLoc(r, c + 1)) {
			map[r][c + 1] = '.';
			locQueue.push(Location2D(r, c + 1));
		}
	}

	return 0;
}






