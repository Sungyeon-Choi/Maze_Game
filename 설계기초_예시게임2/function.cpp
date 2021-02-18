#include<iostream>
#include<Windows.h>
#include<conio.h>
#include"Text.h"
#include"Maze.h"
#include<string>
#include<fstream>
#include<algorithm>
#include<time.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

using namespace std;
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80

#define LEFTSPACE 3     // 미로 왼쪽 초기값
#define LIGHTSPACE 3    // 미로 오른쪽 초기값
#define UPSPACE 2       // 미로 위쪽 초기값
#define DOWNSPACE 5     // 미로 아래쪽 초기값
#define MAP 30          // PlayerInform 초기값

#define ENTER 13
#define BACKSPACE 8
#define ESC 27
#define SPACEBAR 32

#define GITEM_OVER_X 30
#define GITEM_OVER_Y 20

#define _X 2
#define _Y 0

#define GITEM_ORIGIN_X 55
#define GITEM_ORIGIN_Y 2
#define GITEM_WIDTH 22
#define GITEM_HEIGHT 17

/* 미로, 병원스토리, 게임오버, 게임클리어 배열*/
char Hospital[26][31];
char GAME_OVER[20][100];
char Maze[30][31];

int arrowSpeedSet = 2; // 컴퓨터마다 속도가 차이가 나는 경우가 있다. 이 경우에 격차를 해소하기 위해 arrowSpeed을 조정한다. 2 -> 기본 1느리게 3 빠르게 

/*타이머 관련 변수들*/
int TimeLimit;
clock_t start;
clock_t t;
clock_t finish;

/*게임 진행 관련 변수들*/
int locate; // select에서 사용.
int quest; // tutorial 변수
int death; // death count
int num;
int stage;
int number; // 횟수. 튜토리얼에서 사용.
int storycnt = 0;

/*화살 관련 변수들*/
long arrowSpeed; // arrow speed는 stage별 구상하도록 한다. 참고로 sleep의 인수기 때문에 크기가 크면 느려짐
POINT arrowPos[15];     // 미로에서 변화하는 화살의 좌표 저장
int arrowCount = 0;         // 미로에서 화살의 개수 저장
char arrowVector[15];      // 화살의 방향 저장 '>' '<' '+' '-'
POINT arrowStart[15];   // 화살표 시작 위치 저장

POINT npcItemPos[5];
int npcPause = 0;
int npcPauseLimit;
int inpcCount;
int npcItem;


/*PC관련 변수들*/
POINT tStartPos;
POINT tPlayerPosRED;
POINT tPlayerPosBLUE;

/*폭탄 관련 변수들*/
POINT tBombPos[5];
int Bombflag[5];
POINT BombFirePos[4][5];
POINT BombPoint;
POINT BombItemPos[5];
int BombLimit;
int BombItem;
int BombPause = 0;
int iBombCount;

/*회전 관련 변수들*/
int tmp = 0;
int idx = 0;
int RotateCount;


int GameClearScore[3][5] = { 0 };   //라운드별 걸린시간, 죽은 횟수, 회전횟수
char Grade[12][13];

/*스페셜 NPC 관련 변수들*/
long SpecialarrowSpeed; // arrow speed는 stage별 구상하도록 한다. 참고로 sleep의 인수기 때문에 크기가 크면 느려짐
POINT SpecialarrowPos[15];
int SpecialarrowCount = 0;
char SpecialarrowVector[15];
POINT SpecialarrowStart[15];

enum COLOR {
    BLACK, BLUE, GREEN, OC, RED, JAJU, YELLOW, WHITE, GRAY
    , DEEP_BLUE, DEEP_GREEN, DEEP_OC, DEEP_RED, DEEP_JAJU, DEEP_YELLOW,
    DEEP_WHITE, SKY
};

