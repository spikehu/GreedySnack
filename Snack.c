#include "SDL2/SDL.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define WALLOFX 700 //x方向长度
#define WALLOFY 800 //Y方向的长度
//方向分别为左，右，下，上对应 0,1,2,3
#define MOVELEFT 1
#define MOVERIGHT 0
#define MOVEDOWN 2
#define MOVEUP 3
//每个单元的X和Y方向的长度
#define BODYWIDTH 10
#define BODYHEIHT 10
/**
 * 设置渲染器的背景颜色
 * R,G,B,透明度值 169，169，169, 255
 * 
 */
#define R 169
#define G 169
#define B 169
#define A 255
/**蛇和食物的颜色
 */
#define R2 128
#define G2 128
#define B2 0
#define A2 255
/**
 * 四个方向
 * 左，右，下，上
 * 
 */
int dx[4] = {BODYWIDTH, -BODYWIDTH, 0, 0};
int dy[4] = {0, 0, BODYHEIHT, -BODYHEIHT};
int SnackSize = 0; //蛇右多少个矩形组成
/*
贪吃蛇游戏
链表表示蛇
头节点为蛇的头
//该链表为双向链表
*/
typedef struct Snack
{
    int x;
    int y;
    struct Snack *next;
    struct Snack *pre;
} Snack;

typedef struct Food
{
    int x;
    int y;
} Food;
void RandFood(Snack *head, Food *food);
int JudgeStateOfSnackMove(Snack *head, Food *food, int MOVE);
/*
吃到食物，食物的位置就是新增加的头的位置
*/
void Insert(Snack *head, Food *food)
{
    Snack *node = (Snack *)malloc(sizeof(Snack));
    node->next = head;
    head->pre = node;
    node->x = food->x;
    node->y = food->y;
    head = node;
    SnackSize++;
}
//初始化蛇
Snack *CreateSnack()
{
    Snack *head = (Snack *)malloc(sizeof(Snack));
    head->next = NULL;
    head->pre = NULL;
    head->x = 0;
    head->y = 0;
    SnackSize++;
    return head;
}
//初始化食物
Food *CreateFood()
{
    Food *food = (Food *)malloc(sizeof(Food));
    food->x = 0;
    food->y = 0;
    return food;
}
//蛇的移动，上下左右一定一个单位
//没有判断边界和是否吃到自己或者食物
void move(int MOVE, Snack *head, Food *food)
{
    Snack *p = head; //指向链表尾部
    while (p->next)
    {
        p = p->next;
    }
    int state = JudgeStateOfSnackMove(head, food, MOVE);
    switch (state)
    {
    case 1:                 //吃到食物
       // Insert(head, food); 
        RandFood(head, food);//在随机一个食物的位置
        break;
    case 0:

        break;
    case 2: //正常移动
        //方向分别为左，右，下，上对应 0,1,2,3
        //头节点的位置改变，后面的节点位置变为前一个节点的位置
        head->x = head->x + dx[MOVE];
        head->y = head->y + dy[MOVE];
        //后面的节点位置变为前一个节点的位置
        while (p->pre)
        {
            p->x = p->pre->x;
            p->y = p->pre->y;
            p = p->pre;
        }
        break;
    default:
        break;
    }
}
//判断边界和是否吃到食物
//返回1表示吃到食物，返回0表示下一步碰到边界，否则返回2表示正常移动
//x,y是食物的位置
int JudgeStateOfSnackMove(Snack *head, Food *food, int MOVE)
{
    int x = food->x;
    int y = food->y;
    int newX = head->x + dx[MOVE];
    int newY = head->y + dy[MOVE];
    //碰到墙体
    if (newX < 0 || newX > WALLOFX || newY < 0 || newY > WALLOFY)
    {
        return 0;
    }
    //吃到食物
    if (newX == x && newY == y)
    {
        return 1;
    }
    return 2;
}

/**
 * 游戏开始时候初始化蛇和食物的位置
 * 初始化蛇的位置或者食物的位置
 */
void InitTheLocationOfSnackAndFood(Snack *head, Food *food)
{
    head->x = (rand() * BODYWIDTH) % WALLOFX;
    srand((unsigned)time(NULL));
    head->y = (rand() * BODYHEIHT) % WALLOFY;
    food->x = (head->x + rand() * BODYWIDTH) % WALLOFX;
    food->y = (head->y + rand() * BODYHEIHT) % WALLOFY;
}
/**
 * 
 * 随机生成食物的位置，但是不能生成在蛇的头部位置
 * 在游戏过程中调用
 */

