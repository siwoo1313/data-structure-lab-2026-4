#include <QtWidgets>
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>
#include <functional>
#include <cmath>

/*
    ============================================================
    2026-1 데이터구조 최종 프로젝트

    프로젝트명:
        부산 도시철도 환승 경로 추천 프로그램

    구현 환경:
        C++ / Qt Widgets

    사용 자료구조:
        그래프(Graph) - 인접 리스트(Adjacency List)

    구현 기능:
        1. 부산 도시철도 노선도 이미지 표시
        2. 지도 위 역 클릭으로 출발역/도착역 지정
        3. 최소 환승 경로 탐색
        4. 최소 정거장 경로 탐색
        5. 탐색 경로를 지도 위에 표시

    좌표 기준:
        - images/busan_metro_map.jpg 원본 크기 2048 x 626 기준
        - 역 클릭 좌표는 원본 이미지 2048 x 626 기준으로 다시 보정
        - 이미지는 확대/축소 없이 원본 크기로 표시하여 화질과 좌표 안정성을 우선
    ============================================================
*/

// ------------------------------------------------------------
// 한글 문자열을 Qt QString으로 변환하는 함수
// ------------------------------------------------------------
static QString kor(const char* text) {
    return QString::fromUtf8(text);
}

// ------------------------------------------------------------
// 노선도 이미지 위 역 위치를 저장하기 위한 구조체
// name : 역 이름
// x, y : 이미지 원본 기준 좌표
// ------------------------------------------------------------
struct StationDef {
    const char* name;
    double x;
    double y;
};

// ------------------------------------------------------------
// 간선 구조체
// to     : 연결된 다음 역 번호
// line   : 이용 노선 이름
// lineId : 노선 내부 번호, 환승 여부 계산에 사용
// ------------------------------------------------------------
struct Edge {
    int to;
    QString line;
    int lineId;
};

// ------------------------------------------------------------
// 역 구조체
// name  : 역 이름
// lines : 해당 역을 지나는 노선 목록
// pos   : 노선도 이미지 원본 기준 역 좌표
// ------------------------------------------------------------
struct Station {
    QString name;
    QStringList lines;
    QPointF pos;
};

// ------------------------------------------------------------
// 경로 탐색 결과 구조체
// found         : 경로 탐색 성공 여부
// transferCount : 환승 횟수
// stationCount  : 경로에 포함된 전체 역 수
// stations      : 경로에 포함된 역 번호 목록
// usedLines     : 각 구간에서 이용한 노선 목록
// ------------------------------------------------------------
struct RouteResult {
    bool found = false;
    int transferCount = 0;
    int stationCount = 0;
    QVector<int> stations;
    QVector<QString> usedLines;
};

// ============================================================
// MetroGraph 클래스
// ------------------------------------------------------------
// 역할:
//   - 부산 도시철도 역/노선 데이터를 그래프로 저장한다.
//   - 최소 정거장 경로를 BFS로 찾는다.
//   - 최소 환승 경로를 상태 확장 Dijkstra 방식으로 찾는다.
//
// 그래프 표현:
//   - stations_[i] : i번 역 정보
//   - adj_[i]      : i번 역과 연결된 인접 역 목록
// ============================================================
class MetroGraph {
public:
    enum SearchMode {
        MinTransfer,
        MinStation
    };

    MetroGraph() {
        buildGraph();
    }

    // --------------------------------------------------------
    // 전체 역 정보 반환
    // 지도 클릭 시 가장 가까운 역을 찾기 위해 사용한다.
    // --------------------------------------------------------
    const QVector<Station>& stations() const {
        return stations_;
    }

    // --------------------------------------------------------
    // 전체 역 이름 목록 반환
    // ComboBox와 자동완성에 사용한다.
    // --------------------------------------------------------
    QStringList stationNames() const {
        QStringList names;
        for (const Station& s : stations_) {
            names << s.name;
        }
        names.sort(Qt::CaseInsensitive);
        return names;
    }

    // --------------------------------------------------------
    // 역 이름으로 내부 역 번호 찾기
    // 존재하지 않으면 -1 반환
    // --------------------------------------------------------
    int stationId(const QString& name) const {
        return stationNameToId_.value(name, -1);
    }

    // --------------------------------------------------------
    // 역 번호로 역 이름 반환
    // --------------------------------------------------------
    QString stationName(int id) const {
        if (id < 0 || id >= stations_.size()) return QString();
        return stations_[id].name;
    }

    // --------------------------------------------------------
    // 역 번호로 해당 역의 노선 목록 반환
    // --------------------------------------------------------
    QString stationLines(int id) const {
        if (id < 0 || id >= stations_.size()) return QString();
        return stations_[id].lines.join(", ");
    }

