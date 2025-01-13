#include<stdio.h>
#include <time.h>
#include "acllib.h"

//to display macro
#define WIDTH		480
#define HEIGHT		640
#define	X_SEG_0		0
#define	X_SEG_1		160
#define	X_SEG_2		320
#define Y_LOWER		0
#define Y_CENTER	160
#define Y_UPPER		320
#define N_IMG		16
#define HANDLE_T	CLOCKS_PER_SEC/8
#define WIN_T		CLOCKS_PER_SEC*2

//game setting
#define FEE				1
#define REWARD			10
#define START			13//return key
#define ADD_COIN		27//esc key
#define PAT				10//TBD
#define MAX				9//TBD
#define	MIN				0//TBD
#define NORMALMODE		0
#define ONLYWINMODE		1


void MouseEvent(int x, int y, int button, int event);
void KeyboardEvent(int key, int event);
int upper(int disp);
int lower(int disp);
void timerEvent(int key);
int Setup(void);
int slot(int *wallet,int *n,int *win,int *lose,int *disp_num);
int delay_ctrl(int *rand_num,int *disp_num,int state);


int timerID = 0,flag = 0,wallet = 5,n = 0,win = 0,lose = 0;
char img_name[N_IMG][32] = {"..\\img\\slot_0.jpg","..\\img\\slot_1.jpg","..\\img\\slot_2.jpg","..\\img\\slot_3.jpg","..\\img\\slot_4.jpg",
							"..\\img\\slot_5.jpg","..\\img\\slot_6.jpg","..\\img\\slot_7.jpg","..\\img\\slot_8.jpg","..\\img\\slot_9.jpg",
							"..\\img\\win.jpg","..\\img\\title.jpg",
							"..\\img\\top_ungle.jpg","..\\img\\down_ungle.jpg","..\\img\\lisboa.jpg","..\\img\\medal.jpg"};

//int dbgmode = NORMALMODE;
int dbgmode = ONLYWINMODE;

ACL_Image img[N_IMG];
ACL_Sound mp3[2];

int bias[3] = {0,0,0};

void MouseEvent(int x, int y, int button, int event)
{
	if(button == LEFT_BUTTON && event == BUTTON_DOWN){//event == 1 double click
		if(x > 320 && y > 480){
			flag = 1;

		}
	}
}

void KeyboardEvent(int key, int event)
{
	if(event == 1){
		if(key == START)			flag = 1;
		else if(key == ADD_COIN){
			wallet += 5;
			playSound(mp3[0], 0);
		}
	}
}

int upper(int disp)
{
	if(disp == MAX)	return MIN;
	else			return (disp + 1);
}

int lower(int disp)
{
	if(disp == MIN)	return MAX;
	else			return (disp - 1);
}

void timerEvent(int id)
{
	char str[16];
	static int state;
	static int handle = 0;
	static clock_t time0,time1;
	static int disp_num[3] = {0,0,0};
	

	if(flag == 1){
		time0 = clock();
		handle = 1;
	}
	time1 = clock();
	if(handle == 1 && (int)time1 - time0 > HANDLE_T){
		handle = 0;
	}
	
	state = slot(&wallet,&n,&win,&lose,disp_num);
	

	beginPaint();
	putImage(&img[lower(disp_num[0])], X_SEG_0, Y_LOWER+bias[0]);
	putImage(&img[lower(disp_num[1])], X_SEG_1, Y_LOWER+bias[1]);
	putImage(&img[lower(disp_num[2])], X_SEG_2, Y_LOWER+bias[2]);
	putImage(&img[disp_num[0]], X_SEG_0, Y_CENTER+bias[0]);
	putImage(&img[disp_num[1]], X_SEG_1, Y_CENTER+bias[1]);
	putImage(&img[disp_num[2]], X_SEG_2, Y_CENTER+bias[2]);
	putImage(&img[upper(disp_num[0])], X_SEG_0, Y_UPPER+bias[0]);
	putImage(&img[upper(disp_num[1])], X_SEG_1, Y_UPPER+bias[1]);
	putImage(&img[upper(disp_num[2])], X_SEG_2, Y_UPPER+bias[2]);

	if(state == 5){
		playSound(mp3[1], 0);
	}
	setBrushColor(WHITE);
	setPenColor(WHITE);
	if(state == 6){
		putImage(&img[10], X_SEG_0, 200);//TBD
		putImage(&img[15], X_SEG_1, 480);
	}
	else{//clear display
		rectangle(160, 480, WIDTH, 640);
	}

	//rectangle(0, 0, WIDTH, 80);
	putImage(&img[11], X_SEG_0, Y_LOWER);
	rectangle(0, 400, WIDTH, 480);
	setPenWidth(1);
	setPenColor(RED);
	line(0, 240, 480, 240);
	//line(0, 320, 480, 320);	

	setTextColor(BLACK);
	setTextBkColor(EMPTY);
	setTextSize(70);
	//setTextFont(char *pFontName);
	sprintf_s(str,16,"wallet:%d",wallet);
	paintText(0, 400, str);

	putImage(&img[14], X_SEG_0, 480);

	if(handle == 1){
		putImage(&img[13], X_SEG_2, 480);
	}
	else putImage(&img[12], X_SEG_2, 480);


	endPaint();
}

