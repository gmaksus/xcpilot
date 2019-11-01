//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "pies"
#pragma resource "*.dfm"

#define FSS_SAMPLES   8
#define FAR_SAMPLES   128
#define ALT_SAMPLES   16
#define SPEED_SAMPLES 32 //256
#define ELEV_SAMPLES  256
#define ROLL_SAMPLES  64
#define GYRO_SAMPLES  8
#define OFFSET_SAMPLES 2
//#define PI  3.1415926
#define TRIM_AILERONS  0
#define TRIM_ELEVATOR  0

#define ZLIN   1

//#define Engine1  1

HANDLE port;
DCB *dcb;
COMMTIMEOUTS ct;
DWORD tbc;
DWORD mask;
OVERLAPPED ovr;
volatile int RS232_connected;
char ip_address[100];
int i=0;
int calibration = 0;
typedef struct OUT_DATA{
short int i_pitch;
short int i_roll;
short int i_speed;
short int i_altitude;
short int i_heading;
short int i_tab;
long int i_latitude;
long int i_longitude;
}OUT_DATA;
struct OUT_DATA s;

typedef struct FSSi
{
  short int pitch;
  short int roll;
  short int speed;
  short int altitude;
  short int heading;
}FSSi;
typedef struct FSSo
{
  short int ailerons;
  short int elevator;
  short int rudder;
  short int throttle;
}FSSo;
typedef struct FSSp
{
  short int pitch;
  short int roll;
  short int speed;
}FSSp;
typedef struct FSSc
{
  float Kst_roll;
  float Kast_roll;
  float Kfar_roll;
  float Kst_pitch;
  float Kast_pitch;
  float Kfar_pitch;
  float Kst_speed;
  float Kast_speed;
  float Kst_hdg;
  float Kast_hdg;
  float Kst_alt;
  float Kast_alt;
}FSSc;


#ifdef P51

    #ifdef Engine1
        struct FSSc ctrl={
        0.04, //Kst_roll
        0.0, //Kast_roll
        0.4, //Kfar_roll
        0.2, //Kst_pitch
        0.0, //Kast_pitch
        1.0, //Kfar_pitch
        30.0,//Kst_speed
        1.0, //Kast_speed
        0.4, //Kst_hdg
        1.0, //Kast_hdg
        2.0, //Kst_alt
        5.0  //Kast_alt
        };
    #else
        struct FSSc ctrl={
        0.1, //Kst_roll
        0.1, //Kast_roll
        0.2, //Kfar_roll
        0.1, //Kst_pitch
        0.1, //Kast_pitch
        0.5, //Kfar_pitch
        5.0,//Kst_speed
        0.0, //Kast_speed
        1.0, //Kst_hdg
        1.0, //Kast_hdg
        0.5, //Kst_alt
        4.0  //Kast_alt
        };
    #endif

#elif ZLIN

    #ifdef Engine1
        struct FSSc ctrl={
        0.04, //Kst_roll
        0.0, //Kast_roll
        0.4, //Kfar_roll
        0.2, //Kst_pitch
        0.0, //Kast_pitch
        1.0, //Kfar_pitch
        30.0,//Kst_speed
        1.0, //Kast_speed
        0.4, //Kst_hdg
        1.0, //Kast_hdg
        1.0, //Kst_alt
        2.0  //Kast_alt
        };
    #else
        struct FSSc ctrl={
        0.1, //Kst_roll
        0.1, //Kast_roll
        0.2, //Kfar_roll
        0.1, //Kst_pitch
        0.1, //Kast_pitch
        0.5, //Kfar_pitch
        5.0,//Kst_speed
        0.0, //Kast_speed
        1.0, //Kst_hdg
        2.0, //Kast_hdg
        0.5, //Kst_alt
        4.0  //Kast_alt
        };
    #endif

#else

    #ifdef Engine1
        struct FSSc ctrl={
        0.04, //Kst_roll
        0.0, //Kast_roll
        0.4, //Kfar_roll
        0.2, //Kst_pitch
        0.0, //Kast_pitch
        1.0, //Kfar_pitch
        30.0,//Kst_speed
        1.0, //Kast_speed
        0.4, //Kst_hdg
        1.0, //Kast_hdg
        2.0, //Kst_alt
        5.0  //Kast_alt
        };
    #else
        struct FSSc ctrl={
        0.05, //Kst_roll
        0.05, //Kast_roll
        0.05, //Kfar_roll
        0.1, //Kst_pitch
        0.0, //Kast_pitch
        0.5, //Kfar_pitch
        10.0,//Kst_speed
        0.0, //Kast_speed
        1.0, //Kst_hdg
        2.0, //Kast_hdg
        1.0, //Kst_alt
        2.0  //Kast_alt
        };
    #endif

#endif

/****************************************
#define HEAVY
#define Engine1


#ifdef Engine1
struct FSSc ctrl={0.1, 0.1, 1.0, 0.3, 0.3, 1.0, 30.0, 1.0, 0.4, 1.0, 2.0, 5.0};
#else
struct FSSc ctrl={0.2, 0.0, 0.0, 2.0, 0.0, 0.0, 30.0, 0.0, 0.4, 1.0, 2.0, 5.0};
#endif
******************************************/


typedef struct TEST
{
  int  p1;
  int  p2;
  int  p3;
  int  p4;
}TEST;

typedef struct WPNT
{
  int pid;
  int speed;
  int altitude;
  int heading;
  int latitude;
  int longitude;
}WPNT;

#ifdef P51

struct WPNT waypoint[]={
{1,200,5000,0,33939926,-118380219},
{2,200,5000,0,33962887,-118151146},
{3,200,3000,0,33873245,-117993439},
{4,200,1000,0,33819553,-118330345},
{5,200,3000,0,33914970,-118607941},
{6,120,1000,0,33932472,-118431778},
{7,0,500,0,33937504,-118382729},
{0,0,0,0,0,0}};


#elif ZLIN

struct WPNT waypoint[]={
{1,120,5000,0,33939926,-118380219},
{2,120,5000,0,33962887,-118151146},
{3,120,3000,0,33873245,-117993439},
{4,120,1000,0,33819553,-118330345},
{5,120,3000,0,33914970,-118607941},
{6,60,1000,0,33932472,-118431778},
{7,0,200,0,33937504,-118382729},
{0,0,200,0,0,0}};

#else

struct WPNT waypoint[]={
{1,50,1010,0,33939926,-118380219},
{2,50,1020,0,33962887,-118151146},
{3,50,1030,0,33873245,-117993439},
{4,50,1040,0,33819553,-118330345},
{5,50,1040,0,33930748,-118458900},
{6,50,500,0,33933800,-118419029},
{7,0,500,0,33937504,-118382729},
{0,0,0,0,0,0}};

#endif

/***************************************
#ifdef HEAVY
struct WPNT waypoint[]={
{1,180,2010,0,33939926,-118380219},
{2,180,2020,0,33962887,-118151146},
{3,180,2030,0,33873245,-117993439},
{4,180,2040,0,33819553,-118330345},
{5,180,2040,0,33930748,-118458900},
{6,180,1000,0,33933800,-118419029},
{7,180,500,0,33937504,-118382729},
{0,0,0,0,0,0}};
#else
struct WPNT waypoint[]={
{1,50,1010,0,33939926,-118380219},
{2,50,1020,0,33962887,-118151146},
{3,50,1030,0,33873245,-117993439},
{4,50,1040,0,33819553,-118330345},
{5,50,1040,0,33930748,-118458900},
{6,50,500,0,33933800,-118419029},
{7,0,500,0,33937504,-118382729},
{0,0,0,0,0,0}};
#endif
****************************************/

int HorizDiameter = 140;
int HorizLeft = 635 - HorizDiameter/2;
int HorizTop = 410 - HorizDiameter/2;

typedef struct HRZ
{
  double roll;
  double pitch;
  double yaw;
  double roll_dg;
  double pitch_dg;
  double yaw_dg;
  double course;
}HRZ;

struct HRZ    hrz = {0,0,90,0,0,PI/2,0};

struct FSSi in={0,0,0,0,0};
struct FSSo out={127,127,127,0};;
struct FSSp keep={0,0,3};
struct TEST tst={0, 0, 0, 0};

