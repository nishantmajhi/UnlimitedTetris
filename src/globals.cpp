/*
 * ================================================================
 *  UnlimitedTetris — src/globals.cpp
 *  Tetromino data tables, game state definitions, core logic.
 * ================================================================
 */

#include "../include/globals.h"

// ── Tetromino shape table ─────────────────────────────────────
const int SHAPES[PIECE_COUNT][ROT_COUNT][4][2] = {
    // I
    {{{0,1},{1,1},{2,1},{3,1}},{{2,0},{2,1},{2,2},{2,3}},
     {{0,2},{1,2},{2,2},{3,2}},{{1,0},{1,1},{1,2},{1,3}}},
    // O
    {{{1,0},{2,0},{1,1},{2,1}},{{1,0},{2,0},{1,1},{2,1}},
     {{1,0},{2,0},{1,1},{2,1}},{{1,0},{2,0},{1,1},{2,1}}},
    // T
    {{{1,0},{0,1},{1,1},{2,1}},{{1,0},{1,1},{2,1},{1,2}},
     {{0,1},{1,1},{2,1},{1,2}},{{1,0},{0,1},{1,1},{1,2}}},
    // S
    {{{1,0},{2,0},{0,1},{1,1}},{{1,0},{1,1},{2,1},{2,2}},
     {{1,1},{2,1},{0,2},{1,2}},{{0,0},{0,1},{1,1},{1,2}}},
    // Z
    {{{0,0},{1,0},{1,1},{2,1}},{{2,0},{1,1},{2,1},{1,2}},
     {{0,1},{1,1},{1,2},{2,2}},{{1,0},{0,1},{1,1},{0,2}}},
    // J
    {{{0,0},{0,1},{1,1},{2,1}},{{1,0},{2,0},{1,1},{1,2}},
     {{0,1},{1,1},{2,1},{2,2}},{{1,0},{1,1},{0,2},{1,2}}},
    // L
    {{{2,0},{0,1},{1,1},{2,1}},{{1,0},{1,1},{1,2},{2,2}},
     {{0,1},{1,1},{2,1},{0,2}},{{0,0},{1,0},{1,1},{1,2}}}
};

// ── Colour table (R, G, B) ────────────────────────────────────
const float COLORS[PIECE_COUNT][3] = {
    {0.0f,0.9f,0.9f}, // I cyan
    {0.9f,0.9f,0.0f}, // O yellow
    {0.7f,0.0f,0.9f}, // T purple
    {0.0f,0.8f,0.0f}, // S green
    {0.9f,0.0f,0.0f}, // Z red
    {0.0f,0.3f,0.9f}, // J blue
    {0.9f,0.5f,0.0f}  // L orange
};

// ── Game state definitions ────────────────────────────────────
GameState gState     = STATE_MENU;
int  menuItem        = 0;
int  board[ROWS][COLS];
int  boardColor[ROWS][COLS];
int  curType=0, curRot=0, curX=0, curY=0;
int  nextType=0;
int  score=0, lines=0, level=1, highScore=0;
int  dropInterval=500;
float goAngle=0.0f;

// ── Collision test (Unit 3.4 — boundary clipping) ─────────────
bool collision(int tp, int rt, int x, int y)
{
    for(int i=0;i<4;i++){
        int c=x+SHAPES[tp][rt][i][0], r=y+SHAPES[tp][rt][i][1];
        if(c<0||c>=COLS||r<0||r>=ROWS) return true;
        if(board[r][c]) return true;
    }
    return false;
}

void lockPiece()
{
    for(int i=0;i<4;i++){
        int c=curX+SHAPES[curType][curRot][i][0];
        int r=curY+SHAPES[curType][curRot][i][1];
        if(r>=0&&r<ROWS&&c>=0&&c<COLS){board[r][c]=1;boardColor[r][c]=curType;}
    }
}

int clearLines()
{
    int cleared=0;
    for(int r=ROWS-1;r>=0;){
        bool full=true;
        for(int c=0;c<COLS;c++) if(!board[r][c]){full=false;break;}
        if(full){
            for(int rr=r;rr>0;rr--){
                memcpy(board[rr],board[rr-1],sizeof(board[0]));
                memcpy(boardColor[rr],boardColor[rr-1],sizeof(boardColor[0]));
            }
            memset(board[0],0,sizeof(board[0]));
            memset(boardColor[0],0,sizeof(boardColor[0]));
            cleared++;
        } else r--;
    }
    return cleared;
}

int ghostY()
{
    int gy=curY;
    while(!collision(curType,curRot,curX,gy+1)) gy++;
    return gy;
}

void updateLevel()
{
    level=lines/10+1;
    dropInterval=500-(level-1)*40;
    if(dropInterval<60) dropInterval=60;
}