void RemoveCursor(void) {
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = 0;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void SetCurrentCursorPos(int x, int y) {
    COORD position = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}
void SetMaze()
{
    num = 0;
    // death는 건드리지 않는다. (1~6 stage에서 세고서 랭킹에 써야 함) death 0은 튜토리얼 or title에서 잘 처리함.

    if (locate == 0) // 선택 중이 아닐 때
        system("cls");

    while (_kbhit()) _getch();

    switch (stage)
    {
    case -1:
        title(); // 타이틀 음악은 타이틀에서 처리하고 있다. title() SetMaze() ≒ stage = -1; SetMaze() SetMaze()
        system("cls");
        printHospital();
        break;

    case 0:
        /* NPC 속도 설정*/
        arrowSpeed = 15;
        SpecialarrowSpeed = 15;
        if (RotateCount == 0)
        {
            PlaySound(TEXT("라운드시작.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

            /*빨간약 초기위치 설정*/
            tPlayerPosRED.x = 20;
            tPlayerPosRED.y = 14;
            /*파란약 초기위치 설정*/
            tPlayerPosBLUE.x = 10;
            tPlayerPosBLUE.y = 1;

            ResetBomb();        // 폭탄관련 초기화
            ResetTime();        // 타이머관련 초기화
            ResetNPC();         // NPC 초기화
        }
        if (tmp == 0) {

            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "0GGGGGGGG011110011110011111110");
            strcpy_s(Maze[2], 31, "0GGGGssGG000010010010011ggg110");
            strcpy_s(Maze[3], 31, "0GsGsssGG000010010010011gggbb0");
            strcpy_s(Maze[4], 31, "0GGGssGGG000011110010011ggg110");
            strcpy_s(Maze[5], 31, "0GsGGGGGG0000000000100111111g0");
            strcpy_s(Maze[6], 31, "0GGGG0GGG0111111111110111111g0");
            strcpy_s(Maze[7], 31, "0GGG00GGG011111111111e11111110");
            strcpy_s(Maze[8], 31, "0GG0000GG011111111111e11111110");
            strcpy_s(Maze[9], 31, "0GG1100GG011111111111e11000000");
            strcpy_s(Maze[10], 31, "0GG0000GG011111111111011001100");
            strcpy_s(Maze[11], 31, "0GG1100GG01100000000001ss01100");
            strcpy_s(Maze[12], 31, "0GG0000GG0110000000000ssss0000");
            strcpy_s(Maze[13], 31, "0GG1100GG0111111111110ssss0000");
            strcpy_s(Maze[14], 31, "0GGssssGG00000000010001ss00000");
            strcpy_s(Maze[15], 31, "0GG11ssGG000111111100011001100");
            strcpy_s(Maze[16], 31, "0GGssssGG000100000000011001100");
            strcpy_s(Maze[17], 31, "0GG11ssGG000100000000011000000");
            strcpy_s(Maze[18], 31, "0GGssssGG000111111111011111110");
            strcpy_s(Maze[19], 31, "0GG11ssGG000000000001011111110");
            strcpy_s(Maze[20], 31, "0GGssssGG000000111111011111110");
            strcpy_s(Maze[21], 31, "0GGG00GGG00000010000001111T110");
            strcpy_s(Maze[22], 31, "0GGG00GGG011111111111011TTT110");
            strcpy_s(Maze[23], 31, "0GGG00GGG011555555511011TTTbb0");
            strcpy_s(Maze[24], 31, "0GG0000GG0115555555110111gT110");
            strcpy_s(Maze[25], 31, "0GGGGGGGG01155555551101gggg110");
            strcpy_s(Maze[26], 31, "0GGGGGGGG01155535551101gggbbb0");
            strcpy_s(Maze[27], 31, "0GGGGGGGG0115555555110111g1110");
            strcpy_s(Maze[28], 31, "0GGGGGGGG011555555511011111110");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");

        }
        else if (tmp == 1)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "011b1gg1100000000011111b11b110");
            strcpy_s(Maze[2], 31, "011b1111101100011011111b11b110");
            strcpy_s(Maze[3], 31, "01ggg1111011000110111TTTTgb110");
            strcpy_s(Maze[4], 31, "01ggg1111000ss00001111TTgggg10");
            strcpy_s(Maze[5], 31, "01ggg111100ssss0001111TT1gg110");
            strcpy_s(Maze[6], 31, "01111111111ssss1111111111gg110");
            strcpy_s(Maze[7], 31, "011111111111ss1111111111111110");
            strcpy_s(Maze[8], 31, "0000000eee00000000000000000000");
            strcpy_s(Maze[9], 31, "000000111110010000111011111110");
            strcpy_s(Maze[10], 31, "011111111110010000101011111110");
            strcpy_s(Maze[11], 31, "010000111110011100101015555550");
            strcpy_s(Maze[12], 31, "010000111110010100101015555550");
            strcpy_s(Maze[13], 31, "011110111110010100101015555550");
            strcpy_s(Maze[14], 31, "000010111110010100101115553550");
            strcpy_s(Maze[15], 31, "000010111110010100100015555550");
            strcpy_s(Maze[16], 31, "011110111110010100100015555550");
            strcpy_s(Maze[17], 31, "010000111110010111100015555550");
            strcpy_s(Maze[18], 31, "010000111111110000000011111110");
            strcpy_s(Maze[19], 31, "010000111111110000000011111110");
            strcpy_s(Maze[20], 31, "000000000000000000000000000000");
            strcpy_s(Maze[21], 31, "0GGGGGGGGGGGGGGGGGGGGGGGGGGGG0");
            strcpy_s(Maze[22], 31, "0GGGGGGGGGGGGGGGGGGGGGGGGGGGG0");
            strcpy_s(Maze[23], 31, "0GssGGGG000000sssssssGGG0GGGG0");
            strcpy_s(Maze[24], 31, "0GsssG00000000sssssss0000GGGG0");
            strcpy_s(Maze[25], 31, "0GGssGG0010101s1s1s1s0000GGGG0");
            strcpy_s(Maze[26], 31, "0GGGGGGG010101s1s1s1sGGG0GGGG0");
            strcpy_s(Maze[27], 31, "0GGsGsGGGGGGGGGGGGGGGGGGGGGGG0");
            strcpy_s(Maze[28], 31, "0GGGGGGGGGGGGGGGGGGGGGGGGGGGG0");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 2)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "011111110115555555110GGGGGGGG0");
            strcpy_s(Maze[2], 31, "0111g1110115555555110GGGGGGGG0");
            strcpy_s(Maze[3], 31, "0bbbggg10115553555110GGGGGGGG0");
            strcpy_s(Maze[4], 31, "011gggg10115555555110GGGGGGGG0");
            strcpy_s(Maze[5], 31, "011Tg1110115555555110GG0000GG0");
            strcpy_s(Maze[6], 31, "0bbTTT110115555555110GGG00GGG0");
            strcpy_s(Maze[7], 31, "011TTT110111111111110GGG00GGG0");
            strcpy_s(Maze[8], 31, "011T11110000001000000GGG00GGG0");
            strcpy_s(Maze[9], 31, "011111110111111000000GGssssGG0");
            strcpy_s(Maze[10], 31, "011111110100000000000GGss11GG0");
            strcpy_s(Maze[11], 31, "011111110111111111000GGssssGG0");
            strcpy_s(Maze[12], 31, "000000110000000001000GGss11GG0");
            strcpy_s(Maze[13], 31, "001100110000000001000GGssssGG0");
            strcpy_s(Maze[14], 31, "001100110001111111000GGss11GG0");
            strcpy_s(Maze[15], 31, "00000ss10001000000000GGssssGG0");
            strcpy_s(Maze[16], 31, "0000ssss0111111111110GG0011GG0");
            strcpy_s(Maze[17], 31, "0000ssss0000000000110GG0000GG0");
            strcpy_s(Maze[18], 31, "00110ss10000000000110GG0011GG0");
            strcpy_s(Maze[19], 31, "001100110111111111110GG0000GG0");
            strcpy_s(Maze[20], 31, "00000011e111111111110GG0011GG0");
            strcpy_s(Maze[21], 31, "01111111e111111111110GG0000GG0");
            strcpy_s(Maze[22], 31, "01111111e111111111110GGG00GGG0");
            strcpy_s(Maze[23], 31, "0g1111110111111111110GGG0GGGG0");
            strcpy_s(Maze[24], 31, "0g1111110010000000000GGGGGGsG0");
            strcpy_s(Maze[25], 31, "011ggg110010011110000GGGssGGG0");
            strcpy_s(Maze[26], 31, "0bbggg110010010010000GGsssGsG0");
            strcpy_s(Maze[27], 31, "011ggg110010010010000GGssGGGG0");
            strcpy_s(Maze[28], 31, "011111110011110011110GGGGGGGG0");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 3)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "0GGGGGGGGGGGGGGGGGGGGGGGGGGGG0");
            strcpy_s(Maze[2], 31, "0GGGGGGGGGGGGGGGGGGGGGGGsGsGG0");
            strcpy_s(Maze[3], 31, "0GGGG0GGGs1s1s1s101010GGGGGGG0");
            strcpy_s(Maze[4], 31, "0GGGG0000s1s1s1s1010100GGssGG0");
            strcpy_s(Maze[5], 31, "0GGGG0000sssssss00000000GsssG0");
            strcpy_s(Maze[6], 31, "0GGGG0GGGsssssss000000GGGGssG0");
            strcpy_s(Maze[7], 31, "0GGGGGGGGGGGGGGGGGGGGGGGGGGGG0");
            strcpy_s(Maze[8], 31, "0GGGGGGGGGGGGGGGGGGGGGGGGGGGG0");
            strcpy_s(Maze[9], 31, "000000000000000000000000000000");
            strcpy_s(Maze[10], 31, "011111110000000011111111000010");
            strcpy_s(Maze[11], 31, "011111110000000011111111000010");
            strcpy_s(Maze[12], 31, "055555510001111010011111000010");
            strcpy_s(Maze[13], 31, "055555510001001010011111011110");
            strcpy_s(Maze[14], 31, "055555510001001010011111010000");
            strcpy_s(Maze[15], 31, "055355511101001010011111010000");
            strcpy_s(Maze[16], 31, "055555510101001010011111011110");
            strcpy_s(Maze[17], 31, "055555510101001010011111000010");
            strcpy_s(Maze[18], 31, "055555510101001110011111000010");
            strcpy_s(Maze[19], 31, "011111110101000010011111111110");
            strcpy_s(Maze[20], 31, "011111110111000010011111000000");
            strcpy_s(Maze[21], 31, "00000000000000000000eee0000000");
            strcpy_s(Maze[22], 31, "0111111111111111ss111111111110");
            strcpy_s(Maze[23], 31, "011gg1111111111ssss11111111110");
            strcpy_s(Maze[24], 31, "011gg1TT1111000ssss001111ggg10");
            strcpy_s(Maze[25], 31, "01ggggTT11110000ss0001111ggg10");
            strcpy_s(Maze[26], 31, "011bgTTTT1110110001101111ggg10");
            strcpy_s(Maze[27], 31, "011b11b1111101100011011111b110");
            strcpy_s(Maze[28], 31, "011b11b1111100000000011gg1b110");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        break;
    case 1:
        system("cls");
        /* NPC 속도 설정*/
        arrowSpeed = 15;
        if (RotateCount == 0)
        {
            idx = 0;
            system("cls");
            /*빨간약 초기위치 설정*/
            tPlayerPosRED.x = 1;
            tPlayerPosRED.y = 1;
            /* 파란약 초기위치 설정*/
            tPlayerPosBLUE.x = 28;
            tPlayerPosBLUE.y = 1;
            /*NPC 초기위치 설정*/
            arrowCount = 4;     // 1라운드에서 화살표의 개수는 2개
            /*첫번째 화살표 위치와 방향*/
            arrowStart[0].x = 4;
            arrowStart[0].y = 16;
            arrowPos[0] = arrowStart[0];
            arrowVector[0] = '>';

            /*두번째 화살표 위치와 방향*/
            arrowStart[1].x = 4;
            arrowStart[1].y = 17;
            arrowPos[1] = arrowStart[1];
            arrowVector[1] = '>';

            /*세번째 화살표 위치와 방향*/
            arrowStart[2].x = 15;
            arrowStart[2].y = 19;
            arrowPos[2] = arrowStart[2];
            arrowVector[2] = '<';
            /*네번째 화살표 위치와 방향*/
            arrowStart[3].x = 15;
            arrowStart[3].y = 20;
            arrowPos[3] = arrowStart[3];
            arrowVector[3] = '<';

            /*라운드별 시작스토리 출력*/
            if (death == 3)
                startStory();
            PlaySound(TEXT("라운드시작.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

            ResetBomb();    // 폭탄 관련 초기화
            ResetTime();    // 타이머 관련 초기화
            ResetNPC();     // NPC 초기화
        }
        if (tmp == 0) {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "010111111110111111111111151110");
            strcpy_s(Maze[2], 31, "010100001010001000100010155550");
            strcpy_s(Maze[3], 31, "010101111011111110111110101110");
            strcpy_s(Maze[4], 31, "010101111011010000100010100010");
            strcpy_s(Maze[5], 31, "010100011111010111111010111110");
            strcpy_s(Maze[6], 31, "011111010000010111101010001000");
            strcpy_s(Maze[7], 31, "000001011111111100101011111010");
            strcpy_s(Maze[8], 31, "011111010000000110101000101010");
            strcpy_s(Maze[9], 31, "010000010111110110111111100010");
            strcpy_s(Maze[10], 31, "011111111101010110100001111110");
            strcpy_s(Maze[11], 31, "000011000101011110111101010000");
            strcpy_s(Maze[12], 31, "011111011101011100001101010110");
            strcpy_s(Maze[13], 31, "000011010001000111101001010110");
            strcpy_s(Maze[14], 31, "000011011111111111101111111110");
            strcpy_s(Maze[15], 31, "000011000000000000000000000000");
            strcpy_s(Maze[16], 31, "0000111111111111e0000000000000");
            strcpy_s(Maze[17], 31, "0000111111111111e0000000000000");
            strcpy_s(Maze[18], 31, "000011111111111100000000000000");
            strcpy_s(Maze[19], 31, "000e11111111111100000000000000");
            strcpy_s(Maze[20], 31, "000e11111111111100000000000000");
            strcpy_s(Maze[21], 31, "000011111111111111111111111110");
            strcpy_s(Maze[22], 31, "000000000000000000000000000110");
            strcpy_s(Maze[23], 31, "011111111111111111111111110110");
            strcpy_s(Maze[24], 31, "010000000000000000000000010110");
            strcpy_s(Maze[25], 31, "010000000000000000015100010110");
            strcpy_s(Maze[26], 31, "010311111115111000010111110110");
            strcpy_s(Maze[27], 31, "010000000000001000010000000110");
            strcpy_s(Maze[28], 31, "011111111111111000011111111110");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 1)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "015111011110111000000111111110");
            strcpy_s(Maze[2], 31, "015101000010111000000111111110");
            strcpy_s(Maze[3], 31, "015101111010001000000100000010");
            strcpy_s(Maze[4], 31, "055001010011111000000101111010");
            strcpy_s(Maze[5], 31, "011111011110001000000101001010");
            strcpy_s(Maze[6], 31, "010000010111111000000101001010");
            strcpy_s(Maze[7], 31, "011111110100001000000101001010");
            strcpy_s(Maze[8], 31, "010100000101101000000101011010");
            strcpy_s(Maze[9], 31, "010101111101111000000101050010");
            strcpy_s(Maze[10], 31, "010101000101000000000101011110");
            strcpy_s(Maze[11], 31, "011111111111011000000101000000");
            strcpy_s(Maze[12], 31, "010001100000011000000101000000");
            strcpy_s(Maze[13], 31, "0101011011110110ee000101000000");
            strcpy_s(Maze[14], 31, "010101111111111011111101000000");
            strcpy_s(Maze[15], 31, "011100010001101011111101001110");
            strcpy_s(Maze[16], 31, "010111110111101011111101001010");
            strcpy_s(Maze[17], 31, "010100010100001011111101001010");
            strcpy_s(Maze[18], 31, "000111010111111011111101005010");
            strcpy_s(Maze[19], 31, "011111010100001011111101001010");
            strcpy_s(Maze[20], 31, "010001010111101011111101001010");
            strcpy_s(Maze[21], 31, "011111010010101011111101001010");
            strcpy_s(Maze[22], 31, "010111111110111011111101001010");
            strcpy_s(Maze[23], 31, "010110000010000011111101001010");
            strcpy_s(Maze[24], 31, "010110111011111111111101001010");
            strcpy_s(Maze[25], 31, "010000101011111111111101001010");
            strcpy_s(Maze[26], 31, "0111111010101000000ee001003010");
            strcpy_s(Maze[27], 31, "000000101010100000000001000010");
            strcpy_s(Maze[28], 31, "011111101110100000000001111110");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 2)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "011111111110000111111111111110");
            strcpy_s(Maze[2], 31, "011000000010000100000000000010");
            strcpy_s(Maze[3], 31, "011011111010000111511111113010");
            strcpy_s(Maze[4], 31, "011010001510000000000000000010");
            strcpy_s(Maze[5], 31, "011010000000000000000000000010");
            strcpy_s(Maze[6], 31, "011011111111111111111111111110");
            strcpy_s(Maze[7], 31, "011000000000000000000000000000");
            strcpy_s(Maze[8], 31, "011111111111111111111111110000");
            strcpy_s(Maze[9], 31, "00000000000000111111111111e000");
            strcpy_s(Maze[10], 31, "00000000000000111111111111e000");
            strcpy_s(Maze[11], 31, "000000000000001111111111110000");
            strcpy_s(Maze[12], 31, "0000000000000e1111111111110000");
            strcpy_s(Maze[13], 31, "0000000000000e1111111111110000");
            strcpy_s(Maze[14], 31, "000000000000000000000000110000");
            strcpy_s(Maze[15], 31, "011111111101111111111110110000");
            strcpy_s(Maze[16], 31, "011010100101111000100010110000");
            strcpy_s(Maze[17], 31, "011010101100001110101110111110");
            strcpy_s(Maze[18], 31, "000010101111011110101000110000");
            strcpy_s(Maze[19], 31, "011111100001011010101111111110");
            strcpy_s(Maze[20], 31, "010001111111011011111010000010");
            strcpy_s(Maze[21], 31, "010101000101011000000010111110");
            strcpy_s(Maze[22], 31, "010111110101001111111110100000");
            strcpy_s(Maze[23], 31, "000100010101111010000010111110");
            strcpy_s(Maze[24], 31, "011111010111111010111110001010");
            strcpy_s(Maze[25], 31, "010001010001000010110111101010");
            strcpy_s(Maze[26], 31, "011101011111011111110111101010");
            strcpy_s(Maze[27], 31, "055551010001000100010100001010");
            strcpy_s(Maze[28], 31, "011151111111111111011111111010");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 3)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "011111100000000001011101111110");
            strcpy_s(Maze[2], 31, "010000100000000001010101000000");
            strcpy_s(Maze[3], 31, "010300100ee0000001010101111110");
            strcpy_s(Maze[4], 31, "010100101111111111110101000010");
            strcpy_s(Maze[5], 31, "010100101111111111110111011010");
            strcpy_s(Maze[6], 31, "010100101111110000010000011010");
            strcpy_s(Maze[7], 31, "010100101111110111011111111010");
            strcpy_s(Maze[8], 31, "010100101111110101010010111110");
            strcpy_s(Maze[9], 31, "010100101111110101111010100010");
            strcpy_s(Maze[10], 31, "010100101111110100001010111110");
            strcpy_s(Maze[11], 31, "010500101111110111111010111000");
            strcpy_s(Maze[12], 31, "010100101111110100001010001010");
            strcpy_s(Maze[13], 31, "010100101111110101111011111010");
            strcpy_s(Maze[14], 31, "011100101111110101100010001110");
            strcpy_s(Maze[15], 31, "000000101111110111111111101010");
            strcpy_s(Maze[16], 31, "000000101000ee0110111101101010");
            strcpy_s(Maze[17], 31, "000000101000000110000001100010");
            strcpy_s(Maze[18], 31, "000000101000000110111111111110");
            strcpy_s(Maze[19], 31, "011110101000000000101000101010");
            strcpy_s(Maze[20], 31, "010050101000000111101111101010");
            strcpy_s(Maze[21], 31, "010110101000000101101000001010");
            strcpy_s(Maze[22], 31, "010100101000000100001011111110");
            strcpy_s(Maze[23], 31, "010100101000000111111010000010");
            strcpy_s(Maze[24], 31, "010100101000000100011110111110");
            strcpy_s(Maze[25], 31, "010111101000000111110010100550");
            strcpy_s(Maze[26], 31, "010000001000000100010111101510");
            strcpy_s(Maze[27], 31, "011111111000000111010000101510");
            strcpy_s(Maze[28], 31, "011111111000000111011110111510");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        break;
    case 2:
        /* NPC 속도 설정*/
        arrowSpeed = 2;
        SpecialarrowSpeed = 2;
        if (RotateCount == 0)
        {
            idx = 0;
            /*빨간약 초기위치 설정*/
            tPlayerPosRED.x = 12;
            tPlayerPosRED.y = 1;
            /* 파란약 초기위치 설정*/
            tPlayerPosBLUE.x = 28;
            tPlayerPosBLUE.y = 1;

            /*라운드별 시작스토리 출력*/
            if (death == 3)
                startStory();
            PlaySound(TEXT("라운드시작.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

            ResetBomb();        // 폭탄 관련 초기화
            ResetTime();        // 타이머 관련 초기화
            ResetNPC();         // NPC 초기화
        }
        if (tmp == 0)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "011111111111101110111111111110");
            strcpy_s(Maze[2], 31, "010000000000001110100000000000");
            strcpy_s(Maze[3], 31, "010111111111111111111111111e10");
            strcpy_s(Maze[4], 31, "010177777777777777777777771010");
            strcpy_s(Maze[5], 31, "010170000011100000111000001010");
            strcpy_s(Maze[6], 31, "010170GGG01110GGG01110GGG01010");
            strcpy_s(Maze[7], 31, "010170000011100000111000001010");
            strcpy_s(Maze[8], 31, "010170000011100000111000001010");
            strcpy_s(Maze[9], 31, "01017GGGGG111GGGGG111GGGGG1010");
            strcpy_s(Maze[10], 31, "010177777711177777111777771010");
            strcpy_s(Maze[11], 31, "01e177777777777777777777771010");
            strcpy_s(Maze[12], 31, "010111111111111111111111111010");
            strcpy_s(Maze[13], 31, "010000000000100000100000000010");
            strcpy_s(Maze[14], 31, "011111111111111111111111111110");
            strcpy_s(Maze[15], 31, "01000000000e00000e000000000000");
            strcpy_s(Maze[16], 31, "010111101111111511111111111100");
            strcpy_s(Maze[17], 31, "010111101111111511111111111100");
            strcpy_s(Maze[18], 31, "010100101111111511110100000000");
            strcpy_s(Maze[19], 31, "010100101111111511110111111100");
            strcpy_s(Maze[20], 31, "010100101111111511110000000100");
            strcpy_s(Maze[21], 31, "010100101111111511110111111100");
            strcpy_s(Maze[22], 31, "010100101111111511110100000000");
            strcpy_s(Maze[23], 31, "010100101111111511110100000000");
            strcpy_s(Maze[24], 31, "011100111111111511110111111100");
            strcpy_s(Maze[25], 31, "0111000000000e00000e0000000100");
            strcpy_s(Maze[26], 31, "011101111111111111111111111100");
            strcpy_s(Maze[27], 31, "055500001111111111e00000000000");
            strcpy_s(Maze[28], 31, "000000005555511155511111111300");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 1)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "010111111111111000000000000000");
            strcpy_s(Maze[2], 31, "010e00000000001011011100111030");
            strcpy_s(Maze[3], 31, "010111111111101011010100101010");
            strcpy_s(Maze[4], 31, "010170000G77101011010100101010");
            strcpy_s(Maze[5], 31, "010170G00G77101011010100101010");
            strcpy_s(Maze[6], 31, "010170G00G77101011010100101010");
            strcpy_s(Maze[7], 31, "010170G00G77101011010100101010");
            strcpy_s(Maze[8], 31, "010170000G77101011110111101010");
            strcpy_s(Maze[9], 31, "010171111117101011000000001010");
            strcpy_s(Maze[10], 31, "0101711111171010111111111e1010");
            strcpy_s(Maze[11], 31, "011171111117111011111111101e50");
            strcpy_s(Maze[12], 31, "000170000G77101e11111111101150");
            strcpy_s(Maze[13], 31, "011170G00G77101011111111101150");
            strcpy_s(Maze[14], 31, "011170G00G77101055555555501110");
            strcpy_s(Maze[15], 31, "011170G00G77101011111111101110");
            strcpy_s(Maze[16], 31, "000170000G771010111111111e1110");
            strcpy_s(Maze[17], 31, "010171111117111011111111101150");
            strcpy_s(Maze[18], 31, "010171111117101e11111111101150");
            strcpy_s(Maze[19], 31, "010171111117101011111111101150");
            strcpy_s(Maze[20], 31, "010170000G77101011111111101150");
            strcpy_s(Maze[21], 31, "010170G00G77101011111111101150");
            strcpy_s(Maze[22], 31, "010170G00G77101000000000101100");
            strcpy_s(Maze[23], 31, "010170G00G77101011111111101000");
            strcpy_s(Maze[24], 31, "010170000G77101011000000001000");
            strcpy_s(Maze[25], 31, "010177777777101011000000000000");
            strcpy_s(Maze[26], 31, "010111111111101011111111111500");
            strcpy_s(Maze[27], 31, "01000000000e001000000000111500");
            strcpy_s(Maze[28], 31, "011111111111111111111111111500");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 2)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "003111111115551115555500000000");
            strcpy_s(Maze[2], 31, "00000000000e111111111100005550");
            strcpy_s(Maze[3], 31, "001111111111111111111111101110");
            strcpy_s(Maze[4], 31, "0010000000e00000e0000000001110");
            strcpy_s(Maze[5], 31, "001111111011115111111111001110");
            strcpy_s(Maze[6], 31, "000000001011115111111101001010");
            strcpy_s(Maze[7], 31, "000000001011115111111101001010");
            strcpy_s(Maze[8], 31, "001111111011115111111101001010");
            strcpy_s(Maze[9], 31, "001000000011115111111101001010");
            strcpy_s(Maze[10], 31, "001111111011115111111101001010");
            strcpy_s(Maze[11], 31, "000000001011115111111101001010");
            strcpy_s(Maze[12], 31, "001111111111115111111101111010");
            strcpy_s(Maze[13], 31, "001111111111115111111101111010");
            strcpy_s(Maze[14], 31, "000000000000e00000e00000000010");
            strcpy_s(Maze[15], 31, "011111111111111111111111111110");
            strcpy_s(Maze[16], 31, "010000000001000001000000000010");
            strcpy_s(Maze[17], 31, "010111111111111111111111111010");
            strcpy_s(Maze[18], 31, "010177777777777777777777771e10");
            strcpy_s(Maze[19], 31, "010177777111777771117777771010");
            strcpy_s(Maze[20], 31, "0101GGGGG111GGGGG111GGGGG71010");
            strcpy_s(Maze[21], 31, "010100000111000001110000071010");
            strcpy_s(Maze[22], 31, "010100000111000001110000071010");
            strcpy_s(Maze[23], 31, "01010GGG01110GGG01110GGG071010");
            strcpy_s(Maze[24], 31, "010100000111000001110000071010");
            strcpy_s(Maze[25], 31, "010177777777777777777777771010");
            strcpy_s(Maze[26], 31, "01e111111111111111111111111010");
            strcpy_s(Maze[27], 31, "000000000001011100000000000010");
            strcpy_s(Maze[28], 31, "011111111111011101111111111110");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 3)
        {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "005111111111111111111111111110");
            strcpy_s(Maze[2], 31, "005111000000000100e00000000010");
            strcpy_s(Maze[3], 31, "005111111111110101111111111010");
            strcpy_s(Maze[4], 31, "000000000000110101777777771010");
            strcpy_s(Maze[5], 31, "00010000000011010177G000071010");
            strcpy_s(Maze[6], 31, "00010111111111010177G00G071010");
            strcpy_s(Maze[7], 31, "00010100000000010177G00G071010");
            strcpy_s(Maze[8], 31, "05110111111111010177G00G071010");
            strcpy_s(Maze[9], 31, "05110111111111010177G000071010");
            strcpy_s(Maze[10], 31, "051101111111110101711111171010");
            strcpy_s(Maze[11], 31, "05110111111111e101711111171010");
            strcpy_s(Maze[12], 31, "051101111111110111711111171010");
            strcpy_s(Maze[13], 31, "0111e111111111010177G000071000");
            strcpy_s(Maze[14], 31, "01110111111111010177G00G071110");
            strcpy_s(Maze[15], 31, "01110555555555010177G00G071110");
            strcpy_s(Maze[16], 31, "05110111111111010177G00G071110");
            strcpy_s(Maze[17], 31, "05110111111111e10177G000071000");
            strcpy_s(Maze[18], 31, "05e101111111110111711111171110");
            strcpy_s(Maze[19], 31, "0101e1111111110101711111171010");
            strcpy_s(Maze[20], 31, "010100000000110101711111171010");
            strcpy_s(Maze[21], 31, "01010111101111010177G000071010");
            strcpy_s(Maze[22], 31, "01010100101011010177G00G071010");
            strcpy_s(Maze[23], 31, "01010100101011010177G00G071010");
            strcpy_s(Maze[24], 31, "01010100101011010177G00G071010");
            strcpy_s(Maze[25], 31, "01010100101011010177G000071010");
            strcpy_s(Maze[26], 31, "010101001010110101111111111010");
            strcpy_s(Maze[27], 31, "03011100111011010000000000e010");
            strcpy_s(Maze[28], 31, "000000000000000111111111111010");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");

        }
        break;
    case 3:
        system("cls");
        if (RotateCount == 0)
        {
            idx = 0;
            /*빨간약 초기위치 설정*/
            tPlayerPosRED.x = 1;
            tPlayerPosRED.y = 13;
            /* 파란약 초기위치 설정*/
            tPlayerPosBLUE.x = 1;
            tPlayerPosBLUE.y = 28;

            /*라운드별 시작스토리 출력*/
            if (death == 3)
                startStory();
            PlaySound(TEXT("라운드시작.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

            ResetBomb();        // 폭탄 관련 초기화
            ResetTime();        // 타이머 관련 초기화
            ResetNPC();         // NPC 초기화
        }
        if (tmp == 0) {

            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "0oo000000001111511111111111130");
            strcpy_s(Maze[2], 31, "0oo00GGBB00111101111111e000000");
            strcpy_s(Maze[3], 31, "0GG00GGBB001111011111110000000");
            strcpy_s(Maze[4], 31, "0GG000000000005000000110111110");
            strcpy_s(Maze[5], 31, "000000000001111011111110100010");
            strcpy_s(Maze[6], 31, "000000gg0005000011111110101010");
            strcpy_s(Maze[7], 31, "000000gg0001111e11111110101010");
            strcpy_s(Maze[8], 31, "0000gggggg00005011111110101010");
            strcpy_s(Maze[9], 31, "0000gggggg01111011111110101010");
            strcpy_s(Maze[10], 31, "000000gg0005000011111110101010");
            strcpy_s(Maze[11], 31, "000000gg0001111011111110101010");
            strcpy_s(Maze[12], 31, "00000000000001101111111e151010");
            strcpy_s(Maze[13], 31, "011011111110111011111110101010");
            strcpy_s(Maze[14], 31, "001010101010111055000000101010");
            strcpy_s(Maze[15], 31, "001111111110100011111110101010");
            strcpy_s(Maze[16], 31, "001011010110111011111110101510");
            strcpy_s(Maze[17], 31, "0011111111001110e1111110101010");
            strcpy_s(Maze[18], 31, "000110101111100011111110101010");
            strcpy_s(Maze[19], 31, "011100100100111000000115101010");
            strcpy_s(Maze[20], 31, "010101101110001111110000000010");
            strcpy_s(Maze[21], 31, "010101000111111001011110111110");
            strcpy_s(Maze[22], 31, "010101110100011111100010500000");
            strcpy_s(Maze[23], 31, "010101010111010101101110111110");
            strcpy_s(Maze[24], 31, "011101011101110101101000e00010");
            strcpy_s(Maze[25], 31, "000101010111010101101011111110");
            strcpy_s(Maze[26], 31, "011111011101110001001011111110");
            strcpy_s(Maze[27], 31, "000001010111010111151111111110");
            strcpy_s(Maze[28], 31, "011111011101110111101111111110");
            strcpy_s(Maze[29], 31, "00000000000000000000000000e000");

        }
        else if (tmp == 1) {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "030011111111111111111101111110");
            strcpy_s(Maze[2], 31, "010010000000000050000101011110");
            strcpy_s(Maze[3], 31, "01001011111111111111010101111e");
            strcpy_s(Maze[4], 31, "010010000000500000000101011110");
            strcpy_s(Maze[5], 31, "010011111111111111110151e11110");
            strcpy_s(Maze[6], 31, "01e000000000e00000050000011110");
            strcpy_s(Maze[7], 31, "011111111111110111110111011110");
            strcpy_s(Maze[8], 31, "011111111111110111110101000110");
            strcpy_s(Maze[9], 31, "011101111111110111100101111110");
            strcpy_s(Maze[10], 31, "011101111111110111101100000500");
            strcpy_s(Maze[11], 31, "011101111111110111101011110110");
            strcpy_s(Maze[12], 31, "011101111111115111101111111110");
            strcpy_s(Maze[13], 31, "01110111111111511e101010000110");
            strcpy_s(Maze[14], 31, "0500000e0000000000001011110110");
            strcpy_s(Maze[15], 31, "011151015101111011011110000000");
            strcpy_s(Maze[16], 31, "011101010101111011010111111110");
            strcpy_s(Maze[17], 31, "011101010101011111110100101010");
            strcpy_s(Maze[18], 31, "011101510151000000100101111110");
            strcpy_s(Maze[19], 31, "000000000000011110101101010100");
            strcpy_s(Maze[20], 31, "00000000gg00010111111111111110");
            strcpy_s(Maze[21], 31, "00BB0000gg00011101101000101010");
            strcpy_s(Maze[22], 31, "00BB00gggggg010111000011111110");
            strcpy_s(Maze[23], 31, "00GG00gggggg011101111010000000");
            strcpy_s(Maze[24], 31, "00GG0000gg00010111001111111110");
            strcpy_s(Maze[25], 31, "00000000gg00011111100000001010");
            strcpy_s(Maze[26], 31, "000000000000000101111111111010");
            strcpy_s(Maze[27], 31, "0ooGG0000000011111010000101010");
            strcpy_s(Maze[28], 31, "0ooGG0000000010000011111101010");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");

        }
        else if (tmp == 2) {
            strcpy_s(Maze[0], 31, "000e00000000000000000000000000");
            strcpy_s(Maze[1], 31, "011111111101111011101110111110");
            strcpy_s(Maze[2], 31, "011111111151111010111010100000");
            strcpy_s(Maze[3], 31, "011111110100100011101110111110");
            strcpy_s(Maze[4], 31, "011111110101101010111010101000");
            strcpy_s(Maze[5], 31, "01000e000101101011101110101110");
            strcpy_s(Maze[6], 31, "011111011101101010111010101010");
            strcpy_s(Maze[7], 31, "000005010001111110001011101010");
            strcpy_s(Maze[8], 31, "011111011110100111111000101010");
            strcpy_s(Maze[9], 31, "010000000011111100011101101010");
            strcpy_s(Maze[10], 31, "010101511000000111001001001110");
            strcpy_s(Maze[11], 31, "010101011111110001111101011000");
            strcpy_s(Maze[12], 31, "0101010111111e0111001111111100");
            strcpy_s(Maze[13], 31, "015101011111110111011010110100");
            strcpy_s(Maze[14], 31, "010101011111110001011111111100");
            strcpy_s(Maze[15], 31, "010101000000550111010101010100");
            strcpy_s(Maze[16], 31, "010101011111110111011111110110");
            strcpy_s(Maze[17], 31, "010151e11111110110000000000000");
            strcpy_s(Maze[18], 31, "0101010111111101111000gg000000");
            strcpy_s(Maze[19], 31, "0101010111111100005000gg000000");
            strcpy_s(Maze[20], 31, "01010101111111011110gggggg0000");
            strcpy_s(Maze[21], 31, "01010101111111050000gggggg0000");
            strcpy_s(Maze[22], 31, "01010101111111e1111000gg000000");
            strcpy_s(Maze[23], 31, "0101010111111100005000gg000000");
            strcpy_s(Maze[24], 31, "010001011111110111100000000000");
            strcpy_s(Maze[25], 31, "011111011000000500000000000GG0");
            strcpy_s(Maze[26], 31, "000000011111110111100BBGG00GG0");
            strcpy_s(Maze[27], 31, "000000e11111110111100BBGG00oo0");
            strcpy_s(Maze[28], 31, "031111111111115111100000000oo0");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        else if (tmp == 3) {
            strcpy_s(Maze[0], 31, "000000000000000000000000000000");
            strcpy_s(Maze[1], 31, "0101011111100000100000000GGoo0");
            strcpy_s(Maze[2], 31, "0101010000101111100000000GGoo0");
            strcpy_s(Maze[3], 31, "010111111111101000000000000000");
            strcpy_s(Maze[4], 31, "01010000000111111000gg00000000");
            strcpy_s(Maze[5], 31, "01111111110011101000gg0000GG00");
            strcpy_s(Maze[6], 31, "000000010111101110gggggg00GG00");
            strcpy_s(Maze[7], 31, "011111110000111010gggggg00BB00");
            strcpy_s(Maze[8], 31, "01010100010110111000gg0000BB00");
            strcpy_s(Maze[9], 31, "01111111111111101000gg00000000");
            strcpy_s(Maze[10], 31, "001010101101011110000000000000");
            strcpy_s(Maze[11], 31, "011111101001000000151015101110");
            strcpy_s(Maze[12], 31, "010101001011111110101010101110");
            strcpy_s(Maze[13], 31, "011111111010110111101010101110");
            strcpy_s(Maze[14], 31, "000000011110110111101510151110");
            strcpy_s(Maze[15], 31, "0110111101000000000000e0000050");
            strcpy_s(Maze[16], 31, "011000010101e11511111111101110");
            strcpy_s(Maze[17], 31, "011111111101111511111111101110");
            strcpy_s(Maze[18], 31, "011011110101111011111111101110");
            strcpy_s(Maze[19], 31, "005000001101111011111111101110");
            strcpy_s(Maze[20], 31, "011111101001111011111111101110");
            strcpy_s(Maze[21], 31, "011000101011111011111111111110");
            strcpy_s(Maze[22], 31, "011110111011111011111111111110");
            strcpy_s(Maze[23], 31, "01111000005000000e000000000e10");
            strcpy_s(Maze[24], 31, "01111e151011111111111111110010");
            strcpy_s(Maze[25], 31, "011110101000000005000000010010");
            strcpy_s(Maze[26], 31, "e11110101011111111111111010010");
            strcpy_s(Maze[27], 31, "011110101000050000000000010010");
            strcpy_s(Maze[28], 31, "011111101111111111111111110030");
            strcpy_s(Maze[29], 31, "000000000000000000000000000000");
        }
        break;

    case 4: //Ending
        PlaySound(TEXT("승리.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
        printGameSuccess();
        SetMaze();
        SetMaze();
        OutPut();
        return;
    case 5:
        printGameOver();
        //stage = -1;
        SetMaze();
        SetMaze(); // 바뀐 stage값을 이용하여 다시 셋팅.
        OutPut();

        char cInput = NULL;

        while (_kbhit()) _getch();

    }
    if (locate > 0) return; // 먼가를 고른 경우에  더이상 출력 안 함.
    TextColor(DEEP_WHITE);

    if (stage != 0) // main메뉴에서는 'm'을 막아둠.(잠재적 오류 발생) 그래서 보여주지 않도록
    {

        gotoxy(MAP + 5, 3, "＃─────────────＃");
        gotoxy(MAP + 7, 4, "      STAGE : "); printf("%d", stage);
        gotoxy(MAP + 5, 5, "＃─────────────＃");
    }
    else
    {
        gotoxy(MAP + 5, 3, "＃─────────────＃");
        gotoxy(MAP + 7, 4, "     TUTORIAL   ");
        gotoxy(MAP + 5, 5, "＃─────────────＃");
    }

    gotoxy(MAP + 7, 8, "← : 반시계 회전");
    gotoxy(MAP + 7, 10, "→ : 시계 회전");

    TextColor(RED);
    gotoxy(MAP + 7, 14, "     <ITEM>");
    TextColor(WHITE);

    gotoxy(MAP + 7, 16, "SPACE : 폭탄"); //printf("  %d", BombItem);

    gotoxy(MAP + 14, 16, "");

    TextColor(DEEP_OC);
    if (BombItem == 0) {
        printf("           ");
    }
    else if (BombItem == 1) {
        printf("δ           ");
    }
    else if (BombItem == 2) {
        printf("δδ         ");
    }
    else if (BombItem == 3) {
        printf("δδδ       ");
    }
    else if (BombItem == 4) {
        printf("δδδδ     ");
    }
    else if (BombItem == 5) {
        printf("δδδδδ   ");
    }
    TextColor(WHITE);


    finish = clock();
    t = finish - start;

    gotoxy(30 + 10, 26, ""); printf("  %.f   ", TimeLimit - (((float)t) / CLOCKS_PER_SEC));



    //gotoxy(MAP + 16, 18, ""); printf(" %d", npcPauseLimit - inpcCount);

    gotoxy(MAP + 14, 18, "");

    TextColor(DEEP_YELLOW);
    if (npcPauseLimit - inpcCount == 0) {
        printf("           ");
    }
    else if (npcPauseLimit - inpcCount == 1) {
        printf("           ");
    }
    else if (npcPauseLimit - inpcCount == 2) {
        printf("ØØ         ");
    }
    else if (npcPauseLimit - inpcCount == 3) {
        printf("ØØØ       ");
    }
    else if (npcPauseLimit - inpcCount == 4) {
        printf("ØØØØ     ");
    }
    else if (npcPauseLimit - inpcCount == 5) {
        printf("ØØØØØ   ");
    }
    TextColor(WHITE);


    gotoxy(MAP + 14, 24, ""); printf("%d", RotateCount);

    gotoxy(MAP + 7, 28, "LIFE : ");
    if (death == 3) {
        gotoxy(MAP + 11, 28, "♥  ♥  ♥");
    }
    else if (death == 2) {
        gotoxy(MAP + 11, 28, "♥  ♥      ");
    }
    else if (death == 1) {
        gotoxy(MAP + 11, 28, "♥          ");
    }


    TextColor(WHITE);
    gotoxy(MAP + 7, 18, "N : NPC 정지");

    TextColor(DEEP_JAJU);
    gotoxy(MAP + 7, 24, "회전 횟수  : ");

    TextColor(DEEP_GREEN);
    gotoxy(MAP + 7, 26, "TIMER");

    ////각 stage 마다 설정된 현재 좌표가 startPoint 가 된다. (나중에 사용함 ) 
    //if (stage != -1)
    //    tStartPos = tPlayerPosRED;

    while (_kbhit()) _getch();
}

void OutPut()
{
    /* 폭탄 사용시 미로 적용*/
    for (int i = 0; i < iBombCount; i++) {


        BombPoint.x = BombFirePos[tmp][i].x;
        BombPoint.y = BombFirePos[tmp][i].y;


        if (Maze[BombPoint.y + 1][BombPoint.x] == '5')
        {
            Maze[BombPoint.y + 1][BombPoint.x] = '1';
        }

        if (Maze[BombPoint.y - 1][BombPoint.x] == '5')
        {

            Maze[BombPoint.y - 1][BombPoint.x] = '1';
        }
        if (Maze[BombPoint.y][BombPoint.x + 1] == '5')
        {
            Maze[BombPoint.y][BombPoint.x + 1] = '1';

        }

        if (Maze[BombPoint.y][BombPoint.x - 1] == '5')
        {
            Maze[BombPoint.y][BombPoint.x - 1] = '1';

        }

    }


    for (int i = 0; i < 30; i++) //행 y
    {
        for (int j = 0; j < 30; j++) //열 x
        {
            /* 게임 시작시에*/
            if (RotateCount == 0)
            {
                /* 폭탄 좌표에 '*'표시 */
                for (int k = 0; k < BombLimit; k++)
                    Maze[BombItemPos[k].y][BombItemPos[k].x] = '*';

                /* PC 좌표에 각각 표시 */
                Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '6';
                Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '2';

                for (int i = 0; i < arrowCount; i++)
                    Maze[arrowPos[i].y][arrowPos[i].x] = arrowVector[i];
                for (int i = 0; i < SpecialarrowCount; i++)
                    Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = SpecialarrowVector[i];

                /* 게임이 진행되지 않으면 빨간약 파란약 출력하지 않기*/
                if (stage == -1 || stage == 4 || stage == 5)
                {
                    Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '1';
                    Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '1';
                    for (int k = 0; k < BombLimit; k++)
                        Maze[BombItemPos[k].y][BombItemPos[k].x] = '1';
                    if (stage == -1)
                        titlePicture();
                }
            }

            switch (Maze[i][j])
            {
                //꾸미기 용//
            case 's':   //sky 하늘색
                TextColor(DEEP_OC);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case 'G':   //Gray 회색
                TextColor(GRAY);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case 'g':   //green 초록색 연한나무
                TextColor(GREEN);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case 'T':   //Tree 진한나무
                TextColor(DEEP_GREEN);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case 'o':   //알약 진한 주황
                TextColor(DEEP_YELLOW);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case 'b':   //brown 갈색
                TextColor(DEEP_JAJU);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
            case 'B':
                TextColor(DEEP_BLUE);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case 't':   //title text
                TextColor(DEEP_WHITE);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case 'R':   //brown 갈색
                TextColor(DEEP_RED);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                break;
            case ' ':
                if (num == 0)
                    gotoxy(LEFTSPACE + j, UPSPACE + i, " ");
                break;
            case '0':
            case 'e':
                TextColor(DEEP_WHITE);
                if (num == 0)
                {
                    gotoxy(LEFTSPACE + j, UPSPACE + i, "■");
                    // title의 경우 찍은 자리에 또 찍어줌. (속도는 괜찮음 )
                    if (stage == -1 || stage == 7)
                    {
                        TextColor(DEEP_WHITE);
                        gotoxy(LEFTSPACE + j, UPSPACE + i, "◆");
                    }
                }
                break;
            case '2':
                // TextColor(BLUE);
                TextColor(DEEP_OC);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "●");
                break;
            case '3':
                TextColor(DEEP_YELLOW);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "★");
                break;
            case '5':
                TextColor(DEEP_OC);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "▦");
                break;
            case '6':
                TextColor(RED);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "●");
                break;
            case '9':
                TextColor(DEEP_OC);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "※");
                break;

            case '<':
            case '>':
            case '-':
            case '+':
                TextColor(DEEP_WHITE);
                switch (Maze[i][j])
                {
                case '+':
                case 'u':
                    gotoxy(LEFTSPACE + j, UPSPACE + i, "↑");
                    break;
                case '-':
                case 'd':
                    gotoxy(LEFTSPACE + j, UPSPACE + i, "↓");
                    break;
                case '>':
                case 'r':
                    gotoxy(LEFTSPACE + j, UPSPACE + i, "→");
                    break;
                case '<':
                case 'l':
                    gotoxy(LEFTSPACE + j, UPSPACE + i, "←");
                    break;
                }

                break;

            case '#':           // special NPC >
            case '@':           // special NPC <
            case '&':           // special NPC +
            case '=':           // special NPC -
                TextColor(DEEP_WHITE);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "⊙");

                break;

            case '*':
                TextColor(DEEP_OC);
                gotoxy(LEFTSPACE + j, UPSPACE + i, "δ");
                break;



            default: // 1 4 2가 입력된 경우에는 if문으로 처리해주기.    //  분신 PC 사용시 사용 가능@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                if (Maze[i][j] == '1' && tPlayerPosRED.x == j && tPlayerPosRED.y == i
                    || Maze[i][j] == '2' && tPlayerPosRED.x == j && tPlayerPosRED.y == i
                    || Maze[i][j] == '4' && tPlayerPosRED.x == j && tPlayerPosRED.y == i)
                {
                    TextColor(DEEP_RED);
                    gotoxy(LEFTSPACE + j, UPSPACE + i, "♥");
                }// 선처리 완료. 이후에 다른 것은 어떻게 ? ...
                if (Maze[i][j] == '1' || Maze[i][j] == '2')
                    gotoxy(LEFTSPACE + j, UPSPACE + i, " ");

                if (Maze[i][j] == '4')
                {
                    TextColor(DEEP_JAJU);
                    gotoxy(LEFTSPACE + j, UPSPACE + i, "δ");
                } // 1,2,4 부분은 if문 우선순위에 따라 처리하도록 한다.
            }
        }

    }
    if (num == 0) // 처음 set이후에 num == 1이다. 
        num++;
}
void CreateBomb()
{

    // FMOD
    //Fmod->update();

    int block = 0;
    int cnt = 0;

    if (BombItem == 0) // max = 5 아이템 갯수와 터트린 갯수가 같으면 터트릴수 없다!
        return;

    Sleep(50);
    // 폭탄위치와 겹치지 않거나 최대수를 넘지 않은 경우에는 현재 count에 맞도록 플레이어 위치 데입 

    tBombPos[iBombCount] = tPlayerPosBLUE;

    //Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '4';

    if (Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == '5')
    {
        PlaySound(TEXT("29.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
        cnt++;
    }

    if (Maze[tPlayerPosBLUE.y - 1][tPlayerPosBLUE.x] == '5')
    {
        cnt++;
        PlaySound(TEXT("29.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

    }
    if (Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x + 1] == '5')
    {
        PlaySound(TEXT("29.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

        cnt++;
    }

    if (Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x - 1] == '5')
    {
        PlaySound(TEXT("29.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
        cnt++;
    }


    if (cnt > 0) {

        if (tmp == 0) {

            BombFirePos[0][iBombCount] = tPlayerPosBLUE;   //   0일 때 기준으로 저장

            BombFirePos[1][iBombCount].x = BombFirePos[0][iBombCount].y;
            BombFirePos[1][iBombCount].y = 30 - BombFirePos[0][iBombCount].x - 1;

            BombFirePos[2][iBombCount].x = BombFirePos[1][iBombCount].y;
            BombFirePos[2][iBombCount].y = 30 - BombFirePos[1][iBombCount].x - 1;

            BombFirePos[3][iBombCount].x = BombFirePos[2][iBombCount].y;
            BombFirePos[3][iBombCount].y = 30 - BombFirePos[2][iBombCount].x - 1;

        }
        else if (tmp == 1) {


            BombFirePos[1][iBombCount] = tPlayerPosBLUE;   //   0일 때 기준으로 저장

            BombFirePos[2][iBombCount].x = BombFirePos[1][iBombCount].y;
            BombFirePos[2][iBombCount].y = 30 - BombFirePos[1][iBombCount].x - 1;

            BombFirePos[3][iBombCount].x = BombFirePos[2][iBombCount].y;
            BombFirePos[3][iBombCount].y = 30 - BombFirePos[2][iBombCount].x - 1;

            BombFirePos[0][iBombCount].x = BombFirePos[3][iBombCount].y;
            BombFirePos[0][iBombCount].y = 30 - BombFirePos[3][iBombCount].x - 1;



        }
        else if (tmp == 2) {


            BombFirePos[2][iBombCount] = tPlayerPosBLUE;   //   0일 때 기준으로 저장

            BombFirePos[3][iBombCount].x = BombFirePos[2][iBombCount].y;
            BombFirePos[3][iBombCount].y = 30 - BombFirePos[2][iBombCount].x - 1;

            BombFirePos[0][iBombCount].x = BombFirePos[3][iBombCount].y;
            BombFirePos[0][iBombCount].y = 30 - BombFirePos[3][iBombCount].x - 1;

            BombFirePos[1][iBombCount].x = BombFirePos[0][iBombCount].y;
            BombFirePos[1][iBombCount].y = 30 - BombFirePos[0][iBombCount].x - 1;



        }
        else if (tmp == 3) {

            BombFirePos[3][iBombCount] = tPlayerPosBLUE;   //   0일 때 기준으로 저장

            BombFirePos[0][iBombCount].x = BombFirePos[3][iBombCount].y;
            BombFirePos[0][iBombCount].y = 30 - BombFirePos[3][iBombCount].x - 1;

            BombFirePos[1][iBombCount].x = BombFirePos[0][iBombCount].y;
            BombFirePos[1][iBombCount].y = 30 - BombFirePos[0][iBombCount].x - 1;

            BombFirePos[2][iBombCount].x = BombFirePos[1][iBombCount].y;
            BombFirePos[2][iBombCount].y = 30 - BombFirePos[1][iBombCount].x - 1;

        }
        BombItem--;
        ++iBombCount;
        gotoxy(MAP + 16, 16, "");

        TextColor(DEEP_OC);
        if (BombItem == 0) {
            printf("           ");
        }
        else if (BombItem == 1) {
            printf("δ           ");
        }
        else if (BombItem == 2) {
            printf("δδ         ");
        }
        else if (BombItem == 3) {
            printf("δδδ       ");
        }
        else if (BombItem == 4) {
            printf("δδδδ     ");
        }
        else if (BombItem == 5) {
            printf("δδδδδ   ");
        }
        TextColor(WHITE);

    }

    // FMOD
    //Fmod->update();


}

void gravityRED()
{
    if (Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == '3')
    {
        Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '1';
        ++tPlayerPosBLUE.y;
        Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '6';

        RotateCount = 0;
        tmp = 0;
        SetMaze();

        return;
    }

    if (Maze[tPlayerPosRED.y + 1][tPlayerPosRED.x] == '0'
        || Maze[tPlayerPosRED.y + 1][tPlayerPosRED.x] == 'w'
        || Maze[tPlayerPosRED.y + 1][tPlayerPosRED.x] == 'e'
        || Maze[tPlayerPosRED.y + 1][tPlayerPosRED.x] == '5')
        return;

    Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '1';
    ++tPlayerPosRED.y;
    Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '6';

    //출력
    OutPut();

    //Sleep(arrowSpeed);
    Sleep(1);
}
void gravityBLUE()
{
    for (int i = 0; i < BombLimit; i++)
    {
        if (BombItemPos[i].x == tPlayerPosBLUE.x && tPlayerPosBLUE.y == BombItemPos[i].y) {
            if (Bombflag[i] == 0)
            {
                Bombflag[i] = 1;    // idx 0번의 폭탄을 먹었습니다 flag
                BombItem++;
                PlaySound(TEXT("아이템.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                BombPause++;
            }
        }
    }

    if (tPlayerPosBLUE.x == tPlayerPosRED.x && (tPlayerPosBLUE.y + 1 == tPlayerPosRED.y || tPlayerPosBLUE.y == tPlayerPosRED.y))
    {
        death--;
        PlaySound(TEXT("death.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
        if (death == 0)
        {
            if (stage == 0)
            {
                death = 3;
            }
            // GameOver!
            else
                stage = 5;
        }
        RotateCount = 0;
        tmp = 0;
        idx = 0;
        SetMaze();
    }


    if (Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == '0'
        || Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == 'w'
        || Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == 'e'
        || Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == '5'
        || Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == 'G')
        return;

    Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '1';
    ++tPlayerPosBLUE.y;
    Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '2';

    if (Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == '3')
    {
        if (stage != 0)
        {
            stagePlus();
            SetMaze();
            OutPut();
        }
    }

    //출력
    OutPut();
    Sleep(arrowSpeed);
}
void MovePlayer(char cInput)
{
    npcItem = npcPauseLimit - inpcCount;
    switch (cInput)
    {
    case 'n':
    case 'N':
        if (npcItem == 0) // max = 5 아이템 갯수와 터트린 갯수가 같으면 터트릴수 없다!
            break;

        npcPause++;
        inpcCount++;
        gotoxy(MAP + 16, 18, ""); printf(" %d", npcItem);
        break;

    case LEFT:
        PlaySound(TEXT("click1.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

        ++RotateCount;
        idx++;

        if (idx < 0)
        {
            tmp = abs(idx);
            tmp = tmp % 4;
            tmp = (4 - tmp) % 4;
        }
        else  tmp = idx % 4;

        Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '1';
        Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '1';

        for (int i = 0; i < BombLimit; i++)
            Maze[BombItemPos[i].y][BombItemPos[i].x] = '1';

        SetMaze();
        RotateLEFT();   // 인덱스 갱신
        break;

    case RIGHT:
        PlaySound(TEXT("click1.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

        ++RotateCount;
        --idx;

        if (idx < 0)
        {
            tmp = abs(idx);
            tmp = tmp % 4;
            tmp = (4 - tmp) % 4;
        }
        else  tmp = idx % 4;

        Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '1';
        Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '1';


        for (int i = 0; i < BombLimit; i++)
            Maze[BombItemPos[i].y][BombItemPos[i].x] = '1';

        SetMaze();
        RotateRIGHT();
        break;
    }
}
//void ResetNPC() {
//
//    inpcCount = 0;
//    if (stage == 1) {
//        npcLimit = 2;
//    }
//    else if (stage == 2) {
//        npcLimit = 2;
//    }
//    else if (stage == 3) {
//        npcLimit = 3;
//    }
//}
void arrowMove()
{
    if (npcPause > 0)
    {
        npcPause++;
        if (npcPause > 40) {
            gotoxy(12, 1, "                          "); printf("                             ");
            npcPause = 0;
        }
        return;
    }

    for (int i = 0; i < arrowCount; i++)
    {
        if ((arrowPos[i].x == tPlayerPosBLUE.x && arrowPos[i].y == tPlayerPosBLUE.y) // arrow위치와 player위치가 같으면 시작 위치로. 
            || (arrowPos[i].x == tPlayerPosBLUE.x && arrowPos[i].y == tPlayerPosBLUE.y))
        {
            death--;
            PlaySound(TEXT("death.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
            if (death == 0)
            {
                if (stage == 0)
                {
                    death = 3;
                }
                // GameOver!
                else
                    stage = 5;
            }
            ResetBomb();
            RotateCount = 0;
            tmp = 0;
            idx = 0;
            SetMaze();
            continue;
        }

        if (Maze[arrowPos[i].y][arrowPos[i].x] == '0') continue;

        switch (arrowVector[i])
        {
        case '>':
            Maze[arrowPos[i].y][arrowPos[i].x] = '1';
            arrowPos[i].x++;

            if (Maze[arrowPos[i].y][arrowPos[i].x] == 'e')
                arrowPos[i] = arrowStart[i];
            if (Maze[arrowPos[i].y][arrowPos[i].x] == '6')
                arrowPos[i].x--;

            Maze[arrowPos[i].y][arrowPos[i].x] = '>';
            break;


        case '<':
            Maze[arrowPos[i].y][arrowPos[i].x] = '1';
            --arrowPos[i].x;

            if (Maze[arrowPos[i].y][arrowPos[i].x] == 'e')
                arrowPos[i] = arrowStart[i];
            if (Maze[arrowPos[i].y][arrowPos[i].x] == '6')
                arrowPos[i].x++;
            Maze[arrowPos[i].y][arrowPos[i].x] = '<';
            break;

        case '-':
            Maze[arrowPos[i].y][arrowPos[i].x] = '1';
            ++arrowPos[i].y;

            if (Maze[arrowPos[i].y][arrowPos[i].x] == 'e')
                arrowPos[i] = arrowStart[i];
            if (Maze[arrowPos[i].y][arrowPos[i].x] == '6')
                arrowPos[i].y--;
            Maze[arrowPos[i].y][arrowPos[i].x] = '-';
            break;

        case '+':
            Maze[arrowPos[i].y][arrowPos[i].x] = '1';
            --arrowPos[i].y;

            if (Maze[arrowPos[i].y][arrowPos[i].x] == 'e')
                arrowPos[i] = arrowStart[i];
            if (Maze[arrowPos[i].y][arrowPos[i].x] == '6')
                arrowPos[i].y++;
            Maze[arrowPos[i].y][arrowPos[i].x] = '+';

            break;
        }

        Sleep(arrowSpeed);
    }
}

void specialArrowMove() {

    /*화살표NPC가 정지하면 그만큼 스페셜NPC의 속도가 빨라져서 여기서 속도 줄임*/

    if (npcPause > 0)
    {
        SpecialarrowSpeed = arrowCount * arrowSpeed;
    }
    else
        SpecialarrowSpeed = arrowSpeed;

    for (int i = 0; i < SpecialarrowCount; i++)
    {
        if (SpecialarrowPos[i].x == tPlayerPosRED.x && SpecialarrowPos[i].y == tPlayerPosRED.y) // arrow위치와 player위치가 같으면 시작 위치로. 
        {
            continue;
        }

        if (SpecialarrowPos[i].x == tPlayerPosBLUE.x && SpecialarrowPos[i].y == tPlayerPosBLUE.y) // arrow위치와 player위치가 같으면 시작 위치로. 
        {
            death--;
            PlaySound(TEXT("death.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
            if (death == 0)
            {
                if (stage == 0)
                    death = 3;
                // GameOver!
                else stage = 5;
            }
            ResetBomb();
            RotateCount = 0;
            tmp = 0;
            idx = 0;
            SetMaze();
            continue;
        }

        if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == '0') continue;

        switch (SpecialarrowVector[i]) {
        case '#':
            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '1';
            SpecialarrowPos[i].x++;

            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == 'e')
                SpecialarrowPos[i] = SpecialarrowStart[i];
            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == '6')
                SpecialarrowPos[i].x--;

            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '#';
            break;
        case '@':
            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '1';
            --SpecialarrowPos[i].x;

            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == 'e')
                SpecialarrowPos[i] = SpecialarrowStart[i];
            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == '6')
                SpecialarrowPos[i].x++;
            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '@';
            break;

        case'&':
            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '1';
            --SpecialarrowPos[i].y;

            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == 'e')
                SpecialarrowPos[i] = SpecialarrowStart[i];
            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == '6')
                SpecialarrowPos[i].y++;
            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '&';

            break;

        case '=':
            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '1';
            ++SpecialarrowPos[i].y;

            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == 'e')
                SpecialarrowPos[i] = SpecialarrowStart[i];
            if (Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] == '6')
                SpecialarrowPos[i].y--;
            Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = '=';
            break;


        }
        Sleep(SpecialarrowSpeed);
    }
}
void stagePlus()
{
    PlaySound(TEXT("클리어.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

    GameClearScore[0][stage] = (int)(((float)t) / CLOCKS_PER_SEC);
    GameClearScore[1][stage] = death;
    GameClearScore[2][stage] = RotateCount;


    RotateCount = 0;
    tmp = 0;
    idx = 0;

    ResetBomb();
    ResetTime();
    storycnt = 0;
    stage++;
}

void title() // 타이틀은 stage = -1으로 그림 그리기 위한 설정이다.
{
    PlaySound(TEXT("언더테일.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

    for (int i = 0; i < arrowCount; i++)
        arrowPos[i] = { 0,0 };
    for (int i = 0; i < SpecialarrowCount; i++)
        SpecialarrowPos[i] = { 0,0 };


    while (_kbhit()) _getch();

    titlePicture();
    num = 0;
    stage = -1;
    death = 3;

    TextColor(DEEP_WHITE);
    gotoxy(MAP + 5, 9, "＃─────────────＃");
    gotoxy(MAP + 5, 11, "＃─────────────＃");


    TextColor(DEEP_YELLOW);
    gotoxy(MAP + 5, 10, "♥     알약을 탈출시켜라   ♥");
    TextColor(DEEP_WHITE);
    gotoxy(MAP + 5, 14, "        TUTORIAL       ");
    // 처음 셋팅시 새로운 게임을 잡고 있음.

    gotoxy(MAP + 5, 16, "        GAME START       ");
    gotoxy(MAP + 5, 18, "        END GAME       ");

    TextColor(WHITE);
    gotoxy(MAP + 5, 22, "☞  press ENTER to select  ☜");
    gotoxy(MAP + 5, 24, "   〔  ↑  : up     〕   ");
    gotoxy(MAP + 5, 25, "   〔  ↓ : down    〕    ");

    // ICON 의 위치는 바뀌지 않는다.
    titleIcon();

    int locate = 0; // 맨위 무엇을 눌렀는지, 어디 위치 였는지. // 여기서 사용하는 locate와 select에서 사용하는 locate는 다르다.
    char str[40];// 비밀번호와 같은지에 사용할 배열. 너무 쪼잔하게 주면 문자를 많이 입력할 때 재미없어짐.

    TextColor(DEEP_RED);
    gotoxy(MAP + 5, 14, "  ▶");
    // 사실상 처음에는 입력이 없다면 세모가 뜨지 않으므로 미리 잡아둬야 한다.

    //우리는 title에서 while문으로 키보드 입
    // 이후에 누른 것 따로 처리 ..
    OutPut();

    while (_kbhit()) _getch(); // 버퍼.

    while (true)
    {
        if (_kbhit())
        {
            char input = _getch();
            while (_kbhit()) input = _getch();
            switch (input)
            {
            case UP:
                titleIcon();
                switch (locate) // 현재의 상태 4에서 up -> 3으로 이동할 예정임으로 3번위치에 찍어야 함. (이후에 --처리)
                {
                case 0:
                    TextColor(DEEP_RED);
                    gotoxy(MAP + 5, 14, "  ▶"); //더 이상 올라갈 곳이 없으므로 자신의 자리.
                    break;
                case 1:
                    TextColor(DEEP_RED);
                    gotoxy(MAP + 5, 14, "  ▶");
                    break;
                case 2:
                    TextColor(DEEP_RED);
                    gotoxy(MAP + 5, 16, "  ▶");
                    break;
                }
                locate--;
                if (locate == -1)
                    locate = 0; // 맨 위(0)에서 위를 누른 경우. 
                 //위로 이동
                break;
            case DOWN:
                titleIcon();
                switch (locate) // 현재의 상태 0에서 아래로 이동 1의 위치를 찍고 이후에 ++ 처리
                {
                case 0:
                    TextColor(DEEP_RED);
                    gotoxy(MAP + 5, 16, "  ▶");
                    break;
                case 1:
                    TextColor(DEEP_RED);
                    gotoxy(MAP + 5, 18, "  ▶");
                    break;
                case 2:
                    TextColor(DEEP_RED);
                    gotoxy(MAP + 5, 18, "  ▶");
                    break;
                }
                locate++;
                if (locate == 3)
                    locate = 2; // 맨 아래에서 아래를 누름. 
                break;
            case SPACEBAR:
            case ENTER:
                switch (locate)
                {
                case 0: // 시작 하기 (기본 셋팅 대로)
                    locate = 5;
                    stage = 0;
                    break;

                case 1: //튜토리얼 
                    locate = 5;
                    stage = 1; // 이후에 튜토리얼 스테이지에서 클리어시 1stage로 이동. 타이틀은 case -1:
                    break;
                case 2:// 종료
                    exit(0);
                    break;
                }
                while (_kbhit()) _getch(); // 버퍼. enter이후에 누른 값을 일단 무시. ( 무시하지 않으면 이상하게 됨)
                break;
            case ESC:
                exit(0);
                break;
            }
        }
        if (locate == 5)
        {
            while (_kbhit()) _getch(); // 버퍼. 

            return;
        }// 이후에 main -> set 1스테이지부터 . . .  // 튜토리얼 시작
    }//while true
}
void titleIcon()
{
    TextColor(DEEP_WHITE);

    gotoxy(MAP + 5, 14, "  ♥");
    gotoxy(MAP + 5, 16, "  ♣");
    gotoxy(MAP + 5, 18, "  §");

}
void titlePicture()
{
    strcpy_s(Maze[0], 31, "  0000000000000000000000000000");
    strcpy_s(Maze[1], 31, " 0                           0");
    strcpy_s(Maze[2], 31, "0 ttt t t ttt                0");      // THE VACCINE
    strcpy_s(Maze[3], 31, "0  t  t t t                  0");
    strcpy_s(Maze[4], 31, "0  t  ttt ttt                0");
    strcpy_s(Maze[5], 31, "0  t  t t t       BBBB       0");
    strcpy_s(Maze[6], 31, "0  t  t t ttt    BGBBBB      0");
    strcpy_s(Maze[7], 31, "0               BGBBBBB      0");
    strcpy_s(Maze[8], 31, "0              BGBBBBBB      0");
    strcpy_s(Maze[9], 31, "0             BGBBBBBBB      0");
    strcpy_s(Maze[10], 31, "0            BGBBBBBBB       0");
    strcpy_s(Maze[11], 31, "0           RBBBBBBBB        0");
    strcpy_s(Maze[12], 31, "0          RRRBBBBBB         0");
    strcpy_s(Maze[13], 31, "0         RGRRRBBBB          0");
    strcpy_s(Maze[14], 31, "0        RGRRRRRBB           0");
    strcpy_s(Maze[15], 31, "0       RGRRRRRRB            0");
    strcpy_s(Maze[16], 31, "0      RGRRRRRRR             0");
    strcpy_s(Maze[17], 31, "0      RGRRRRRR              0");
    strcpy_s(Maze[18], 31, "0      RRRRRRR               0");
    strcpy_s(Maze[19], 31, "0      RRRRRR                0");
    strcpy_s(Maze[20], 31, "0       RRRR                 0");
    strcpy_s(Maze[21], 31, "0                            0");
    strcpy_s(Maze[22], 31, "0 t t  t   tt  tt t t  t ttt 0");
    strcpy_s(Maze[23], 31, "0 t t t t t   t   t tt t t   0");
    strcpy_s(Maze[24], 31, "0 t t ttt t   t   t tt t ttt 0");
    strcpy_s(Maze[25], 31, "0 t t t t t   t   t t tt t   0");
    strcpy_s(Maze[26], 31, "0 t t t t t   t   t t tt t   0");
    strcpy_s(Maze[27], 31, "0  t  t t  tt  tt t t  t ttt 0");
    strcpy_s(Maze[28], 31, "0                           0 ");
    strcpy_s(Maze[29], 31, "0000000000000000000000000000  ");
}
void cheat()
{
    stagePlus();
    SetMaze();
    OutPut();
}
void playerInform()
{
    finish = clock();
    t = finish - start;

    gotoxy(30 + 10, 26, ""); printf("  %.f   ", TimeLimit - (((float)t) / CLOCKS_PER_SEC));


    if ((TimeLimit - (((float)t) / CLOCKS_PER_SEC) <= 1)) {

        system("cls");
        printTimeOver();

        BombPause = 0;
        iBombCount;

        tmp = 0;
        idx = 0;
        RotateCount = 0;

        stage = -1; // title
        SetMaze();  // title함수로 들어감 => 메뉴선택통해 stage값 변경

        SetMaze(); // 바뀐 stage값을 이용하여 다시 셋팅.
        OutPut();

        char cInput = NULL;
        while (_kbhit()) _getch();



        return;
    }



    gotoxy(MAP + 14, 16, "");

    TextColor(DEEP_OC);
    if (BombItem == 0) {
        printf("           ");
    }
    else if (BombItem == 1) {
        printf("δ           ");
    }
    else if (BombItem == 2) {
        printf("δδ         ");
    }
    else if (BombItem == 3) {
        printf("δδδ       ");
    }
    else if (BombItem == 4) {
        printf("δδδδ     ");
    }
    else if (BombItem == 5) {
        printf("δδδδδ   ");
    }
    TextColor(WHITE);

    //TextColor(DEEP_JAJU);
    //gotoxy(MAP + 7, 24, "회전 횟수  : "); printf("%d", RotateCount);
    //gotoxy(MAP + 16, 18, ""); printf(" %d", npcPauseLimit - inpcCount);

    gotoxy(MAP + 14, 18, "");

    TextColor(DEEP_YELLOW);
    if (npcPauseLimit - inpcCount == 0) {
        printf("           ");
    }
    else if (npcPauseLimit - inpcCount == 1) {
        printf("Ø          ");
    }
    else if (npcPauseLimit - inpcCount == 2) {
        printf("ØØ         ");
    }
    else if (npcPauseLimit - inpcCount == 3) {
        printf("ØØØ       ");
    }
    else if (npcPauseLimit - inpcCount == 4) {
        printf("ØØØØ     ");
    }
    else if (npcPauseLimit - inpcCount == 5) {
        printf("ØØØØØ   ");
    }
    TextColor(WHITE);

    gotoxy(MAP + 14, 24, ""); printf("%d", RotateCount);

    gotoxy(MAP + 7, 28, "LIFE : ");
    if (death == 3) {
        gotoxy(MAP + 11, 28, "♥  ♥  ♥");
    }
    else if (death == 2) {
        gotoxy(MAP + 11, 28, "♥  ♥      ");
    }
    else if (death == 1) {
        gotoxy(MAP + 11, 28, "♥          ");
    }

    if (BombPause > 0)
    {
        if (BombPause < 20)
            gotoxy(12, 1, "δ 폭탄을 획득하셨습니다! δ");
        BombPause++;
        if (BombPause >= 20)
        {
            gotoxy(12, 1, "                             ");
            BombPause = 0;
        }
    }

}
void tutorial(char input)
{
    switch (quest)
    {
    case 0: //0. Backspace를 눌러보아요 0일때 한번 출력하게 하구. 이후에 미션 성공시에 넘어가주는 식.
        if (number == 0)
        {
            gotoxy_C(5, 1, "미로판을 회전시키며 파란약을 탈출시켜야 합니다", DEEP_WHITE); number = 1; //number =1 number를 통해서 한번만 출력해주도록 설정.
            Sleep(600);
            gotoxy(5, 1, "                                                ");

            gotoxy_C(5, 1, "타임이 초과되거나 ♥(life)가 모두 사라지면           ", DEEP_WHITE); number = 1; //number =1 number를 통해서 한번만 출력해주도록 설정.
            Sleep(600);
            gotoxy(5, 1, "                                                ");

            gotoxy_C(5, 1, "GAME OVER 입니다!                              ", DEEP_WHITE); number = 1; //number =1 number를 통해서 한번만 출력해주도록 설정.
            Sleep(600);
            gotoxy(5, 1, "                                                ");

            gotoxy_C(5, 1, "좌/우 방향키를 눌러서 미로를 회전시키세요.", DEEP_WHITE); number = 1; //number =1 number를 통해서 한번만 출력해주도록 설정.
            Sleep(600);
            gotoxy(5, 1, "                                                ");

            gotoxy_C(5, 1, "미로가 회전함에 따라서", DEEP_WHITE); number = 1; //number =1 number를 통해서 한번만 출력해주도록 설정.
            Sleep(600);
            gotoxy(5, 1, "                                                                  ");

            gotoxy_C(5, 1, "파란약과 빨간약이 중력의 방향으로 떨어집니다.", DEEP_WHITE); number = 1;
            Sleep(600);
            gotoxy(5, 1, "                                                ");


            while (_kbhit()) _getch(); // 버퍼. 
            number = 1;
        }
        if (tPlayerPosBLUE.x == 13 && tPlayerPosBLUE.y == 4)
        {
            quest++;
            number = 0;
        }
        break;
    case 1: // 폭탄을 획득하세요
        if (number == 0)
        {
            gotoxy_C(5, 1, "폭탄을 획득하세요                    ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                             ");

            gotoxy_C(5, 1, "오른쪽에 획득한 폭탄이 표시됩니다           ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                ");

            number = 1;
        }
        if (tPlayerPosBLUE.x == 28 && tPlayerPosBLUE.y == 19)
        {
            quest++;
            number = 0;
        }
        break;
    case 2://2. 화살에 맞으면 죽어요.
        if (number == 0)
        {
            gotoxy_C(5, 1, "N 키를 누르면 화살표 NPC(->)가 정지해요.                     ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                            ");
            //while (_kbhit()) _getch(); // 버퍼. 

            gotoxy_C(5, 1, "하지만 ⊙ 동글뱅이 NPC는 정지하지 않습니다                  ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                             ");

            gotoxy_C(5, 1, "화살표 NPC가 정지한 동안, 동글뱅이 NPC 를 피해 지나가세요.  ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                                ");

            gotoxy_C(5, 1, "NPC에 닿으면 ♥가 사라지고 게임이 초기화됩니다      ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                    ");

            gotoxy_C(5, 1, "튜토리얼에서는 ♥가 무한으로 주어집니다               ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                      ");

            number = 1;
        }
        if (tPlayerPosBLUE.y == 10 && (tPlayerPosBLUE.x == 19 || tPlayerPosBLUE.x == 20))
        {
            quest++;
            number = 0;
        }
        break;

    case 3:
        //3. 스페이스바를 통해 폭탄을 설치
        if (number == 0)
        {

            number = 1;
            gotoxy_C(5, 1, "빨간약을 만나지 않게 미로를 회전시키세요.              ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                            ");

            gotoxy_C(5, 1, "빨간약과 만나면 ♥가 하나 사라지고 게임이 초기화됩니다.", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                           ");

            while (_kbhit()) _getch(); // 버퍼. 

        }
        if (tPlayerPosBLUE.x == 18 && tPlayerPosBLUE.y == 15)
        {
            quest++;
            number = 0;
        }
        break;
    case 4:
        if (number == 0)
        {
            number = 1;
            gotoxy_C(5, 1, "미로 회전을 해서 탈출구가 있는 쪽으로 이동하세요.", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                    ");
        }
        if (tPlayerPosBLUE.y == 22 && tPlayerPosBLUE.x == 15)
        {
            quest++;
            number = 0;
        }
        break;
    case 5:
        if (number == 0)
        {
            number = 1;
            gotoxy_C(5, 1, "스페이스 바를 누르면 폭탄을 사용할 수 있습니다. ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                     ");
            gotoxy_C(5, 1, "폭탄은 상하좌우, 4가지 방향으로 터집니다. ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                     ");

        }
        if (Maze[tPlayerPosBLUE.y + 1][tPlayerPosBLUE.x] == '3')
        {
            quest++;
            number = 0;
        }
        break;
    case 6:
        if (number == 0)
        {
            system("cls");
            gotoxy_C(15, 15, "튜토리얼 탈출에 성공하셨습니다.    ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                     ");

            gotoxy_C(15, 15, "이제 게임을 시작해 볼까요 ☞☞☞☞ ", DEEP_WHITE);
            Sleep(600);
            gotoxy(5, 1, "                                                     ");

            Sleep(1000);

            stagePlus();
            SetMaze();
            OutPut();
        }

    }

}
void RotateLEFT()
{
    int tmp_y;
    int tmp_x;

    /* 빨간약, 파란약 좌표 Rotate*/
    tmp_x = tPlayerPosRED.y;  // j
    tmp_y = 30 - tPlayerPosRED.x - 1;  // i
    tPlayerPosRED.y = tmp_y;
    tPlayerPosRED.x = tmp_x;

    tmp_x = tPlayerPosBLUE.y;
    tmp_y = 30 - tPlayerPosBLUE.x - 1;
    tPlayerPosBLUE.y = tmp_y;
    tPlayerPosBLUE.x = tmp_x;



    /* 폭탄 좌표 Rotate*/
    for (int i = 0; i < BombLimit; i++)
    {
        tmp_x = BombItemPos[i].y;
        tmp_y = 30 - BombItemPos[i].x - 1;
        BombItemPos[i].y = tmp_y;
        BombItemPos[i].x = tmp_x;
        if (Bombflag[i] == 1)   // 폭탄을 먹은 상태
            Maze[BombItemPos[i].y][BombItemPos[i].x] = '1';
        else
            Maze[BombItemPos[i].y][BombItemPos[i].x] = '*';
    }

    /* 화살표 NPC Rotate*/
    for (int i = 0; i < arrowCount; i++)
    {
        switch (arrowVector[i])
        {
        case '>':
            arrowVector[i] = '+';
            break;
        case '-':
            arrowVector[i] = '>';
            break;
        case '<':
            arrowVector[i] = '-';
            break;
        case '+':
            arrowVector[i] = '<';
            break;
        }
        tmp_x = arrowStart[i].y;
        tmp_y = 30 - arrowStart[i].x - 1;
        arrowStart[i].y = tmp_y;
        arrowStart[i].x = tmp_x;
        tmp_x = arrowPos[i].y;
        tmp_y = 30 - arrowPos[i].x - 1;
        arrowPos[i].y = tmp_y;
        arrowPos[i].x = tmp_x;
        Maze[arrowPos[i].y][arrowPos[i].x] = arrowVector[i];
    }

    /* 동그라미  NPC Rotate*/
    for (int i = 0; i < SpecialarrowCount; i++)
    {
        switch (SpecialarrowVector[i])
        {
        case '#':
            SpecialarrowVector[i] = '&';
            break;
        case '=':
            SpecialarrowVector[i] = '#';
            break;
        case '@':
            SpecialarrowVector[i] = '=';
            break;
        case '&':
            SpecialarrowVector[i] = '@';
            break;
        }
        tmp_x = SpecialarrowStart[i].y;
        tmp_y = 30 - SpecialarrowStart[i].x - 1;
        SpecialarrowStart[i].y = tmp_y;
        SpecialarrowStart[i].x = tmp_x;
        tmp_x = SpecialarrowPos[i].y;
        tmp_y = 30 - SpecialarrowPos[i].x - 1;
        SpecialarrowPos[i].y = tmp_y;
        SpecialarrowPos[i].x = tmp_x;
        Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = SpecialarrowVector[i];
    }

    Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '6';
    Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '2';
    OutPut();
}
void RotateRIGHT()
{
    int tmp_y;
    int tmp_x;

    /* 빨간약, 파란약 좌표 Rotate*/
    tmp_y = tPlayerPosRED.x;  // j
    tmp_x = 30 - tPlayerPosRED.y - 1;  // i
    tPlayerPosRED.y = tmp_y;
    tPlayerPosRED.x = tmp_x;

    tmp_y = tPlayerPosBLUE.x;
    tmp_x = 30 - tPlayerPosBLUE.y - 1;
    tPlayerPosBLUE.y = tmp_y;
    tPlayerPosBLUE.x = tmp_x;


    /* 폭탄 좌표 Rotate*/
    for (int i = 0; i < BombLimit; i++)
    {
        tmp_y = BombItemPos[i].x;
        tmp_x = 30 - BombItemPos[i].y - 1;
        BombItemPos[i].y = tmp_y;
        BombItemPos[i].x = tmp_x;
        if (Bombflag[i] == 1)   // 폭탄을 먹은 상태
            Maze[BombItemPos[i].y][BombItemPos[i].x] = '1';
        else
            Maze[BombItemPos[i].y][BombItemPos[i].x] = '*';
    }
    /* 화살표 NPC Rotate*/
    for (int i = 0; i < arrowCount; i++)
    {
        switch (arrowVector[i])
        {
        case '>':
            arrowVector[i] = '-';
            break;
        case '-':
            arrowVector[i] = '<';
            break;
        case '<':
            arrowVector[i] = '+';
            break;
        case '+':
            arrowVector[i] = '>';
            break;
        }
        tmp_y = arrowStart[i].x;
        tmp_x = 30 - arrowStart[i].y - 1;
        arrowStart[i].y = tmp_y;
        arrowStart[i].x = tmp_x;
        tmp_y = arrowPos[i].x;
        tmp_x = 30 - arrowPos[i].y - 1;
        arrowPos[i].y = tmp_y;
        arrowPos[i].x = tmp_x;
        Maze[arrowPos[i].y][arrowPos[i].x] = arrowVector[i];
    }
    /* 동그라미  NPC Rotate*/
    for (int i = 0; i < SpecialarrowCount; i++)
    {
        switch (SpecialarrowVector[i])
        {
        case '#':
            SpecialarrowVector[i] = '=';
            break;
        case '=':
            SpecialarrowVector[i] = '@';
            break;
        case '@':
            SpecialarrowVector[i] = '&';
            break;
        case '&':
            SpecialarrowVector[i] = '#';
            break;
        }
        tmp_y = SpecialarrowStart[i].x;
        tmp_x = 30 - SpecialarrowStart[i].y - 1;
        SpecialarrowStart[i].y = tmp_y;
        SpecialarrowStart[i].x = tmp_x;
        tmp_y = SpecialarrowPos[i].x;
        tmp_x = 30 - SpecialarrowPos[i].y - 1;
        SpecialarrowPos[i].y = tmp_y;
        SpecialarrowPos[i].x = tmp_x;
        Maze[SpecialarrowPos[i].y][SpecialarrowPos[i].x] = SpecialarrowVector[i];
    }

    Maze[tPlayerPosRED.y][tPlayerPosRED.x] = '6';
    Maze[tPlayerPosBLUE.y][tPlayerPosBLUE.x] = '2';
    OutPut();
}
void DrawItemBoard() {
    int x, y;
    SetCurrentCursorPos(GITEM_ORIGIN_X, GITEM_ORIGIN_Y);

    for (y = 0; y <= GITEM_HEIGHT; y++) {
        SetCurrentCursorPos(GITEM_ORIGIN_X, GITEM_ORIGIN_Y + y);

        if (y == 0 || y == GITEM_HEIGHT) {
            if (y == 0) printf("┏");
            if (y == GITEM_HEIGHT) printf("┗");
        }
        else printf("┃");
    }
    for (y = 0; y <= GITEM_HEIGHT; y++) {
        SetCurrentCursorPos(GITEM_ORIGIN_X + (GITEM_WIDTH + 1) * 2, GITEM_ORIGIN_Y + y);

        if (y == 0 || y == GITEM_HEIGHT) {
            if (y == 0) printf("┓");
            if (y == GITEM_HEIGHT) printf("┛");
        }
        else printf("┃");
    }
    for (x = 1; x < GITEM_WIDTH; x++) {
        SetCurrentCursorPos(GITEM_ORIGIN_X + x * 2, GITEM_ORIGIN_Y + GITEM_HEIGHT);
        printf("━━");
    }
    for (x = 1; x < GITEM_WIDTH; x++) {
        SetCurrentCursorPos(GITEM_ORIGIN_X + x * 2, GITEM_ORIGIN_Y);
        printf("━━");
    }

    TextColor(DEEP_WHITE);
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 1);
    printf("            < 배경스토리 >");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 3);
    printf("병원에는 여러 지병이 있는 환자들이 \n");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 4);
    printf("입원중이었다. 어느날, 이 병원에 코로나");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 5);
    printf("바이러스가 들어오게 되면서 병원은\n");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 6);
    printf("비상체제로 돌입하게 된다.         ");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 8);
    printf("환자들의 지병과 코로나를 치료하기 위해서 ");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 9);
    printf("백신 파란약 "); TextColor(DEEP_BLUE); printf("●"); TextColor(DEEP_WHITE); printf("이 개발되었다");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 10);
    printf("하지만 실험도중 부작용을 동반하는 빨간약"); TextColor(DEEP_RED); printf("●"); TextColor(DEEP_WHITE);
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 11);
    printf("도 나타나고 말았다. 파란약"); TextColor(DEEP_BLUE); printf("●"); TextColor(DEEP_WHITE); printf("은");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 12);
    printf("빨간약"); TextColor(DEEP_RED); printf("●"); TextColor(DEEP_WHITE); printf("과 만나면 효력을 잃는다");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 14);
    printf("미로에서 빨간약"); TextColor(DEEP_RED); printf("●"); TextColor(DEEP_WHITE); printf("을 피해 파란약"); TextColor(DEEP_BLUE); printf("●"); TextColor(DEEP_WHITE); printf("만을");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 15);
    printf("탈출시켜 환자들에게 건강을 되찾아주자\n");

    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 20);
    printf("▶");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 19);
    printf("＃───────────────────＃");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 20);
    printf("★         Press Enter to START         ★");
    SetCurrentCursorPos(GITEM_ORIGIN_X + 3, GITEM_ORIGIN_Y + 21);
    printf("＃───────────────────＃");
}
void printHospital() {

    strcpy_s(Hospital[0], 31, "000000000000000000000000000");
    strcpy_s(Hospital[1], 31, "000000000022200000000000000");
    strcpy_s(Hospital[2], 31, "000000000022200000000000000");
    strcpy_s(Hospital[3], 31, "000000000022200000000000000");
    strcpy_s(Hospital[4], 31, "000000022222222200000000000");
    strcpy_s(Hospital[5], 31, "000000022222222200000000000");
    strcpy_s(Hospital[6], 31, "111111122222222211111110000");
    strcpy_s(Hospital[7], 31, "133333333322233333333310000");
    strcpy_s(Hospital[8], 31, "133333333322233333333310000");
    strcpy_s(Hospital[9], 31, "133333333322233333333310000");
    strcpy_s(Hospital[10], 31, "133333333333333333333310000");
    strcpy_s(Hospital[11], 31, "133111133111133111133310000");
    strcpy_s(Hospital[12], 31, "133100133100133100133310000");
    strcpy_s(Hospital[13], 31, "133100133100133100133310000");
    strcpy_s(Hospital[14], 31, "133111133111133111133310000");
    strcpy_s(Hospital[15], 31, "13333333333333333333331000");
    strcpy_s(Hospital[16], 31, "13333333331133333333331000");
    strcpy_s(Hospital[17], 31, "13311113311113311113331000");
    strcpy_s(Hospital[18], 31, "13310013331133310013331000");
    strcpy_s(Hospital[19], 31, "13310013111111310013331000");
    strcpy_s(Hospital[20], 31, "13311113100001311113331000");
    strcpy_s(Hospital[21], 31, "13333333100001333333331000");
    strcpy_s(Hospital[22], 31, "13333333100001333333331000");
    strcpy_s(Hospital[23], 31, "13333333100001333333331000");
    strcpy_s(Hospital[24], 31, "11111111111111111111111");
    strcpy_s(Hospital[25], 31, "000000000000000000000");

    for (int i = 0; i < 26; i++) //행 y
    {
        for (int j = 0; j < 31; j++) //열 x
        {
            switch (Hospital[i][j])
            {
            case '0':
                gotoxy(_X + j, _Y + i, " ");
                break;
            case '1':
                TextColor(DEEP_WHITE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '2':
                TextColor(DEEP_GREEN);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '3':
                TextColor(GRAY);
                TextColor(DEEP_BLUE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '4':
                TextColor(DEEP_RED);
                gotoxy(_X + j, _Y + i, "■");
                break;
            }
        }
    }
    DrawItemBoard();
    getchar();
}
void startStory()
{
    ScorePage();

    PlaySound(TEXT("키보드5.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    storycnt++;



    if (stage == 1)
    {


        system("cls");
        SetCurrentCursorPos(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 1);
        TextColor(DEEP_WHITE);
        printf(" ♥ MISSION :");
        TextColor(DEEP_WHITE);

        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 3, "감기에 걸린 할아버지에게 타미플루를 전달해야 한다!", DEEP_WHITE);
        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 5, "  치료를 위한 파란 알약을 전달해라!", DEEP_WHITE);
        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 7, " 단 빨간 알약은 부작용을 초래한다!", DEEP_WHITE);

        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 13, "Press Enter to skip!");

        _getch();
        system("cls");

    }
    else if (stage == 2)
    {


        system("cls");

        SetCurrentCursorPos(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 1);
        TextColor(DEEP_WHITE);
        printf(" ♥ MISSION :");
        TextColor(DEEP_WHITE);

        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 3, "치매에 걸린 할머니에게 치매 치료제를 전달해야 한다.", DEEP_WHITE);
        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 5, "할머니의 치매가 늦춰져서", DEEP_WHITE);
        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 7, "가족들과의 추억을 오래도록 기억할 수 있도록 도와주자.", DEEP_WHITE);

        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 13, "Press Enter to skip!");

        _getch();
        system("cls");
    }
    else if (stage == 3)
    {
        system("cls");

        SetCurrentCursorPos(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 1);
        TextColor(DEEP_WHITE);
        printf(" ♥ MISSION :");
        TextColor(DEEP_WHITE);



        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 3, "코로나에 걸린 알바생에게 코로나 치료제를 전달해야 한다.", DEEP_WHITE);
        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 5, "미로의 NPC를 잘 피하고 가진 아이템을 잘 활용해야 한다.", DEEP_WHITE);
        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 7, "알바생이 다시 알바를 시작해 알바비를 벌어", DEEP_WHITE);
        gotoxy_C(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 9, "일상으로 돌아갈 수 있게 도와주자.", DEEP_WHITE);

        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 13, "Press Enter to skip!");

        _getch();
        //Sleep(4000);

        system("cls");
    }
    else
        return;

}

