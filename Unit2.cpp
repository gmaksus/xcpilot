//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
#pragma package(smart_init)
extern volatile int RS232_connected;
extern HANDLE port;
extern DWORD mask;
DWORD rcv_bc;
OVERLAPPED rcv_ovr;
char receive_buffer[1000];



//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TUartReceive::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TUartReceive::TUartReceive(bool CreateSuspended)
        : TThread(CreateSuspended)
{



}
//---------------------------------------------------------------------------
void __fastcall TUartReceive::Execute()
{     char buffer1[10000];
      char chout[10];
      char buf[10000];
      char *ptr;
      int sumx = 0, sumz = 0;
      int x[120], z[120];
      int ax, az;
      int averx, averz, dx, dz;
      double dfx;
      unsigned i,j=0,k=1;
 //    buffer[0]='\0';
        //---- Place thread code here ----
       for(i=0; i<100; i++)
       {
        x[i]=0;
        z[i]=0;
        }

   while(RS232_connected == 0);

        memset(&rcv_ovr,0,sizeof(rcv_ovr));
        rcv_ovr.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
        sprintf(buf,"%s",Form1->Memo1->Text);


   while(RS232_connected == 1)
   {
       //Sleep((unsigned)2000);
  //     WaitCommEvent(port,&mask,NULL);

    //   ReadFile(port,receive_buffer,100,&rcv_bc,NULL);

        WaitCommEvent(port,&mask,NULL);
      //  memset(&rcv_ovr,0,sizeof(rcv_ovr));
      //  rcv_ovr.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
        ReadFile(port,receive_buffer,100,&rcv_bc,&rcv_ovr);

        if(WaitForSingleObject(rcv_ovr.hEvent,10000)==WAIT_OBJECT_0)
        {
        GetOverlappedResult(port,&rcv_ovr,&rcv_bc,FALSE);

if( Form1->CheckBox4->Checked == FALSE )
{

      Form1->Label1->Caption = strlen(buf);

        if(strlen(buf) >= 9000)
        {
            ptr=strchr(buf, 0x0a);
            if(ptr == NULL) ptr=strchr(buf, 0x0d);
            if(ptr == NULL) strcpy(buf, &buf[100]);
            else  strcpy(buf, ptr+1);
        }
       // strncat(buf, receive_buffer, rcv_bc);
       ptr= receive_buffer;
/*        for(i=0; i<9; i++)
        {
       ptr=strchr(ptr+1, 0x20);
       }
       ax=atol(ptr);
       ptr=strchr(ptr+1, 0x20);
       az=atol(ptr);

       x[j]=ax;
       z[j]=az;
       if(j==5) j=0;
       else j++;
       sumz=sumz-z[j]+az;
       sumx=sumx-x[j]+ax;
       averx = (int)((float)sumx/k);
      // averx=5;
       averz = (int)((float)sumz/k);
//       dx = ax-averx;
//       dz = az-averz;
       dz = averz-495;
       if((averx-10)>=0)
       dfx = asin((double)(averx-10+(double)dz/10)/(double)430);
       else
       dfx = asin((double)(averx-10-(double)dz/10)/(double)430);
       dfx=(dfx*180)/3.1415926;
       dx = (int)(5*dfx);

       if(dx >= 0)
       {
       Form1->Pie1->FAngles->StartAngle=360-abs(dx);
       Form1->Pie1->FAngles->EndAngle=180-abs(dx);
       }
       else
       {
       Form1->Pie1->FAngles->StartAngle=abs(dx);
       Form1->Pie1->FAngles->EndAngle=180+abs(dx);
       }





       sprintf(buffer1, "% 3d % 4d % 4d % 4d % 4d % 4d % 4d\n", k, ax,
        az, averx, dx, averz, dz);
       strncat(buf, buffer1, strlen(buffer1));

       if( k != 5) k++;
         //    strcat(buf, receive_buffer);
       //     Form1->Memo1->Lines->SetText(receive_buffer);
            Form1->Memo1->Lines->SetText(buf);
            Form1->Memo1->Lines->Add("");
            */
}
else
{

   sprintf(buffer1,"");
   for(i=0; i<rcv_bc; i++)
   {
   sprintf(chout,"%02x",(byte)receive_buffer[i]);
   strncat(buffer1,chout,2);
   }
   strcat(buffer1,"\r");



      Form1->Label1->Caption = strlen(buf);

        if(strlen(buf) >= 9000)
        {
            ptr=strchr(buf, 0x0a);
            if(ptr == NULL) ptr=strchr(buf, 0x0d);
            if(ptr == NULL) strcpy(buf, &buf[100]);
            else  strcpy(buf, ptr+1);
        }
   strncat(buf,buffer1,rcv_bc*2+1);
            Form1->Memo1->Lines->SetText(buf);
            Form1->Memo1->Lines->Add("");

}





/*      if(strlen(buf)>= 9000)
      {
        ptr= strchr(buf,0x0a)+1;
        if( ptr == NULL )
        {
            strcpy(buf,&buf[100]);
        }
        else
        {
            strcpy(buf,ptr);
        }
      }
*/
      //  strcpy(buf, receive_buffer);
      //   buf[strlen(buf)-1]='\0';





//        for(i=0; (receive_buffer[i] != 0) && (i<rcv_bc) ;i++);

//         sprintf(buf, "i = %d char= %x" , i,receive_buffer[i]);
//            Form1->Memo1->Lines->Add(buf);

      //  if(receive_buffer[i] == 0)
      //   {
            //Form1->Memo1->Lines->Add("");

            //Form1->Memo1->Text = Form1->Memo1->Text + buffer;
//            Form1->Memo1->Text = Form1->Memo1->Text + buffer;

  /* sprintf(buffer1,"");
   for(i=0; i<rcv_bc; i++)
   {
   sprintf(chout,"%02x",(byte)receive_buffer[i]);
   strncat(buffer1,chout,2);
   }
    */
//            Form1->Memo1->Text = "";
  //         sprintf(buffer1,"%d\r",ptr-buf);

          //  Form1->Memo1->Lines->Add(buffer1);
//            Form1->Memo1->Lines->SetText(buf);
//            Form1->Memo1->Lines->Add("");

  //          buffer[0]='\0';
      //   }
        }
        else
        {

        //Form1->Memo1->Lines->Add("Receive error");

        }


        /*        if( rcv_bc != 0)
        {
         strncat(buffer, receive_buffer, rcv_bc);

  //           Form1->Memo1->Lines->Add(buffer);
   //          buffer[0]='\0';
             //Form1->Memo1->Lines->Add(receive_buffer);
          }
         else
        {
//         sprintf(buf, "Bytes = %d", rcv_bc);
//         Form1->Memo1->Lines->Add(buf);

           Form1->Memo1->Lines->Add(buffer);
           buffer[0]='\0';
             //Form1->Memo1->Lines->Add(receive_buffer);
          }
  */









      //  Synchronize(UpdateCaption);

    }
    Terminate();

}
//---------------------------------------------------------------------------
void __fastcall TUartReceive::UpdateCaption()
{
        Form1->Memo1->Lines->Add("Srabotalo");
}
void __fastcall TUartReceive::ThWaitCommEvent()
{
      if( SetCommMask(port,EV_RXCHAR))
      {
        Form1->Memo1->Lines->Add("SetComm = OK");
      }
      else
      {
        Form1->Memo1->Lines->Add("SetComm = FALSE");
      }


//       PurgeComm(port,PURGE_RXCLEAR);
//       WaitCommEvent(port,&mask,NULL);
//       ReadFile(port,receive_buffer,100,&bc,NULL);

}