int Setup(void)
{
	int i;
	
	initWindow("slot", 0, 0, WIDTH, HEIGHT);

	for(i=0;i<N_IMG;i++){
		loadImage(img_name[i],&img[i]);
	}
	
	loadSound("..\\alipay_notice.mp3",&mp3[0]);
	loadSound("..\\medal.mp3",&mp3[1]);
	registerKeyboardEvent(KeyboardEvent);
	registerTimerEvent(timerEvent);
	registerMouseEvent(MouseEvent);
	startTimer(timerID, 100);

	return 0;
}

int slot(int *wallet,int *n,int *win,int *lose,int *disp_num)
{
	static int rand_num[3] = {0,0,0};
	static int state = 0;
	static int busy;
	static clock_t time0,time1;

	if(flag == 1){//keyboard event
		flag = 0;
		if(state == 0){
			if(*wallet >= FEE){
				*wallet -= FEE; 
				state++;
				*n++;
			}
		}
		else if(state >= 1 && state <= 3){
			state++;
		}
	}
	
	if(state == 1){
		rand_num[0] = rand() % PAT + MIN;
		rand_num[1] = rand() % PAT + MIN;
		rand_num[2] = rand() % PAT + MIN;
	}
	else if(state == 2){
		if (dbgmode == NORMALMODE) {
			rand_num[1] = rand() % PAT + MIN;
			rand_num[2] = rand() % PAT + MIN;
		}
		else if (dbgmode == ONLYWINMODE) {
			rand_num[1] = rand_num[0];
			rand_num[2] = rand() % PAT + MIN;
		}
	}
	else if(state == 3){
		if (dbgmode == NORMALMODE) {
			rand_num[2] = rand() % PAT + MIN;
		}
		else if (dbgmode == ONLYWINMODE) {
			rand_num[2] = rand_num[0];
		}
	}
	else if(state == 4 && busy == 0){//TBD
		if (dbgmode == ONLYWINMODE) {
			state++;
		}
		else if(dbgmode == NORMALMODE) {
			if (rand_num[0] == rand_num[1] && rand_num[0] == rand_num[2]) {//TBD
				state++;
			}
			else {
				(*lose)++;
				state = 0;
			}
		}
	}
	else if(state == 5){//only win
		(*win)++;
		*wallet += REWARD;
		time0 = clock();
		state++;
	}
	else if(state == 6){
		time1 = clock();
		if((int)time1 - time0 > WIN_T){
			state = 0;
		}
	}

	busy = delay_ctrl(rand_num, disp_num, state);

	return state;
}


int delay_ctrl(int *rand_num,int *disp_num,int state)
{
	int busy;
	static int delay_cnt[3] = {0,0,0};
	static int cycle[3] = {1,1,1};

	if(state > 0 && state <= 1){
		if(disp_num[0] == MAX)	disp_num[0] = MIN;
		else disp_num[0]++;
		delay_cnt[0] = 0;
		cycle[0] = 0;
	}
	else if(rand_num[0] != disp_num[0] || cycle[0] == 0){
		//delay control
		if(delay_cnt[0] == 2){
			if(rand_num[0] == disp_num[0])	cycle[0]++;
			delay_cnt[0] = 0;
			if(disp_num[0] == MAX)	disp_num[0] = MIN;
			else disp_num[0]++;
		}
		else delay_cnt[0]++;
	}
	else if(cycle[0] == 1)	cycle[0]++;//TBD

	if(state > 0 && state <= 2){
		if(disp_num[1] == MAX)	disp_num[1] = MIN;
		else disp_num[1]++;
		delay_cnt[1] = 0;
		cycle[1] = 0;
	}
	else if(rand_num[1] != disp_num[1] || cycle[1] == 0){
		//delay control
		if(delay_cnt[1] == 2){
			if(rand_num[1] == disp_num[1])	cycle[1]++;
			delay_cnt[1] = 0;
			if(disp_num[1] == MAX)	disp_num[1] = MIN;
			else disp_num[1]++;
		}
		else delay_cnt[1]++;
	}
	else if(cycle[1] == 1)	cycle[1]++;//TBD

	if(state > 0 && state <= 3){
		if(disp_num[2] == MAX)	disp_num[2] = MIN;
		else disp_num[2]++;
		delay_cnt[2] = 0;
		cycle[2] = 0;
	}
	else if(rand_num[2] != disp_num[2] || cycle[2] == 0){
		//delay control
		if(delay_cnt[2] == 2){
			if(rand_num[2] == disp_num[2])	cycle[2]++;
			delay_cnt[2] = 0;
			if(disp_num[2] == MAX)	disp_num[2] = MIN;
			else disp_num[2]++;
		}
		else delay_cnt[2]++;
	}
	else if(cycle[2] == 1)	cycle[2]++;//TBD

	if(cycle[0] == 2 && cycle[1] == 2 && cycle[2] == 2){//TBD
		busy = 0;
	}
	else busy = 1;

	return busy;
}

