#include <reg51.h>
#define uchar unsigned char
#define uint unsigned int
	
sbit rs=P2^6;
sbit lcden=P2^7;
sbit wr=P2^5;

uchar code table[]="I LOVE MCU!";
uchar code table1[]="TO Yu Pei";
uchar num;

void delayms(uint xms);
void write_com(uchar com);
void write_date(uchar date);
void init();

void main()
{
	init();
	write_com(0x80);
	for(num=0;num<11;num++)
	{
		write_date(table[num]);
		delayms(5);
	}
	write_com(0x80+0x40);
	for(num=0;num<9;num++)
	{
		write_date(table1[num]);
		delayms(5);
	}
	while(1);
}

void delayms(uint xms)
{
	uint i,j;
	for(i=xms;i>0;i--)
		for(j=123;j>0;j--);
}

void write_com(uchar com)							 
{
	rs=0;
	lcden=0;
	wr=0;
	P0=com;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void write_date(uchar date)
{
	rs=1;
	lcden=0;
	wr=0;
	P0=date;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void init()
{
	lcden=0;
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
}