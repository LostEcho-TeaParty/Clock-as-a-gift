#define uchar unsigned char 
#define uint unsigned int
sbit rw=P2^5;
sbit rs=P2^6;
sbit lcden=P2^7;
sbit s1=P2^4;//���ܼ�
sbit s2=P2^3;//���Ӽ�
sbit s3=P2^2;//��С��
sbit s4=P2^1;//���Ӳ鿴��
sbit beep=P2^0;//������
sbit dscs=P3^4;
sbit dsas=P3^5;
sbit dsrw=P3^6;
sbit dsds=P3^7;
sbit dsirq=P3^3;

bit flag1,flag_ri;//��������λ����
uchar count,s1num,flag,t0_num;//������������
char miao,shi,fen,year,month,day,week,amiao,afen,ashi;
uchar code table[]=" 20  -  -      ";//Һ���̶���ʾ����
uchar code table1[]="      :  :  ";
void write_ds(uchar,uchar);//��������
void set_alarm(uchar,uchar,uchar);void read_alarm();
uchar read_ds(uchar);void set_time();
