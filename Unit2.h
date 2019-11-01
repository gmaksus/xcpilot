//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TUartReceive : public TThread
{
private:
protected:
        void __fastcall Execute();
public:
        __fastcall TUartReceive(bool CreateSuspended);
   void __fastcall TUartReceive::UpdateCaption();
   void __fastcall TUartReceive::ThWaitCommEvent();

        };
//---------------------------------------------------------------------------
#endif
 