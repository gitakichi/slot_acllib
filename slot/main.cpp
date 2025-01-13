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
#define DEFAULT_WALLET	5
#define PAT				10//TBD
#define MAX				9//TBD
#define	MIN				0//TBD
#define NORMALMODE		0
#define ONLYWINMODE		1

//key map list
#define START			13//return key
#define ADD_COIN		27//esc key
#define TOGGLE_DBG		17//Ctrl

//game state list
#define STANDBY_GAME		0
#define DECISION_SEG0		1
#define DECISION_SEG1		2
#define DECISION_SEG2		3
#define RESULT_JUDGE		4
#define MESSEAGE_WIN		5
#define MESSEAGE_WIN_CLR	6

//other
#define KBEVENT_AVAILABLE	1

//seg rotate mode
#define ROTATE_SEG			2
#define DECERATE_SEG		1
#define STOPPED_SEG			0

//seg ctrl mode
#define ROTATE_REQ			0
#define SELECT_REQ			1
#define PREVIOUA_SEG_REQ	2


void MouseEvent(int x, int y, int button, int event);
void KeyboardEvent(int key, int event);
int upper(int disp);
int lower(int disp);
void timerEvent(int key);
int Setup(void);
int slot(int *wallet, int *disp_num);
int delay_ctrl(int *rand_num,int *disp_num,int state);
void delay_ctrl_seg(int* rand_num, int* disp_num, int *seg_rotate_stt, int dispMode, int seg);


int timerID = 0,flag = 0,wallet = DEFAULT_WALLET;
char img_name[N_IMG][32] = {"..\\img\\slot_0.jpg","..\\img\\slot_1.jpg","..\\img\\slot_2.jpg","..\\img\\slot_3.jpg","..\\img\\slot_4.jpg",
							"..\\img\\slot_5.jpg","..\\img\\slot_6.jpg","..\\img\\slot_7.jpg","..\\img\\slot_8.jpg","..\\img\\slot_9.jpg",
							"..\\img\\win.jpg","..\\img\\title.jpg",
							"..\\img\\top_ungle.jpg","..\\img\\down_ungle.jpg","..\\img\\lisboa.jpg","..\\img\\medal.jpg"};

int dbgmode = NORMALMODE;

