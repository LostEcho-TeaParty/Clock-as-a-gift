/***********************************************
1.0程序内容
调用之前单片机定时器时钟程序
新增基础的ds12c887使用程序

结束更新1系程序 2018/1/26 15:17
***********************************************/
#include <reg51.h>

#define uchar unsigned char
#define uint unsigned int

sbit beep=P2^0;
sbit lcdrs=P2^3;
sbit lcden=P2^2;
sbit s1=P2^1;//功能键
sbit s2=P3^7;//增大建
sbit s3=P3^1;//减小键
sbit s4=P3^0;//闹钟查看健
sbit dscs=P3^3;
sbit dsas=P3^4;
sbit dsrw=P3^5;
sbit dsds=P3^6;
sbit dsirq=P3^2;

bit flag1,flag_ri,tnum,ri_enable,full_enable,apnum;
/**************************************************
flag1=闹铃设置全局变量
flag_ri=闹铃中断信号
tbnum=调整按键变动信号
ri_enable=闹铃允许变量
full_enable=24小时制允许变量
apnum=AM/PM控制变量
**************************************************/
uchar count,s1num,flag;
/**************************************************
count=定时器计数变量（可能不会用到，考虑中断运行双程序）
s1num=s1按键全局变量
flag=正常显示确定位
**************************************************/
char miao,shi,fen,year,mouth,day,we,amiao,afen,ashi;
/**************************************************
无所谓char or uchar
**************************************************/
uchar code atable[]=" 20  -  -       ";//液晶固定显示内容
uchar code btable[]="      :  :      ";

void delayms(uint z);
void di();
void write_com(uchar com);
void write_date(uchar date);
void init();
void write_sfm(uchar add,char date);//1602刷新时分秒函数 时为4 分为7 秒为10
void write_nyr(uchar add,char date);//1602刷新年月日函数 年为3 月为6 日为9
void write_week(char we);//1602刷新星期函数 位数为12
void keyscan();
void write_ds(uchar add,uchar date);
uchar read_ds(uchar add);
void set_time();//首次使用
void read_alarm();

void main()
{
	init();
	
	
	while(1)
	{
		keyscan();
		if(flag_ri==1)
		{
			di();
			delayms(100);
			di();
			delayms(500);
		}
		
		
		if(s1num==0&&flag==0)
		{
			if(ri_enable==1)
			{
				write_com(0x80+0x40);
				write_date('R');
				write_date('i');
			}
			if(full_enable==1)
			{
				shi=read_ds(4);
			}
			else
			{
				apnum=read_ds(4);
				apnum=apnum&0x80;
				if(apnum==0)
				{
					write_com(0x80+0x40+13);
					write_date('A');
					write_date('m');
					shi=read_ds(4);
				}
				else
				{
					write_com(0x80+0x40+13);
					write_date('P');
					write_date('m');
					shi=read_ds(4);
					shi=shi<<1;
					shi=shi>>1;
				}
			}
			year=read_ds(9);
			mouth=read_ds(8);
			day=read_ds(7);
			we=read_ds(6);
			fen=read_ds(2);
			miao=read_ds(0);
			write_sfm(10,miao);
			write_sfm(7,fen);
			write_sfm(4,shi);
			write_week(we);
			write_nyr(3,year);
			write_nyr(6,mouth);
			write_nyr(9,day);
		}
	}
	
}

uchar read_ds(uchar add)
{
	uchar ds_date;
	dsas=1;
	dsds=1;
	dsrw=1;
	dscs=0;
	P1=add;
	dsas=0;
	dsds=0;
	P1=0xff;
	ds_date=P0;
	dsds=1;
	dsas=1;
	dscs=1;
	return ds_date;
}

void delayms(uint z)
{
	uint x,y;
	for(x=z;z>0;x--)
		for(y=123;y>0;y--);
}
void di()
{
	beep=0;
	delayms(100);
	beep=1;
}

