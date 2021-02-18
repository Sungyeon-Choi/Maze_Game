#include<iostream>
#include<conio.h>
#include<Windows.h>
#include"Text.h"
#include"Maze.h"
extern float speaker; // speaker 소환.
extern int death;
extern int stage;
extern int create;
extern int quest;
extern int hansei;
extern int titleOn;
extern int arrowSpeedSet;
using namespace std;

#define ESC 27
#define SPACEBAR 32
#define BACKSPACE 8
#define LEFT 75
#define RIGHT 77

struct _tagPoint
{
    int x;
    int y;
};

int main()
{

    system("mode con cols=110 lines=34  "); //CLI 크기
    system("THE VACCINE");//상단 이름 

    /* 커서 없애기 */
    CONSOLE_CURSOR_INFO ConsoleCursor;
    ConsoleCursor.bVisible = false;
    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleCursor);
    stage = -1; // title
    SetMaze();  // title함수로 들어감 => 메뉴선택통해 stage값 변경

    SetMaze(); // 바뀐 stage값을 이용하여 다시 셋팅.
    char cInput = NULL;
    while (_kbhit()) _getch();
    while (true)
    {
        OutPut();
        //미로 출력


        if (_kbhit())
        {
            cInput = _getch();
            while (_kbhit()) cInput = _getch();
            // 방향키에 대힌 처리에 오류가 계속 되어 없애버림. 문자인식 X ,  m으로 인식. 방향키의 경우를 대비하여 _getch()
            switch (cInput)
            {
            case ESC:
                exit(0);
                break;
            case SPACEBAR:  // 폭탄을 터트림
                CreateBomb();
                break;
            case BACKSPACE: // 이 경우는 게임중에 back을 누른 것이다. 따라서 title과는 무관.
                stage = -1;
                SetMaze();   //튜토리얼이면 return
                SetMaze();   //튜토리얼이면 return
                OutPut();
                if (stage != 0) // 튜토리얼이 아닐 때만.
                    death++; // backspace 죽은 것으로 처리
                break;
            case '0':
                cheat();
                break;
            }
            MovePlayer(cInput);
            arrowMove();
            specialArrowMove();
            gravityRED();
            gravityBLUE();
        }
        else
        {
            arrowMove();
            specialArrowMove();
            gravityRED();
            gravityBLUE();
        }
        // 입력이 있으면 값을 받아 처리하고 움직이는 동시에 화살도 움직이도록 한다.
        // 입력이 없으면 화살만 움직이도록 한다.

        playerInform(); // 플레이어 inform
        if (stage == 0) // tutorial
            tutorial(cInput);
    }
    system("pause");
    return 0;

}