ACL_Image img[N_IMG];
ACL_Sound mp3[2];


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
	if(event == KBEVENT_AVAILABLE){
		if (key == START) {
			flag = 1;
		}
		else if(key == ADD_COIN){
			wallet += 5;
			playSound(mp3[0], 0);
		}
		else if (key == TOGGLE_DBG) {
			if (dbgmode == NORMALMODE) {
				dbgmode = ONLYWINMODE;
			}
			else if (dbgmode == ONLYWINMODE) {
				dbgmode = NORMALMODE;
			}
		}
		//printf("%d\n", key);
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
	int state;
	static int handle = 0;
	static clock_t time0,time1;
	static int disp_num[3] = {7,8,9};
	

	if(flag == 1){
		time0 = clock();
		handle = 1;
	}
	time1 = clock();
	if(handle == 1 && (int)time1 - time0 > HANDLE_T){
		handle = 0;
	}
	
	state = slot(&wallet, disp_num);
	

	beginPaint();
	//seg0
	putImage(&img[lower(disp_num[0])], X_SEG_0, Y_LOWER);
	putImage(&img[disp_num[0]], X_SEG_0, Y_CENTER);
	putImage(&img[upper(disp_num[0])], X_SEG_0, Y_UPPER);
	//seg1
	putImage(&img[lower(disp_num[1])], X_SEG_1, Y_LOWER);
	putImage(&img[disp_num[1]], X_SEG_1, Y_CENTER);
	putImage(&img[upper(disp_num[1])], X_SEG_1, Y_UPPER);
	//seg2
	putImage(&img[lower(disp_num[2])], X_SEG_2, Y_LOWER);
	putImage(&img[disp_num[2]], X_SEG_2, Y_CENTER);
	putImage(&img[upper(disp_num[2])], X_SEG_2, Y_UPPER);

	if(state == MESSEAGE_WIN){
		playSound(mp3[1], 0);
	}
	setBrushColor(WHITE);
	setPenColor(WHITE);
	if(state == MESSEAGE_WIN_CLR){
		putImage(&img[10], X_SEG_0, 200);//TBD
		putImage(&img[15], X_SEG_1, 480);
	}
	else{//clear display
		rectangle(160, 480, WIDTH, 640);
	}

	putImage(&img[11], X_SEG_0, Y_LOWER);
	rectangle(0, 400, WIDTH, 480);
	setPenWidth(1);
	setPenColor(RED);
	line(0, 240, 480, 240);	

	setTextColor(BLACK);
	setTextBkColor(EMPTY);
	setTextSize(70);
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

int slot(int *wallet, int *disp_num)
{
	static int rand_num[3] = {0,0,0};
	static int state = STANDBY_GAME;
	static int busy;
	static clock_t time0,time1;

	if(flag == 1){//keyboard event
		flag = 0;
		if(state == 0){
			if(*wallet >= FEE){
				*wallet -= FEE; 
				state++;
			}
		}
		else if(state >= DECISION_SEG0 && state <= DECISION_SEG2){
			state++;
		}
	}
	
	if(state == DECISION_SEG0){
		rand_num[0] = rand() % PAT + MIN;
	}
	else if(state == DECISION_SEG1){
		if (dbgmode == NORMALMODE) {
			rand_num[1] = rand() % PAT + MIN;
		}
		else if (dbgmode == ONLYWINMODE) {
			rand_num[1] = rand_num[0];
		}
	}
	else if(state == DECISION_SEG2){
		if (dbgmode == NORMALMODE) {
			rand_num[2] = rand() % PAT + MIN;
		}
		else if (dbgmode == ONLYWINMODE) {
			rand_num[2] = rand_num[0];
		}
	}
	else if(state == RESULT_JUDGE && busy == 0){//TBD
		if (rand_num[0] == rand_num[1] && rand_num[0] == rand_num[2]) {//TBD
			state = MESSEAGE_WIN;
		}
		else {
			state = STANDBY_GAME;
		}
	}
	else if(state == MESSEAGE_WIN){
		*wallet += REWARD;
		time0 = clock();
		state++;
	}
	else if(state == MESSEAGE_WIN_CLR){
		time1 = clock();
		if((int)time1 - time0 > WIN_T){
			state = STANDBY_GAME;
		}
	}

	busy = delay_ctrl(rand_num, disp_num, state);
	

	return state;
}


int delay_ctrl(int *rand_num,int *disp_num,int state)
{
	int busy;
	static int seg_rotate_stt[3];

	if (state == DECISION_SEG0) {
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, ROTATE_REQ, 0);//表示する数字を順に変える
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, ROTATE_REQ, 1);//表示する数字を順に変える
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, ROTATE_REQ, 2);//表示する数字を順に変える
	}
	else if (state == DECISION_SEG1) {
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, SELECT_REQ, 0);//目標値で止める
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, ROTATE_REQ, 1);//表示する数字を順に変える
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, ROTATE_REQ, 2);//表示する数字を順に変える
	}
	else if (state == DECISION_SEG2) {
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, SELECT_REQ, 0);//目標値で止める
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, SELECT_REQ, 1);//目標値で止める
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, ROTATE_REQ, 2);//表示する数字を順に変える
	}
	else if (state == RESULT_JUDGE) {
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, SELECT_REQ, 0);//目標値で止める
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, SELECT_REQ, 1);//目標値で止める
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, SELECT_REQ, 2);//目標値で止める
	}
	else {
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, PREVIOUA_SEG_REQ, 0);//前回値を表示する
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, PREVIOUA_SEG_REQ, 1);//前回値を表示する
		delay_ctrl_seg(rand_num, disp_num, seg_rotate_stt, PREVIOUA_SEG_REQ, 2);//前回値を表示する
	}

	if(seg_rotate_stt[0] == STOPPED_SEG && seg_rotate_stt[1] == STOPPED_SEG && seg_rotate_stt[2] == STOPPED_SEG){
		busy = 0;
	}
	else busy = 1;

	return busy;
}

void delay_ctrl_seg(int *rand_num, int *disp_num, int *seg_rotate_stt, int dispMode,int seg) {
	if (dispMode == ROTATE_REQ) {//表示する数字を順に変える
		if (disp_num[seg] == MAX)	disp_num[seg] = MIN;
		else disp_num[seg]++;
		seg_rotate_stt[seg] = ROTATE_SEG;//表示値選択中
	}
	else if (dispMode == SELECT_REQ) {//目標値で止める
		if (disp_num[seg] == rand_num[seg]) {
			if (seg_rotate_stt[seg] == ROTATE_SEG) {
				if (disp_num[seg] == MAX) {
					disp_num[seg] = MIN;
				}
				else {
					disp_num[seg]++;
				}
				seg_rotate_stt[seg] = DECERATE_SEG;//表示値に1度達する
			}

			else if (seg_rotate_stt[seg] == DECERATE_SEG) {
				seg_rotate_stt[seg] = STOPPED_SEG;//表示値に2度達する
			}
		}
		else{
			if (disp_num[seg] == MAX) {
				disp_num[seg] = MIN;
			}
			else {
				disp_num[seg]++;
			}
		}
	}
}