    // --------------------------------------------------------
    // 외부에서 호출하는 경로 탐색 함수
    // mode에 따라 최소 환승 또는 최소 정거장 탐색을 수행한다.
    // --------------------------------------------------------
    RouteResult searchRoute(const QString& startName,
                            const QString& endName,
                            SearchMode mode) const {
        int start = stationId(startName);
        int goal = stationId(endName);

        if (start < 0 || goal < 0) {
            return RouteResult();
        }

        if (start == goal) {
            RouteResult r;
            r.found = true;
            r.transferCount = 0;
            r.stationCount = 1;
            r.stations << start;
            return r;
        }

        if (mode == MinStation) {
            return searchMinStation(start, goal);
        }

        return searchMinTransfer(start, goal);
    }

    // --------------------------------------------------------
    // 이미지 좌표를 기준으로 가장 가까운 역 찾기
    // maxDistance보다 멀면 -1 반환한다.
    // --------------------------------------------------------
    int nearestStation(const QPointF& imagePoint, double maxDistance = 46.0) const {
        int bestId = -1;
        double bestDist = maxDistance;

        for (int i = 0; i < stations_.size(); ++i) {
            double dx = stations_[i].pos.x() - imagePoint.x();
            double dy = stations_[i].pos.y() - imagePoint.y();
            double d = std::sqrt(dx * dx + dy * dy);

            if (d <= bestDist) {
                bestDist = d;
                bestId = i;
            }
        }

        return bestId;
    }

private:
    QVector<Station> stations_;
    QVector<QVector<Edge>> adj_;
    QHash<QString, int> stationNameToId_;
    QHash<QString, int> lineNameToId_;
    QStringList lineNames_;

    // --------------------------------------------------------
    // 노선을 내부 번호로 변환한다.
    // 처음 등장한 노선이면 새 번호를 부여한다.
    // --------------------------------------------------------
    int ensureLine(const QString& lineName) {
        if (lineNameToId_.contains(lineName)) {
            return lineNameToId_[lineName];
        }

        int id = lineNames_.size();
        lineNames_ << lineName;
        lineNameToId_[lineName] = id;
        return id;
    }

    // --------------------------------------------------------
    // 역을 그래프에 추가한다.
    // 이미 존재하는 역이면 기존 노드를 재사용한다.
    // 이 방식으로 환승역이 하나의 노드로 자연스럽게 합쳐진다.
    // --------------------------------------------------------
    int ensureStation(const QString& stationName,
                      const QString& lineName,
                      const QPointF& pos) {
        if (stationNameToId_.contains(stationName)) {
            int id = stationNameToId_[stationName];

            if (!stations_[id].lines.contains(lineName)) {
                stations_[id].lines << lineName;
            }

            return id;
        }

        int id = stations_.size();

        Station s;
        s.name = stationName;
        s.lines << lineName;
        s.pos = pos;

        stations_ << s;
        adj_ << QVector<Edge>();
        stationNameToId_[stationName] = id;

        return id;
    }

    // --------------------------------------------------------
    // 양방향 간선 추가
    // 지하철은 양방향 이동이 가능하다고 가정한다.
    // --------------------------------------------------------
    void addEdge(int a, int b, const QString& lineName) {
        int lineId = ensureLine(lineName);
        adj_[a].push_back({b, lineName, lineId});
        adj_[b].push_back({a, lineName, lineId});
    }

    // --------------------------------------------------------
    // 하나의 노선을 역 순서대로 추가한다.
    // A-B-C-D 형태로 들어오면 A-B, B-C, C-D 간선을 생성한다.
    // --------------------------------------------------------
    void addLine(const QString& lineName, std::initializer_list<StationDef> sequence) {
        if (sequence.size() < 2) return;

        ensureLine(lineName);

        QVector<int> ids;
        for (const StationDef& s : sequence) {
            ids << ensureStation(kor(s.name), lineName, QPointF(s.x, s.y));
        }

        for (int i = 0; i + 1 < ids.size(); ++i) {
            addEdge(ids[i], ids[i + 1], lineName);
        }
    }

    // --------------------------------------------------------
    // 사용 노선 배열을 보고 환승 횟수 계산
    // 예: 1호선, 1호선, 2호선, 2호선이면 환승 1회
    // --------------------------------------------------------
    static int countTransfers(const QVector<QString>& usedLines) {
        if (usedLines.isEmpty()) return 0;

        int count = 0;
        QString prev = usedLines[0];

        for (int i = 1; i < usedLines.size(); ++i) {
            if (usedLines[i] != prev) {
                ++count;
                prev = usedLines[i];
            }
        }

        return count;
    }

