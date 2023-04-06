#include <at89x51.h>

#define TIME 20

static unsigned char sleep = TIME;
static unsigned int count;
static unsigned char flag = 1;
static unsigned char yellowLED = 0;
static unsigned char leftLED = 0;
static unsigned char time = TIME;
static unsigned char button = TIME;
static unsigned char walking_time = TIME - 10;
unsigned char code number[] = {
0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void showTime();

void Delay1ms(unsigned int time){
	int i, j;
	for (i = 0; i < time; i++){
		for (j = 0; j < 120; j++){;}
	}
}

void intt1() interrupt 1{
	TH0 = 15535 / 256;
	TL0 = 15535 % 256;
	count++;
	if (count == sleep * 20){
		count = 0;
		yellowLED = 0;
		leftLED = 0;
		sleep = button;
		time = sleep;
		flag = flag ^ 1;
		walking_time = sleep - 10;
	}
	else if (count % 20 == 0){
		time--;
		if (walking_time > 0){
			walking_time--;
		}
	}
	if (count == sleep * 20 - 60 || count == (sleep / 2) * 20 - 60){
		yellowLED = 1;
	}
	if (count == (sleep / 2) * 20){
		leftLED = 1;
		yellowLED = 0;
	}
}

void init(){
	P0 = 0x00;
	P1 = 0x00;
	P2 = 0x00;
	P3 = 0x00;
	TMOD = 0x01;
	TH0 = -50000 / 256;
	TL0 = -50000 % 256;
	ET0 = 1;
	EA = 1;
	TR0 = 1;
}

void event(){
	unsigned char busy;
	unsigned char add;
	unsigned char subtract;
	P3_2 = 1;
	P3_6 = 1;
	P3_3 = 1;
	P3_4 = 1;
	busy = P3_6;
	add = P3_3;
	subtract = P3_4;
	Delay1ms(20);
	if (busy == 0){
		ET0 = 0;
		EA = 1;
		do{
			Delay1ms(20);
			P3_2 = 1;
			P3_6 = 1;
			P2_7 = 1;
			busy = P3_6;
			Delay1ms(20);
		}while(busy == 1);
		P2_7 = 0;
		ET0 = 1;
	}
	if (add == 0){
		Delay1ms(20);
		button += 1;
		add = 1;
		P2_7 = 1;
		Delay1ms(200);
		P2_7 = 0;
	}
	if (subtract == 0){
		Delay1ms(20);
		button -= 1;
		subtract = 1;
		P2_7 = 1;
		Delay1ms(200);
		P2_7 = 0;
	}
}

void showTime(){
	P0 = number[time / 10];
	P2 = 0x05;
	Delay1ms(5);
	P2 = 0x00;
	P0 = number[time % 10];
	P2 = 0x0A;
	Delay1ms(5);
	P2 = 0x00;
	P0 = number[walking_time / 10];
	P2 = 0x20;
	Delay1ms(5);
	P2 = 0x00;
	P0 = number[walking_time % 10];
	P2 = 0x10;
	Delay1ms(5);
	P2 = 0x00;
}

void main(){
	init();
	P0 = number[time];
	P2_3 = 1;
	P0 = number[time];
	while(1){
		if (flag == 1 && yellowLED == 0){
			if (leftLED == 0){
				P1 = 0x82;
				P3 = 0x81;
			}
			else if (leftLED == 1){
				P1 = 0x89;
				P3 = 0x80;
			}
			
			showTime();
			event();
		}
		else if (flag == 0 && yellowLED == 0){
			if (leftLED == 0){
				P1 = 0x28;
				P3 = 0x02;
			}
			else if (leftLED == 1){
				P1 = 0x98;
				P3 = 0x00;
			}
			showTime();
			event();
		}
		if (flag == 1 && yellowLED == 1){
			P3 = 0x81;
			P1 = 0x84;
			showTime();
			event();
		}
		else if (flag == 0 && yellowLED == 1){
			P3 = 0x02;
			P1 = 0x48;
			showTime();
			event();
		}
	}
}