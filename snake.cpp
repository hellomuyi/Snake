/*
 *Author: Jin
 *Time  : 2020/11/10
 */
#include<cstdio>
#include<iostream>
#include<windows.h>
#include<ctime>
#include<conio.h>	// kbhit 
#pragma warning(disable:4996)	// VS����������ϴ���

//��������ṹ��
struct node{
	int x;
	int y;
	node(int _x, int _y):x(_x), y(_y){}
	node(){}
};
// �Զ�����״
const char* wallshape = "#";		// �߽�ǽ��״ 
const char* headshape = "O";		// ��ͷ��״ 
const char* bodyshape = "o";		// ��β��״ 
const char* foodshape = "$";		// ʳ����״ 
int len = 3;					// �ߵĵ�ǰ���� 
const int MAX_X_ = 20;			// �߼��߽糤�ȣ�X�ᣩ��ʵ�ʳ���Ϊ2*MAX_X_-1���±�Ϊ0-MAX_X_-2���������궼��ż������ 
const int MAX_X = 2*MAX_X_-1;	// ʵ�ʱ߽糤�ȣ��߽�����궼��ż�����ϣ�0:2:MAX_X-1 
const int MAX_Y = 20;			// �߽��ȣ�Y�ᣩ��0-MAX_Y-1
int speed = 400;				// �ƶ��ٶȡ���ʱ������ 
const node scorexy = node(MAX_X+2,MAX_Y/2);	//ʵʱ��¼������λ��
// ��ɫֵ
const int defacol = 10;		// defaultĬ����ɫ 
const int wallcol = 9;		// �߽�ǽ����ɫ 
const int foodcol = 11;		// ʳ����ߵ���ɫ 

bool gameflag = true;	// ��Ϸ��ʼ������־ 
int score = 0;			// ��Ϸ�÷� 
const int MAX = 100;	// �ߵ���󳤶�
node snake[MAX];		// ���߽ڵ������
node food;				// ʳ������ 
int dx, dy;				// ��һ�����ƶ���x\y����

void gotoxy(short x, short y);	 // ����̨���λ�ã������ع��
void setColor(int color=defacol);// ���ÿ���̨������ɫ ��ע�ⲻ��ͬʱ�ں����������Ͷ�����ʹ��Ĭ���βΣ��Ա����ĺ������� 
void welcome();			// ��ӭ���� 
void initMap();			// ��ӡ�߽�ǽ������˵���ͳ�ʼ���� 
void initSnake();		// ��ʼ���߲���ӡ 
void generateFood();	// �������ʳ�� ��ʳ�ﲻ���������� 
void moveSnake();		// �ƶ��ߣ�������ͷ�����߽ڵ����꣬����ӡ 
void tellFood();		// �ж��Ƿ��ʳ�������·����������ٶȡ������ߡ�������һ��ʳ�� 
void tellDeath();		// �ж����� 
void gameOver();		// �������� 
 
int main(){
	setColor();		// ���ÿ���̨������ɫ��Ĭ���β� 
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

void gotoxy(short x, short y){	// ָ������̨���
 
	COORD pos;
	pos.X = x;
	pos.Y = y;
	HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, pos);
	// ���ع�� 
    CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = FALSE;    
	cursor.dwSize = sizeof(cursor);
	SetConsoleCursorInfo(hOutput, &cursor);
}

void setColor(int color){	// ���ÿ���̨������ɫ 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void welcome(){				// ��ӭ���� 
	gotoxy(20, 10);
	std::cout << "Welcome to Retro Snaker!";
    gotoxy(20, 15); 
	std::cout << "ͨ���������w,a,s,d��Ӣ�����뷨���ı䷽��";
	gotoxy(20, 16);
	std::cout << "�� �ո�� ��ͣ���� �س��� ����";
	gotoxy(20, 20);
	std::cout << "Press enter to start the game.";
	std::cin.get();
	system("cls");
}

void initMap(){			// ��ʼ����ͼ 
	setColor(wallcol); 
	// �����±߽磬X�� 
	for(int i=0; i<MAX_X; i+=2){
		gotoxy(i, 0);
		std::cout << wallshape;
		gotoxy(i, MAX_Y - 1);
		std::cout << wallshape;
	}
	
	// ��ӡ���ҽ磬Y�� 
	for(int i=0; i<MAX_Y; i++){
		gotoxy(0, i);
		std::cout << wallshape;
		gotoxy(MAX_X - 1, i);
		std::cout << wallshape;
	}
	setColor();		 
	gotoxy(scorexy.x, scorexy.y - 3);
	std::cout << "ͨ���������w,a,s,d��Ӣ�����뷨���ı䷽��";
	gotoxy(scorexy.x, scorexy.y + 3);
	std::cout << "�� �ո�� ��ͣ���� �س��� ����";
	setColor(foodcol);
	gotoxy(scorexy.x, scorexy.y);
	std::cout << "score��" << score;
	setColor(defacol);	// �ָ�Ĭ����ɫ 
}

void initSnake(){		// ��ʼ���� 
	dx = 0;
	dy = 1;		// ��ʼʱ�������ƶ�����Y��������
	int tmp_x = (MAX_X - 1) / 2;
	if (tmp_x & 1)		// ��ͷX����Ϊż�� 
		tmp_x += 1;
	for(int i=0; i<len; i++)
		snake[i] = node(tmp_x, MAX_Y/len+len-1-i);
	// snake[0] = node(MAX_X/2, MAX_Y/3+2);
	// snake[1] = node(MAX_X/2, MAX_Y/3+1);
	// snake[2] = node(MAX_X/2, MAX_Y/3+0);
	// ��ӡ��ʼ��
	setColor(foodcol);	
	gotoxy(snake[0].x, snake[0].y);
	std::cout << headshape; 		// ��ͷ 
	for(int i=1; i<len; i++){
		gotoxy(snake[i].x, snake[i].y);
		std::cout << bodyshape; 	// ���� 
	} 
	setColor(defacol);	// �ָ�Ĭ����ɫ 
}

