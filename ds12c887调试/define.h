#define uchar unsigned char 
#define uint unsigned int
sbit rw=P2^5;
sbit rs=P2^6;
sbit lcden=P2^7;
sbit s1=P2^4;//功能键
sbit s2=P2^3;//增加键
sbit s3=P2^2;//减小键
sbit s4=P2^1;//闹钟查看键
sbit beep=P2^0;//蜂鸣器
sbit dscs=P3^4;
sbit dsas=P3^5;
sbit dsrw=P3^6;
sbit dsds=P3^7;
sbit dsirq=P3^3;

bit flag1,flag_ri;//定义两个位变量
uchar count,s1num,flag,t0_num;//其它变量定义
char miao,shi,fen,year,month,day,week,amiao,afen,ashi;
uchar code table[]=" 20  -  -      ";//液晶固定显示内容
uchar code table1[]="      :  :  ";
void write_ds(uchar,uchar);//函数申明
void set_alarm(uchar,uchar,uchar);void read_alarm();
uchar read_ds(uchar);void set_time();