int roll_sum, roll_dim[FSS_SAMPLES+1], fss_j=0, roll_pr=0, ri=0,
    roll_var_dim[FSS_SAMPLES+1], roll_var_sum, ailerons_corr=0, roll_long_sum=0,
    keep_roll_sum = 0, roll_sumsum=0;
int pitch_sum=0, pitch_dim[FSS_SAMPLES+1], pi=0,
    pitch_var_dim[FSS_SAMPLES+1], pitch_var_sum=0, pitch_last=0, elevator_corr=0, pitch_long_sum=0,
    keep_pitch_sum = 0, pitch_pr = 0;

int offset_dim[OFFSET_SAMPLES+1], offset_sq_dim[OFFSET_SAMPLES+1], offset_var_dim[OFFSET_SAMPLES+1];
int fi=0;
int offset_sum=0, offset_sq_sum=0, offset_var_sum=0, offset_i;
int head_correction_i=0, wind_speed_rdy=0;

double offset_sq, offset_med, offset_var, offset_var_med, hcorrmem=0;
double head_correction =0;
double wind_speed;
double gnd_speed, gnd_speed_mem;
double shift_angle,k_of=0.1, k_ov=0.1;




int alt_error=0, head_error=0, speed_error=0;
int ParA, ParB, ParC;

int speed_sum, speed_dim[FSS_SAMPLES], speed_pr;
    speed_var_dim[FSS_SAMPLES], speed_var_sum;

long int longitude_0=0, latitude_0=0, lat_s, lon_s;
long int longitude_old=0, latitude_old=0;

float dlat, dlon, distance=0, offset = 0, distance_old=0, Azimut=0, Course_rad, TrueAzimut=0;
int  point_number = 0, init_last_point = 0;
int  time_to_point, ttp_sec, ttp_min, ttp_hr, Course_dig;

void FlStSys(FSSi * in, FSSo *out, FSSp * keep, FSSc * ctrl, TEST *tst);
void FlStSys2(FSSi * in, FSSo *out, FSSp * keep, FSSc * ctrl, TEST *tst);



 int seconds=0, mseconds=0;
 int alt_var, alt_var_req, alt_old, head_var, head_old, head_requested=1000, alt_requested=2000;
#ifdef P51
 int speed_requested =200;
 int RadRazv  = 3000;
#elif ZLIN
 int speed_requested =120;
 int RadRazv  = 1000;
#else
 int speed_requested =40;
 int RadRazv  = 200;
#endif

 int speed_var, speed_old;
 bool ap0, ap1, ap2;
 int alt_err_sum=0, head_err_sum, alt_err_long=0, head_err_long;

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{

if(ComboBoxEx1->ItemIndex== -1) ComboBoxEx1->ItemIndex=0;
if(ComboBoxEx2->ItemIndex== -1) ComboBoxEx2->ItemIndex=8;
if(ComboBoxEx3->ItemIndex== -1) ComboBoxEx3->ItemIndex=3;
if(ComboBoxEx4->ItemIndex== -1) ComboBoxEx4->ItemIndex=0;
if(ComboBoxEx5->ItemIndex== -1) ComboBoxEx5->ItemIndex=0;
Edit2->Text =  IdUDPServer1->DefaultPort;
RS232_connected=0;
Form1->Memo1->Lines->Text="";

Form1->Left = 200;
Form1->Top = 100;



  //Form1->HorizMsk->Brush->Color->clWhite;
  //Form1->HorizMsk->Brush->Style->bsClear;
  //Form1->HorizMsk->Pen->Stile = psSolid;
  //Form1->HorizBottom->Brush->Color->cl3DDkShadow;
 Form1->HorizMsk->Left = HorizLeft;//-HorizDiameter/8;
 Form1->HorizMsk->Top  = HorizTop;//-HorizDiameter/8;
 Form1->HorizMsk->Height  = HorizDiameter*3;
 Form1->HorizMsk->Width  = HorizDiameter*3;
 Form1->HorizMsk->Pen->Width  = HorizDiameter;
 Form1->HorizMsk->Pen->Color = clBtnFace;
 Form1->HorizPie->Left = HorizLeft + HorizDiameter/2;
 Form1->HorizPie->Top  = HorizTop + HorizDiameter/2;
 Form1->HorizPie->Height  = HorizDiameter*2;
 Form1->HorizPie->Width  = HorizDiameter*2;
 Form1->HorizPie->Pen->Color = clBtnFace;
 Form1->HorizBottom->Left = HorizLeft + HorizDiameter/2;
 Form1->HorizBottom->Top  = HorizTop + HorizDiameter/2;
 Form1->HorizBottom->Height  = HorizDiameter*2;
 Form1->HorizBottom->Width  = HorizDiameter*2;



}
//---------------------------------------------------------------------------







void __fastcall TForm1::IdUDPServer1UDPRead(TObject *Sender,
      TStream *AData, TIdSocketHandle *ABinding)
{

typedef struct SDATA{
int index;
float data[8];
}SDATA;
char *chin, *chtmp, *outstream;
char chout[100];
char  buffer[1024],buffer1[1024],buf[100];
int length,i;
struct SDATA *simdata1;
struct SDATA *simdata2;
struct SDATA *simdata3;
struct SDATA *simdata4;
float speed, pitch, roll, heading, altitude, latitude, longitude, tspeed;

//short int i_pitch, i_roll, i_speed, i_heading, i_altitude;
//int i_latitude, i_longitude;
 outstream=(char*)&s;
 length = AData->Size;
 AData->Read(buffer,length);
 chin = (char*)(&buffer[6]);

 simdata1 = (SDATA*)(chin);
 simdata4 = (SDATA*)(chin+(1*sizeof(SDATA)));
 simdata2 = (SDATA*)(chin+(2*sizeof(SDATA)));
 simdata3 = (SDATA*)(chin+(3*sizeof(SDATA)));
 speed = simdata1->data[1];
 tspeed = simdata1->data[0];
 pitch = simdata2->data[0];
 roll  = simdata4->data[2];
 heading = 0;//simdata2->data[2];    //3
 altitude = simdata3->data[5];
 latitude = simdata3->data[0];
 longitude = simdata3->data[1];
 chtmp= (chin+(3*sizeof(SDATA)));
 Edit1->Text=chtmp+1;

 sprintf(buf,"%03.1f",pitch);
 Form1->Pitch->Text=buf;
 sprintf(buf,"%03.1f",(float)s.i_roll/10);
 Form1->Roll->Text=buf;
 sprintf(buf,"%03.1f",(float)Course_dig/10);//heading);
 Form1->Heading->Text=buf;
 sprintf(buf,"%03.1f",speed);
 Form1->Speed->Text=buf;
 sprintf(buf,"%03.1f",altitude);
 Form1->Altitude->Text=buf;
 sprintf(buf,"%03.6f",latitude);
 Form1->Latitude->Text=buf;
 sprintf(buf,"%03.6f",longitude);
 Form1->Longitude->Text=buf;

 gnd_speed = (double)tspeed;


//   sprintf(buffer1,"% 3.1f  % 3.1f % 3.1f % 3.1f % 3.1f %f %f \r\n",
//   pitch, roll, speed, heading, altitude, latitude, longitude);
//   Memo1->Lines->Add(buffer1);


/*   sprintf(buffer1,"");
   for(i=0; i<length; i++)
   {
   sprintf(chout,"%02x",chin[i]);
   strncat(buffer1,chout,2);
   }
  */


    /* sprintf(buffer1,"%02d  % 3.3f  % 3.3f % 3.3f % 3.3f % 3.3f % 3.3f % 3.3f % 3.3f\r\n",
   simdata1->index,simdata1->data[0],simdata1->data[1],simdata1->data[2],
   simdata1->data[3],simdata1->data[4],simdata1->data[5],simdata1->data[6],
   simdata1->data[7]);*/

s.i_pitch = (short int)(pitch*10);
//s.i_roll = (short int)(roll*10);

//if( roll >= 30 ) roll = 30;       // ����������� roll, �.�. ������� ��� ������� ��������...
//else if( roll <= -30 ) roll = -30; 



s.i_roll = (short int)((10*180/PI)*atan(((roll*PI/180)*speed*1.852)/(3.6*9.8)));


//s.i_roll = (short int)atan((roll*10*speed*1.852)/(3.6*9.8));
s.i_speed = (short int)(speed);
s.i_altitude = (short int)altitude;
s.i_heading = (short int)Course_dig;//(short int)(heading*10);
s.i_tab = 0x5555;
s.i_latitude = (int)(latitude*1000000);
s.i_longitude = (int)(longitude*1000000);

in.pitch=s.i_pitch;
in.roll=s.i_roll;
//in.roll=(s.i_roll*in.speed*(1.852/3.6))/9.8;
in.speed=(short int)(speed);
in.altitude=(short int)(altitude);
in.heading=(short int)(heading);



/*if( RS232_connected==1)
{
 //WriteFile(port,outstream,sizeof(OUT_DATA),&tbc,&ovr);
 WriteFile(port,buffer1,strlen(buffer1),&tbc,&ovr);
 if(WaitForSingleObject(ovr.hEvent,1000)==WAIT_OBJECT_0)
 {
        GetOverlappedResult(port,&ovr,&tbc,FALSE);
 }
  else
   {

       Form1->Memo1->Lines->Add("Transmitt error");

   }

  //     sprintf(buffer1,"%d",tbc);
  //     Form1->Memo1->Lines->Add(buffer1);



 }*/

}
//---------------------------------------------------------------------------


