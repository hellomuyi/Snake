/*
 *Author: Jin
 *Time  : 2020/11/10
 */
#include<cstdio>
#include<iostream>
#include<windows.h>
#include<ctime>
#include<conio.h>	// kbhit 
#pragma warning(disable:4996)	// VS下运行须加上此行

//定义坐标结构体
struct node{
	int x;
	int y;
	node(int _x, int _y):x(_x), y(_y){}
	node(){}
};
// 自定义形状
const char* wallshape = "#";		// 边界墙形状 
const char* headshape = "O";		// 蛇头形状 
const char* bodyshape = "o";		// 蛇尾形状 
const char* foodshape = "$";		// 食物形状 
int len = 3;					// 蛇的当前长度 
const int MAX_X_ = 20;			// 逻辑边界长度（X轴），实际长度为2*MAX_X_-1，下标为0-MAX_X_-2，各点坐标都在偶数点上 
const int MAX_X = 2*MAX_X_-1;	// 实际边界长度，边界点坐标都在偶数点上，0:2:MAX_X-1 
const int MAX_Y = 20;			// 边界宽度（Y轴），0-MAX_Y-1
int speed = 400;				// 移动速度、延时毫秒数 
const node scorexy = node(MAX_X+2,MAX_Y/2);	//实时记录分数的位置
// 颜色值
const int defacol = 10;		// default默认颜色 
const int wallcol = 9;		// 边界墙的颜色 
const int foodcol = 11;		// 食物和蛇的颜色 

bool gameflag = true;	// 游戏开始结束标志 
int score = 0;			// 游戏得分 
const int MAX = 100;	// 蛇的最大长度
node snake[MAX];		// 各蛇节点的坐标
node food;				// 食物坐标 
int dx, dy;				// 下一步蛇移动的x\y分量

void gotoxy(short x, short y);	 // 控制台光标位置，并隐藏光标
void setColor(int color=defacol);// 设置控制台文字颜色 ，注意不能同时在函数的声明和定义中使用默认形参，对比下文函数定义 
void welcome();			// 欢迎界面 
void initMap();			// 打印边界墙、操作说明和初始分数 
void initSnake();		// 初始化蛇并打印 
void generateFood();	// 随机生成食物 ，食物不能在蛇身上 
void moveSnake();		// 移动蛇：更新蛇头方向、蛇节点坐标，并打印 
void tellFood();		// 判断是否吃食物，是则更新分数、更新速度、更新蛇、产生下一个食物 
void tellDeath();		// 判断死亡 
void gameOver();		// 结束界面 
 
int main(){
	setColor();		// 设置控制台文字颜色，默认形参 
	welcome();		
	initMap(); 
	initSnake();
	generateFood();
	while(gameflag){
		moveSnake();
		tellFood();
		tellDeath();
	}
	gameOver();
	return 0;
}

void gotoxy(short x, short y){	// 指定控制台光标
 
	COORD pos;
	pos.X = x;
	pos.Y = y;
	HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, pos);
	// 隐藏光标 
    CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = FALSE;    
	cursor.dwSize = sizeof(cursor);
	SetConsoleCursorInfo(hOutput, &cursor);
}

void setColor(int color){	// 设置控制台文字颜色 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void welcome(){				// 欢迎界面 
	gotoxy(20, 10);
	std::cout << "Welcome to Retro Snaker!";
    gotoxy(20, 15); 
	std::cout << "通过方向键或w,a,s,d（英文输入法）改变方向";
	gotoxy(20, 16);
	std::cout << "按 空格键 暂停，按 回车键 继续";
	gotoxy(20, 20);
	std::cout << "Press enter to start the game.";
	std::cin.get();
	system("cls");
}

void initMap(){			// 初始化地图 
	setColor(wallcol); 
	// 画上下边界，X轴 
	for(int i=0; i<MAX_X; i+=2){
		gotoxy(i, 0);
		std::cout << wallshape;
		gotoxy(i, MAX_Y - 1);
		std::cout << wallshape;
	}
	
	// 打印左右界，Y轴 
	for(int i=0; i<MAX_Y; i++){
		gotoxy(0, i);
		std::cout << wallshape;
		gotoxy(MAX_X - 1, i);
		std::cout << wallshape;
	}
	setColor();		 
	gotoxy(scorexy.x, scorexy.y - 3);
	std::cout << "通过方向键或w,a,s,d（英文输入法）改变方向";
	gotoxy(scorexy.x, scorexy.y + 3);
	std::cout << "按 空格键 暂停，按 回车键 继续";
	setColor(foodcol);
	gotoxy(scorexy.x, scorexy.y);
	std::cout << "score：" << score;
	setColor(defacol);	// 恢复默认颜色 
}

void initSnake(){		// 初始化蛇 
	dx = 0;
	dy = 1;		// 初始时蛇往下移动，即Y轴正方向
	int tmp_x = (MAX_X - 1) / 2;
	if (tmp_x & 1)		// 蛇头X坐标为偶数 
		tmp_x += 1;
	for(int i=0; i<len; i++)
		snake[i] = node(tmp_x, MAX_Y/len+len-1-i);
	// snake[0] = node(MAX_X/2, MAX_Y/3+2);
	// snake[1] = node(MAX_X/2, MAX_Y/3+1);
	// snake[2] = node(MAX_X/2, MAX_Y/3+0);
	// 打印初始蛇
	setColor(foodcol);	
	gotoxy(snake[0].x, snake[0].y);
	std::cout << headshape; 		// 蛇头 
	for(int i=1; i<len; i++){
		gotoxy(snake[i].x, snake[i].y);
		std::cout << bodyshape; 	// 蛇身 
	} 
	setColor(defacol);	// 恢复默认颜色 
}