void GameItemBoard()
{
    int x, y;

    TextColor(GRAY);
    SetCurrentCursorPos(GITEM_OVER_X, GITEM_OVER_Y + 4);
    printf(" ▶ PRESS ENTER TO RESTART GAME\n");
}

void ResetNPC() {
    inpcCount = 0;

    if (stage == 0) {
        /*NPC 초기위치 설정*/
        arrowCount = 2;     // 튜토리얼에서 화살표의 개수는 2개
        npcPauseLimit = 2; // 정지아이템 개수

        /*첫번째 화살표 위치와 방향*/
        arrowStart[0].x = 10;
        arrowStart[0].y = 7;
        arrowPos[0] = arrowStart[0];
        arrowVector[0] = '>';
        /*두번째 화살표 위치와 방향*/
        arrowStart[1].x = 10;
        arrowStart[1].y = 9;
        arrowPos[1] = arrowStart[1];
        arrowVector[1] = '>';

        /*special NPC 위치 설정*/
        SpecialarrowCount = 1;
        SpecialarrowStart[0].x = 10;
        SpecialarrowStart[0].y = 8;
        SpecialarrowPos[0] = SpecialarrowStart[0];
        SpecialarrowVector[0] = '#';

    }
    else if (stage == 1) {
        /*NPC 초기위치 설정*/
        arrowCount = 4;     // 1라운드에서 화살표의 개수는 2개
        npcPauseLimit = 1; // 정지아이템 개수

          /*첫번째 화살표 위치와 방향*/
        arrowStart[0].x = 4;
        arrowStart[0].y = 16;
        arrowPos[0] = arrowStart[0];
        arrowVector[0] = '>';

        /*두번째 화살표 위치와 방향*/
        arrowStart[1].x = 4;
        arrowStart[1].y = 17;
        arrowPos[1] = arrowStart[1];
        arrowVector[1] = '>';

        /*세번째 화살표 위치와 방향*/
        arrowStart[2].x = 15;
        arrowStart[2].y = 19;
        arrowPos[2] = arrowStart[2];
        arrowVector[2] = '<';
        /*네번째 화살표 위치와 방향*/
        arrowStart[3].x = 15;
        arrowStart[3].y = 20;
        arrowPos[3] = arrowStart[3];
        arrowVector[3] = '<';

        /*special NPC 위치 설정*/
        SpecialarrowCount = 0;

    }
    else if (stage == 2) {
        /*NPC 초기위치 설정*/
        arrowCount = 2;     // 2라운드에서 화살표의 개수는 3개
        npcPauseLimit = 2; // 정지아이템 개수

        /*첫번째 화살표 위치와 방향*/
        arrowStart[0].x = 11;
        arrowStart[0].y = 23;
        arrowPos[0] = arrowStart[0];
        arrowVector[0] = '+';
        /*두번째 화살표 위치와 방향*/
        arrowStart[1].x = 13;
        arrowStart[1].y = 16;
        arrowPos[1] = arrowStart[1];
        arrowVector[1] = '-';

        /*special NPC 위치 설정*/
        SpecialarrowCount = 4;

        SpecialarrowStart[0].x = 8;
        SpecialarrowStart[0].y = 27;
        SpecialarrowPos[0] = SpecialarrowStart[0];
        SpecialarrowVector[0] = '#';

        SpecialarrowStart[1].x = 3;
        SpecialarrowStart[1].y = 3;
        SpecialarrowPos[1] = SpecialarrowStart[1];
        SpecialarrowVector[1] = '#';

        SpecialarrowStart[2].x = 26;
        SpecialarrowStart[2].y = 11;
        SpecialarrowPos[2] = SpecialarrowStart[2];
        SpecialarrowVector[2] = '@';

        /*세번째 화살표 위치와 방향*/
        SpecialarrowStart[3].x = 17;
        SpecialarrowStart[3].y = 23;
        SpecialarrowPos[3] = SpecialarrowStart[3];
        SpecialarrowVector[3] = '&';
    }
    else if (stage == 3) {
        /*NPC 초기위치 설정*/
        arrowCount = 3;     // 2라운드에서 화살표의 개수는 3개
        npcPauseLimit = 3;   // 정지아이템 개수

        /*첫번째 화살표 위치와 방향*/
        arrowStart[0].x = 16;
        arrowStart[0].y = 2;
        arrowPos[0] = arrowStart[0];
        arrowVector[0] = '>';

        /*다섯번째 화살표 위치와 방향*/
        arrowStart[1].x = 26;
        arrowStart[1].y = 25;
        arrowPos[1] = arrowStart[1];
        arrowVector[1] = '-';
        /*여섯번째 화살표 위치와 방향*/
        arrowStart[2].x = 24;
        arrowStart[2].y = 28;
        arrowPos[2] = arrowStart[2];
        arrowVector[2] = '+';

        /*special NPC 위치 설정*/
        SpecialarrowCount = 3;

        /*두번째 화살표 위치와 방향*/
        SpecialarrowStart[0].x = 17;
        SpecialarrowStart[0].y = 12;
        SpecialarrowPos[0] = SpecialarrowStart[0];
        SpecialarrowVector[0] = '#';
        /*세번째 화살표 위치와 방향*/
        SpecialarrowStart[1].x = 22;
        SpecialarrowStart[1].y = 7;
        SpecialarrowPos[1] = SpecialarrowStart[1];
        SpecialarrowVector[1] = '@';
        /*네번째 화살표 위치와 방향*/
        SpecialarrowStart[2].x = 22;
        SpecialarrowStart[2].y = 17;
        SpecialarrowPos[2] = SpecialarrowStart[2];
        SpecialarrowVector[2] = '@';
    }
}
void ResetBomb() {

    iBombCount = 0; //쓴 폭탄

    for (int i = 0; i < 5; i++)
        Bombflag[i] = 0;

    if (stage == 0) {

        BombItemPos[0].x = 15;  //먹을 수 있는 폭탄 아이템의 위치
        BombItemPos[0].y = 4;
        BombItemPos[1].x = 12;
        BombItemPos[1].y = 17;
        BombItemPos[2].x = 20;
        BombItemPos[2].y = 19;

        BombItem = 0;   // 획득한 폭탄 갯수
        BombLimit = 3;  //주어지는 폭탄 갯수
    }
    else if (stage == 1) {
        BombItemPos[0].x = 26;  //먹을 수 있는 폭탄 아이템의 위치
        BombItemPos[0].y = 1;
        BombItemPos[1].x = 11;
        BombItemPos[1].y = 9;
        BombItemPos[2].x = 10;
        BombItemPos[2].y = 18;
        BombItem = 0;           //획득한 폭탄 갯수
        BombLimit = 3;          //주어지는 폭탄 갯수
    }
    else if (stage == 2) {
        BombItemPos[0].x = 28;  //먹을 수 있는 폭탄 아이템의 위치
        BombItemPos[0].y = 3;
        BombItemPos[1].x = 26;
        BombItemPos[1].y = 12;
        BombItemPos[2].x = 5;
        BombItemPos[2].y = 26;
        BombItemPos[3].x = 14;
        BombItemPos[3].y = 16;
        BombItem = 0;   // 획득한 폭탄 갯수
        BombLimit = 4;  //주어지는 폭탄 갯수
    }
    else if (stage == 3) {
        BombItemPos[0].x = 4;  //먹을 수 있는 폭탄 아이템의 위치
        BombItemPos[0].y = 28;

        BombItemPos[1].x = 9;
        BombItemPos[1].y = 26;

        BombItemPos[2].x = 8;
        BombItemPos[2].y = 13;

        BombItemPos[3].x = 13;
        BombItemPos[3].y = 11;

        BombItemPos[4].x = 26;
        BombItemPos[4].y = 6;

        BombItem = 0;   // 획득한 폭탄 갯수
        BombLimit = 5;  //주어지는 폭탄 갯수
    }


    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            BombFirePos[i][j].x = 0;
            BombFirePos[i][j].y = 0;
        }
    }
}
void ResetTime() {

    start = clock();

    if (stage == 0) {
        TimeLimit = 1000;
    }
    else if (stage == 1) {
        TimeLimit = 150;
    }
    else if (stage == 2) {
        TimeLimit = 190;
    }
    else if (stage == 3) {

        TimeLimit = 200;
    }
}

