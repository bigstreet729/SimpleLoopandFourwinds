#include<vector>
#include<iostream>
#include<string>
#include<utility>
#include<variant>
#include<unordered_map>
#include<set>
using namespace std;
vector<vector<int>> grid = {
    { 0, 0, 1, 0, 0, -1, 0, 1, 0 },
    { 1, 0, 0, 0, 6, 0, 0, 0, 0 },
    { 0, 4, 0, 0, 0, 0, 0, 2, 2 },
    { 0, 0, 2, 0, 0, 5, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 5, 0 },
    { 0, 0, 0, 4, 0, 0, 1, 0, 0 },
    { 4, 4, 0, 0, 0, 0, 0, 2, 0 },
    { 0, 0, 0, 0, 5, 0, 0, 0, 5 },
    { 0, -1, 0, 3, 0, 0, 1, 0, 1 },
};


int DIRS[4][2] = { 
    {0, -1},   // LEFT
    {-1, 0},   // UP
    {0, 1},    // RIGHT
    {1, 0}     // DOWN
};

struct Vertex{
    int r,c, val;
    int maxlen[4];
    int minlen[4];
    bool is_occupied;
    Vertex* owner;
    vector<Vertex*>neighbours;
    int capacity;
    Vertex(){
        r=c=val=0;
        is_occupied=false;
        owner = nullptr;
        neighbours={};
        capacity=0;
    }
    Vertex(int row, int col, int v){
        r=row; c=col;
        val=v;
        is_occupied=false;
        owner=nullptr;
        capacity=0;
    }
    Vertex(int row, int col, int v, Vertex*o){
        r=row; c=col; val=v; owner=o;
        is_occupied=true; capacity=val;
    }
    void add_neighbor(Vertex* v){
        if(neighbours.size()>=val){
            cout<<"U CANT ADD MORE VETICES\n";
            return;
        }
        capacity--;
        neighbours.push_back(v);
    }
    void set_owner(Vertex *v){
        owner=v;
        this->val=v->val;
    }
    
    bool operator==(const Vertex&other) const{
        return r==other.r && c==other.c;
    }  
    bool operator<(const Vertex& other) const {
        if (r != other.r) return r < other.r;
        return c < other.c;
    } 
};
vector<Vertex*> prime_cells;
vector<vector<Vertex>>vertex_grid;
void makeVerticesGrid(){
    int R=grid.size();
    int C=grid[0].size();
    vertex_grid.clear();
    vertex_grid.resize(R);
    for(int r=0;r<R;r++){
        vertex_grid[r].resize(C);
    }
    for(int r=0;r<R;r++){
        for(int c=0;c<C;c++){
            int v = grid[r][c]; 
            if(v!=0){
                vertex_grid[r][c]=Vertex(r,c,v,&vertex_grid[r][c]);
                prime_cells.push_back(&vertex_grid[r][c]);
            }
            else{
                vertex_grid[r][c]=Vertex(r,c,0);
            }
        }
    }
}
void fillThePath(Vertex* P, int r, int c){
    int dr = 0, dc = 0;
    if (P->r == r) 
        dc = (c > P->c ? 1 : -1);
    else 
        dr = (r > P->r ? 1 : -1);
    int nr = P->r + dr;
    int nc = P->c + dc;
    while (nr != r || nc != c) {
        Vertex &cell = vertex_grid[nr][nc];
        if (!cell.is_occupied) {
            cell.set_owner(P);
            P->add_neighbor(&cell); 
        }
        nr += dr;
        nc += dc;
    }

    Vertex &last = vertex_grid[r][c];
    if (!last.is_occupied) {
        last.set_owner(P);
        P->add_neighbor(&last);
    }
}

vector<Vertex*>reachableVertices(int r, int c){
    vector<Vertex*>result;
    for(Vertex* P : prime_cells){
        if(P->r!=r && P->c!=c) continue;
        int dr=0;int dc=0;
        if(P->r==r) dc = (c>P->c ? 1 :-1);
        else        dr = (r>P->r ? 1 :-1);
        int cap_used=0;
        int capacity=P->capacity;
        int nr=P->r+dr;
        int nc=P->c+dc;
        bool ok =true;

        while(true){
            if(nr<0 || nr>=grid.size() || nc<0 || nc>=grid[0].size()){
                ok=false;
                break;
            }
            Vertex& cell = vertex_grid[nr][nc];
            if(cell.val==-1){
                ok=false;
                break;
            }
            if(cell.val>0 && !(cell.r==P->r && cell.c ==P->c)){
                ok=false;
                break;
            }
            if(cell.is_occupied && cell.owner!=P){
                ok=false;
                break;
            }
            if(!cell.is_occupied && cell.val==0){
                cap_used++;
                if(cap_used>capacity){
                    ok=false;
                    break;
                }
            }
            if(nr==r && nc==c){
                break;
            }
            nr +=dr;
            nc +=dc;
        }
        if(ok){
            result.push_back(P);
        }
    }
    return result;
}

bool propogateOnce(){
    bool changed=false;
    int R = grid.size();
    int C =grid[0].size();

    for(int r=0;r<R;r++){
        for(int c=0;c<C;c++){
            if(vertex_grid[r][c].is_occupied) continue;
            auto reach = reachableVertices(r,c);
            cout<<"for ("<<r<<" ,"<<c<<") reach elements "<<reach.size()<<" \n";
            if(reach.size()==1){
                cout<<"FILLING "<<"( "<<r<<", "<<c<<" ) "<<"from ("<<reach[0]->r<<" , "<<reach[0]->c<<")\n";
                fillThePath(reach[0],r,c);
            }
           
        }
    }
    return changed;
}

void propogateAll(){
    cout<<"Propogating Start\n";
        while(propogateOnce());
    return;
}
void print_grid(){
    int R=grid.size();
    int C=grid[0].size();
    for(int r=0;r<R;r++){
        for(int c=0;c<C;c++){
            cout<<vertex_grid[r][c].val<<" ";
        }cout<<"\n";

    }
}
int main(){
        
        makeVerticesGrid();
        propogateAll();
        print_grid();

        return 0;

}