void generateFood(){	// ��������ڱ߽����Ҳ��������ϵ�ʳ�� 
	srand(time(NULL));
	while(1){
		int x = rand() % (MAX_X - 4) + 2;		// X��Χ1-MAX_X-2������ż��������ż����MAX_X-3(MAX_X-1���ұ߽�) 
		if (x & 1)	// xΪ���� 
			x += 1;		// ��һ��һ������ 
		int y = rand() % (MAX_Y - 2) + 1;		// Y��Χ1-MAX_Y-2
		food = node(x, y);
		// �ж�ʳ���Ƿ���������
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
	// ��ӡʳ��
	setColor(foodcol);
	gotoxy(food.x, food.y);
	std::cout << foodshape;
	setColor(defacol);	// �ָ�Ĭ����ɫ 
}

void moveSnake(){		// ���������޸���ͷ���򣬲���ӡ�ƶ������ 
	if(kbhit()){
		unsigned char c = getch();//�� Ӧ224��char��Ӧ-32 
		switch(c){ 
			case ' ':
				std::cin.get();
				break;
			case 'a': 	// �����ƶ�
				if(0 == dx) {// ԭ�������ϻ����²���ı� 
					dx = -1;
					dy = 0;
				}
				break;
			case 'd':	// �����ƶ�
				if(0 == dx) {// �������ϻ����²���ı�  
					dx = 1;
					dy = 0;
				}
				break;
			case 'w':	// �����ƶ�
				if(0 == dy) {// ԭ������������Ҳ���ı� 
					dx = 0;
					dy = -1;
				}
				break; 
			case 's': 	// �����ƶ�	
				if(0 == dy) {//������������Ҳ���ı� 
					dx = 0;
					dy = 1;
				}
				break;
			case 224:	// ���������ASCII�� 
				c = getch();
				switch(c){
					// �����ƶ� 
					case 75: 
						if(0 == dx) {// ԭ�������ϻ����²���ı� 
							dx = -1;
							dy = 0;
						}
						break;
					// �����ƶ�
					case 77:
						if(0 == dx) {// �������ϻ����²���ı�  
							dx = 1;
							dy = 0;
						}
						break;
					// �����ƶ�
					case 72:
						if(0 == dy) {// ԭ������������Ҳ���ı� 
							dx = 0;
							dy = -1;
						}
						break;
					// �����ƶ�	 
					case 80: 
						if(0 ==dy) {// ������������Ҳ���ı� 
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
	
	// ���������ߣ�����ӡ 
	gotoxy(snake[len - 1].x, snake[len - 1].y);
	std::cout << " ";				// �����β 
	setColor(wallcol);
	gotoxy(0, 0);		// ȥ����������䣬�߽�ǽ���Ͻǻ��ȱ 
	std::cout << "#";
	setColor(foodcol);
	gotoxy(snake[0].x, snake[0].y);
	std::cout << bodyshape;				// ��ǰ�ƶ���֮ǰ����ͷλ�ñ�Ϊ���� 
	for (int i = len - 1; i > 0; i--)
		snake[i] = snake[i - 1];	// ����β���������
	 
	snake[0] = node(snake[0].x + dx * 2, snake[0].y + dy);	// ������ͷ���� ,dx�ó�2 
	gotoxy(snake[0].x, snake[0].y);
	std::cout << headshape; 			// ��ӡ��ͷ 
	setColor(defacol);	// �ָ�Ĭ����ɫ 
	Sleep(speed);
}

void tellFood(){		// �ж��Ƿ��ʳ���������ʳ������ٶȡ��������߳��� 
	if(food.x==snake[0].x && food.y==snake[0].y){
		generateFood();	// ������ʳ�� 
		len++; 		// �����ߣ�ֻ��Ҫlen++��ע����⣬moveSnack�����и���ʱlen�Ѿ�+1 
		score += 10;					// ���·���
		speed>=130?speed-=30:speed=100;	// �����ٶ� 
		setColor(foodcol);
		gotoxy(scorexy.x, scorexy.y);
		if (score < 150)	// ˢ�·��� 
			std::cout << "score��" << score;
		else{
			speed = 50;
			std::cout << "score��" << score << "   �ռ�ģʽ";
		} 
			
		setColor(defacol);	// �ָ�Ĭ����ɫ 
	}
}

void tellDeath(){		// �ж����� 
	// ײǽ���� 
	for(int i=0; i<MAX_X; i++)
		if(0==snake[0].y || MAX_Y-1==snake[0].y || 0==snake[0].x || MAX_X-1==snake[0].x){
			gameflag = false;
			return;
		}	
		
	// �����Լ�����(�����������Ż����ͷ��������) 
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
 *ע��㣺
 *����̨�ַ��Ŀ�߱�Ϊ1:2��Ϊ������Ҳ������ʵ���߽�ǽ��㡢�߽�㡢ʳ���x����ʼ��Ϊż������ͷ��x�᷽����ƶ�һ��Ϊ������λ����	
 *������ɫ����Ҫ��ɫ�ĵط�������������������ó�Ĭ����ɫ������ʹ��Ĭ����ɫʱ����������	

 *�Ľ���
 *1.��ͷΪʳ���ɶ��ͷ���ɶ������
 *2.�������
 */ 
