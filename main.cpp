#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int    map  [100][100];
bool visited[100][100];
const int I = 0;
const int J = 1;
const int D = 2;

int nextPosition[8][3] = {
    {-1, -1, 14},
    {-1,  1, 14},
    { 1, -1, 14},
    { 1,  1, 14},
    {-1,  0, 10},
    { 1,  0, 10},
    { 0, -1, 10},
    { 0,  1, 10}
};

class Cell {
    public :
        int i, j;
        int move;
        int need;
        int parent_i, parent_j;

        Cell(int i=0, int j=0) : i(i), j(j) {
        }

        void getNeedManhattan( int goal_i, int goal_j ) {
            need = abs(goal_i - i) + abs(goal_j - j);
            need *= 10;
        }

        friend bool operator < ( const Cell &c1, const Cell &c2 ) {
            return c1.move + c1.need > c2.move + c2.need;
        }
};

int main() {
    
    // get my position
    int my_i = 10;
    int my_j = 82;

    // get goal position
    int goal_i = 77;
    int goal_j = 15;

    const int ARRIVED_DISTANCE = 15;

    priority_queue< Cell >  openPQ;
    vector< Cell > closeVector;
    
    Cell tempCell(my_i, my_j);
    tempCell.parent_i = my_i; tempCell.parent_j = my_j;
    tempCell.getNeedManhattan( goal_i, goal_j );
    tempCell.move = 0;
    closeVector.push_back( tempCell );

    for(int i=0; i<8; i++) {
        Cell direct8( tempCell.i + nextPosition[i][I], tempCell.j + nextPosition[i][J] );
        direct8.parent_i = tempCell.i; direct8.parent_j = tempCell.j;
        direct8.getNeedManhattan( goal_i, goal_j );
        direct8.move = tempCell.move + nextPosition[i][D];
        openPQ.push( direct8 );
    }

    while( !openPQ.empty() ) {
        Cell c = openPQ.top(); openPQ.pop();
        if( c.i < 0 || c.j < 0 || c.i >= 100 || c.j >= 100 ) continue;
        if( visited[ c.i ][ c.j ] ) continue;
        visited[ c.i ][ c.j ] = true;
        closeVector.push_back(c);
        if( c.need < ARRIVED_DISTANCE ) {
            break;
        }

        for(int i=0; i<8; i++) {
            Cell direct8( c.i + nextPosition[i][I], c.j + nextPosition[i][J] );
            direct8.parent_i = c.i; direct8.parent_j = c.j;
            direct8.move = c.move + nextPosition[i][D];
            direct8.getNeedManhattan( goal_i, goal_j );
            openPQ.push( direct8 );
        }
    }

    vector<Cell> path;
    for(vector<Cell>::reverse_iterator rit = closeVector.rbegin(); rit != closeVector.rend(); rit++) {
        Cell c = *rit;
        if( path.empty() ) {
            path.push_back( c );
        } else {
            if( path.back().parent_i == c.i && path.back().parent_j == c.j ) {
                path.push_back( c );
            }
        }
    }

    cout << "from : " <<  my_i  << ", " <<  my_j  << endl;
    cout << " to  : " << goal_i << ", " << goal_j << endl;
    for(auto &p : path)  {
        cout << p.i << " " << p.j << endl; 
    }
}