void __fastcall TForm1::IdUDPServer1Status(TObject *axSender,
      const TIdStatus axStatus, const AnsiString asStatusText)
{
//             Memo1->Lines->Add("Status read");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::StartClick(TObject *Sender)
{
char control_buf[10];
char buf[100];
char  buffer[1024];
         IdUDPServer1->Active=true;
         UDP_stat->Caption="Listening...";
       // sprintf(buffer,"test= %d",ComboBoxEx3->ItemIndex);
        //Memo1->Lines->Add(buffer);

 sprintf(buf,"% 5f",ctrl.Kst_roll);
 Form1->Kstroll->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_roll);
 Form1->Kastroll->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kst_pitch);
 Form1->Kstpitch->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_pitch);
 Form1->Kastpitch->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kst_speed);
 Form1->Kstspeed->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_speed);
 Form1->Kastspeed->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kfar_roll);
 Form1->Kfar_roll->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kfar_pitch);
 Form1->Kfar_pitch->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kst_hdg);
 Form1->Kst_hdg->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_hdg);
 Form1->Kast_hdg->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kst_alt);
 Form1->Kst_alt->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_alt);
 Form1->Kast_alt->Text=buf;

 Form1->TrackBar1->Position =  127;
 Form1->TrackBar2->Position =  127;
 Form1->TrackBar3->Position =  127;
 Form1->TrackBar4->Position =  255;



 if( RS232_connected==1)
{

      //  sprintf(buffer,"UDP connected..");
      //  WriteFile(port,buffer,strlen(buffer),&tbc, &ovr);

        if(WaitForSingleObject(ovr.hEvent,10000)==WAIT_OBJECT_0) {
        GetOverlappedResult(port,&ovr,&tbc,FALSE);
        } else {

       Form1->Memo1->Lines->Add("Transmitt error");

        }







  //      sprintf(buffer,"Sent %d bytes", tbc);
  //      Memo1->Lines->Add(buffer);

}

}
//---------------------------------------------------------------------------

void __fastcall TForm1::StopClick(TObject *Sender)
{
         IdUDPServer1->Active=false;
         UDP_stat->Caption="Idle...";

}
//---------------------------------------------------------------------------


void __fastcall TForm1::UDP_statClick(TObject *Sender)
{

UDP_stat->Caption="Idle...";
}
//---------------------------------------------------------------------------







void __fastcall TForm1::Edit2DblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int port;
        sprintf(buffer,"%s",Edit2->Text);
        port=strtod(buffer,endptr);
        sprintf(buffer,"Info:port= %d",port);

        Memo1->Lines->Add(buffer);

        IdUDPServer1->DefaultPort= port;
}
//---------------------------------------------------------------------------