void write_com(uchar com)
{
	lcdrs=0;
	P0=com;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void write_date(uchar date)
{
	lcdrs=1;
	P0=date;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void init()
{
	uchar num,bcon;
	EA=1;//总中断
	EX0=1;//外部中断0
	IT1=1;//负跳变沿触发
	flag1=0;
	s1num=0;
	
	write_ds(0x0a,0x20);//一次
	write_ds(0x0b,0x26);
	set_time();
	
	bcon=read_ds(0x0b);
	bcon=bcon&0x22;
	switch(bcon)
	{
		case 0x22:
			ri_enable=1;
			full_enable=1;
			break;
		case 0x02:
			ri_enable=0;
			full_enable=1;
			break;
		case 0x20:
			ri_enable=1;
			full_enable=0;
			break;
		case 0x00:
			ri_enable=0;
			full_enable=0;
	}
	
	lcden=0;
	write_com(0x38);//1602init
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
	write_com(0x80);
	for(num=0;num<9;num++)
	{
		write_date(atable[num]);
		delayms(5);
	}
	write_com(0x80+0x40);
	for(num=0;num<10;num++)
	{
		write_date(btable[num]);
		delayms(5);
	}
}

void write_sfm(uchar add,char date)
{//1602刷新时分秒函数 时为4 分为7 秒为10
	char shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+0x40+add);
	write_date(0x30+shi);
	write_date(0x30+ge);
}

void write_nyr(uchar add,char date)
{//1602刷新年月日函数 年为3 月为6 日为9
	char shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+add);
	write_date(0x30+shi);
	write_date(0x30+ge);
}

void write_week(char we)
{//1602刷新星期函数 位数为12
	uchar m;
	uchar code table1[]="MON";
	uchar code table2[]="TUE";
	uchar code table3[]="WED";
	uchar code table4[]="THU";
	uchar code table5[]="FRI";
	uchar code table6[]="SAT";
	uchar code table7[]="SUN";
	write_com(0x80+12);
	for(m=0;m<3;m++)
	{
		switch(we)
		{
			case 1:
				write_date(table1[m]);
				break;
			case 2:
				write_date(table2[m]);
				break;
			case 3:
				write_date(table3[m]);
				break;
			case 4:
				write_date(table4[m]);
				break;
			case 5:
				write_date(table5[m]);
				break;
			case 6:
				write_date(table6[m]);
				break;
			case 7:
				write_date(table7[m]);
				break;	
		}
	}
}

