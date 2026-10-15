/*
 * ================================================================
 *  UnlimitedTetris — src/unit10_opengl.cpp
 *
 *  Unit 10: Introduction to OpenGL
 *  ─────────────────────────────────
 *  10.1  display(), keyboard(), specialKeys(), timerDrop()
 *        Color commands, drawing primitives, double-buffering,
 *        blend, bitmap text, viewing.
 *  Also contains all screen renderers and the menu system.
 * ================================================================
 */

#include "../include/unit10_opengl.h"
#include "../include/unit2_scan.h"
#include "../include/unit3_transform.h"
#include "../include/unit4_3d.h"
#include "../include/unit5_curves.h"
#include "../include/unit8_lighting.h"

// ── Menu box ──────────────────────────────────────────────────
void drawMenuBox(float cx, float cy, float bw, float bh,
                 const char** items, int cnt, int sel,
                 const char* title)
{
    float x0=cx-bw*.5f, y0=cy-bh*.5f;
    float x1=cx+bw*.5f, y1=cy+bh*.5f;

    quad(x0+5,y0-5,x1+5,y1-5, 0,0,0,.50f);              // shadow
    quad(x0,y0,x1,y1, .07f,.07f,.13f,.97f);              // background
    bline((int)x0,(int)y0,(int)x1,(int)y0,.28f,.52f,.9f); // border
    bline((int)x1,(int)y0,(int)x1,(int)y1,.28f,.52f,.9f);
    bline((int)x1,(int)y1,(int)x0,(int)y1,.28f,.52f,.9f);
    bline((int)x0,(int)y1,(int)x0,(int)y0,.28f,.52f,.9f);

    float rowH=28.0f, startY;
    if(strlen(title) > 0){
        float ty = y1-28.0f;
        float titleW = (float)strlen(title)*13.0f;
        textHeading(cx-titleW*.5f, ty, title, .28f,.80f,1.0f);
        bline((int)(x0+12),(int)(ty-10),(int)(x1-12),(int)(ty-10),.18f,.30f,.55f);
        startY = y1-58.0f;
    } else {
        startY = y1-20.0f;
    }

    for(int i=0;i<cnt;i++){
        float fy=startY-i*rowH;
        if(i==sel){
            quad(x0+10,fy-3,x1-10,fy+16, .18f,.42f,.88f,.20f);
            bline((int)(x0+10),(int)(fy-3),(int)(x0+10),(int)(fy+16),.35f,.82f,1.0f);
            text18(cx-(float)strlen(items[i])*4.8f,fy,items[i],.90f,.95f,1.0f);
        } else {
            textLabel(cx-(float)strlen(items[i])*4.6f,fy+1,items[i],.44f,.48f,.58f);
        }
    }
    text12(x0+12,y0+7,"UP/DOWN select   ENTER confirm",.22f,.24f,.32f);
}

// ── Main menu ─────────────────────────────────────────────────
void renderMainMenu()
{
    float w=(float)WIN_W, h=(float)WIN_H;
    quad(0,0,w,h, .05f,.05f,.09f);

    V2 a0{20,h*.68f},a1{w*.35f,h*1.1f},a2{w*.65f,h*.58f},a3{w-20,h*.83f};
    V2 b0{20,h*.32f},b1{w*.4f,-h*.08f},b2{w*.6f,h*.42f},b3{w-20,h*.17f};
    drawBezier(a0,a1,a2,a3,80,.18f,.38f,.88f,.22f);
    drawBezier(b0,b1,b2,b3,80,.68f,.18f,.58f,.18f);

    textTitle(w*.5f-88,h*.5f+150,"Unlimited",.28f,.80f,1.0f);
    textTitle(w*.5f-44,h*.5f+120,"Tetris",   .72f,.26f,.88f);

    if(highScore>0){
        std::string hs="Best: "+std::to_string(highScore);
        textLabel(w*.5f-(float)hs.size()*4.5f,h*.5f+72,hs,.48f,.84f,.48f);
    } else {
        std::string hs="Be the first to set the score!";
        textLabel(w*.5f-(float)hs.size()*4.5f,h*.5f+72,hs,.48f,.84f,.48f);
    }
    const char* items[]={"New Game","Exit"};
    drawMenuBox(w*.5f,h*.5f-18,268,168,items,2,menuItem,"");
}

// ── Board (grid + locked cells + ghost + active piece) ────────
void renderBoard()
{
    quad(0,0,(float)(COLS*CELL),(float)WIN_H,.09f,.09f,.13f);
    drawGrid();

    for(int r=0;r<ROWS;r++)
        for(int c=0;c<COLS;c++)
            if(board[r][c]){int t=boardColor[r][c];drawCell(c,r,COLORS[t][0],COLORS[t][1],COLORS[t][2]);}

    if(gState==STATE_PLAY){
        int gy=ghostY();
        if(gy!=curY)
            for(int i=0;i<4;i++){
                int c=curX+SHAPES[curType][curRot][i][0];
                int r=gy +SHAPES[curType][curRot][i][1];
                drawGhost(c,r,COLORS[curType][0],COLORS[curType][1],COLORS[curType][2]);
            }
        for(int i=0;i<4;i++){
            int c=curX+SHAPES[curType][curRot][i][0];
            int r=curY+SHAPES[curType][curRot][i][1];
            drawCell(c,r,COLORS[curType][0],COLORS[curType][1],COLORS[curType][2]);
        }
    }
}