void __fastcall TForm1::Button1Click(TObject *Sender)
{
char *endptr[100];
char buffer[100];
int baud;

if (RS232_connected == 0)
    {

    sprintf(buffer,"%s",ComboBoxEx1->Text);
    port=CreateFile(buffer,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
    Memo1->Lines->Add(buffer);

        if(port == INVALID_HANDLE_VALUE)
        {
                 rs232_stat->Caption="Error: Unable open port.";
        }
        else
        {
         rs232_stat->Caption="Connected...";

        dcb=(DCB*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(DCB));
        dcb->DCBlength=sizeof(DCB);

//         BuildCommDCB("baud=9600] parity=N data=8 stop=1",dcb);

                sprintf(buffer,"%s",ComboBoxEx2->Text);
                baud=strtod(buffer,endptr);
        dcb->BaudRate=baud;
                sprintf(buffer,"Baud is:%d",dcb->BaudRate);
                Memo1->Lines->Add(buffer);
        dcb->fBinary = TRUE;
        dcb->fOutxCtsFlow = CheckBox2->Checked;
        dcb->fOutxDsrFlow = CheckBox1->Checked;
        if (CheckBox1->Checked == true)
                dcb->fDtrControl = DTR_CONTROL_HANDSHAKE;
        else  dcb->fDtrControl = DTR_CONTROL_DISABLE;
       // dcb->fDtrControl = DTR_CONTROL_HANDSHAKE;
        dcb->fDsrSensitivity = CheckBox1->Checked;
        dcb->fOutX = CheckBox3->Checked;
        dcb->fInX = CheckBox3->Checked;

        sprintf(buffer,"%s",ComboBoxEx3->Text);
        dcb->ByteSize=strtod(buffer,endptr);
                sprintf(buffer,"Bits is: %d",dcb->ByteSize);
                Memo1->Lines->Add(buffer);
        dcb->Parity = ComboBoxEx4->ItemIndex;
        dcb->Parity = ComboBoxEx5->ItemIndex;

        if( SetCommState(port,dcb))
        {
                Memo1->Lines->Add("Config OK\r\n");
        }
        

        ct.ReadIntervalTimeout=10;
        ct.ReadTotalTimeoutMultiplier=ct.ReadTotalTimeoutConstant=0;
        ct.WriteTotalTimeoutMultiplier=ct.WriteTotalTimeoutConstant=0;
        SetCommTimeouts(port,&ct);
        PurgeComm(port,PURGE_TXCLEAR|PURGE_RXCLEAR);
//        SetCommMask(port,EV_RXCHAR);
        memset(&ovr,0,sizeof(ovr));
        ovr.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);




//        sprintf(buffer,"Hello world");
//        WriteFile(port,buffer,strlen(buffer),&tbc,NULL);

        uart = new TUartReceive(false);


        RS232_connected = 1;
   }





 }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
     if( RS232_connected == 1)
     {
         CloseHandle(port);
         rs232_stat->Caption="Disconnected...";
         RS232_connected = 0;
      }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Edit2Change(TObject *Sender)
{

        char * endptr[10];
        char buffer[100];
        int port;
        sprintf(buffer,"%s",Edit2->Text);
        port=strtod(buffer,endptr);
       // sprintf(buffer,"Info:port= %d",port);

       // Memo1->Lines->Add(buffer);

        IdUDPServer1->DefaultPort = port;




}
//---------------------------------------------------------------------------



void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
 char control_buf[100];
 char buf[100];
 char buffer[100];
 int tmp, htmp, head_current, alt_current, roll_current, pitch_current;
 int throttle_current, speed_current, stmp;

if(mseconds == 20)
{
 mseconds = 0;
 seconds++;
}
else mseconds++;

        hrz.roll_dg = -in.roll/10;
        //hrz.roll = (in.roll/10)*3.1415926/180;
        hrz.pitch_dg = in.pitch/10;
        //hrz.pitch = (in.pitch/10)*3.1415926/180;



       if( hrz.pitch_dg >=90 ) Form1->HorizPie->Top = HorizTop + HorizDiameter/2 + 90/2;
       else if ( hrz.pitch_dg <=-90 ) Form1->HorizPie->Top = HorizTop + HorizDiameter/2 - 90/2;
       else Form1->HorizPie->Top = HorizTop + HorizDiameter/2 + ((int)hrz.pitch_dg)*(HorizDiameter/2)/90;


   //    if(rl.radiobuttonroll == 1){
   //    if(hrz.yaw_dg >= 0){
          Form1->HorizPie->FAngles->StartAngle=360-hrz.roll_dg;
          Form1->HorizPie->FAngles->EndAngle=180-hrz.roll_dg;
   //     }
   //     else{
   //       Form1->HorizPie->FAngles->StartAngle=180+hrz.roll_dg;
   //       Form1->HorizPie->FAngles->EndAngle=360+hrz.roll_dg;
   //     }
   //   }
   //   else{
   //     Form1->HorizPie->FAngles->StartAngle=360-hrz.roll_dg;
   //     Form1->HorizPie->FAngles->EndAngle=180-hrz.roll_dg;
   //   }


/*
       if(in.roll/10 > 0)
       {
       Form1->Pie1->FAngles->StartAngle=360-abs(in.roll/10);
       Form1->Pie1->FAngles->EndAngle=180-abs(in.roll/10);
       Form1->Pie3->FAngles->StartAngle=180-abs(in.roll/10);
       Form1->Pie3->FAngles->EndAngle=360-abs(in.roll/10);
       }
       else
       {
       Form1->Pie1->FAngles->StartAngle=abs(in.roll/10);
       Form1->Pie1->FAngles->EndAngle=180+abs(in.roll/10);
       Form1->Pie3->FAngles->StartAngle=180+abs(in.roll/10);
       Form1->Pie3->FAngles->EndAngle=abs(in.roll/10);
       }
       Form1->TrackBar5->Position =  127 - in.pitch/5;

 */      if((CheckBox5->Checked == true) || (CheckBox7->Checked == true)|| (CheckBox8->Checked == true)|| (CheckBox6->Checked == true))
       {
            Form1->TrackBar1->Position =  127 + out.ailerons;
            Form1->TrackBar2->Position =  127 + out.elevator;
            Form1->TrackBar3->Position =  127 + out.rudder;
//            Form1->TrackBar4->Position =  255 - out.throttle;
       }
       if(CheckBox9->Checked == true)
       {
            Form1->TrackBar4->Position =  255 - out.throttle;
       }

//clibration joystick
if(CheckBox6->Checked == true)
{
    control_buf[0]=0xFF;
    control_buf[1]=127 + out.ailerons;
    control_buf[2]=127 + out.elevator;
    control_buf[3]=127 + out.rudder;
    control_buf[4]=out.throttle;
    control_buf[5]=0;
    i=i+10;
/***************************************************
    out.ailerons=out.elevator=out.rudder=(i&0xFF)-127;
    out.throttle =i&0xFF;
    control_buf[0]=0xFF;
    control_buf[1]=out.ailerons+127;
    control_buf[2]=out.elevator+127;
    control_buf[3]=out.rudder+127;
    control_buf[4]=out.throttle;
    control_buf[5]=0;
    i=i+10;
***************************************************/

    WriteFile(port,control_buf,strlen(control_buf),&tbc,&ovr);
    if(WaitForSingleObject(ovr.hEvent,1000)==WAIT_OBJECT_0)
    {
        GetOverlappedResult(port,&ovr,&tbc,FALSE);
    }
    else
    {
   //    Form1->Memo1->Lines->Add("Transmitt error");
    }
    if( i >= 512 )
    {
    CheckBox6->Checked =false;
    i=0;
    out.ailerons=out.elevator=out.rudder=1;
    out.throttle = 0;
    }
    else
    {
    out.ailerons=out.elevator=out.rudder=(i&0xFF)-127;
    out.throttle =i&0xFF;
    }


}
else if((CheckBox5->Checked == true) || (CheckBox7->Checked == true)|| (CheckBox8->Checked == true) )
{


#ifdef Engine1
    FlStSys(&in, &out, &keep, &ctrl, &tst);
#else
    FlStSys2(&in, &out, &keep, &ctrl, &tst);
#endif


//if(mseconds%4 == 0)
//{
//      sprintf(buffer,"speed_sum=%d rsl=%6d, ps=%d, psl=%d", tst.p1, tst.p2, tst.p3, tst.p4);
//      Memo1->Lines->Add(buffer);
// }
      //  sprintf(buffer,"roll_sum=%d pitch_sum=%d speed_sum=%d", tst.p1, tst.p2, tst.p3);
      //  Memo1->Lines->Add(buffer);



    control_buf[0]=0xFF;
    control_buf[1]=127 + out.ailerons;
    control_buf[2]=127 + out.elevator;
    control_buf[3]=127 + out.rudder;
        control_buf[4]=  255 - Form1->TrackBar4->Position;
    //control_buf[4]=out.throttle;
    control_buf[5]=0;
/***************************************************
    control_buf[0]=0xFF;
    control_buf[1]=out.ailerons+127;
    control_buf[2]=out.elevator+127;
    control_buf[3]=out.rudder+127;
    control_buf[4]=out.throttle;
    control_buf[5]=0;
***************************************************/





    WriteFile(port,control_buf,strlen(control_buf),&tbc,&ovr);
    if(WaitForSingleObject(ovr.hEvent,1000)==WAIT_OBJECT_0)
    {
        GetOverlappedResult(port,&ovr,&tbc,FALSE);
    }
    else
    {

    //    Form1->Memo1->Lines->Add("Transmitt error");

    }

}else
{

        out.ailerons =  (Form1->TrackBar1->Position - 127)*0.95;
        out.elevator =  (Form1->TrackBar2->Position - 127)*0.95;
        out.rudder   =  (Form1->TrackBar3->Position - 127)*0.95;
        out.throttle =  (255 - Form1->TrackBar4->Position)*0.95;

    control_buf[0]=0xFF;
    control_buf[1]=127 + out.ailerons;
    control_buf[2]=127 + out.elevator;
    control_buf[3]=127 + out.rudder;
    control_buf[4]=out.throttle;
    control_buf[5]=0;

 WriteFile(port,control_buf,strlen(control_buf),&tbc,&ovr);
 if(WaitForSingleObject(ovr.hEvent,1000)==WAIT_OBJECT_0)
 {
        GetOverlappedResult(port,&ovr,&tbc,FALSE);
 }
  else
   {

   //    Form1->Memo1->Lines->Add("Transmitt error");

   }



}


if((CheckBox5->Checked != ap1) && (ap1 == false))
{
    CheckBox7->Checked = false;
    CheckBox8->Checked = false;
 }
if((CheckBox7->Checked != ap2) && (ap2 == false))
{
    CheckBox5->Checked = false;
    CheckBox8->Checked = false;
 }
if((CheckBox8->Checked != ap0) && (ap0 == false))
{
    CheckBox5->Checked = false;
    CheckBox7->Checked = false;
 }






if((mseconds == 1) || (mseconds == 11))
{

    head_current = s.i_heading;
    alt_current = s.i_altitude;
    speed_current = s.i_speed;


    alt_var = alt_current-alt_old;
    htmp = head_current-head_old;
    speed_var = speed_current - speed_old;


    if(htmp >= 1800) head_var =  htmp-3600;
    else if(htmp < -1800) head_var = htmp+3600;
    else head_var = htmp;


    alt_old=s.i_altitude;
    head_old=s.i_heading;
    speed_old = s.i_speed;

    if(CheckBox5->Checked == true)
    {
    keep.roll = keep.roll - head_var/16;
    keep.pitch = keep.pitch - alt_var/4;
    keep.speed = speed_requested;//keep_throttle - speed_var;
    head_error = 0;
    alt_error = 0;
    }
    else if(CheckBox7->Checked == true)
    {
/************************************** Heading control ***********************************************************/

        htmp =head_requested - head_current;

        if(htmp >= 1800) head_error =  htmp-3600;
        else if(htmp < -1800) head_error = htmp + 3600 ;
        else head_error = htmp;

//        head_err_sum = head_err_sum + head_error - head_err_long;
//        head_err_long =  head_err_sum/20;

/************************************** Averaging *****************************************************************/

/************************************** Variant 2 *****************************************************************/
//        keep_roll_sum = keep_roll_sum + head_error*ctrl.Kst_hdg - head_var*ctrl.Kast_hdg - keep.roll;
//
//        if (keep_roll_sum >= 600)  keep_roll_sum=600;
//        else if (keep_roll_sum <= -600)  keep_roll_sum=-600;
//
//        keep.roll =  keep_roll_sum/4;
/************************************** Variant 2 ******************************************************************/

/************************************** Variant 1 ******************************************************************/

     keep.roll = (head_error*ctrl.Kst_hdg)  - (head_var*ctrl.Kast_hdg) - keep_roll_sum;

      if (keep.roll >= 300)  keep.roll=300;
      else if (keep.roll <= -300)  keep.roll=-300;

      // ������ �������������
        if(head_error > 20)
        {
        keep_roll_sum = keep_roll_sum - 1;
        }
        else if(head_error <= -20)
        {
        keep_roll_sum = keep_roll_sum + 1;
        }

      if (keep_roll_sum >= 60)  keep_roll_sum=60;
      else if (keep_roll_sum <= -60)  keep_roll_sum=-60;




/*******************************************************************************************************************/

/************************************** Altitude control ***********************************************************/

        alt_error =    alt_current - alt_requested;

//        alt_err_sum = alt_err_sum + alt_error - alt_err_long;
//        alt_err_long =  alt_err_sum/20;

/************************************** Averaging *****************************************************************/

/************************************** Variant 2 *****************************************************************/
//        keep_pitch_sum = keep_pitch_sum - alt_var*ctrl.Kast_alt + alt_error*ctrl.Kst_alt - keep.pitch;
//
//        if (keep_pitch_sum >= 480)  keep_pitch_sum=480;
//        else if (keep_pitch_sum <= -480)  keep_pitch_sum=-480;
//
//        keep.pitch =  keep_pitch_sum/4;
/*******************************************************************************************************************/

/************************************** Variant 1 ******************************************************************/

//      keep.pitch = (alt_error*ctrl.Kst_alt)  - (alt_var*ctrl.Kast_alt) - keep_pitch_sum;


      alt_var_req = -alt_error/100;//ctrl.Kst_alt;

      if (alt_var_req >= 10)  alt_var_req=10;
      else if (alt_var_req <= -10)  alt_var_req =-10;



//      keep.pitch = (alt_error*ctrl.Kst_alt)  - (alt_var*ctrl.Kast_alt) - keep_pitch_sum;
//       keep.pitch = (alt_var_req-alt_var)*ctrl.Kast_alt;// - keep_pitch_sum;



//       keep.pitch = ctrl.Kst_alt*atan(((alt_var_req-alt_var)^3) -  (alt_var_req-alt_var)*(alt_var*ctrl.Kast_alt));// - keep_pitch_sum;


//       keep.pitch = ctrl.Kst_alt*atan(((alt_error/100)^3) +  (alt_var*ctrl.Kast_alt));// + keep_pitch_sum;

      if (alt_error >= 1000)  alt_error=1000;
      else if (alt_error <= -1000)  alt_error =-1000;

      if (alt_var >= 40)  alt_var = 40;
      else if (alt_var <= -40)  alt_var =-40;



//      keep.pitch = (alt_error*ctrl.Kst_alt)  - (((alt_var/7)^3)*ctrl.Kast_alt)+ keep_pitch_sum;




//if (alt_error >= 300)                               ParA = -180;
//else if ((alt_error <= 300) && (alt_error > 50))    ParA = -100;
//else if ((alt_error <= 50) && (alt_error > 20))     ParA = ParA + 1;
//else if ((alt_error <= 20) && (alt_error > -20))    ParA = ParA;
//else if ((alt_error <= -20) && (alt_error > -50))   ParA = ParA - 1;
//else if ((alt_error <= -50) && (alt_error > -300))  ParA = 100;
//else if(alt_error <= -300)                          ParA = 180;

ParA =  -alt_error*ctrl.Kst_alt;
ParB =  -alt_var*ctrl.Kast_alt;
ParC =  keep_pitch_sum;



//ParA =  160*tan((float)alt_error/160)*ctrl.Kst_alt;
//ParB =  32*tan((float)alt_var/32)*ctrl.Kast_alt;
//ParC = keep_pitch_sum;

       keep.pitch = ParA + ParB + ParC;


       Form1->TrackAltErr->Position =  ParA+Form1->TrackAltErr->Height;
       Form1->TrackVarErr->Position =  ParB+Form1->TrackVarErr->Height;
       Form1->TrackTrim->Position =    ParC+Form1->TrackTrim->Height;


//     if( alt_var*alt_var <= 100 )
//     {
      // ������ �������������
       if((alt_error > 300) && (alt_var > 0))
       {
        keep_pitch_sum = keep_pitch_sum - 5;
       }
       else if((alt_error > 10) && ((alt_var*alt_var) < 4))
       {
        keep_pitch_sum = keep_pitch_sum - 1;
       }
       else if((alt_error <= -10) && ((alt_var*alt_var) < 4))
       {
        keep_pitch_sum = keep_pitch_sum + 1;
       }

       else if((alt_error <= -300) && (alt_var < 0))
       {
        keep_pitch_sum = keep_pitch_sum + 5;
       }


       //     }



      if (keep_pitch_sum >= 120)  keep_pitch_sum=120;
      else if (keep_pitch_sum <= -120)  keep_pitch_sum =-120;

      if (keep.pitch >= 180)  keep.pitch=180;
      else if (keep.pitch <= -180)  keep.pitch=-180;

/*******************************************************************************************************************/

/************************************** Speed    control ***********************************************************/
        speed_error =  speed_requested - speed_current;

//    keep.speed = keep.speed + speed_error - speed_var;
        keep.speed = speed_requested;

//     if (keep.speed >= 250)  keep.speed=250;
//      else if (keep.speed <= 5)  keep.speed=5;

/************************************** NAVIGATION ******************************************************************/

if(GPSCheckBox->Checked == true)
{
latitude_0 = waypoint[point_number].latitude;
longitude_0 = waypoint[point_number].longitude;
speed_requested =waypoint[point_number].speed;
alt_requested =waypoint[point_number].altitude;
    if(point_number != 0)
    {
        lat_s = waypoint[point_number-1].latitude;
        lon_s = waypoint[point_number-1].longitude;
    }
    else
    if(init_last_point == 0)
    {
      lat_s = s.i_latitude;
      lon_s = s.i_longitude;
      init_last_point = 1;
    }
}


/************************************ Current course calculating *************************************************/

dlat = (float)(s.i_latitude - latitude_old);
dlon = (float)(s.i_longitude - longitude_old)*cos(((float)s.i_latitude*PI)/18E7);


    if (dlat >= 0 && dlon >= 0)
            if(dlat==0) Course_rad = PI/2;
            else Course_rad = atan(dlon/dlat);
    else if (dlat <= 0 && dlon >= 0)
            if(dlat==0) Course_rad = PI/2;
            else Course_rad = PI+atan(dlon/dlat);
    else if (dlat < 0 && dlon < 0)
            if(dlat==0) Course_rad = 3*PI/2;
            else Course_rad = PI+atan(dlon/dlat);
    else
            if(dlat==0) Course_rad = 3*PI/2;
            else Course_rad = (2*PI)+atan(dlon/dlat);


 Course_dig =(int)(((Course_rad*180)/PI)*10);
latitude_old  = s.i_latitude;
longitude_old = s.i_longitude;

/************************************ Direct course calculating *************************************************/

dlat = (float)(latitude_0 - s.i_latitude);
dlon = (float)(longitude_0 - s.i_longitude)*cos(((float)s.i_latitude*PI)/18E7);
distance = (sqrt(dlat*dlat + dlon*dlon))/9;


    if (dlat >= 0 && dlon >= 0)
            if(dlat==0) Azimut = PI/2;
            else Azimut = atan(dlon/dlat);
    else if (dlat <= 0 && dlon >= 0)
            if(dlat==0) Azimut = PI/2;
            else Azimut = PI+atan(dlon/dlat);
    else if (dlat < 0 && dlon < 0)
            if(dlat==0) Azimut = 3*PI/2;
            else Azimut = PI+atan(dlon/dlat);
    else
            if(dlat==0) Azimut = 3*PI/2;
            else Azimut = (2*PI)+atan(dlon/dlat);

/************************************** Course correction calculate*****************************************************/

    dlat = (float)(latitude_0 - lat_s);
    dlon = (float)(longitude_0 - lon_s)*cos(((float)lat_s*PI)/18E7);

    if (dlat >= 0 && dlon >= 0)
            if(dlat==0) TrueAzimut = PI/2;
            else TrueAzimut = atan(dlon/dlat);
    else if (dlat <= 0 && dlon >= 0)
            if(dlat==0) TrueAzimut = PI/2;
            else TrueAzimut = PI+atan(dlon/dlat);
    else if (dlat < 0 && dlon < 0)
            if(dlat==0) TrueAzimut = 3*PI/2;
            else TrueAzimut = PI+atan(dlon/dlat);
    else
            if(dlat==0) TrueAzimut = 3*PI/2;
            else TrueAzimut = (2*PI)+atan(dlon/dlat);

        offset = distance*sin( TrueAzimut - Azimut);

/******* course trim section *************************/


    if ((offset > (-10000)) && (offset < 10000)) {

       offset_i = (int)offset;
       offset_dim[fi] = offset_i;

       tmp = (offset_i-offset_med)*(offset_i-offset_med);
       offset_sq_dim[fi] = tmp;

       htmp = offset_i-offset_med;
       offset_var_dim[fi] = htmp;


       fi++;
       if(fi == OFFSET_SAMPLES+1) fi=0;

       offset_sum = offset_sum + offset_i - offset_dim[fi];
       offset_sq_sum = offset_sq_sum + tmp - offset_sq_dim[fi];
       offset_var_sum = offset_var_sum + htmp - offset_var_dim[fi];

       offset_sq = sqrt((double)offset_sq_sum/OFFSET_SAMPLES);
       offset_med = (double)offset_sum/OFFSET_SAMPLES;
       offset_var = (double)offset_var_sum/OFFSET_SAMPLES;


       sprintf(buf,"%02.1f",offset_med);
       Form1->OffsetMed->Text=buf;
       sprintf(buf,"%02.1f", offset_sq);
       Form1->OffsetSq->Text=buf;
       sprintf(buf,"%02.1f",offset_var);
       Form1->OffsetVar->Text=buf;


       if(offset_sq <= 4){
          if(( offset_med > 50)&&(offset_var >= 0)){
           head_correction = head_correction - 0.1;
           hcorrmem = head_correction;
          }
          else if ((offset_med < -50)&&(offset_var <= 0)){
           head_correction = head_correction + 0.1;
           hcorrmem = head_correction;
          }
          else{
           head_correction = hcorrmem*(1-k_of) - abs(hcorrmem)*k_of*offset_med/50;
          }
          if((abs(offset_med) < 10 )&&(wind_speed_rdy == 0)){
          wind_speed =gnd_speed*sin((head_correction*3.14)/180);
          gnd_speed_mem = gnd_speed;
          wind_speed_rdy = 1;
          }
       }
       else if(offset_sq > 20){
        head_correction = 0;
        hcorrmem = 0;
        wind_speed_rdy = 0;
        wind_speed = 0;
        }

        shift_angle = (180/3.14)*atan(wind_speed/gnd_speed+1);



      /*     //variant 1
       if(offset_sq <= 4){
          if(( offset_med > 30)&&(offset_var >= 0)){
           head_correction = head_correction - 0.1;
           hcorrmem = head_correction;
          }
          else if ((offset_med < -30)&&(offset_var <= 0)){
           head_correction = head_correction + 0.1;
           hcorrmem = head_correction;
          }
          else if( hcorrmem >=0 ){
           head_correction = hcorrmem*0.9-(hcorrmem*0.1*(offset_med-offset_var))/30;
          }
          else {
           head_correction = hcorrmem*0.9+(hcorrmem*0.1*(offset_med-offset_var))/30;
          }
          if((abs(offset_med) < 10 )&&(wind_speed_rdy == 0)&&(offset_sq <= 4)){
          wind_speed =gnd_speed*sin((head_correction*3.14)/180);
          gnd_speed_mem = gnd_speed;
          wind_speed_rdy = 1;
          }
       }
       else if(offset_sq > 100){
        head_correction = 0;
        wind_speed_rdy = 0;
        wind_speed = 0;
        }


        if (gnd_speed != 0){
        shift_angle = (180/3.14)*asin(wind_speed/gnd_speed);
        }

*/
       if(head_correction > 30) head_correction = 30;
       if(head_correction < -30) head_correction = -30;

     if(wind_speed_rdy == 0){
        head_correction_i =  (int)(head_correction*10);
     }
     else{
        head_correction_i =  (int)((head_correction)*10)*(gnd_speed_mem/(gnd_speed+1));
        //head_correction_i =  (int)(shift_angle*10);
     }


        sprintf(buf,"%0.1f", head_correction);
        Form1->HeadCorrect->Text=buf;
        sprintf(buf,"%02.1f",gnd_speed);
        Form1->SpeedGround->Text=buf;
        sprintf(buf,"%02.1f",wind_speed);
        Form1->SpeedWind->Text=buf;
        sprintf(buf,"%02.1f",(float)Course_dig/10);
        Form1->ShiftAngle->Text=buf;

       }

     // ������ �������������
/*       if((offset > 50) && (offset_var > 0))
       {
        trim_angle = trim_angle - 1 ;
       }
       else if((offset < -50 ) && ((offset_var < 0))
       {
        keep_pitch_sum = keep_pitch_sum - 1;
       }
       else if((alt_error <= -10) && ((alt_var*alt_var) < 4))
       {
        keep_pitch_sum = keep_pitch_sum + 1;
       }

       else if((alt_error <= -300) && (alt_var < 0))
       {
        keep_pitch_sum = keep_pitch_sum + 5;
       }

 */






        sprintf(buffer,"DST =%d HDG=%.2f THDG=%.2f OFF=%d SP=%d ALT=%d WP=%d", (int)distance,
        ((Azimut*180)/PI),((TrueAzimut*180)/PI), (int)offset, speed_requested, alt_requested, waypoint[point_number].pid),
        Memo1->Lines->Add(buffer);

    if((GPSCheckBox->Checked == true)||(ReturnCheckBox->Checked == true))
    {

     sprintf(buf,"%d",speed_requested);
     Form1->SpeedRequest->Text=buf;
     sprintf(buf,"%d",alt_requested);
     Form1->AltRequest->Text=buf;
     sprintf(buf,"%d",(int)distance);
     Form1->Distance->Text=buf;
     sprintf(buf,"%d",waypoint[point_number].pid);
     Form1->WayPoint->Text=buf;
     if(speed_current > 0)
     {
     time_to_point = distance*3600/(speed_current*1852);
     ttp_hr  =time_to_point/3600;
     ttp_min =(time_to_point/60)%60;
     ttp_sec =time_to_point-(ttp_hr*3600)-(ttp_min*60);
     }
     sprintf(buf,"%02d:%02d:%02d", ttp_hr, ttp_min, ttp_sec);
     Form1->TimeToPoint->Text=buf;
/*********************************************************************
//     head_requested =(int)(((Azimut*180)/PI)*10);
//     head_requested =(int)((((Azimut+(Azimut-TrueAzimut))*180)/PI)*10);
    if((distance >= 100)&& (offset > 1)&&(ReturnCheckBox->Checked == false)
    &&(abs(Azimut - TrueAzimut)<(PI/2)))
//    if(distance >= 50)
    {
//                head_requested =(int)((((Azimut-((Sign(offset))*acos(offset/distance)-atan(100/offset)))*180)/PI)*10);
//              head_requested =(int)((((Azimut-(PI/2-atan((200*200)/(20*offset*abs(offset)))))*180)/PI)*10);
                head_requested =(int)((TrueAzimut-(PI/2-atan(200/offset)))*(180/PI)*10);
//                head_requested =(int)((((Azimut-(asin(offset/distance)))*180)/PI)*10);
    }
    else
    {
       head_requested =(int)(((Azimut*180)/PI)*10);
    }
***********************************************************************/

//     head_requested =(int)(((Azimut*180)/PI)*10);
//     head_requested =(int)((((Azimut+(Azimut-TrueAzimut))*180)/PI)*10);

    if((distance >= 200)&&(offset!=0)&&(ReturnCheckBox->Checked == false)
//    if((ReturnCheckBox->Checked == false)
    &&(abs(Azimut - TrueAzimut)<(PI/2)))
//    if(distance >= 50)
    {
        if(offset >=0)
        {
//                head_requested =(int)((((Azimut-((Sign(offset))*acos(offset/distance)-atan(100/offset)))*180)/PI)*10);
//              head_requested =(int)((((Azimut-(PI/2-atan((200*200)/(20*offset*abs(offset)))))*180)/PI)*10);
                htmp =(int)((TrueAzimut-(PI/2-atan(RadRazv/offset)))*(180/PI)*10);
//                head_requested =(int)((((Azimut-(asin(offset/distance)))*180)/PI)*10);
        }
        else
        {
                htmp =(int)((TrueAzimut+(PI/2+atan(RadRazv/offset)))*(180/PI)*10);

        }
    }
    else
    {
       htmp =(int)(((Azimut*180)/PI)*10);
    }

    if(distance > 200)
    {

       //htmp = htmp + head_correction_i;

        if(htmp > 3599)
        head_requested = htmp - 3600;
        else if(htmp < 0)
        head_requested = htmp + 3600;
        else head_requested = htmp;
    }


     if((waypoint[point_number+1].pid!=0) && (distance <= RadRazv))
     {
      if((distance_old > distance)||(distance < 50)) point_number++;
     }
      distance_old = distance;
    }
/*************************************************************
     if((waypoint[point_number+1].pid!=0) && (distance <= 500))
     {
      if((distance_old < distance)||(distance < 50)) point_number++;
      distance_old = distance;
     }
     else distance_old = 510;
    }
***************************************************************/



    }
    else
    {
    head_error = 0;
    alt_error = 0;
    keep.speed = speed_requested;
    keep.roll = 0;
    keep.pitch = 0;
    }

}

if(mseconds == 2)
{


 sprintf(buf,"%05d",head_error);
 Form1->Head_Err->Text=buf;
 sprintf(buf,"%05d",alt_error);
 Form1->Alt_Err->Text=buf;
 sprintf(buf,"%05d",speed_error);
 Form1->Speed_Err->Text=buf;



 sprintf(buf,"%05d",seconds);
 Form1->Time->Text=buf;
 sprintf(buf,"% 5d",out.ailerons);
 Form1->Ailerons->Text=buf;
 sprintf(buf,"% 5d",out.elevator);
 Form1->Elevator->Text=buf;
 sprintf(buf,"% 5d",out.rudder);
 Form1->Rudder->Text=buf;
 sprintf(buf,"% 5d",out.throttle);
 Form1->Throttle->Text=buf;

 sprintf(buf,"% 5d",alt_var);
 Form1->AVariation->Text=buf;
 sprintf(buf,"% 5d",keep_pitch_sum);
 Form1->PitchTrim->Text=buf;
 sprintf(buf,"% 5d",keep_roll_sum);
 Form1->RollTrim->Text=buf;
 sprintf(buf,"% 5d",head_var);
 Form1->HVariation->Text=buf;
 sprintf(buf,"% 5d",speed_var);
 Form1->SVariation->Text=buf;
 sprintf(buf,"% 5d",keep.roll);
 Form1->KeepRoll->Text=buf;
 sprintf(buf,"% 5d",keep.pitch);
 Form1->KeepPitch->Text=buf;
 sprintf(buf,"% 5d",keep.speed);
 Form1->KeepThrottle->Text=buf;

     if((ReturnCheckBox->Checked == true)||(GPSCheckBox->Checked == true))
    {
        sprintf(buf,"%3d",head_requested/10);
        Form1->HeadRequest->Text=buf;
    }


/*
 sprintf(buf,"% 5f",ctrl.Kst_roll);
 Form1->Kstroll->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_roll);
 Form1->Kastroll->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kst_pitch);
 Form1->Kstpitch->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_pitch);
 Form1->Kastpitch->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kst_speed);
 Form1->Kstspeed->Text=buf;
 sprintf(buf,"% 5f",ctrl.Kast_speed);
 Form1->Kastspeed->Text=buf;
*/


}

    ap1 = CheckBox5->Checked;
    ap2 = CheckBox7->Checked;
    ap0 = CheckBox8->Checked;



}
//---------------------------------------------------------------------------

