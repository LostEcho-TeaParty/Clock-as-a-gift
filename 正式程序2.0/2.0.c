/***********************************************
2.0程序目标
确定按键功能（包括两个按键一起按的功能）即keysvan
函数要大幅度修改
完成全部的闹钟功能（包括查看，调整，以及正常显示
时的闹钟提示）
全面完成12小时制功能
初始化一次ds12c887

注意 
本次使用的单片机为stc12c5a16s2，速度为51的12倍
需要修改delayms函数

2.0版本日志
新发现：该ds12c887芯片12小时制与24小时制不同
其中12小时制是从1开始的，24小时制是从0开始的，
keyscan函数需要更大的修改，崩溃	>_<

新思路：建立两个数组让1602不断显示数组内容，只要
更改数组内容即可，考虑到会将整个程序大幅度改动，
放弃该思路，以后有机会再去实现

新程序在硬件上无法工作，原来的程序工作正常，不知
道为什么。如果这件事解决不了，所有的工作都是白费
结果是玄学问题

该程序存在巨大的问题，且硬件不能工作，不知是程序的
还是硬件接线的问题，目前确定ds12c887芯片是没有问题
的，改程序可以暂时作废，以后有时间再弄
***********************************************/
#include <reg51.h>

#define uchar unsigned char
#define uint unsigned int

sbit beep=P2^0;
sbit rs=P2^3;
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

bit flag1,flag_ri,tnum,ri_enable,full_enable,apnum,ap,chnum;
/**************************************************
flag1=闹铃设置全局变量
flag_ri=闹铃中断信号
tbnum=调整按键变动信号
ri_enable=闹铃允许变量
full_enable=24小时制允许变量
apnum=AM/PM控制变量（供realtime用）
ap是供闹钟显示使用的 注意与apnum分开
chnum=闹钟与realtime调整切换变量
**************************************************/
uchar count,s1num,flag,hour,arap;
/**************************************************
count=定时器计数变量（可能不会用到，考虑中断运行双程序）
s1num=s1按键全局变量
flag=闹钟与realtime显示切换变量
hour,arap都是供write_aphour函数使用的变量，不单独使用，
必须紧跟该函数后使用，否则会产生错误
**************************************************/
char miao,shi,fen,year,mouth,day,we,amiao,afen,ashi;
/**************************************************
无所谓char or uchar
**************************************************/
uchar code atable[]=" 20  -  -       ";//液晶固定显示内容
uchar code btable[]="      :  :      ";

void delayms(uint xms);
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
void write_aphour(char ra);//ra为realtime or alarm 该函数可在1602上显示AM/PM，并得出小时值，值为hour

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

void delayms(uint xms)
{
	uint i,j;
	for(i=xms;i>0;i--)
		for(j=123;j>0;j--);
}
void di()
{
	beep=0;
	delayms(100);
	beep=1;
}