//void npc_Limit() {
//
//
//    if (stage == 1) {
//        npcLimit = 2;
//    }
//    else if (stage == 2) {
//        npcLimit = 2;
//    }
//    else if (stage == 3) {
//        npcLimit = 3;
//    }
//}
void printGameOver() {
    PlaySound(TEXT("패배.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

    strcpy_s(GAME_OVER[0], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[1], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[2], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[3], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[4], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[5], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[6], 100, "0000000004444000444400440004404444440000000000000000000000000000000");
    strcpy_s(GAME_OVER[7], 100, "0000000040000004000040404040404000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[8], 100, "0000000440000004000040400400404000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[9], 100, "0000000040004404444440400000404444440000000000000000000000000000000");
    strcpy_s(GAME_OVER[10], 100, "000000004444400400004040000040400000000000000000000000000000000000");
    strcpy_s(GAME_OVER[11], 100, "000000000000400400004040000040444444000000000000000000000000000000");
    strcpy_s(GAME_OVER[12], 100, "000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[13], 100, "000000000444440004000040044444004444440000000000000000000000000000");
    strcpy_s(GAME_OVER[14], 100, "000000004000004004000040040000004000004000000000000000000000000000");
    strcpy_s(GAME_OVER[15], 100, "000000004000004004000040044444004444440000000000000000000000000000");
    strcpy_s(GAME_OVER[16], 100, "000000004000004000400400040000004000040000000000000000000000000000");
    strcpy_s(GAME_OVER[17], 100, "000000000444440000044000044444404000004000000000000000000000000000");
    strcpy_s(GAME_OVER[18], 100, "000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[19], 100, "000000000000000000000000000000000000000000000000000000000000000000");


    for (int i = 0; i < 20; i++) //행 y
    {
        for (int j = 0; j < 100; j++) //열 x
        {
            switch (GAME_OVER[i][j])
            {
            case '0':
                gotoxy(_X + j, _Y + i, " ");
                break;
            case '1':
                TextColor(DEEP_WHITE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '2':
                TextColor(DEEP_GREEN);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '3':
                TextColor(GRAY);
                TextColor(DEEP_BLUE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '4':
                TextColor(DEEP_RED);
                gotoxy(_X + j, _Y + i, "■");
                break;
            }
        }
    }
    GameItemBoard();
    getchar();
    stage = -1;
}
void printGameSuccess() {

    system("cls");
    system("Color 7");
    strcpy_s(GAME_OVER[0], 100, "000000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[0], 100, "000000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[2], 100, "000000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[3], 100, "000000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[4], 100, "000000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[5], 100, "000000003333300033330033000330333333000000000000000000000000000000000");
    strcpy_s(GAME_OVER[6], 100, "000000030000000300003030303030300000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[7], 100, "000000330000000300003030030030300000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[8], 100, "000000030003300333333030000030333333300000000000000000000000000000000");
    strcpy_s(GAME_OVER[9], 100, "000000033333300300003030000030300000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[10], 100, "00000000000300300003030000030333333300000000000000000000000000000000");
    strcpy_s(GAME_OVER[11], 100, "00000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[12], 100, "00000033333030000033333300033300033333000000000000000000000000000000");
    strcpy_s(GAME_OVER[13], 100, "00000300000030000030000000030030030000300000000000000000000000000000");
    strcpy_s(GAME_OVER[14], 100, "00000300000030000033333300333333030333300000000000000000000000000000");
    strcpy_s(GAME_OVER[15], 100, "00000300000030000030000000300003030000300000000000000000000000000000");
    strcpy_s(GAME_OVER[16], 100, "00000033333033333033333300300003030000300000000000000000000000000000");



    int totalTime = 0;
    int totalCount = 0;
    int total;

    for (int i = 1; i < 4; i++) {

        totalTime += GameClearScore[0][i];
        totalCount += GameClearScore[2][i];

    }

    total = totalTime + totalCount;

    /*for (int i = 1; i < 4; i++) {
        gotoxy(10 + i * 10, GITEM_ORIGIN_Y + 17, ""); printf("%d 라운드", i );
        gotoxy(10 + i * 10, GITEM_ORIGIN_Y + 19, "총 걸린 시간 : "); printf("%d초", GameClearScore[0][i ]);
        gotoxy(10 + i * 10, GITEM_ORIGIN_Y + 21, "총 죽은 횟수 : "); printf("%d번", 3 - GameClearScore[1][i]);
        gotoxy(10 + i * 10, GITEM_ORIGIN_Y + 23, "총 회전 횟수 : "); printf("%d번", GameClearScore[2][i]);

    }*/

    gotoxy(10 + 16, GITEM_ORIGIN_Y + 18, ""); printf("------- 합산 점수 -------");
    gotoxy(10 + 17, GITEM_ORIGIN_Y + 20, "☆ 총 걸린 시간 : "); printf("%d초", totalTime);
    gotoxy(10 + 17, GITEM_ORIGIN_Y + 22, "☆ 총 죽은 횟수 : "); printf("%d번", 3 - death);
    gotoxy(10 + 17, GITEM_ORIGIN_Y + 24, "☆ 총 회전 횟수 : "); printf("%d번", totalCount);


    if (death == 3) {

        strcpy_s(Grade[0], 13, "000000000000");     //S
        strcpy_s(Grade[2], 13, "000444444000");
        strcpy_s(Grade[3], 13, "003333333300");
        strcpy_s(Grade[4], 13, "005500005500");
        strcpy_s(Grade[5], 13, "002200000000");
        strcpy_s(Grade[6], 13, "006666666000");
        strcpy_s(Grade[7], 13, "000444444400");
        strcpy_s(Grade[8], 13, "000000003300");
        strcpy_s(Grade[9], 13, "005500005500");
        strcpy_s(Grade[10], 13, "002222222200");
        strcpy_s(Grade[11], 13, "000666666000");

    }

    else if (death == 2 && total <= 500) {

        strcpy_s(Grade[0], 13, "000000000000");   //A   :4
        strcpy_s(Grade[2], 13, "000004400000");
        strcpy_s(Grade[3], 13, "000044440000");
        strcpy_s(Grade[4], 13, "000444444000");
        strcpy_s(Grade[5], 13, "004440044400");
        strcpy_s(Grade[6], 13, "004400004400");
        strcpy_s(Grade[7], 13, "004444444400");
        strcpy_s(Grade[8], 13, "004444444400");
        strcpy_s(Grade[9], 13, "004400004400");
        strcpy_s(Grade[10], 13, "004400004400");
        strcpy_s(Grade[11], 13, "000000000000");

    }


    else if (death == 2 && total > 500) {
        strcpy_s(Grade[0], 13, "00000000000");  //B   :2
        strcpy_s(Grade[2], 13, "00222222000");
        strcpy_s(Grade[3], 13, "00222222220");
        strcpy_s(Grade[4], 13, "002200002200");
        strcpy_s(Grade[5], 13, "002200002200");
        strcpy_s(Grade[6], 13, "002222222000");
        strcpy_s(Grade[7], 13, "002222222000");
        strcpy_s(Grade[8], 13, "002200002200");
        strcpy_s(Grade[9], 13, "002200002200");
        strcpy_s(Grade[10], 13, "002222222200");
        strcpy_s(Grade[11], 13, "002222222000");

    }


    else if (death == 1 && total <= 500) {
        strcpy_s(Grade[0], 13, "000000000000");     //C   :3
        strcpy_s(Grade[2], 13, "000333333000");
        strcpy_s(Grade[3], 13, "003333333300");
        strcpy_s(Grade[4], 13, "003300003300");
        strcpy_s(Grade[5], 13, "003300000000");
        strcpy_s(Grade[6], 13, "003300000000");
        strcpy_s(Grade[7], 13, "003300000000");
        strcpy_s(Grade[8], 13, "003300003300");
        strcpy_s(Grade[9], 13, "003333333300");
        strcpy_s(Grade[10], 13, "000333333000");
        strcpy_s(Grade[11], 13, "000000000000");
    }

    else {
        strcpy_s(Grade[0], 13, "000000000000");     //D   :5
        strcpy_s(Grade[2], 13, "005555555000");
        strcpy_s(Grade[3], 13, "005555555500");
        strcpy_s(Grade[4], 13, "000550005500");
        strcpy_s(Grade[5], 13, "000550005500");
        strcpy_s(Grade[6], 13, "000550005500");
        strcpy_s(Grade[7], 13, "000550005500");
        strcpy_s(Grade[8], 13, "000550005500");
        strcpy_s(Grade[9], 13, "005555555500");
        strcpy_s(Grade[10], 13, "00555555000");
        strcpy_s(Grade[11], 13, "00000000000");

    }


    for (int i = 0; i < 20; i++) //행 y
    {
        for (int j = 0; j < 100; j++) //열 x
        {
            switch (GAME_OVER[i][j])
            {
            case '0':
                gotoxy(_X + j, _Y + i, " ");
                break;
            case '1':
                TextColor(DEEP_WHITE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '2':
                TextColor(DEEP_OC);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '3':
                TextColor(GRAY);
                TextColor(DEEP_BLUE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '4':
                TextColor(DEEP_RED);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '5':
                TextColor(DEEP_YELLOW);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '6':
                TextColor(DEEP_JAJU);
                gotoxy(_X + j, _Y + i, "■");
                break;
            }

        }
        Sleep(50);
    }


    gotoxy(10 + 10, GITEM_ORIGIN_Y + 17, "");

    for (int i = 0; i < 12; i++) //행 y
    {
        for (int j = 0; j < 13; j++) //열 x
        {
            switch (Grade[i][j])
            {
            case '0':
                gotoxy(_X + j + 10, _Y + i + 17, " ");
                break;
            case '1':
                TextColor(DEEP_WHITE);
                gotoxy(_X + j + 10, _Y + i + 17, "■");
                break;
            case '5':
                TextColor(DEEP_GREEN);
                gotoxy(_X + j + 10, _Y + i + 17, "■");
                break;
            case '2':
                TextColor(GRAY);
                TextColor(DEEP_OC);
                gotoxy(_X + j + 10, _Y + i + 17, "■");
                break;
            case '4':
                TextColor(DEEP_RED);
                gotoxy(_X + j + 10, _Y + i + 17, "■");
                break;
            case '3':
                TextColor(DEEP_YELLOW);
                gotoxy(_X + j + 10, _Y + i + 17, "■");
                break;
            case '6':
                TextColor(DEEP_JAJU);
                gotoxy(_X + j + 10, _Y + i + 17, "■");
                break;
            }

        }
    }




    //GameItemBoard();
    getchar();
    stage = -1;
}
void ScorePage()
{

    if (stage == 2)
    {
        system("cls");
        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 7, "걸린 시간 : "); printf("%d초", GameClearScore[0][stage - 1]);
        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 9, "죽은 횟수 : "); printf("%d번", 3 - GameClearScore[1][stage - 1]);
        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 11, "회전 횟수 : "); printf("%d번", GameClearScore[2][stage - 1]);

        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 20, "                  \"Enter\" to next stage ☞");


        _getch();
        system("cls");
    }
    else if (stage == 3)
    {
        system("cls");
        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 7, "걸린 시간 : "); printf("%d초", GameClearScore[0][stage - 1]);
        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 9, "죽은 횟수 : "); printf("%d번", 3 - GameClearScore[1][stage - 1]);
        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 11, "회전 횟수 : "); printf("%d번", GameClearScore[2][stage - 1]);




        gotoxy(GITEM_ORIGIN_X / 2 - 10, GITEM_ORIGIN_Y + 20, "                  \"Enter\" to next stage ☞");




        _getch();
        system("cls");
    }
    else
        return;
}
void printTimeOver() {
    PlaySound(TEXT("패배.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

    strcpy_s(GAME_OVER[0], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[1], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[2], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[3], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[4], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[5], 100, "0000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[6], 100, "0000000044444404444440440004404444440000000000000000000000000000000");
    strcpy_s(GAME_OVER[7], 100, "0000000000040000040000404040404000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[8], 100, "0000000000040000040000400400404000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[9], 100, "0000000000040000040000400000404444440000000000000000000000000000000");
    strcpy_s(GAME_OVER[10], 100, "000000000004000004000040000040400000000000000000000000000000000000");
    strcpy_s(GAME_OVER[11], 100, "000000000004000444444040000040444444000000000000000000000000000000");
    strcpy_s(GAME_OVER[12], 100, "000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[13], 100, "000000000444440004000040044444004444440000000000000000000000000000");
    strcpy_s(GAME_OVER[14], 100, "000000004000004004000040040000004000004000000000000000000000000000");
    strcpy_s(GAME_OVER[15], 100, "000000004000004004000040044444004444440000000000000000000000000000");
    strcpy_s(GAME_OVER[16], 100, "000000004000004000400400040000004000040000000000000000000000000000");
    strcpy_s(GAME_OVER[17], 100, "000000000444440000044000044444404000004000000000000000000000000000");
    strcpy_s(GAME_OVER[18], 100, "000000000000000000000000000000000000000000000000000000000000000000");
    strcpy_s(GAME_OVER[19], 100, "000000000000000000000000000000000000000000000000000000000000000000");


    for (int i = 0; i < 20; i++) //행 y
    {
        for (int j = 0; j < 100; j++) //열 x
        {
            switch (GAME_OVER[i][j])
            {
            case '0':
                gotoxy(_X + j, _Y + i, " ");
                break;
            case '1':
                TextColor(DEEP_WHITE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '2':
                TextColor(DEEP_GREEN);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '3':
                TextColor(GRAY);
                TextColor(DEEP_BLUE);
                gotoxy(_X + j, _Y + i, "■");
                break;
            case '4':
                TextColor(DEEP_RED);
                gotoxy(_X + j, _Y + i, "■");
                break;

            }
        }
    }
    GameItemBoard();
    getchar();
    stage = -1;
}