void __fastcall TForm1::KeepRollChange(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",KeepRoll->Text);
        tmp=strtod(buffer,endptr);
        if(tmp >= 80) keep.roll = 80;
        else if (tmp <= -80) keep.roll = -80;
        else keep.roll = tmp;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::KeepPitchChange(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",KeepPitch->Text);
        tmp=strtod(buffer,endptr);
        if(tmp >= 180) keep.pitch = 180;
        else if (tmp <= -180) keep.pitch = -180;
        else keep.pitch = tmp;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::AltRequestDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",AltRequest->Text);
        alt_requested=strtod(buffer,endptr);


}
//---------------------------------------------------------------------------

void __fastcall TForm1::HeadRequestDblClick(TObject *Sender)
{

        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",HeadRequest->Text);
        head_requested=10*strtod(buffer,endptr);

}
//---------------------------------------------------------------------------





void __fastcall TForm1::SpeedRequestDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",SpeedRequest->Text);
        speed_requested=strtod(buffer,endptr);

}
//---------------------------------------------------------------------------


void __fastcall TForm1::KstrollDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kstroll->Text);
        ctrl.Kst_roll=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kst_roll);
        Form1->Kstroll->Text=buffer;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::KastrollDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kastroll->Text);
        ctrl.Kast_roll=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kast_roll);
        Form1->Kastroll->Text=buffer;

}