// ── Right info panel ──────────────────────────────────────────
void renderPanel()
{
    float px=(float)(COLS*CELL);
    quad(px,0,(float)WIN_W,(float)WIN_H,.05f,.05f,.09f);
    bline((int)px,0,(int)px,WIN_H,.26f,.26f,.38f);

    V2 a0{px+8,(float)WIN_H-46},a1{px+148,(float)WIN_H-6},
       a2{px+16,(float)WIN_H-90},a3{px+158,(float)WIN_H-52};
    V2 b0{px+8,58-24},b1{px+148,8-24},b2{px+16,102-24},b3{px+158,54-24};
    drawBezier(a0,a1,a2,a3,60,.24f,.52f,.88f,.28f);
    drawBezier(b0,b1,b2,b3,60,.82f,.32f,.52f,.28f);

    float lx=px+10;
    textHeading(lx,WIN_H-30,"Unlimited Tetris",.28f,.78f,1.0f);

    textLabel(lx,WIN_H-78, "SCORE",.48f,.48f,.60f);
    text18   (lx,WIN_H-98, std::to_string(score));
    textLabel(lx,WIN_H-130,"BEST", .48f,.48f,.60f);
    text18   (lx,WIN_H-148,std::to_string(highScore),.38f,.76f,.38f);
    textLabel(lx,WIN_H-182,"LINES",.48f,.48f,.60f);
    text18   (lx,WIN_H-202,std::to_string(lines));
    textLabel(lx,WIN_H-234,"LEVEL",.48f,.48f,.60f);
    text18   (lx,WIN_H-254,std::to_string(level));

    textLabel(lx,WIN_H-286,"NEXT",.48f,.48f,.60f);
    float nx=px+20, ny=WIN_H-342;
    for(int i=0;i<4;i++){
        int dc=SHAPES[nextType][0][i][0], dr=SHAPES[nextType][0][i][1];
        drawPreview(nx+dc*22, ny-dr*22, nextType);
    }

    float hy=128;
    textLabel(lx,hy,    "CONTROLS",       .30f,.30f,.40f);
    text12(lx,hy-18,    "< >  Move",      .28f,.50f,.28f);
    text12(lx,hy-32,    "^    Rotate",    .28f,.50f,.28f);
    text12(lx,hy-46,    "v    Soft drop", .28f,.50f,.28f);
    text12(lx,hy-60,    "SPC  Hard drop", .28f,.50f,.28f);
    text12(lx,hy-74,    "M    Pause",     .28f,.50f,.28f);
}

// ── Pause menu ────────────────────────────────────────────────
void renderPauseMenu()
{
    quad(0,0,(float)WIN_W,(float)WIN_H, 0,0,0,.60f);
    float bx=(float)(COLS*CELL)*.5f, by=(float)WIN_H*.5f;
    const char* items[]={"Continue","New Game","Exit"};
    drawMenuBox(bx,by,268,205,items,3,menuItem,"PAUSED");
}

// ── Game Over screen ──────────────────────────────────────────
void renderGameOver()
{
    quad(0,0,(float)(COLS*CELL),(float)WIN_H, 0,0,0,.72f);

    float bw=(float)(COLS*CELL);

    draw3DBanner();

    float bx=bw*.5f;
    std::string sc="Score: "+std::to_string(score);
    std::string hs="Best:  "+std::to_string(highScore);
    textLabel(bx-(float)sc.size()*4.5f,(float)WIN_H*.5f-72,sc,.45f,.86f,.45f);
    textLabel(bx-(float)hs.size()*4.5f,(float)WIN_H*.5f-92,hs,.38f,.68f,.38f);

    const char* items[]={"Play Again","Main Menu","Exit"};
    drawMenuBox(bx,(float)WIN_H*.5f-145,268,205,items,3,menuItem,"GAME  OVER");
}

// ── Menu dispatch ─────────────────────────────────────────────
//  The choice itself is made by menuActivate() in globals.cpp.
//  All that is left here is the one thing the logic cannot do
//  from inside a test: end the process.
static void applyMenu(MenuAction action)
{
    if(action==MENU_ACTION_QUIT) exit(0);
    glutPostRedisplay();
}

// ── 10.1  Display callback ────────────────────────────────────
void display()
{
    glClearColor(.07f,.07f,.11f,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    set2D();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    if(gState==STATE_MENU){
        renderMainMenu();
    } else {
        renderBoard();
        renderPanel();
        if(gState==STATE_PAUSE) renderPauseMenu();
        if(gState==STATE_OVER)  renderGameOver();
    }
    glutSwapBuffers();
}

// ── 10.1  Gravity timer ───────────────────────────────────────
//  One reschedule per firing, so the timers never stack.
void timerDrop(int)
{
    stepGravity();
    glutPostRedisplay();
    glutTimerFunc(dropInterval,timerDrop,0);
}

// ── 10.1  Keyboard callback ───────────────────────────────────
void keyboard(unsigned char key, int, int)
{
    if(key==13){ applyMenu(menuActivate()); return; }
    if((key=='m'||key=='M'||key==27)&&gState==STATE_PLAY){
        pauseGame(); glutPostRedisplay(); return;
    }
    if(key==' '){ hardDrop(); glutPostRedisplay(); }
}

// ── 10.1  Special keys callback ───────────────────────────────
void specialKeys(int key, int, int)
{
    if(gState!=STATE_PLAY){
        if(key==GLUT_KEY_UP)   { menuMove(-1); glutPostRedisplay(); }
        if(key==GLUT_KEY_DOWN) { menuMove(+1); glutPostRedisplay(); }
        if(key==GLUT_KEY_RIGHT) applyMenu(menuActivate());
        return;
    }
    switch(key){
        case GLUT_KEY_LEFT:  moveSideways(-1); break;
        case GLUT_KEY_RIGHT: moveSideways(+1); break;
        case GLUT_KEY_DOWN:  softDrop();       break;
        case GLUT_KEY_UP:    rotateCW();       break;
    }
    glutPostRedisplay();
}
