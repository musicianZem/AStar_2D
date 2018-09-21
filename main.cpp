#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <SDL2/SDL.h>
#include <algorithm>
using namespace std;

int obstacle[160][90];
int fromI = 0, fromJ = 0, goalI = 15, goalJ = 8;
bool hasPath = true;
const int I = 0;
const int J = 1;
const int D = 2;
int nextPosition[8][3] = {
    {-1, -1, 3}, {-1,  1, 3}, { 1, -1, 3}, { 1,  1, 3},
    {-1,  0, 2}, { 1,  0, 2}, { 0, -1, 2}, { 0,  1, 2}
};

bool isOutofBound(int i, int j) {
    return i < 0 || i >= 160 || j < 0 || j >= 90;
}

class Cell {
    public :
        int i, j;
        int FScore, GScore, HScore;
        // F : total
        // G : 이 Cell까지 걸린 비용
        // H : 예상 비용
        int pi, pj;

        Cell(int i = 0, int j = 0) : i(i), j(j) {}

        void calHScore() {
            HScore = 2 * (abs(goalI - i) + abs(goalJ - j));
        }

        void calFScore() {
            FScore = HScore + GScore;
        }
};

bool compCell( const Cell& c1, const Cell& c2 ) {
    return c1.FScore > c2.FScore;
}

SDL_Window *window;
SDL_Renderer *renderer;
vector<pair<int, int>> path;

void initSDL();
bool pollEvent();
void render();
void astar();

int main() {
    initSDL();

    while( !pollEvent() ) {
        SDL_RenderClear( renderer );

        astar();
        render();

        SDL_RenderPresent( renderer );
    }
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );
    SDL_Quit();
}

void astar() {
    path.clear();
    vector<Cell> O, C;
    Cell firstCell( fromI, fromJ );
    firstCell.GScore = 0;
    firstCell.FScore = firstCell.HScore;
    firstCell.calHScore();
    firstCell.pi = fromI;
    firstCell.pj = fromJ;
    O.push_back( firstCell );
    obstacle[1][3] = 1;
    obstacle[2][3] = 1;
    obstacle[3][3] = 1;
    obstacle[3][2] = 1;
    obstacle[3][1] = 1;

    while( !O.empty() ) {
        sort( O.begin(), O.end(), [](const Cell& o1, const Cell& o2){ return o1.FScore < o2.FScore; });
        Cell beforeCell = O[0];
        C.push_back( beforeCell );
        O.erase( O.begin() );

        if( beforeCell.HScore < 1 ) break;

        Cell nextCell;
        for(int pos=0; pos<8; pos++) {
            int i = beforeCell.i + nextPosition[pos][0];
            int j = beforeCell.j + nextPosition[pos][1];

            if( !isOutofBound(i, j) ) {
                if( obstacle[i][j] != 0 ) continue;
                bool isExist = false;
                for(auto &c : C) {
                    if( c.i == i && c.j == j ) {
                        isExist = true;
                        break;
                    }
                }
                if( isExist ) continue;
                nextCell.i = i; nextCell.j = j;
                nextCell.GScore = beforeCell.GScore + nextPosition[pos][2];
                nextCell.calHScore();
                nextCell.calFScore();
                nextCell.pi = beforeCell.i;
                nextCell.pj = beforeCell.j;

                for(auto &o : O) {
                    if( o.i == i && o.j == j ) {
                        isExist = true;
                        if( o.FScore > nextCell.FScore ) {
                            o = nextCell;
                        }
                        break;
                    }
                }
                if( !isExist ) {
                    O.push_back( nextCell );
                }
            }
        }
    }

    int traceI = C.back().pi, traceJ = C.back().pj;
    while( traceI != fromI || traceJ != fromJ ) {
        obstacle[ traceI ][ traceJ ] = -1;
        for(vector<Cell>::iterator it = C.begin(); it != C.end(); ) {
            if( it->i == traceI && it->j == traceJ ) {
                traceI = it->pi;
                traceJ = it->pj;
                C.erase( it );
                break;
            } else {
                it++;
            }
        }
    }
    cout << traceI << " " << fromI << " " << traceJ << " " << fromJ << endl;
    if( traceI == fromI && traceJ == fromJ ) {
        hasPath = true;
    } else {
        hasPath = false;
    }
}

void initSDL() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("ASTAR", 0, 0, 1600, 900, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, 0);
}

bool pollEvent() {
    SDL_Event e;
    bool quit = false;
    static bool setStart = true;
    static int mouse_sx, mouse_sy, mouse_ex, mouse_ey;
    while( SDL_PollEvent( &e ) != 0 ) {
        switch( e.type ) {
            case SDL_QUIT :
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN :
                SDL_GetMouseState( &mouse_sx, &mouse_sy );
                if( e.button.button == SDL_BUTTON_LEFT ) {
                }
                if( e.button.button == SDL_BUTTON_RIGHT ) {
                    if( setStart ) {
                        fromI = mouse_sx / 12;
                        fromJ = mouse_sy / 12;
                    } else {
                        goalI = mouse_sx / 12;
                        goalJ = mouse_sy / 12;
                    }
                    setStart = setStart == false;
                }
                break;
            case SDL_MOUSEBUTTONUP :
                SDL_GetMouseState( &mouse_ex, &mouse_ey );
                if( e.button.button == SDL_BUTTON_LEFT ) {
                    if( mouse_sx > mouse_ex ) swap( mouse_sx, mouse_ex );
                    if( mouse_sy > mouse_ey ) swap( mouse_sy, mouse_ey );
                    int si = mouse_sx/12, sj = mouse_sy/12;
                    int ei = mouse_ex/12, ej = mouse_ey/12;
                    for(int i=si; i<=ei; i++) {
                        for(int j=sj; j<=ej; j++) {
                            obstacle[ i ][ j ] = 1;
                        }
                    }
                }
                if( e.button.button == SDL_BUTTON_RIGHT ) {
                }
                break;
        }
    }
    return quit;
}

void render() {
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
    SDL_Rect rect;
    rect.w = 12; rect.h = 12;
    for(int i=0; i<160; i++) {
        for(int j=0; j<90; j++) {
            rect.x = i*12;
            rect.y = j*12;
            if( i == goalI && j == goalJ ) {
                SDL_SetRenderDrawColor( renderer, 0, 0, 255, 0 );
                SDL_RenderFillRect( renderer, &rect );
                continue;
            }
            if( i == fromI && j == fromJ ) {
                SDL_SetRenderDrawColor( renderer, 0, 255, 0, 0 );
                SDL_RenderFillRect( renderer, &rect );
                continue;
            }
            if( obstacle[i][j] == 1 ) {
                SDL_SetRenderDrawColor( renderer, 100, 100, 100, 100 );
                SDL_RenderFillRect( renderer, &rect );
                continue;
            }
            if( obstacle[i][j] == -1  && hasPath ) {
                printf("%3d, %3d -> %3d, %3d path of one, [%3d][%3d]\n",fromI, fromJ, goalI, goalJ, i, j);
                SDL_SetRenderDrawColor( renderer, 255, 0, 0, 0 );
                SDL_RenderFillRect( renderer, &rect );
                obstacle[i][j] = 0;
                continue;
            }
        }
    }
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 0 );
}