//---------------------------------------------------------------------------
void __fastcall TForm1::Kfar_rollDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kfar_roll->Text);
        ctrl.Kfar_roll=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kfar_roll);
        Form1->Kfar_roll->Text=buffer;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::KstpitchDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kstpitch->Text);
        ctrl.Kst_pitch=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kst_pitch);
        Form1->Kstpitch->Text=buffer;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::KastpitchDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kastpitch->Text);
        ctrl.Kast_pitch=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kast_pitch);
        Form1->Kastpitch->Text=buffer;
}

void __fastcall TForm1::Kfar_pitchDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kfar_pitch->Text);
        ctrl.Kfar_pitch=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kfar_pitch);
        Form1->Kfar_pitch->Text=buffer;

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TForm1::KstspeedDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kstspeed->Text);
        ctrl.Kst_speed=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kst_speed);
        Form1->Kstspeed->Text=buffer;

}

void __fastcall TForm1::Kst_hdgDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kst_hdg->Text);
        ctrl.Kst_hdg=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kst_hdg);
        Form1->Kst_hdg->Text=buffer;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Kast_hdgDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kast_hdg->Text);
        ctrl.Kast_hdg=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kast_hdg);
        Form1->Kast_hdg->Text=buffer;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Kst_altDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kst_alt->Text);
        ctrl.Kst_alt=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kst_alt);
        Form1->Kst_alt->Text=buffer;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Kast_altDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kast_alt->Text);
        ctrl.Kast_alt=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kast_alt);
        Form1->Kast_alt->Text=buffer;


}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------

