/***********************************************
2.0����Ŀ��
ȷ���������ܣ�������������һ�𰴵Ĺ��ܣ���keysvan
����Ҫ������޸�
���ȫ�������ӹ��ܣ������鿴���������Լ�������ʾ
ʱ��������ʾ��
ȫ�����12Сʱ�ƹ���
��ʼ��һ��ds12c887

ע�� 
����ʹ�õĵ�Ƭ��Ϊstc12c5a16s2���ٶ�Ϊ51��12��
��Ҫ�޸�delayms����

2.0�汾��־
�·��֣���ds12c887оƬ12Сʱ����24Сʱ�Ʋ�ͬ
����12Сʱ���Ǵ�1��ʼ�ģ�24Сʱ���Ǵ�0��ʼ�ģ�
keyscan������Ҫ������޸ģ�����	>_<

��˼·����������������1602������ʾ�������ݣ�ֻҪ
�����������ݼ��ɣ����ǵ��Ὣ�����������ȸĶ���
������˼·���Ժ��л�����ȥʵ��

�³�����Ӳ�����޷�������ԭ���ĳ�������������֪
��Ϊʲô���������½�����ˣ����еĹ������ǰ׷�
�������ѧ����

�ó�����ھ޴�����⣬��Ӳ�����ܹ�������֪�ǳ����
����Ӳ�����ߵ����⣬Ŀǰȷ��ds12c887оƬ��û������
�ģ��ĳ��������ʱ���ϣ��Ժ���ʱ����Ū
***********************************************/
#include <reg51.h>

#define uchar unsigned char
#define uint unsigned int

sbit beep=P2^0;
sbit rs=P2^3;
sbit lcden=P2^2;
sbit s1=P2^1;//���ܼ�
sbit s2=P3^7;//����
sbit s3=P3^1;//��С��
sbit s4=P3^0;//���Ӳ鿴��
sbit dscs=P3^3;
sbit dsas=P3^4;
sbit dsrw=P3^5;
sbit dsds=P3^6;
sbit dsirq=P3^2;

bit flag1,flag_ri,tnum,ri_enable,full_enable,apnum,ap,chnum;
/**************************************************
flag1=��������ȫ�ֱ���
flag_ri=�����ж��ź�
tbnum=���������䶯�ź�
ri_enable=�����������
full_enable=24Сʱ���������
apnum=AM/PM���Ʊ�������realtime�ã�
ap�ǹ�������ʾʹ�õ� ע����apnum�ֿ�
chnum=������realtime�����л�����
**************************************************/
uchar count,s1num,flag,hour,arap;
/**************************************************
count=��ʱ���������������ܲ����õ��������ж�����˫����
s1num=s1����ȫ�ֱ���
flag=������realtime��ʾ�л�����
hour,arap���ǹ�write_aphour����ʹ�õı�����������ʹ�ã�
��������ú�����ʹ�ã�������������
**************************************************/
char miao,shi,fen,year,mouth,day,we,amiao,afen,ashi;
/**************************************************
����νchar or uchar
**************************************************/
uchar code atable[]=" 20  -  -       ";//Һ���̶���ʾ����
uchar code btable[]="      :  :      ";

void delayms(uint xms);
void di();
void write_com(uchar com);
void write_date(uchar date);
void init();
void write_sfm(uchar add,char date);//1602ˢ��ʱ���뺯�� ʱΪ4 ��Ϊ7 ��Ϊ10
void write_nyr(uchar add,char date);//1602ˢ�������պ��� ��Ϊ3 ��Ϊ6 ��Ϊ9
void write_week(char we);//1602ˢ�����ں��� λ��Ϊ12
void keyscan();
void write_ds(uchar add,uchar date);
uchar read_ds(uchar add);
void set_time();//�״�ʹ��
void read_alarm();
void write_aphour(char ra);//raΪrealtime or alarm �ú�������1602����ʾAM/PM�����ó�Сʱֵ��ֵΪhour

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
	EA=1;//���ж�
	EX0=1;//�ⲿ�ж�0
	IT1=1;//�������ش���
	flag1=0;
	s1num=0;
	flag_ri=0;
	
	write_ds(0x0a,0x20);//һ��
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
{//1602ˢ��ʱ���뺯�� ʱΪ4 ��Ϊ7 ��Ϊ10
	char shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+0x40+add);
	write_date(0x30+shi);
	write_date(0x30+ge);
}

void write_nyr(uchar add,char date)
{//1602ˢ�������պ��� ��Ϊ3 ��Ϊ6 ��Ϊ9
	char shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+add);
	write_date(0x30+shi);
	write_date(0x30+ge);
}

void write_week(char we)
{//1602ˢ�����ں��� λ��Ϊ12
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
	if(flag_ri==1)//�������ֹͣ����
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
					{//����ʱ������
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
					{//������������
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
				//��Ч����ȷ���Ƿ����while(!sd2);ϣ��ͨ������delayms�����ﵽ������Ч��     
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
				else//�����ʱ��֮+
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
				//��Ч��ȷ���Ƿ����while(!s3);
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
				else//�����ʱ��֮-
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
			{//����������ʾ
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
			{//������������
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
{//��ȡ12c887����ֵ
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