    // --------------------------------------------------------
    // 최소 정거장 탐색
    // 모든 간선 비용이 1인 그래프에서 최단 간선 수 경로는 BFS로 구한다.
    // --------------------------------------------------------
    RouteResult searchMinStation(int start, int goal) const {
        RouteResult result;
        int n = stations_.size();

        QVector<bool> visited(n, false);
        QVector<int> parent(n, -1);
        QVector<QString> parentLine(n);

        std::queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int cur = q.front();
            q.pop();

            if (cur == goal) break;

            for (const Edge& e : adj_[cur]) {
                if (!visited[e.to]) {
                    visited[e.to] = true;
                    parent[e.to] = cur;
                    parentLine[e.to] = e.line;
                    q.push(e.to);
                }
            }
        }

        if (!visited[goal]) return result;

        QVector<int> path;
        for (int cur = goal; cur != -1; cur = parent[cur]) {
            path.push_front(cur);
        }

        QVector<QString> usedLines;
        for (int i = 1; i < path.size(); ++i) {
            usedLines << parentLine[path[i]];
        }

        result.found = true;
        result.stations = path;
        result.usedLines = usedLines;
        result.stationCount = path.size();
        result.transferCount = countTransfers(usedLines);
        return result;
    }

    // --------------------------------------------------------
    // 최소 환승 탐색
    // 상태 = (현재 역, 현재 이용 중인 노선)
    // 비용 = (환승 횟수, 정거장 수)
    // 환승 횟수를 먼저 최소화하고, 동률이면 정거장 수를 최소화한다.
    // --------------------------------------------------------
    RouteResult searchMinTransfer(int start, int goal) const {
        RouteResult result;

        int stationCount = stations_.size();
        int lineStateCount = lineNames_.size() + 1;
        int totalStateCount = stationCount * lineStateCount;
        const int INF = 1000000000;

        auto makeState = [lineStateCount](int station, int lineState) {
            return station * lineStateCount + lineState;
        };

        auto getStation = [lineStateCount](int state) {
            return state / lineStateCount;
        };

        auto getLineState = [lineStateCount](int state) {
            return state % lineStateCount;
        };

        struct Cost {
            int transfer;
            int station;
        };

        auto isBetter = [](const Cost& a, const Cost& b) {
            if (a.transfer != b.transfer) return a.transfer < b.transfer;
            return a.station < b.station;
        };

        struct Node {
            int state;
            Cost cost;
        };

        auto cmp = [](const Node& a, const Node& b) {
            if (a.cost.transfer != b.cost.transfer) {
                return a.cost.transfer > b.cost.transfer;
            }
            return a.cost.station > b.cost.station;
        };

        QVector<Cost> dist(totalStateCount, {INF, INF});
        QVector<int> parent(totalStateCount, -1);
        QVector<QString> parentLine(totalStateCount);

        std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);

        int startState = makeState(start, 0);
        dist[startState] = {0, 1};
        pq.push({startState, {0, 1}});

        int bestGoalState = -1;

        while (!pq.empty()) {
            Node node = pq.top();
            pq.pop();

            int curState = node.state;
            int curStation = getStation(curState);
            int curLineState = getLineState(curState);

            if (dist[curState].transfer != node.cost.transfer ||
                dist[curState].station != node.cost.station) {
                continue;
            }

            if (curStation == goal) {
                bestGoalState = curState;
                break;
            }

            for (const Edge& e : adj_[curStation]) {
                int nextLineState = e.lineId + 1;
                int addTransfer = 0;

                if (curLineState != 0 && curLineState != nextLineState) {
                    addTransfer = 1;
                }

                Cost nextCost;
                nextCost.transfer = node.cost.transfer + addTransfer;
                nextCost.station = node.cost.station + 1;

                int nextState = makeState(e.to, nextLineState);

                if (isBetter(nextCost, dist[nextState])) {
                    dist[nextState] = nextCost;
                    parent[nextState] = curState;
                    parentLine[nextState] = e.line;
                    pq.push({nextState, nextCost});
                }
            }
        }

        if (bestGoalState < 0) return result;

        QVector<int> statePath;
        for (int s = bestGoalState; s != -1; s = parent[s]) {
            statePath.push_front(s);
        }

        QVector<int> path;
        QVector<QString> usedLines;

        for (int i = 0; i < statePath.size(); ++i) {
            path << getStation(statePath[i]);
            if (i > 0) {
                usedLines << parentLine[statePath[i]];
            }
        }

        result.found = true;
        result.stations = path;
        result.usedLines = usedLines;
        result.stationCount = path.size();
        result.transferCount = countTransfers(usedLines);
        return result;
    }

    // --------------------------------------------------------
    // 부산 도시철도 그래프 데이터 구성
    // 좌표는 노선도 이미지(2048 x 626) 기준이다.
    // --------------------------------------------------------
    void buildGraph() {
        addLine(kor("1호선"), {
            {"다대포해수욕장", 410, 608}, {"다대포항", 448, 608}, {"낫개", 488, 608},
            {"신장림", 526, 608}, {"장림", 564, 608}, {"동매", 602, 608},
            {"신평", 642, 608}, {"하단", 680, 608}, {"당리", 718, 608},
            {"사하", 758, 608}, {"괴정", 796, 608}, {"대티", 834, 608},
            {"서대신", 874, 608}, {"동대신", 912, 608}, {"토성", 952, 608},
            {"자갈치", 990, 608}, {"남포", 1028, 608}, {"중앙", 1068, 608},
            {"부산역", 1106, 608}, {"초량", 1144, 608}, {"부산진", 1182, 608},
            {"좌천(1호선)", 1222, 608}, {"범일", 1254, 586}, {"범내골", 1256, 546},
            {"서면", 1256, 504}, {"부전(1호선)", 1256, 455}, {"양정", 1256, 408},
            {"시청", 1256, 360}, {"연산", 1256, 316}, {"교대", 1256, 248},
            {"동래", 1256, 170}, {"명륜", 1298, 82}, {"온천장", 1380, 82},
            {"부산대", 1462, 82}, {"장전", 1544, 82}, {"구서", 1628, 82},
            {"두실", 1710, 82}, {"남산", 1792, 82}, {"범어사", 1874, 82}, {"노포", 1954, 82}
        });

        addLine(kor("2호선"), {
            {"양산", 1165, 82}, {"남양산", 1110, 82}, {"부산대양산캠퍼스", 1056, 82},
            {"증산", 1002, 82}, {"호포", 946, 82}, {"금곡", 890, 82}, {"동원", 838, 85},
            {"율리", 819, 132}, {"화명", 819, 190}, {"수정", 819, 247}, {"덕천", 818, 316},
            {"구명", 820, 346}, {"구남", 820, 380}, {"모라", 820, 412}, {"모덕", 820, 444},
            {"덕포", 820, 476}, {"사상", 846, 504}, {"감전", 906, 504}, {"주례", 954, 504},
            {"냉정", 1004, 504}, {"개금", 1054, 504}, {"동의대", 1104, 504}, {"가야", 1154, 504},
            {"부암", 1202, 504}, {"서면", 1256, 504}, {"전포", 1308, 504},
            {"국제금융센터·부산은행", 1347, 504}, {"문현", 1380, 504}, {"지게골", 1416, 504},
            {"못골", 1450, 504}, {"대연", 1486, 504}, {"경성대·부경대", 1525, 487},
            {"남천", 1527, 445}, {"금련산", 1527, 405}, {"광안", 1526, 365}, {"수영", 1542, 318},
            {"민락", 1608, 318}, {"센텀시티", 1676, 318}, {"벡스코", 1740, 318},
            {"동백", 1802, 318}, {"해운대", 1850, 318}, {"중동", 1902, 318}, {"장산", 1957, 318}
        });

        addLine(kor("3호선"), {
            {"대저", 410, 316}, {"체육공원", 512, 318}, {"강서구청", 616, 316},
            {"구포", 718, 316}, {"덕천", 818, 316}, {"숙등", 886, 316},
            {"남산정", 938, 316}, {"만덕", 990, 316}, {"미남", 1030, 316},
            {"사직", 1084, 316}, {"종합운동장", 1126, 316}, {"거제", 1178, 316},
            {"연산", 1256, 316}, {"물만골", 1324, 316}, {"배산", 1388, 316},
            {"망미", 1450, 316}, {"수영", 1542, 318}
        });

        addLine(kor("4호선"), {
            {"미남", 1030, 316}, {"동래", 1254, 170}, {"수안", 1318, 170},
            {"낙민", 1372, 170}, {"충렬사", 1435, 170}, {"명장", 1492, 170},
            {"서동", 1552, 170}, {"금사", 1608, 170}, {"반여농산물시장", 1666, 170},
            {"석대", 1724, 170}, {"영산대", 1782, 170}, {"윗반송", 1840, 170},
            {"고촌", 1900, 170}, {"안평", 1957, 170}
        });

        addLine(kor("부산김해경전철"), {
            {"가야대", 794, 82}, {"장신대", 756, 82}, {"연지공원", 714, 82},
            {"박물관", 672, 82}, {"수로왕릉", 628, 82}, {"봉황", 586, 82},
            {"부원", 544, 82}, {"김해시청", 503, 82}, {"인제대", 460, 82},
            {"김해대학", 422, 88}, {"지내", 409, 126}, {"불암", 409, 170},
            {"대사", 409, 215}, {"평강", 409, 259}, {"대저", 410, 316},
            {"등구", 409, 384}, {"덕두", 409, 458}, {"공항", 496, 504},
            {"서부산유통지구", 596, 504}, {"괘법르네시떼", 694, 504}, {"사상", 846, 504}
        });

        addLine(kor("동해선"), {
            {"부전(동해선)", 1178, 438}, {"거제해맞이", 1178, 377}, {"거제", 1178, 316},
            {"교대", 1256, 248}, {"동래(동해선)", 1353, 246}, {"안락", 1442, 246},
            {"부산원동", 1534, 246}, {"재송", 1624, 246}, {"센텀", 1714, 246},
            {"벡스코", 1740, 318}, {"신해운대", 1740, 348}, {"송정", 1740, 374},
            {"오시리아", 1740, 398}, {"기장", 1740, 422}, {"일광", 1740, 448},
            {"좌천(동해선)", 1740, 472}, {"월내", 1786, 504}, {"서생", 1816, 504},
            {"남창", 1844, 504}, {"망양", 1872, 504}, {"덕하", 1900, 504},
            {"개운포", 1928, 504}, {"태화강", 1957, 504}
        });
    }
};