void __fastcall TForm1::KastspeedDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",Kastspeed->Text);
        ctrl.Kast_speed=strtod(buffer,endptr);
        sprintf(buffer,"% 5f",ctrl.Kast_speed);
        Form1->Kastspeed->Text=buffer;

}
//---------------------------------------------------------------------------


void FlStSys(FSSi *in, FSSo *out, FSSp *keep, FSSc *ctrl, TEST *tst)
{
  int roll_current, pitch_current, roll, roll_var, pitch, pitch_var;
  int speed_current, tmp, jcycle;


if(++fss_j == FSS_SAMPLES)
{
  fss_j = 0;
  jcycle = FSS_SAMPLES-1;
}
else
jcycle=fss_j-1;

// *************************** Roll control **********************************
roll_current= in->roll-keep->roll;

roll_sum = roll_sum + roll_current-roll_dim[fss_j];
roll_var_sum = roll_var_sum + roll_current - roll_dim[jcycle] -roll_var_dim[fss_j];
roll_var_dim[fss_j] = roll_current - roll_dim[jcycle];
roll_dim[fss_j] = roll_current;
roll = roll_sum/FSS_SAMPLES;
roll_var = roll_var_sum/FSS_SAMPLES;
roll_long_sum = roll_long_sum + roll_current -  roll_long_sum/FAR_SAMPLES;

tst->p1 = roll;
tst->p2 = roll_long_sum/FAR_SAMPLES;

tmp = -(int)((ctrl->Kst_roll*(float)roll_sum + ctrl->Kst_roll*(float)roll_var_sum)/FSS_SAMPLES+
            (roll_long_sum*ctrl->Kfar_roll)/FAR_SAMPLES);
/*
if(roll >= 80)        ailerons_corr--;
else
 if (roll <= 80)     ailerons_corr++;

if(ailerons_corr >= 100) ailerons_corr=100;
else if(ailerons_corr <= -100) ailerons_corr = -100;
*/





 if(tmp >= 110) tmp=110;
 else if(tmp <= -110) tmp = -110;

    out->ailerons =tmp;
    out->rudder = tmp/4;

// ************************** Pitch control **********************************

pitch_current = in->pitch - keep->pitch;

pitch_sum = pitch_sum + pitch_current-pitch_dim[fss_j];
pitch_var_sum = pitch_var_sum + pitch_current - pitch_dim[jcycle]- pitch_var_dim[fss_j];
pitch_var_dim[fss_j]= pitch_current - pitch_dim[jcycle];
pitch_dim[fss_j] = pitch_current;
pitch = pitch_sum/FSS_SAMPLES;
pitch_var = pitch_var_sum/FSS_SAMPLES;
pitch_long_sum = pitch_long_sum + pitch_current -  pitch_long_sum/FAR_SAMPLES;

tst->p3 = pitch;
tst->p4 = pitch_long_sum/FAR_SAMPLES;

tmp = -(int)((ctrl->Kst_pitch*(float)pitch_sum+ctrl->Kast_pitch*(float)pitch_var_sum)/FSS_SAMPLES+
        (pitch_long_sum*ctrl->Kfar_pitch)/FAR_SAMPLES);

/*
if(pitch >= 80)        elevator_corr--;
else
 if (pitch <= -80)     elevator_corr++;

if(elevator_corr >= 100) elevator_corr=100;
else if(elevator_corr <= -100) elevator_corr = -100;
*/

//tmp = tmp + elevator_corr;


if(tmp >= 110) tmp=110;
else if(tmp <= -110) tmp = -110;

out->elevator =  tmp;

// ************************** Throttle control **********************************


speed_current = in->speed - keep->speed;
speed_sum = speed_sum + speed_current -  speed_sum/SPEED_SAMPLES;
tst->p3 = speed_sum;
tmp = 127-(ctrl->Kst_speed*(float)speed_sum)/SPEED_SAMPLES;


/*
speed_current = in->speed - keep->speed;

speed_sum = speed_sum + speed_current-speed_dim[fss_j];
speed_var_sum = speed_var_sum + speed_current - speed_dim[jcycle]- speed_var_dim[fss_j];
speed_var_dim[fss_j]= speed_current - speed_dim[jcycle];
speed_dim[fss_j] = speed_current;

tmp = out->throttle-(ctrl->Kst_speed*(float)speed_sum+ctrl->Kast_speed*(float)speed_var_sum)/FSS_SAMPLES;
*/
if(tmp >= 250) tmp=250;
else if(tmp <= 2) tmp = 2;
out->throttle = tmp;


}