void keyscan()
{
	uchar bcon;
	if(flag_ri==1)//按任意键停止闹铃
	{
		if((s1==0)||(s2==0)||(s3==0)||(s4==0))
		{
			delayms(5);
			if((s1==0)||(s2==0)||(s3==0)||(s4==0))
			{
				while(!(s1&&s2&&s3&&s4));
				di();
				flag_ri=0;
			}
		}
	}
	if(s1==0)
	{
		delayms(5);
		if(s1==0)
		{
			s1num++;
			switch(s1num)
			{
				case 1:
					write_com(0x80+0x40+13);
					write_com(0x0f);
					tnum=0;
					break;
				case 2:
					write_com(0x80+0x40+10);
					break;
				case 3:
					write_com(0x80+0x40+7);
					break;
				case 4:
					write_com(0x80+0x40+4);
					break;
				case 5:
					write_com(0x80+0x40);
					break;
				case 6:
					write_com(0x80+12);
					break;
				case 7:
					write_com(0x80+9);
					break;
				case 8:
					write_com(0x80+6);
					break;
				case 9:
					write_com(0x80+3);
					break;
				case 10:
					s1num=0;
					write_com(0x0c);
					if(tnum==1)
					{
						write_ds(0x0b,0x84);
						write_ds(0,miao);
						write_ds(2,fen);
						write_ds(4,shi);
						write_ds(6,we);
						write_ds(7,day);
						write_ds(8,mouth);
						write_ds(9,year);
						bcon=0x04;
						bcon=bcon+ri_enable*32+full_enable*2;
						write_ds(0x0b,bcon);
					}
					break;
		}
	}
		
	}
	if(s1num!=0)
	{
		if(s2==0)
		{
			delayms(5);
			if(s2==0)
			{
				tnum=1;
				di();
				switch(s1num)
				{
					case 1:
						full_enable=~full_enable;
						break;
					case 2:
						miao++;
						if(miao==60)
						miao=0;
						write_sfm(10,miao);
						write_com(0x80+0x40+10);
						break;
					case 3:
						fen++;
						if(fen==60)
						fen=0;
						write_sfm(7,fen);
						write_com(0x80+0x40+7);
						break;
					case 4:
						shi++;
						if(shi==24)
						shi=0;
						write_sfm(4,shi);
						write_com(0x80+0x40+4);
						break;
					case 5:
						ri_enable=~ri_enable;
						break;
					case 6:
						we++;
						if(we==8)
						we=1;
						write_week(we);
						write_com(0x80+12);
						break;
					case 7:
						day++;
						if(day==32)
						day=1;
						write_nyr(9,day);
						write_com(0x80+9);
						break;
					case 8:
						mouth++;
						if(mouth==13)
						mouth=1;
						write_nyr(6,mouth);
						write_com(0x80+6);
						break;
					case 9:
						year++;
						if(year==100)
						year=0;
						write_nyr(3,year);
						write_com(0x80+3);
						break;
				}	
				delayms(200);
			}
		}
		if(s3==0)
		{
			delayms(5);
			if(s3==0)
			{
				tnum=1;
				di();
				switch(s1num)
				{
					case 1:
						full_enable=~full_enable;
						break;
					case 2:
						if(miao==0)
						miao=60;
						miao--;
						write_sfm(10,miao);
						write_com(0x80+0x40+10);
						break;
					case 3:
						if(fen==0)
						fen=60;
						fen--;
						write_sfm(7,fen);
						write_com(0x80+0x40+7);
						break;
					case 4:
						if(shi==0)
						shi=24;
						shi--;
						write_sfm(4,shi);
						write_com(0x80+0x40+4);
						break;
					case 5:
						ri_enable=~ri_enable;
						break;
					case 6:
						if(we==1)
						we=8;
						we--;
						write_week(we);
						write_com(0x40+12);
						break;
					case 7:
						if(day==1)
						day=32;
						day--;
						write_nyr(9,day);
						write_com(0x40+9);
						break;
					case 8:
						if(mouth==1)
						mouth=13;
						mouth--;
						write_nyr(6,mouth);
						write_com(0x40+6);
						break;
					case 9:
						if(year==0)
						year=100;
						year--;
						write_nyr(3,year);
						write_com(0x40+3);
						break;
				}	
				delayms(200);
				
			}
		}
	}
	if(s4==0)
	{
		delayms(5);
		if(s4==0)
		{
			flag1=~flag1;
			while(!s4);
			di();
			if(flag1==0)
			{//保存闹钟设置
				flag=0;
				
			}
			
			
		}
	}
}


void write_ds(uchar add,uchar date)
{
	dscs=0;
	dsas=1;
	dsds=1;
	dsrw=1;
	P1=add;
	dsas=0;
	dsrw=0;
	P1=date;
	dsrw=1;
	dsas=1;
	dscs=1;
}

void set_time()
{
	write_ds(0,0);
	write_ds(1,0);
	write_ds(2,0);
	write_ds(3,0);
	write_ds(4,0);
	write_ds(5,0);
	write_ds(6,0);
	write_ds(7,0);
	write_ds(8,0);
	write_ds(9,0);
}

void read_alarm()
{//读取12c887闹钟值
	amiao=read_ds(1);
	afen=read_ds(3);
	ashi=read_ds(5);
}

void exter() interrupt 0
{
	uchar c;
	flag_ri=1;
	c=read_ds(0x0c);
}