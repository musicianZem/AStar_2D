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
        // F : total
        // G : 이 Cell까지 걸린 비용
        // H : 예상 비용
        mutable int FScore, GScore, HScore;
        mutable int pi, pj;

        Cell(int i = 0, int j = 0) : i(i), j(j) {} 
        int getH() const {
            return 1 * (abs(goalI - i) + abs(goalJ - j));
        } 
        int getF() const {
            return getH() + GScore;
        }
        bool operator<(const Cell& h) const {
            return !(getF() < h.getF());
        }
}; 

struct SetComparer {
    bool operator() (const Cell& lhs, const Cell& rhs) const {
        if(lhs.i == rhs.i) return lhs.j < rhs.j; 
        return lhs.i < rhs.i;
    }
};

SDL_Window *window;
SDL_Renderer *renderer;

void initSDL();
bool pollEvent();
void render();
void astar();
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
    Cell firstCell( fromI, fromJ );
    firstCell.GScore = 0;
    firstCell.pi = fromI;
    firstCell.pj = fromJ;
    std::set<Cell, SetComparer> C;
    C.insert( firstCell );

    std::priority_queue<Cell> queueO;
    std::set<Cell, SetComparer> setO; 
    for(int pos=4; pos<8; pos++) {
        int i = firstCell.i + nextPosition[pos][0];
        int j = firstCell.j + nextPosition[pos][1];

        if( !isOutofBound(i, j) ) {
            if( obstacle[i][j] == 1) continue;

            Cell nextCell;
            nextCell.i = i; nextCell.j = j;
            nextCell.GScore = firstCell.GScore + nextPosition[pos][2];
            nextCell.pi = firstCell.i;
            nextCell.pj = firstCell.j; 
            setO.insert(nextCell); 
            queueO.push(nextCell);
        }
    }

    while(!queueO.empty()) {
        Cell beforeCell = queueO.top();
        queueO.pop();

        C.insert(beforeCell);
        if(beforeCell.i == goalI && beforeCell.j == goalJ) {
            break;
        }

        for(int pos=4; pos<8; pos++) {
            int i = beforeCell.i + nextPosition[pos][0];
            int j = beforeCell.j + nextPosition[pos][1];

            if( !isOutofBound(i, j) ) {
                if( obstacle[i][j] == 1 ) continue;

                Cell nextCell;
                nextCell.i = i; nextCell.j = j;
                nextCell.GScore = beforeCell.GScore + nextPosition[pos][2];
                nextCell.pi = beforeCell.i;
                nextCell.pj = beforeCell.j; 
                if(C.find(nextCell) == C.end()) {
                    auto iter = setO.find(nextCell);
                    if(iter != setO.end()) {
                        if(iter->getF() > nextCell.getF()) {
                            iter->pi = beforeCell.i;
                            iter->pi = beforeCell.j;
                            iter->GScore = beforeCell.GScore + nextPosition[pos][2]; 
                            //*iter = nextCell; 
                        }
                    }
                    else {
                        setO.insert(nextCell); 
                        queueO.push(nextCell); 
                    }
                }
            }
        } 
    }

    auto iter = C.find(Cell(goalI, goalJ));
    if(iter != C.end()) {
        int traceI = goalI;
        int traceJ = goalJ;
        while( traceI != fromI || traceJ != fromJ ) {
            auto toFindParent = C.find(Cell(traceI, traceJ));
            traceI = toFindParent->pi;
            traceJ = toFindParent->pj;
            obstacle[ traceI ][ traceJ ] = -1;
        }
    } 
    obstacle[ fromI ][ fromJ ] = 0;
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
            case SDL_KEYDOWN :
                memset(obstacle, 0, sizeof(obstacle));
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
            if( obstacle[i][j] == 1 ) {
                SDL_SetRenderDrawColor( renderer, 100, 100, 100, 100 );
                SDL_RenderFillRect( renderer, &rect );
                //continue;
            }
            if( i == goalI && j == goalJ ) {
                SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
                SDL_RenderFillRect( renderer, &rect );
                //continue;
            }
            if( i == fromI && j == fromJ ) {
                SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
                SDL_RenderFillRect( renderer, &rect );
                //continue;
            }
            if( obstacle[i][j] == -1) {
                SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );
                SDL_RenderFillRect( renderer, &rect );
                obstacle[i][j] = 0;
                //continue;
            }
        }
    }
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 0 );
}