void write_com(uchar com)							 
{
	rs=0;
	P0=com;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void write_date(uchar date)
{
	rs=1;
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
	flag_ri=0;
	
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
	if(s1==0&&s2==1)
	{
		delayms(5);
		if(s1==0&&s2==1)
		{
			while(!s1);
			di();
			s1num++;
			if(s1num==1&&full_enable==0)
			{
				s1num=2;
			}
			if(chnum==1&&s1num==5)
			{
				s1num=9;
			}
			
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
					write_com(0x80+12);
					break;
				case 6:
					write_com(0x80+9);
					break;
				case 7:
					write_com(0x80+6);
					break;
				case 8:
					write_com(0x80+3);
					break;
				case 9:
					s1num=0;
					write_com(0x0c);
					if(chnum==0&&tnum==1)
					{//保存时钟设置
						write_ds(0x0b,0x84);
						write_ds(0,miao);
						write_ds(2,fen);
						if(full_enable==0)
						{
							shi=shi+0x80*apnum;
						}
						write_ds(4,shi);
						write_ds(6,we);
						write_ds(7,day);
						write_ds(8,mouth);
						write_ds(9,year);
						bcon=0x04;
						bcon=bcon+ri_enable*32+full_enable*2;
						write_ds(0x0b,bcon);
					}
					if(chnum==1&&tnum==1)
					{//保存闹钟设置
						write_ds(1,amiao);
						write_ds(3,afen);
						if(full_enable==0)
						{
							ashi=ashi+0x80*ap;
						}
						write_ds(5,ashi);
					}
					break;
		}
	}
		
	}
	if(s1num!=0)
	{
		if(s2==0&&s1==1)
		{
			delayms(5);
			if(s2==0&&s1==1)
			{
				tnum=1;
				//看效果来确定是否添加while(!sd2);希望通过调整delayms函数达到连按的效果     
				di();
				if(chnum==0)
				{
					switch(s1num)
					{
						case 1:
							apnum=~apnum;
							if(apnum==0)
						  {
						  	write_date('A');
						  	write_date('M');
						  }
						  else
						  {
						  	write_date('P');
						  	write_date('M');
						  }
						  write_com(0x80+0x40+13);
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
							if(full_enable==1)
							{
								if(shi==24)
									shi=0;
							}
							else
							{
								if(shi==13)
									shi=1;
							}
							write_sfm(4,shi);
							write_com(0x80+0x40+4);
							break;
						case 5:
							we++;
							if(we==8)
							we=1;
							write_week(we);
							write_com(0x80+12);
							break;
						case 6:
							day++;
							if(day==32)
							day=1;
							write_nyr(9,day);
							write_com(0x80+9);
							break;
						case 7:
							mouth++;
							if(mouth==13)
							mouth=1;
							write_nyr(6,mouth);
							write_com(0x80+6);
							break;
						case 8:
							year++;
							if(year==100)
							year=0;
							write_nyr(3,year);
							write_com(0x80+3);
							break;
					}	
				}
				else//闹铃调时间之+
				{
					switch(s1num)
					{
						case 1:
							ap=~ap;
							if(ap==0)
						  {
						  	write_date('A');
						  	write_date('M');
						  }
						  else
						  {
						  	write_date('P');
						  	write_date('M');
						  }
						  write_com(0x80+0x40+13);
						  break;
						case 2:
							amiao++;
							if(amiao==60)
							amiao=0;
							write_sfm(10,amiao);
							write_com(0x80+0x40+10);							
							break;
						case 3:
							afen++;
							if(afen==60)
							afen=0;
							write_sfm(7,afen);
							write_com(0x80+0x40+7);
							break;
						case 4:
							ashi++;
							if(full_enable==1)
							{
								if(ashi==24)
									ashi=0;
							}
							else
							{
								if(ashi==13)
									ashi=1;
							}
							write_sfm(4,ashi);
							write_com(0x80+0x40+4);
							break;
				}
				delayms(200);
			}
		}
		if(s3==0&&s4==1)
		{
			delayms(5);
			if(s3==0&&s4==1)
			{
				tnum=1;
				//看效果确定是否添加while(!s3);
				di();
				if(chnum==0)
				{
					switch(s1num)
					{
						case 1:
							apnum=~apnum;
							if(apnum==0)
							{
								write_date('A');
								write_date('M');
							}
							else
							{
								write_date('P');
								write_date('M');
							}
							write_com(0x80+0x40+13);
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
							if(full_enable==1)
							{
								if(shi==0)
									shi=24;
							}
							else
							{
								if(shi==1)
									shi=13;
							}								
							shi--;
							write_sfm(4,shi);
							write_com(0x80+0x40+4);
							break;
						case 5:
							if(we==1)
							we=8;
							we--;
							write_week(we);
							write_com(0x40+12);
							break;
						case 6:
							if(day==1)
							day=32;
							day--;
							write_nyr(9,day);
							write_com(0x40+9);
							break;
						case 7:
							if(mouth==1)
							mouth=13;
							mouth--;
							write_nyr(6,mouth);
							write_com(0x40+6);
							break;
						case 8:
							if(year==0)
							year=100;
							year--;
							write_nyr(3,year);
							write_com(0x40+3);
							break;
					}	
				}
				else//闹铃调时间之-
				{
					switch(s1num)
					{
						case 1:
							ap=~ap;
							if(ap==0)
						  {
						  	write_date('A');
						  	write_date('M');
						  }
						  else
						  {
						  	write_date('P');
						  	write_date('M');
						  }
						  write_com(0x80+0x40+13);
						  break;
						case 2:
							if(amiao==0)
							amiao=60;
							amiao--;
							write_sfm(10,amiao);
							write_com(0x80+0x40+10);							
							break;
						case 3:
							if(afen==0)
							afen=60;
							afen--;
							write_sfm(7,afen);
							write_com(0x80+0x40+7);
							break;
						case 4:
							if(full_enable==1)
							{
								if(shi==0)
									shi=24;
							}
							else
							{
								if(shi==1)
									shi=13;
							}								
							shi--;
							write_sfm(4,shi);
							write_com(0x80+0x40+4);
							break;
					}
				}
				delayms(200);
				
			}
		}
	}
	}
	if(s4==0&&s3==1)
	{
		delayms(5);
		if(s4==0&&s3==1)
		{
			flag1=~flag1;
			while(!s4);
			di();
			if(flag1==0)
			{//结束闹钟显示
				flag=0;
				chnum=0;
				write_com(0x80+0x40+2);
				write_date(' ');
				write_date(' ');
				if(ri_enable==0)
				{
					write_com(0x80+0x40);
					write_date(' ');
					write_date(' ');
				}
			}
			if(flag1==1)
			{//进入闹钟设置
				flag=1;
				chnum=1;
				read_alarm();
				write_com(0x80+0x40);
				write_date('R');
				write_date('i');
				write_date('A');
				write_date('D');
				write_sfm(4,ashi);
				write_sfm(7,afen);
				write_sfm(10,amiao);
				if(full_enable==0)
				{
					write_com(0x80+0x40+13);
					if(ap==0)
					{
						write_date('A');
						write_date('M');
					}
					else
					{
						write_date('P');
						write_date('M');
					}					
				}

			}
		}
	}
	if(s1==0&&s2==0)
	{
		delayms(5);
		if(s1==0&&s2==0)
		{
			while(!(s1&&s2));
			di();
			full_enable=~full_enable;
			bcon=0x04;
			bcon=bcon+ri_enable*32+full_enable*2;
			write_ds(0x0b,bcon);
			write_aphour(0);
//			if(full_enable==0)
//			{
//				write_com(0x80+0x40+13);
//				if(apnum==0)
//				{
//					write_date('A');
//					write_date('M');
//				}
//				else
//				{
//					write_date('P');
//					write_date('M');
//				}
//			}
		}
	}
	if(s3==0&&s4==0)
	{
		delayms(5);
		if(s3==0&&s4==0)
		{
			while(!(s3&&s4));
			di();
			ri_enable=~ri_enable;
			bcon=0x04;
			bcon=bcon+ri_enable*32+full_enable*2;
			write_ds(0x0b,bcon);			
		}
		write_com(0x80+0x40);
		if(ri_enable==0)
		{
			write_date(' ');
			write_date(' ');
		}
		else
		{
			write_date('R');
			write_date('i');
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
	write_aphour(1);
	ashi=hour;
	ap=arap;
}

void write_aphour(char ra)
{
	uchar conb,conap;
	hour=0;arap=0;
	conb=read_ds(0x0b);
	conb=conb&0x02;
	switch(conb)
	{
		case 0x00:
			conap=1;
			break;
		case 0x02:
			conap=0;
			break;
	}
	if(conap==1)
	{
		arap=read_ds(4+ra);
		arap=arap&0x80;
		if(arap==0)
		{
			write_com(0x80+0x40+13);
			write_date('A');
			write_date('m');
			hour=read_ds(4+ra);
			arap=0;
		}
		else
		{
			write_com(0x80+0x40+13);
			write_date('P');
			write_date('m');
			hour=read_ds(4+ra);
			hour=hour<<1;
			hour=hour>>1;
			arap=1;
		}
	}
	else
	{
		hour=read_ds(4+ra);
	}
}

void exter() interrupt 0
{
	uchar c;
	flag_ri=1;
	c=read_ds(0x0c);
}