// ============================================================
// MetroMapView 클래스
// ------------------------------------------------------------
// QGraphicsView를 상속하여 노선도 이미지를 표시하고,
// 사용자의 마우스 클릭 좌표를 이미지 좌표로 변환한다.
// 변환된 좌표와 가장 가까운 역을 찾아 MainWindow에 전달한다.
// ============================================================
class MetroMapView : public QGraphicsView {
public:
    explicit MetroMapView(QWidget* parent = nullptr)
        : QGraphicsView(parent) {
        scene_ = new QGraphicsScene(this);
        setScene(scene_);
        setRenderHint(QPainter::Antialiasing, true);
        setDragMode(QGraphicsView::ScrollHandDrag);
        setResizeAnchor(QGraphicsView::AnchorViewCenter);
        // 확대/축소 기능을 사용하지 않고 원본 화질과 좌표 정확도를 우선한다.
    }

    // --------------------------------------------------------
    // 지도 이미지 설정
    // --------------------------------------------------------
    void setMapImage(const QPixmap& pixmap) {
        scene_->clear();
        overlayItems_.clear();
        stationPointItems_.clear();

        // 이미지의 화질이 깨지지 않게 원본 2048 x 626 이미지를 그대로 두고
        // 필요한 경우 스크롤바로 이동하면서 클릭하도록 구성했다.
        resetTransform();
        pixmapItem_ = scene_->addPixmap(pixmap);
        pixmapItem_->setPos(0, 0);
        scene_->setSceneRect(pixmap.rect());
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    // --------------------------------------------------------
    // 역 좌표 표시용 작은 점을 지도 위에 그린다.
    // 좌표가 맞는지 눈으로 확인할 수 있도록 하기 위한 기능이다.
    // --------------------------------------------------------
    void drawClickablePoints(const QVector<Station>& stations) {
        for (QGraphicsItem* item : stationPointItems_) {
            scene_->removeItem(item);
            delete item;
        }
        stationPointItems_.clear();

        for (const Station& s : stations) {
            double r = 4.2;
            QGraphicsEllipseItem* dot = scene_->addEllipse(
                s.pos.x() - r,
                s.pos.y() - r,
                r * 2,
                r * 2,
                QPen(QColor(220, 0, 0, 220), 2.0),
                QBrush(QColor(255, 255, 255, 170))
            );
            dot->setToolTip(s.name);
            stationPointItems_ << dot;
        }
    }

    // --------------------------------------------------------
    // 경로 결과를 지도 위에 붉은 선과 점으로 표시한다.
    // --------------------------------------------------------
    void drawRouteOverlay(const QVector<Station>& stations,
                          const QVector<int>& path) {
        clearRouteOverlay();

        if (path.isEmpty()) return;

        QPen routePen(QColor(220, 0, 0), 5.0);
        routePen.setCapStyle(Qt::RoundCap);
        routePen.setJoinStyle(Qt::RoundJoin);

        for (int i = 0; i + 1 < path.size(); ++i) {
            QPointF a = stations[path[i]].pos;
            QPointF b = stations[path[i + 1]].pos;
            QGraphicsLineItem* line = scene_->addLine(QLineF(a, b), routePen);
            line->setData(0, kor("route"));   // 결과 초기화 시 경로 아이템만 찾기 위한 표시
            line->setZValue(10);              // 노선도와 좌표 점 위에 경로가 보이도록 설정
            overlayItems_ << line;
        }

        for (int i = 0; i < path.size(); ++i) {
            int stationId = path[i];
            QPointF p = stations[stationId].pos;

            QColor color = QColor(255, 60, 60);
            if (i == 0) color = QColor(0, 120, 255);
            if (i == path.size() - 1) color = QColor(255, 0, 0);

            double r = (i == 0 || i == path.size() - 1) ? 10.0 : 6.0;

            QGraphicsEllipseItem* mark = scene_->addEllipse(
                p.x() - r,
                p.y() - r,
                r * 2,
                r * 2,
                QPen(Qt::black, 2.0),
                QBrush(color)
            );
            mark->setToolTip(stations[stationId].name);
            mark->setData(0, kor("route"));   // 결과 초기화 시 경로 아이템만 찾기 위한 표시
            mark->setZValue(11);              // 경로 선보다 점이 위에 보이도록 설정
            overlayItems_ << mark;
        }
    }

    // --------------------------------------------------------
    // 경로 표시 제거
    // --------------------------------------------------------
    void clearRouteOverlay() {
        if (!scene_) return;

        // overlayItems_에 저장된 포인터만 지우면,
        // 예외적으로 벡터와 실제 장면의 아이템 상태가 어긋났을 때 경로가 남을 수 있다.
        // 따라서 장면(scene)에 있는 모든 아이템 중 data(0)가 "route"인 것만 찾아서 제거한다.
        const QList<QGraphicsItem*> items = scene_->items();

        for (QGraphicsItem* item : items) {
            if (item->data(0).toString() == kor("route")) {
                scene_->removeItem(item);
                delete item;
            }
        }

        overlayItems_.clear();

        // 화면 갱신을 강제로 요청하여 제거된 경로가 즉시 사라지도록 한다.
        scene_->update();
        viewport()->update();
    }

    // --------------------------------------------------------
    // 지도 클릭 시 호출할 함수 등록
    // MainWindow에서 람다 함수를 넘겨준다.
    // --------------------------------------------------------
    void setClickHandler(std::function<void(QPointF)> handler) {
        clickHandler_ = std::move(handler);
    }

protected:
    // --------------------------------------------------------
    // 마우스 클릭 이벤트 처리
    // 클릭 위치를 장면 좌표로 바꾼 뒤 MainWindow에 전달한다.
    // --------------------------------------------------------
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton && clickHandler_) {
            QPointF scenePoint = mapToScene(event->pos());
            clickHandler_(scenePoint);
            return;
        }

