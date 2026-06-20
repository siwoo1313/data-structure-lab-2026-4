A = {
    0: [1, 2, 3],
    1: [0, 2, 4, 5],
    2: [0, 1, 6],
    3: [0],
    4: [1],
    5: [1],
    6: [2]
}


def bfs(A):
    """
    그래프 A를 너비 우선 탐색(BFS) 방식으로 방문한다.

    시작 노드는 0번으로 고정한다.
    BFS는 시작 노드와 가까운 노드부터 차례대로 방문하는 탐색 방법이다.
    시작 노드에서 거리가 1인 노드들을 먼저 방문하고,
    그 다음 거리가 2인 노드들을 방문하는 방식으로 진행된다.

    Problem 1에서는 같은 깊이의 노드를 방문할 수 있는 경우
    번호가 작은 노드부터 방문해야 한다.
    따라서 각 노드의 인접 노드를 확인할 때 sorted()를 사용한다.

    Parameters
    ----------
    A : dict
        각 노드와 연결된 이웃 노드들을 저장한 인접 리스트 형태의 그래프

    Returns
    -------
    answer : list
        BFS 순서대로 방문한 노드 번호를 저장한 리스트
    """
    answer = []
    visited = set()

    queue = [0]
    visited.add(0)

    while queue:
        node = queue.pop(0)
        answer.append(node)

        for next_node in sorted(A[node]):
            if next_node not in visited:
                visited.add(next_node)
                queue.append(next_node)

    return answer


def dfs(A):
    """
    그래프 A를 깊이 우선 탐색(DFS) 방식으로 방문한다.

    시작 노드는 0번으로 고정한다.
    DFS는 한 방향으로 갈 수 있는 만큼 깊게 들어간 뒤,
    더 이상 갈 곳이 없으면 이전 노드로 돌아와 다른 경로를 탐색하는 방식이다.

    Problem 2에서는 같은 단계에서 여러 노드를 방문할 수 있는 경우
    번호가 작은 노드부터 방문해야 한다.

    DFS는 스택을 사용하여 구현하였다.
    스택은 마지막에 들어간 값이 먼저 나오는 구조이므로,
    번호가 작은 노드가 먼저 pop되도록 인접 노드를 내림차순으로 스택에 넣는다.

    또한 노드를 스택에 넣는 순간 visited에 추가한다.
    이렇게 하면 이미 방문 예정인 노드가 다른 경로에서 중복으로 추가되는 것을
    방지할 수 있다.

    Parameters
    ----------
    A : dict
        각 노드와 연결된 이웃 노드들을 저장한 인접 리스트 형태의 그래프

    Returns
    -------
    answer : list
        DFS 순서대로 방문한 노드 번호를 저장한 리스트
    """
    answer = []
    visited = set()

    stack = [0]
    visited.add(0)

    while stack:
        node = stack.pop()
        answer.append(node)

        for next_node in sorted(A[node], reverse=True):
            if next_node not in visited:
                visited.add(next_node)
                stack.append(next_node)

    return answer


# 아래는 체크함수입니다. 수정하실 필요 없습니다.
bfs_result = bfs(A)
dfs_result = dfs(A)

assert bfs_result == [0,1,2,3,4,5,6]
assert dfs_result == [0,1,4,5,2,6,3]
print('PASSED!')

