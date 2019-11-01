//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdUDPBase.hpp>
#include <IdUDPServer.hpp>
#include <ComCtrls.hpp>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "Unit2.h"
#include <ExtCtrls.hpp>
#include "pies.h"

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TIdUDPServer *IdUDPServer1;
        TMemo *Memo1;
        TEdit *Edit1;
        TEdit *Edit2;
        TButton *Start;
        TButton *Stop;
        TLabel *UDP_stat;
        TButton *Button1;
        TButton *Button2;
        TComboBoxEx *ComboBoxEx1;
        TComboBoxEx *ComboBoxEx2;
        TComboBoxEx *ComboBoxEx3;
        TComboBoxEx *ComboBoxEx4;
        TComboBoxEx *ComboBoxEx5;
        TCheckBox *CheckBox1;
        TCheckBox *CheckBox2;
        TCheckBox *CheckBox3;
        TStaticText *StaticText2;
        TStaticText *StaticText3;
        TStaticText *StaticText4;
        TStaticText *StaticText5;
        TStaticText *StaticText6;
        TGroupBox *GroupBox1;
        TGroupBox *GroupBox2;
        TGroupBox *GroupBox3;
        TGroupBox *GroupBox5;
        TStaticText *StaticText1;
        TStaticText *StaticText7;
        TLabel *rs232_stat;
    TLabeledEdit *Pitch;
    TLabeledEdit *Roll;
    TLabeledEdit *Speed;
    TLabeledEdit *Heading;
    TLabeledEdit *Altitude;
    TLabeledEdit *Latitude;
    TLabeledEdit *Longitude;
    TCheckBox *CheckBox4;
    TLabel *Label1;
  TPie *HorizPie;
    TTimer *Timer1;
    TGroupBox *GroupBox4;
    TCheckBox *CheckBox5;
    TCheckBox *CheckBox6;
    TLabeledEdit *Time;
    TLabeledEdit *Ailerons;
    TLabeledEdit *Elevator;
    TLabeledEdit *Rudder;
    TLabeledEdit *Throttle;
    TLabeledEdit *HVariation;
    TLabeledEdit *AVariation;
    TEdit *KeepRoll;
    TEdit *KeepPitch;
    TStaticText *Keep_Roll;
    TStaticText *Keep_Pitch;
    TCheckBox *CheckBox7;
    TEdit *AltRequest;
    TEdit *HeadRequest;
    TStaticText *StaticText8;
    TStaticText *StaticText9;
    TLabeledEdit *Alt_Err;
    TLabeledEdit *Head_Err;
    TEdit *KeepThrottle;
    TLabel *Keep_Trottle;
    TEdit *SpeedRequest;
    TLabel *Label2;
    TLabeledEdit *Speed_Err;
    TLabeledEdit *SVariation;
    TEdit *Kstpitch;
    TLabel *Label3;
    TCheckBox *CheckBox8;
    TEdit *Kastpitch;
    TLabel *Label4;
    TEdit *Kstroll;
    TEdit *Kastroll;
    TEdit *Kstspeed;
    TEdit *Kastspeed;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TLabel *Label8;
    TEdit *Kfar_roll;
    TEdit *Kfar_pitch;
    TLabel *Label9;
    TLabel *Label10;
    TEdit *Kst_hdg;
    TLabel *Label11;
    TEdit *Kast_hdg;
    TLabel *Label12;
    TEdit *Kst_alt;
    TEdit *Kast_alt;
    TLabel *Label13;
    TLabel *Label14;
    TButton *StorePointButton;
    TCheckBox *ReturnCheckBox;
    TCheckBox *GPSCheckBox;
    TLabeledEdit *Distance;
    TLabeledEdit *WayPoint;
    TLabeledEdit *TimeToPoint;
    TTrackBar *TrackBar1;
    TTrackBar *TrackBar2;
    TTrackBar *TrackBar3;
    TTrackBar *TrackBar4;
    TEdit *PitchTrim;
    TLabel *Label15;
    TEdit *RollTrim;
    TLabel *Label16;
    TCheckBox *CheckBox9;
    TLabel *Label17;
    TLabel *Label18;
    TLabel *Label19;
    TLabel *Label20;
    TTrackBar *TrackAltErr;
    TTrackBar *TrackVarErr;
    TTrackBar *TrackTrim;
  TShape *HorizMsk;
  TShape *HorizBottom;
  TLabel *Label21;
  TLabel *Label22;
  TLabel *Label23;
  TLabeledEdit *OffsetMed;
  TLabeledEdit *OffsetSq;
  TLabeledEdit *HeadCorrect;
  TLabeledEdit *SpeedGround;
  TLabeledEdit *SpeedWind;
  TLabeledEdit *OffsetVar;
  TLabeledEdit *ShiftAngle;
        void __fastcall IdUDPServer1UDPRead(TObject *Sender,
          TStream *AData, TIdSocketHandle *ABinding);
        void __fastcall IdUDPServer1Status(TObject *axSender,
          const TIdStatus axStatus, const AnsiString asStatusText);
        void __fastcall StartClick(TObject *Sender);
        void __fastcall StopClick(TObject *Sender);
        void __fastcall UDP_statClick(TObject *Sender);
        void __fastcall Edit2DblClick(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
    void __fastcall Edit2Change(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall KeepRollChange(TObject *Sender);
    void __fastcall KeepPitchChange(TObject *Sender);
    void __fastcall AltRequestDblClick(TObject *Sender);
    void __fastcall HeadRequestDblClick(TObject *Sender);
    void __fastcall SpeedRequestDblClick(TObject *Sender);
    void __fastcall KstrollDblClick(TObject *Sender);
    void __fastcall KastrollDblClick(TObject *Sender);
    void __fastcall KstpitchDblClick(TObject *Sender);
    void __fastcall KastpitchDblClick(TObject *Sender);
    void __fastcall KstspeedDblClick(TObject *Sender);
    void __fastcall KastspeedDblClick(TObject *Sender);
    void __fastcall Kfar_rollDblClick(TObject *Sender);
    void __fastcall Kfar_pitchDblClick(TObject *Sender);
    void __fastcall Kst_hdgDblClick(TObject *Sender);
    void __fastcall Kast_hdgDblClick(TObject *Sender);
    void __fastcall Kst_altDblClick(TObject *Sender);
    void __fastcall Kast_altDblClick(TObject *Sender);
    void __fastcall StorePointButtonClick(TObject *Sender);
    void __fastcall WayPointDblClick(TObject *Sender);
private:	// User declarations
        TUartReceive *uart;
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
