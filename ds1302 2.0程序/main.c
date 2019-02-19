#include<reg51.h>
#include"lcd.h"
#include"ds1302.h"

sbit s1=P2^1;
sbit s2=P3^7;
sbit s3=P3^1;
sbit s4=P3^0;	
sbit beep=P2^0;

bit flag;
uchar miao,fen,shi,ri,yue,we,nian,s1num;

void LCD_Display();
void delayms();
void di();
void write_week(uchar we);
void keyscan();
void init();
void write_sfm(uchar add,uchar date);
void write_nyr(uchar add,uchar date);


void main()
{
	init();
	while(1)
	{
		if(flag==1)
		{
			Ds1302ReadTime();
			LCD_Display();
			keyscan();
			miao=(TIME[0]/16*10+TIME[0]%16);
			fen=(TIME[1]/16*10+TIME[1]%16);
			shi=(TIME[2]/16*10+TIME[2]%16);
			ri=(TIME[3]/16*10+TIME[3]%16);
			yue=(TIME[4]/16*10+TIME[4]%16);
			we=(TIME[5]/16*10+TIME[5]%16);
			nian=(TIME[6]/16*10+TIME[6]%16);		
		}		
		else
		{
			while(!flag)
			{
				keyscan();
			}
		}
	}
}

void LCD_Display()
{
	LcdWriteCom(0x80+0X40+4);
	LcdWriteData('0'+TIME[2]/16);				//时
	LcdWriteData('0'+(TIME[2]&0x0f));				 
	LcdWriteData(':');
	LcdWriteData('0'+TIME[1]/16);				//分
	LcdWriteData('0'+(TIME[1]&0x0f));	
	LcdWriteData(':');
	LcdWriteData('0'+TIME[0]/16);				//秒
	LcdWriteData('0'+(TIME[0]&0x0f));
	LcdWriteCom(0x80+1);
	LcdWriteData('2');
	LcdWriteData('0');
	LcdWriteData('0'+TIME[6]/16);			//年
	LcdWriteData('0'+(TIME[6]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+TIME[4]/16);			//月
	LcdWriteData('0'+(TIME[4]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+TIME[3]/16);			//日
	LcdWriteData('0'+(TIME[3]&0x0f));
	write_week(TIME[5]);	//星期			
}

void delayms(uchar xms)   //误差 0us
{
    unsigned char a,b,c;
    for(c=xms;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}

void di()
{
	beep=0;
	delayms(100);
	beep=1;
}

void write_week(uchar we)
{
	{//1602刷新星期函数 位数为12
		uchar m;
		uchar code table1[]="MON";
		uchar code table2[]="TUE";
		uchar code table3[]="WED";
		uchar code table4[]="THU";
		uchar code table5[]="FRI";
		uchar code table6[]="SAT";
		uchar code table7[]="SUN";
		LcdWriteCom(0x80+12);
		for(m=0;m<3;m++)
		{
			switch(we)
			{
				case 1:
					LcdWriteData(table1[m]);
					break;
				case 2:
					LcdWriteData(table2[m]);
					break;
				case 3:
					LcdWriteData(table3[m]);
					break;
				case 4:
					LcdWriteData(table4[m]);
					break;
				case 5:
					LcdWriteData(table5[m]);
					break;
				case 6:
					LcdWriteData(table6[m]);
					break;
				case 7:
					LcdWriteData(table7[m]);
					break;	
			}
		}
	}
}

void keyscan()
{
	uchar m,n;
	if(s1==0)
	{
		delayms(5);
		if(s1==0)
		{
			s1num++;
			while(!s1);
			di();
			if(s1num==1)
			{
				LcdWriteCom(0x80+0x40+10);
				LcdWriteCom(0x0f);
				flag=0;
			}
			if(s1num==2)
			{
				LcdWriteCom(0x80+0x40+7);
			}
			if(s1num==3)
			{
				LcdWriteCom(0x80+0x40+4);
			}
			if(s1num==4)
			{
				LcdWriteCom(0x40+12);
			}
			if(s1num==5)
			{
				LcdWriteCom(0x40+9);		
			}
			if(s1num==6)
			{
				LcdWriteCom(0x40+6);
			}
			if(s1num==7)
			{
				LcdWriteCom(0x40+3);
			}
			if(s1num==8)
			{
				s1num=0;
				LcdWriteCom(0x0c);
				for(m=0;m<7;m++)
				{
					switch(m)
					{
						case 0:
							TIME[m]=miao;
							break;
						case 1:
							TIME[m]=fen;
							break;
						case 2:
							TIME[m]=shi;
							break;
						case 3:
							TIME[m]=ri;
							break;
						case 4:
							TIME[m]=yue;
							break;
						case 5:
							TIME[m]=we;
							break;
						case 6:
							TIME[m]=nian;
							break;
					}
					if(TIME[m]/10>0)
					{
						n=TIME[m]/10;
						TIME[m]=TIME[m]+6*n;
					}
				}
				Ds1302Init();
				flag=1;
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
				di();
				switch(s1num)
				{
				case 1:
					miao++;
					if(miao==60)
					miao=0;
					write_sfm(10,miao);
					LcdWriteCom(0x80+0x40+10);
					break;
				case 2:
					fen++;
					if(fen==60)
					fen=0;
					write_sfm(7,fen);
					LcdWriteCom(0x80+0x40+7);
					break;
				case 3:
					shi++;
					if(shi==24)
					shi=0;
					write_sfm(4,shi);
					LcdWriteCom(0x80+0x40+4);
					break;
				case 4:
					we++;
					if(we==8)
					we=1;
					write_week(we);
					LcdWriteCom(0x40+12);
					break;
				case 5:
					ri++;
					if(ri==32)
					ri=1;
					write_nyr(9,ri);
					LcdWriteCom(0x40+9);
					break;
				case 6:
					yue++;
					if(yue==13)
					yue=1;
					write_nyr(6,yue);
					LcdWriteCom(0x40+6);
					break;
				case 7:
					nian++;
					if(nian==100)
						nian=0;
					write_nyr(3,nian);
					LcdWriteCom(0x40+3);
					break;
				}	
			}
		}
		if(s3==0)
		{
			delayms(5);
				if(s3==0)
			{
				di();
				switch(s1num)
				{
				case 1:
					if(miao==0)
					miao=60;
					miao--;
					write_sfm(10,miao);
					LcdWriteCom(0x80+0x40+10);
					break;
				case 2:
					if(fen==0)
					fen=60;
					fen--;
					write_sfm(7,fen);
					LcdWriteCom(0x80+0x40+7);
					break;
				case 3:
					if(shi==0)
					shi=24;
					shi--;
					write_sfm(4,shi);
					LcdWriteCom(0x80+0x40+4);
					break;
				 case 4:
					if(we==1)
					we=8;
					we--;
					write_week(we);
					LcdWriteCom(0x40+12);
					break;
				case 5:
					if(ri==1)
					ri=32;
					ri--;
					write_nyr(9,ri);
					LcdWriteCom(0x40+9);
					break;
				case 6:
					if(yue==1)
					yue=13;
					yue--;
					write_nyr(6,yue);
					LcdWriteCom(0x40+6);
					break;
				case 7:
					if(nian==1)
						nian=100;
					nian--;
					write_nyr(3,nian);
					LcdWriteCom(0x40+3);
					break;
				}	
			}	
		}
	}
}

void init()
{
	miao=(TIME[0]/16*10+TIME[0]&0x0f);
	fen=(TIME[1]/16*10+TIME[1]&0x0f);
	shi=(TIME[2]/16*10+TIME[2]&0x0f);
	ri=(TIME[3]/16*10+TIME[3]&0x0f);
	yue=(TIME[4]/16*10+TIME[4]&0x0f);
	we=(TIME[5]/16*10+TIME[5]&0x0f);
	nian=(TIME[6]/16*10+TIME[6]&0x0f);
	s1num=0;
	flag=1;
	LcdInit();
//	Ds1302Init();
}

void write_sfm(uchar add,uchar date)
{
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	LcdWriteCom(0x80+0x40+add);
	LcdWriteData('0'+shi);
	LcdWriteData('0'+ge);	
}

void write_nyr(uchar add,uchar date)
{
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	LcdWriteCom(0x80+add);
	LcdWriteData('0'+shi);
	LcdWriteData('0'+ge);
}