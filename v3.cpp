#include <bits/stdc++.h>
using namespace std;

struct SudokuCell {
    int val;
    int x, y;
    vector<SudokuCell*> neighbors;
};
class SudokuGrid {
private:
    int N;
    vector<vector<SudokuCell>> grid;
public:
    SudokuGrid(int n) : N(n), grid(n, vector<SudokuCell>(n)) {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                grid[i][j].x = i;
                grid[i][j].y = j;
                grid[i][j].val = 0;
            }
    }
    void setValue(int i, int j, int v) { grid[i][j].val = v; }

    void printEdgesStep() {
        vector<string> canvas(2*N+1, string(2*N+1, ' '));
        for(int i=0;i<N;i++)
            for(int j=0;j<N;j++)
                canvas[2*i+1][2*j+1] = 'O';

        for(int i=0;i<N;i++){
            for(int j=0;j<N;j++){
                for(auto nb : grid[i][j].neighbors){
                    int x1 = 2*i+1, y1 = 2*j+1;
                    int x2 = 2*nb->x+1, y2 = 2*nb->y+1;

                    if (x1 == x2) {
                        canvas[x1][min(y1,y2)+1] = '-';
                    }
                    else if (y1 == y2) {
                        canvas[min(x1,x2)+1][y1] = '|';
                    }
                }
            }
        }
        for (auto &row : canvas) cout << row << "\n";
        cout << "-------------------------------------\n";
    }

    bool pathExistsDFS(SudokuCell* cur, SudokuCell* target, SudokuCell* parent) {
        // target  and parent returns if there is a path this is DFS search 
        if (cur == target) return true;
        for (auto nb : cur->neighbors) {
            if (nb == parent) continue;
            if (pathExistsDFS(nb, target, cur)) return true;
        }
        return false;
    }
    bool isFullZeroCycle() {
        // check any full cycle has formed 
        vector<SudokuCell*> zeros;

        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (grid[i][j].val == 0)
                    zeros.push_back(&grid[i][j]);

        if (zeros.empty()) return false;

        for (auto z : zeros)
            if (z->neighbors.size() != 2)
                return false;

        unordered_set<SudokuCell*> vis;
        queue<SudokuCell*> q;
        q.push(zeros[0]);
        vis.insert(zeros[0]);

        while (!q.empty()) {
            auto cur = q.front(); q.pop();
            for (auto nb : cur->neighbors) {
                if (vis.count(nb) == 0) {
                    vis.insert(nb);
                    q.push(nb);
                }
            }
        }
        return vis.size() == zeros.size();
    }
    bool formsCycle(SudokuCell* a, SudokuCell* b) {

        //NEEDS IMPROVEMENT very inefficient 
        cout << " Checking cycle: (" << a->x << "," << a->y << ") <-> (" << b->x << "," << b->y << ")\n";
        bool cycle = pathExistsDFS(a, b, nullptr);
        if (!cycle) {
            cout << "Safe\n";
            return false;
        }
        cout << "Cycle detected  checking for complette cycle\n";
        a->neighbors.push_back(b);
        b->neighbors.push_back(a);
        bool fullLoop = isFullZeroCycle();
        a->neighbors.pop_back();
        b->neighbors.pop_back();

        if (fullLoop) {
            cout << "Allowed: full 0 cycle\n";
            return false;
        }
        cout << "Premature cycle\n";
        return true;
    }
    bool addBidirectionalNeighbor(SudokuCell &a, SudokuCell &b) {
        // addding neighbours to each other
        if (a.neighbors.size() >= 2) return false;
        if (b.neighbors.size() >= 2) return false;
        for (auto *ptr : a.neighbors)
            if (ptr == &b) return false;
        if (formsCycle(&a,&b)) return false;

        a.neighbors.push_back(&b);
        b.neighbors.push_back(&a);

        cout << "Added edge: (" << a.x << "," << a.y << ") to (" << b.x << "," << b.y << ")\n";
        printEdgesStep();
        return true;
    }
    vector<pair<int,int>> getCandidates(int i, int j) {
        vector<pair<int,int>> c;

        //CAN BE IMPROVED

        auto tryPush = [&](int x, int y) {
            if (x < 0 || x >= N || y < 0 || y >= N) return;
            SudokuCell &nb = grid[x][y];
            if (nb.val == 1) return;
            if (nb.neighbors.size() >= 2) return;
            c.push_back({x,y});
        };

        tryPush(i-1, j);
        tryPush(i+1, j);
        tryPush(i, j-1);
        tryPush(i, j+1);
        return c;
    }
    void buildNeighborsUntilStable() {
        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    SudokuCell &cell = grid[i][j];
                    if (cell.val == 1) continue;
                    if (cell.neighbors.size()==2) continue;
                    vector<pair<int,int>> cand = getCandidates(i,j);
                    if (cell.neighbors.size()==1) {
                        vector<pair<int,int>> safe;
                        for (auto [x,y] : cand)
                            if (!formsCycle(&cell,&grid[x][y]))
                                safe.push_back({x,y});
                        if (safe.size() == 1) {
                            auto [x,y] = safe[0];
                            cout << "Forcing: single safe candidate for (" << i << "," << j << ")\n";
                            if (addBidirectionalNeighbor(cell, grid[x][y]))
                                changed = true;
                        }
                        continue;
                    }
                    if (cell.neighbors.size() == 0 && cand.size() == 2) {
                        auto [x1,y1] = cand[0];
                        auto [x2,y2] = cand[1];
                        bool c1 = false, c2 = false;
                        if (!formsCycle(&cell,&grid[x1][y1]))
                            c1 = addBidirectionalNeighbor(cell, grid[x1][y1]);
                        if (!formsCycle(&cell,&grid[x2][y2]))
                            c2 = addBidirectionalNeighbor(cell, grid[x2][y2]);
                        if (c1 || c2) changed = true;
                    }
                }
            }
        }
    }
    void printNeighbors() {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                cout << "Cell (" << i << "," << j << ") -> ";
                for (auto *nb : grid[i][j].neighbors){
                    cout << "(" << nb->x << "," << nb->y << ") ";
                }
                cout << "\n";
            }
    }
};
int main() {
    int n = 14;
    SudokuGrid g(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            int v; 
            cin >> v;
            g.setValue(i, j, v);
        }
    g.buildNeighborsUntilStable();
    g.printNeighbors();

    return 0;
}