void __fastcall TForm1::StorePointButtonClick(TObject *Sender)
{

latitude_0 = s.i_latitude;
longitude_0 = s.i_longitude;
}
//---------------------------------------------------------------------------





void __fastcall TForm1::WayPointDblClick(TObject *Sender)
{
        char * endptr[10];
        char buffer[100];
        int tmp;
        sprintf(buffer,"%s",WayPoint->Text);
        tmp=strtod(buffer,endptr)-1;
        if(tmp >= 0) point_number = tmp;
        else point_number=0;
        sprintf(buffer,"%d",waypoint[point_number].pid);
        Form1->WayPoint->Text=buffer;
}
//---------------------------------------------------------------------------


void FlStSys2(FSSi *in, FSSo *out, FSSp *keep, FSSc *ctrl, TEST *tst)
{
  int roll_current, pitch_current, roll, roll_var, pitch, pitch_var;
  int speed_current, r, p,t, a,b,c,d;
  char buffer[100];

// *************************** Roll control **********************************
roll_dim[ri]=roll_current= in->roll-keep->roll;
pitch_dim[ri]=pitch_current= in->pitch-keep->pitch;
ri++;
if ( ri >= FSS_SAMPLES+1) ri=0;
roll_sum = roll_sum + roll_current-roll_dim[ri];
roll_var = roll_sum - roll_pr;
roll_long_sum = roll_long_sum + roll_current - roll_long_sum/FAR_SAMPLES;
roll_pr=roll_sum;

a = ctrl->Kst_roll*((float)roll_sum/FSS_SAMPLES);
b = ctrl->Kast_roll*((float)roll_var/FSS_SAMPLES);
c = ctrl->Kfar_roll*((float)roll_long_sum/FAR_SAMPLES);

r = -a+b-c;

tst->p1 = a;
tst->p2 = b;
tst->p3 = c;
tst->p4 = r;

 if(r >= 50) r=50;
 else if(r <= -50) r = -50;

if (in->altitude >= 100)
   {
        out->ailerons = r;
        out->rudder = r/4;
   }
else
   {
        out->ailerons = 0;
        out->rudder = r;
   }
//}
// ************************** Pitch control **********************************

//pitch_dim2[pi]=pitch_current= in->pitch-keep->pitch;

pitch_sum = pitch_sum + pitch_current-pitch_dim[ri];
pitch_var = pitch_sum - pitch_pr;
pitch_long_sum = pitch_long_sum + pitch_current - pitch_long_sum/FAR_SAMPLES;
pitch_pr=pitch_sum;

a = ctrl->Kst_pitch*((float)pitch_sum/FSS_SAMPLES);
b = ctrl->Kast_pitch*((float)pitch_var/FSS_SAMPLES);
c = ctrl->Kfar_pitch*((float)pitch_long_sum/FAR_SAMPLES);

p = -a+b-c;

if(p >= 100) p=100;
else if(p <= -100) p = -100;
out->elevator =  p;


// ************************** Throttle control **********************************

speed_current = in->speed - keep->speed;
speed_sum = speed_sum + speed_current -  speed_sum/SPEED_SAMPLES;
speed_var = speed_current - speed_pr;
speed_pr = speed_current;
//tst->p3 = speed_sum;
t = 127-(ctrl->Kst_speed*(float)speed_sum)/SPEED_SAMPLES+ctrl->Kast_speed*(float)speed_var;


if(t >= 250) t=250;
else if(t <= 2) t = 2;
out->throttle = t;


}


/********************************************************************************
void FlStSys2(FSSi *in, FSSo *out, FSSp *keep, FSSc *ctrl, TEST *tst)
{
  int roll_current, pitch_current, roll, roll_var, pitch, pitch_var;
  int speed_current, tmp, jcycle;
  char buffer[100];

// *************************** Roll control **********************************
roll_current= in->roll-keep->roll;

roll_sum = roll_sum + roll_current -  roll_sum/ROLL_SAMPLES;
tst->p1 = roll_sum;
tmp =  - (int)(ctrl->Kst_roll*(float)roll_sum)/ROLL_SAMPLES;



 if(tmp >= 110) tmp=110;
 else if(tmp <= -110) tmp = -110;

    out->ailerons =tmp;
    out->rudder = tmp/4;

// ************************** Pitch control **********************************

pitch_current = in->pitch - keep->pitch;
pitch_var = in->pitch - pitch_last;
pitch_var_sum = pitch_var_sum + pitch_var - pitch_var_sum/32;



pitch_sum = pitch_sum + pitch_current -  pitch_sum/ELEV_SAMPLES;
tst->p2 = pitch_sum;
tmp =  - (int)(ctrl->Kst_pitch*(float)pitch_sum)/ELEV_SAMPLES - (int)(ctrl->Kast_pitch*(float)pitch_var_sum)/32;


if(tmp >= 110) tmp=110;
else if(tmp <= -110) tmp = -110;
out->elevator =  tmp;
pitch_last = pitch_current;


// ************************** Throttle control **********************************

speed_current = in->speed - keep->speed;
speed_sum = speed_sum + speed_current -  speed_sum/SPEED_SAMPLES;
tst->p3 = speed_sum;
tmp = 127-(ctrl->Kst_speed*(float)speed_sum)/SPEED_SAMPLES;


if(tmp >= 250) tmp=250;
else if(tmp <= 2) tmp = 2;
out->throttle = tmp;


}
********************************************************************************/

