// ── Scoring ───────────────────────────────────────────────────
//  The running best is updated here, as the score changes, and
//  nowhere else. It used to be updated only on a line clear and
//  at the start of the *next* game, which meant a game won on
//  drop points alone ended on a screen reading "Score: 412" and
//  "Best:  0" at the same time.
void addScore(int points)
{
    score += points;
    if(score>highScore) highScore=score;
}

//  The one copy of the line-clear rule. It was previously written
//  out twice, in the gravity tick and in the hard drop, with the
//  high-score comparison appearing twice more elsewhere.
void awardLines(int cleared)
{
    if(cleared<=0) return;
    addScore(100*level*cleared*cleared);   // scored at the level it was cleared on
    lines+=cleared;
    updateLevel();
}

// ── Piece lifecycle ───────────────────────────────────────────
static void lockScoreAndSpawn()
{
    lockPiece();
    awardLines(clearLines());
    spawnPiece();
}

void spawnPiece()
{
    curType=nextType; curRot=0; curX=COLS/2-2; curY=0;
    nextType=rand()%PIECE_COUNT;
    if(collision(curType,curRot,curX,curY)) endGame();
}

void endGame()
{
    gState=STATE_OVER;
    menuItem=0;              // the Game Over menu always opens on its first item
    persistHighScore();
}

void startGame()
{
    memset(board,0,sizeof(board));
    memset(boardColor,0,sizeof(boardColor));
    persistHighScore();
    score=lines=0; level=1; dropInterval=500;
    goAngle=0; menuItem=0; gState=STATE_PLAY;
    nextType=rand()%PIECE_COUNT;
    spawnPiece();
}

void goMainMenu()
{
    persistHighScore();
    score=lines=0; level=1; dropInterval=500; menuItem=0;
    gState=STATE_MENU;
}

// ── Player and timer actions ──────────────────────────────────
void stepGravity()
{
    if(gState!=STATE_PLAY) return;
    if(!collision(curType,curRot,curX,curY+1)) curY++;
    else lockScoreAndSpawn();
}

void softDrop()
{
    if(gState!=STATE_PLAY) return;
    if(!collision(curType,curRot,curX,curY+1)){ curY++; addScore(1); }
}

void hardDrop()
{
    if(gState!=STATE_PLAY) return;
    while(!collision(curType,curRot,curX,curY+1)){ curY++; addScore(2); }
    lockScoreAndSpawn();
}

void moveSideways(int dx)
{
    if(gState!=STATE_PLAY) return;
    if(!collision(curType,curRot,curX+dx,curY)) curX+=dx;
}

// Rotate clockwise, kicking one cell right then one cell left if
// the rotation would not otherwise fit. Refuses if neither works.
void rotateCW()
{
    if(gState!=STATE_PLAY) return;
    int nr=(curRot+1)%ROT_COUNT;
    if     (!collision(curType,nr,curX,  curY))  curRot=nr;
    else if(!collision(curType,nr,curX+1,curY)){ curRot=nr; curX++; }
    else if(!collision(curType,nr,curX-1,curY)){ curRot=nr; curX--; }
}

void pauseGame()
{
    if(gState!=STATE_PLAY) return;
    gState=STATE_PAUSE; menuItem=0;
}

// ── Menu state machine ────────────────────────────────────────
int menuCount()
{
    if(gState==STATE_MENU)  return 2;   // New Game, Exit
    if(gState==STATE_PAUSE) return 3;   // Continue, New Game, Exit
    if(gState==STATE_OVER)  return 3;   // Play Again, Main Menu, Exit
    return 0;
}

void menuMove(int delta)
{
    int cnt=menuCount();
    if(cnt<=0) return;
    menuItem=((menuItem+delta)%cnt+cnt)%cnt;   // wraps in both directions
}

MenuAction menuActivate()
{
    if(gState==STATE_MENU){
        if(menuItem==0){ startGame(); return MENU_ACTION_HANDLED; }
        persistHighScore(); return MENU_ACTION_QUIT;
    }
    if(gState==STATE_PAUSE){
        if(menuItem==0){ gState=STATE_PLAY; menuItem=0; return MENU_ACTION_HANDLED; }
        if(menuItem==1){ startGame();       return MENU_ACTION_HANDLED; }
        persistHighScore(); return MENU_ACTION_QUIT;
    }
    if(gState==STATE_OVER){
        if(menuItem==0){ startGame();  return MENU_ACTION_HANDLED; }
        if(menuItem==1){ goMainMenu(); return MENU_ACTION_HANDLED; }
        persistHighScore(); return MENU_ACTION_QUIT;
    }
    return MENU_ACTION_NONE;
}

// ── High-score persistence hook ───────────────────────────────
void (*onHighScorePersist)(int) = nullptr;

void persistHighScore()
{
    if(onHighScorePersist) onHighScorePersist(highScore);
}
