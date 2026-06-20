import sys
from collections import deque

## 입력 받는 코드입니다. 수정할 필요 없습니다.
sys.stdin = open('case.txt')
N, M = list(map(int,input().split()))
print(N, M)
concerts = []
for v in range(N):
    values = list(map(int, input().split()))
    concerts.append(values)
# print(concerts)
# [[1, 0, 0, 1, 1, 0], [1, 0, 1, 1, 0, 0], [1, 1, 1, 1, 0, 1], [0, 1, 1, 0, 1, 1], [0, 1, 0, 0, 1, 0]]
###################################

def count_stages(concerts):
    """
    콘서트장 약도에서 독립적인 무대 공간의 개수를 계산한다.

    콘서트장 약도는 0과 1로 이루어진 2차원 리스트이다.
    1은 펜스가 있는 공간이고, 0은 빈 공간을 의미한다.
    빈 공간인 0들이 상하좌우로 연결되어 있으면 하나의 같은 무대 공간으로 본다.

    이 함수는 BFS를 사용하여 아직 방문하지 않은 0을 찾고,
    그 0과 연결된 모든 0을 하나의 무대 공간으로 처리한다.
    따라서 새롭게 발견되는 연결된 0의 묶음 개수가 곧 독립적인 무대 공간의 개수이다.

    Parameters
    ----------
    concerts : list[list[int]]
        N x M 크기의 콘서트장 약도.
        각 원소는 0 또는 1이다.
        0은 무대 공간, 1은 펜스를 의미한다.

    Returns
    -------
    int
        독립적인 무대 공간의 개수.
    """
    if not concerts:
        return 0

    n = len(concerts)
    m = len(concerts[0])

    visited = [[False] * m for _ in range(n)]

    # 상, 하, 좌, 우 이동 방향
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    stage_count = 0

    for row in range(n):
        for col in range(m):
            # 아직 방문하지 않은 빈 공간을 발견하면 새로운 무대 공간이다.
            if concerts[row][col] == 0 and not visited[row][col]:
                stage_count += 1

                # BFS 시작
                queue = deque()
                queue.append((row, col))
                visited[row][col] = True

                while queue:
                    current_row, current_col = queue.popleft()

                    for dr, dc in directions:
                        next_row = current_row + dr
                        next_col = current_col + dc

                        # 콘서트장 범위 안에 있는지 확인
                        if 0 <= next_row < n and 0 <= next_col < m:
                            # 방문하지 않은 빈 공간이면 같은 무대 공간으로 연결
                            if concerts[next_row][next_col] == 0 and not visited[next_row][next_col]:
                                visited[next_row][next_col] = True
                                queue.append((next_row, next_col))

    return stage_count


print(count_stages(concerts))