void RandFood(Snack *head, Food *food)
{
    while (true)
    {

        food->x = rand() % WALLOFX;
        srand((unsigned)time(NULL));
        food->y = rand() % WALLOFY;
        if (food->x == head->x && food->y == head->y)
        {
            continue;
        }
        else
        {
            break;
        }
    }
}

/**绘制蛇的矩形
 * 为传入的矩形赋值
 * rects:矩形数组
 * RectSize：数组长度
 * head：连边蛇
 */
bool RectsOfSnack(SDL_Rect *rects, Snack *head, int RectSize)
{
    Snack *p = head;
    int count = 0;
    while (p)
    {
        if (count >= RectSize)
        {
            return false; //绘制失败
        }
        (rects[count]).x = p->x;
        (rects[count]).y = p->y;
        (rects[count]).w = BODYWIDTH;
        (rects[count]).h = BODYHEIHT;
        count++;
        p = p->next;
    }
    return true; //绘制成功
}
/**
 * 绘制蛇和食物
 * head蛇的头节点
 * food：食物
 * renderer:渲染器
 * 
 */
void DrawSnackAndFood(Snack *head, Food *food, SDL_Renderer *renderer)
{
    //重置背景颜色
    SDL_SetRenderDrawColor(renderer, R, G, B, A);
    SDL_RenderClear(renderer);
    //绘制蛇和食物
    SDL_Rect *SnackBody = (SDL_Rect *)malloc(sizeof(SDL_Rect) * SnackSize); //蛇的矩形数组
    SDL_Rect *foodBody = (SDL_Rect *)malloc(sizeof(SDL_Rect));              //食物
    //为矩形赋值
    foodBody->x = food->x;
    foodBody->y = food->y;
    foodBody->w = BODYWIDTH;
    foodBody->h = BODYHEIHT; //食物矩形
    //蛇的矩形

    if (RectsOfSnack(SnackBody, head, SnackSize) == false)
    {
        printf("绘制失败");
        return;
    }
    //绘制
    //设置蛇和食物的颜色
    SDL_SetRenderDrawColor(renderer, R2, G2, B2, A2);
    SDL_RenderFillRect(renderer, foodBody);  //渲染器上绘制食物
    SDL_RenderFillRect(renderer, SnackBody); //渲染器上绘制蛇
}

//todo:界面的绘制以及SDL库的使用
static SDL_Rect bigrect = {0, 0, BODYWIDTH, BODYHEIHT};
int main(int args, char *argv[])
{
    //初始化SDL相关套件
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window *window = NULL;
    bool quit = false;
    SDL_Event event;
    SDL_Renderer *renderer = NULL;
    window = SDL_CreateWindow("GreedySanckGame",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WALLOFX,
                              WALLOFY,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED);
    //初始化蛇和食物
    Snack *head = CreateSnack();
    Food *food = CreateFood();
    InitTheLocationOfSnackAndFood(head, food);
    DrawSnackAndFood(head, food, renderer);
    printf("按任意键开始游戏\n");
    if (window == NULL)
    {
        printf("could not create the window %s\n", SDL_GetError());
        return 1;
    }

    int index = 0;
    while (!quit)
    {

        while (!quit && SDL_PollEvent(&event))
        {

            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;

                break;
            case SDL_KEYDOWN:
                //键盘事件
                switch (event.key.keysym.sym)
                {
                case SDLK_UP: //按上键
                    move(MOVEUP, head, food);
                    break;
                case SDLK_DOWN: //按下键
                    move(MOVEDOWN, head, food);

                    break;
                case SDLK_LEFT: //按左键
                    move(MOVELEFT, head, food);
                    break;
                case SDLK_RIGHT: //按右键
                    move(MOVERIGHT, head, food);
                    break;
                case SDLK_ESCAPE: //退出游戏
                    quit = true;
                    break;
                default:
                    break;
                } //键盘事件结尾
                DrawSnackAndFood(head, food, renderer);
                SDL_RenderPresent(renderer); //刷新帧数
                break;
            default:
                break;
            }
        }
        printf("the rendereer");
        SDL_Delay(100);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}