        QGraphicsView::mousePressEvent(event);
    }


    // --------------------------------------------------------
    // 창 크기 변경 시 지도가 너무 작아지지 않도록 기본 맞춤 처리
    // --------------------------------------------------------
    void resizeEvent(QResizeEvent* event) override {
        QGraphicsView::resizeEvent(event);
    }

private:
    QGraphicsScene* scene_ = nullptr;
    QGraphicsPixmapItem* pixmapItem_ = nullptr;
    QVector<QGraphicsItem*> overlayItems_;
    QVector<QGraphicsItem*> stationPointItems_;
    std::function<void(QPointF)> clickHandler_;
};

// ============================================================
// MainWindow 클래스
// ------------------------------------------------------------
// 전체 GUI를 구성하는 클래스이다.
// - 왼쪽: 부산 도시철도 노선도
// - 오른쪽: 출발역/도착역/탐색 기준/결과창
// ============================================================
class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setWindowTitle(kor("부산 도시철도 환승 경로 추천 프로그램"));
        resize(1500, 850);
        setupUi();
    }

private:
    MetroGraph graph_;
    MetroMapView* mapView_ = nullptr;
    QComboBox* startCombo_ = nullptr;
    QComboBox* endCombo_ = nullptr;
    QComboBox* modeCombo_ = nullptr;
    QTextEdit* resultText_ = nullptr;
    QRadioButton* startPickRadio_ = nullptr;
    QRadioButton* endPickRadio_ = nullptr;

    // --------------------------------------------------------
    // UI 구성
    // --------------------------------------------------------
    void setupUi() {
        QWidget* central = new QWidget(this);
        setCentralWidget(central);

        QHBoxLayout* rootLayout = new QHBoxLayout(central);

        // -----------------------------
        // 왼쪽: 노선도 이미지 영역
        // -----------------------------
        mapView_ = new MetroMapView(this);
        mapView_->setMinimumWidth(950);

        QPixmap mapPixmap(":/images/busan_metro_map.jpg");
        if (mapPixmap.isNull()) {
            QMessageBox::warning(this,
                                 kor("이미지 오류"),
                                 kor("노선도 이미지를 불러오지 못했습니다. resources.qrc를 확인하세요."));
        } else {
            mapView_->setMapImage(mapPixmap);
                        // 클릭 가능한 역 좌표를 작은 원으로 표시한다.
            // 사용자가 실제 역 원형 위치를 보고 정확히 클릭할 수 있도록 한다.
            mapView_->drawClickablePoints(graph_.stations());
        }

        rootLayout->addWidget(mapView_, 3);

        // -----------------------------
        // 오른쪽: 입력 및 결과 영역
        // -----------------------------
        QVBoxLayout* sideLayout = new QVBoxLayout();
        rootLayout->addLayout(sideLayout, 1);

        QLabel* title = new QLabel(kor("부산 도시철도 환승 경로 추천"));
        QFont titleFont = title->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        title->setFont(titleFont);
        sideLayout->addWidget(title);

        QLabel* guide = new QLabel(kor(
            "지도 위 역을 클릭해서 출발역/도착역을 지정할 수 있습니다.\n"
            "탐색 기준은 최소 환승과 최소 정거장만 제공합니다.\n"
        ));
        guide->setWordWrap(true);
        sideLayout->addWidget(guide);

        sideLayout->addSpacing(10);

        // -----------------------------
        // 지도 클릭 모드 선택
        // -----------------------------
        QGroupBox* pickGroup = new QGroupBox(kor("지도 클릭 지정"));
        QVBoxLayout* pickLayout = new QVBoxLayout(pickGroup);

        startPickRadio_ = new QRadioButton(kor("클릭한 역을 출발역으로 지정"));
        endPickRadio_ = new QRadioButton(kor("클릭한 역을 도착역으로 지정"));
        startPickRadio_->setChecked(true);

        pickLayout->addWidget(startPickRadio_);
        pickLayout->addWidget(endPickRadio_);
        sideLayout->addWidget(pickGroup);

        // -----------------------------
        // 출발역 ComboBox
        // -----------------------------
        sideLayout->addWidget(new QLabel(kor("출발역")));
        startCombo_ = new QComboBox();
        startCombo_->setEditable(true);
        startCombo_->addItems(graph_.stationNames());
        startCombo_->setCurrentText(kor("다대포해수욕장"));
        sideLayout->addWidget(startCombo_);

        QCompleter* startCompleter = new QCompleter(graph_.stationNames(), this);
        startCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        startCombo_->setCompleter(startCompleter);

        // -----------------------------
        // 도착역 ComboBox
        // -----------------------------
        sideLayout->addWidget(new QLabel(kor("도착역")));
        endCombo_ = new QComboBox();
        endCombo_->setEditable(true);
        endCombo_->addItems(graph_.stationNames());
        endCombo_->setCurrentText(kor("노포"));
        sideLayout->addWidget(endCombo_);

        QCompleter* endCompleter = new QCompleter(graph_.stationNames(), this);
        endCompleter->setCaseSensitivity(Qt::CaseInsensitive);
        endCombo_->setCompleter(endCompleter);

        // -----------------------------
        // 탐색 기준 선택
        // -----------------------------
        sideLayout->addWidget(new QLabel(kor("탐색 기준")));
        modeCombo_ = new QComboBox();
        modeCombo_->addItem(kor("최소 환승"));
        modeCombo_->addItem(kor("최소 정거장"));
        sideLayout->addWidget(modeCombo_);

        // -----------------------------
        // 버튼 영역
        // -----------------------------
        QPushButton* searchButton = new QPushButton(kor("경로 찾기"));
        QPushButton* swapButton = new QPushButton(kor("출발/도착 바꾸기"));
        QPushButton* clearButton = new QPushButton(kor("결과 초기화"));

        sideLayout->addWidget(searchButton);
        sideLayout->addWidget(swapButton);
        sideLayout->addWidget(clearButton);

        // -----------------------------
        // 결과 출력창
        // -----------------------------
        resultText_ = new QTextEdit();
        resultText_->setReadOnly(true);
        resultText_->setMinimumWidth(390);
        resultText_->setText(kor(
            "지도 위 빨간 원이 표시된 위치가 실제 클릭 기준점입니다.\n\n"
            "1. 지도 클릭 지정에서 출발역/도착역 모드를 선택하세요.\n"
            "2. 지도 위 역을 클릭하거나 출발역/도착역 칸에서 역을 선택하세요.\n"
            "3. [경로 찾기] 버튼을 누르세요.\n"
        ));
        sideLayout->addWidget(resultText_, 1);

        // -----------------------------
        // 이벤트 연결
        // -----------------------------
        connect(searchButton, &QPushButton::clicked, this, [this]() {
            searchRoute();
        });

        connect(swapButton, &QPushButton::clicked, this, [this]() {
            QString temp = startCombo_->currentText();
            startCombo_->setCurrentText(endCombo_->currentText());
            endCombo_->setCurrentText(temp);
        });

        connect(clearButton, &QPushButton::clicked, this, [this]() {
            mapView_->clearRouteOverlay();
            resultText_->setText(kor("결과가 초기화되었습니다."));
        });

        // 지도 클릭 처리
        mapView_->setClickHandler([this](QPointF imagePoint) {
            handleMapClick(imagePoint);
        });
    }

    // --------------------------------------------------------
    // 현재 선택된 탐색 기준 반환
    // --------------------------------------------------------
    MetroGraph::SearchMode currentMode() const {
        if (modeCombo_->currentIndex() == 0) {
            return MetroGraph::MinTransfer;
        }
        return MetroGraph::MinStation;
    }

    // --------------------------------------------------------
    // 지도 클릭 처리
    // 클릭 좌표에서 가장 가까운 역을 찾아 출발역 또는 도착역으로 지정한다.
    // --------------------------------------------------------
    void handleMapClick(const QPointF& imagePoint) {
                // 클릭 판정 반경을 너무 크게 잡으면 인접 역이 잘못 선택될 수 있으므로
        // 실제 역 원형 근처만 반응하도록 반경을 줄였다.
        int id = graph_.nearestStation(imagePoint, 13.0);

        if (id < 0) {
            resultText_->setText(QString(
                "가까운 역을 찾지 못했습니다.\n"
                "역 원형 표시 주변을 클릭해 주세요.\n\n"
                "클릭 좌표: x=%1, y=%2"
            ).arg(imagePoint.x(), 0, 'f', 1).arg(imagePoint.y(), 0, 'f', 1));
            return;
        }

        QString name = graph_.stationName(id);

        if (startPickRadio_->isChecked()) {
            startCombo_->setCurrentText(name);
            endPickRadio_->setChecked(true);
            resultText_->setText(QString("출발역으로 [%1]을 선택했습니다.\n이제 도착역을 클릭하세요.").arg(name));
        } else {
            endCombo_->setCurrentText(name);
            resultText_->setText(QString("도착역으로 [%1]을 선택했습니다.\n[경로 찾기]를 누르세요.").arg(name));
        }
    }

    // --------------------------------------------------------
    // 경로 탐색 수행
    // --------------------------------------------------------
    void searchRoute() {
        // 새 경로를 탐색하기 전에 지도 위에 남아 있는 이전 경로를 먼저 제거한다.
        mapView_->clearRouteOverlay();

        QString start = startCombo_->currentText().trimmed();
        QString end = endCombo_->currentText().trimmed();

        RouteResult result = graph_.searchRoute(start, end, currentMode());

        if (!result.found) {
            resultText_->setText(kor(
                "경로를 찾을 수 없습니다.\n\n"
                "역 이름이 정확한지 확인하세요."
                ));
            return;
        }

        mapView_->drawRouteOverlay(graph_.stations(), result.stations);
        resultText_->setText(formatResult(start, end, result));
    }

    // --------------------------------------------------------
    // 경로 결과를 화면 출력용 문자열로 변환
    // --------------------------------------------------------
    QString formatResult(const QString& start,
                         const QString& end,
                         const RouteResult& result) const {
        QString text;

        text += QString("출발역: %1\n").arg(start);
        text += QString("도착역: %1\n").arg(end);
        text += QString("탐색 기준: %1\n\n").arg(modeCombo_->currentText());

        text += QString("총 경유 역 수: %1개\n").arg(result.stationCount);
        text += QString("환승 횟수: %1회\n\n").arg(result.transferCount);

        text += kor("[전체 경로]\n");
        for (int i = 0; i < result.stations.size(); ++i) {
            text += graph_.stationName(result.stations[i]);
            if (i + 1 < result.stations.size()) text += " → ";
        }
        text += "\n\n";

        text += kor("[구간별 이용 노선]\n");

        if (result.usedLines.isEmpty()) {
            text += kor("- 같은 역입니다.\n");
            return text;
        }

        int segmentStart = 0;
        QString currentLine = result.usedLines[0];

        for (int i = 1; i <= result.usedLines.size(); ++i) {
            bool isLast = (i == result.usedLines.size());
            bool isChanged = (!isLast && result.usedLines[i] != currentLine);

            if (isLast || isChanged) {
                QString fromStation = graph_.stationName(result.stations[segmentStart]);
                QString toStation = graph_.stationName(result.stations[i]);

                text += QString("- %1: %2 → %3\n")
                            .arg(currentLine)
                            .arg(fromStation)
                            .arg(toStation);

                if (!isLast) {
                    text += QString("  환승역: %1, 다음 노선: %2\n")
                                .arg(toStation)
                                .arg(result.usedLines[i]);

                    currentLine = result.usedLines[i];
                    segmentStart = i;
                }
            }
        }

        text += "\n";
        text += kor("[출발/도착역 노선 정보]\n");
        text += QString("- %1: %2\n")
                    .arg(start)
                    .arg(graph_.stationLines(graph_.stationId(start)));
        text += QString("- %1: %2\n")
                    .arg(end)
                    .arg(graph_.stationLines(graph_.stationId(end)));

        return text;
    }
};

// ============================================================
// main 함수
// Qt 애플리케이션의 시작점
// ============================================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