void generateFood(){	// 随机产生在边界内且不在蛇身上的食物 
	srand(time(NULL));
	while(1){
		int x = rand() % (MAX_X - 4) + 2;		// X范围1-MAX_X-2，得是偶数，最大的偶数是MAX_X-3(MAX_X-1是右边界) 
		if (x & 1)	// x为奇数 
			x += 1;		// 加一减一都可以 
		int y = rand() % (MAX_Y - 2) + 1;		// Y范围1-MAX_Y-2
		food = node(x, y);
		// 判断食物是否在蛇身上
		bool foodFlag = true;
		for(int i=0; i<len; i++){
			if(food.x==snake[i].x && food.y==snake[i].y){
				foodFlag = false;
				break;
			}
		}
		if(foodFlag)
			break;
	}
	// 打印食物
	setColor(foodcol);
	gotoxy(food.x, food.y);
	std::cout << foodshape;
	setColor(defacol);	// 恢复默认颜色 
}

void moveSnake(){		// 监听键盘修改蛇头方向，并打印移动后的蛇 
	if(kbhit()){
		unsigned char c = getch();//对 应224，char对应-32 
		switch(c){ 
			case ' ':
				std::cin.get();
				break;
			case 'a': 	// 向左移动
				if(0 == dx) {// 原方向向上或向下才须改变 
					dx = -1;
					dy = 0;
				}
				break;
			case 'd':	// 向右移动
				if(0 == dx) {// 方向向上或向下才须改变  
					dx = 1;
					dy = 0;
				}
				break;
			case 'w':	// 向上移动
				if(0 == dy) {// 原方向向左或向右才须改变 
					dx = 0;
					dy = -1;
				}
				break; 
			case 's': 	// 向下移动	
				if(0 == dy) {//方向向左或向右才须改变 
					dx = 0;
					dy = 1;
				}
				break;
			case 224:	// 方向键区的ASCII码 
				c = getch();
				switch(c){
					// 向左移动 
					case 75: 
						if(0 == dx) {// 原方向向上或向下才须改变 
							dx = -1;
							dy = 0;
						}
						break;
					// 向右移动
					case 77:
						if(0 == dx) {// 方向向上或向下才须改变  
							dx = 1;
							dy = 0;
						}
						break;
					// 向上移动
					case 72:
						if(0 == dy) {// 原方向向左或向右才须改变 
							dx = 0;
							dy = -1;
						}
						break;
					// 向下移动	 
					case 80: 
						if(0 ==dy) {// 方向向左或向右才须改变 
							dx = 0;
							dy = 1;
						}
						break;
					default:
						break;
				}
			default:
				break;
		}
	}
	
	// 更新整条蛇，并打印 
	gotoxy(snake[len - 1].x, snake[len - 1].y);
	std::cout << " ";				// 清除蛇尾 
	setColor(wallcol);
	gotoxy(0, 0);		// 去除这三条语句，边界墙左上角会空缺 
	std::cout << "#";
	setColor(foodcol);
	gotoxy(snake[0].x, snake[0].y);
	std::cout << bodyshape;				// 向前移动后，之前的蛇头位置变为蛇身 
	for (int i = len - 1; i > 0; i--)
		snake[i] = snake[i - 1];	// 从蛇尾起更新坐标
	 
	snake[0] = node(snake[0].x + dx * 2, snake[0].y + dy);	// 更新蛇头坐标 ,dx得乘2 
	gotoxy(snake[0].x, snake[0].y);
	std::cout << headshape; 			// 打印蛇头 
	setColor(defacol);	// 恢复默认颜色 
	Sleep(speed);
}

void tellFood(){		// 判断是否吃食物，并产生新食物，更新速度、分数和蛇长度 
	if(food.x==snake[0].x && food.y==snake[0].y){
		generateFood();	// 产生新食物 
		len++; 		// 更新蛇，只需要len++，注意理解，moveSnack函数中更新时len已经+1 
		score += 10;					// 更新分数
		speed>=130?speed-=30:speed=100;	// 更新速度 
		setColor(foodcol);
		gotoxy(scorexy.x, scorexy.y);
		if (score < 150)	// 刷新分数 
			std::cout << "score：" << score;
		else{
			speed = 50;
			std::cout << "score：" << score << "   终极模式";
		} 
			
		setColor(defacol);	// 恢复默认颜色 
	}
}

void tellDeath(){		// 判断死亡 
	// 撞墙而死 
	for(int i=0; i<MAX_X; i++)
		if(0==snake[0].y || MAX_Y-1==snake[0].y || 0==snake[0].x || MAX_X-1==snake[0].x){
			gameflag = false;
			return;
		}	
		
	// 碰到自己而死(至少有五个点才会可能头碰到身体) 
	for(int i=len-1; i>=4; i--)
		if(snake[0].x==snake[i].x && snake[0].y==snake[i].y){
		 	gameflag = false;
		 	return;
		 }
}

void gameOver(){
	gotoxy(0, MAX_Y + 1);
	std::cout << "Game over! Your score is " << score << ".";
	gotoxy(0, MAX_Y + 3);
	std::cout << "Press Enter to continue.";
	std::cin.get();
}
/*
 *注意点：
 *控制台字符的宽高比为1:2，为了美化也更加真实，边界墙结点、蛇结点、食物的x坐标始终为偶数，蛇头的x轴方向的移动一次为两个单位长度	
 *关于颜色：需要颜色的地方设置且用完后立即设置成默认颜色，这样使用默认颜色时无需再设置	

 *改进：
 *1.蛇头为食物，吃啥蛇头变成啥，哈哈
 *2.输出汉字
 */ 
