//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "thermostat.h"
#include "data_algs.h"

#include <systdate.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{

  for (int i = 0; i < 24; i++)
  {
    Setpoints [i] = 0.0; // inicializuji pole setpointu
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Menu_Exit(TObject *Sender)
// metoda volana z menu - polozka "Thermostat"->"Exit"
{
  Close();
  // kdyz exit tak exit
}
//---------------------------------------------------------------------------

void __fastcall TForm1::About1Click(TObject *Sender)
// metoda volana z menu "Help"->"About"
{
  About_form = new TAbout_form(this);
  // vytvorim promennou s dialogem
  About_form->ShowModal();  // a modalne zobrazim
  delete About_form; // nakonec dialog zrusim
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Settings1Click(TObject *Sender)
// spustim setting dialog
{
  SetpointForm= new TSetpointForm(this);
  // radsi dynamicky - to zaruci uplnou inicializaci dat a zedne mrtvoly
  SetpointForm->ShowModal();
  // modalne
  delete (SetpointForm);
  // a uklid
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Auto_buttonClick(TObject *Sender)
// radiobutton - "Auto"
{
  thermostat.SetSystemMode(TM_AUTO);
  //thermostat.UpdateSystem (te_IDLE);
  //Status->Caption = thermostat.GetSystemStatus();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Cool_buttonClick(TObject *Sender)
// radiobutton - "Cool"
{
  thermostat.SetSystemMode(TM_COOL);
  //thermostat.UpdateSystem (te_COOL);
  //Status->Caption = thermostat.GetSystemStatus();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Heat_buttonClick(TObject *Sender)
// radiobutton - "Heat"
{
  thermostat.SetSystemMode(TM_HEAT);
  //thermostat.UpdateSystem (te_HEAT);
  //Status->Caption = thermostat.GetSystemStatus();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Idle_buttonClick(TObject *Sender)
// radiobutton - "Idle"
{
  thermostat.SetSystemMode(TM_IDLE);
  //thermostat.UpdateSystem (te_IDLE);
  //Status->Caption = thermostat.GetSystemStatus();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Hlavni_timerTimer(TObject *Sender)
// reakce na casovac aplikace: nastaven na jedno-minutovy interval
// pouziva se pro aktualizaci systemu
{
    TDateTime now = Time ();  // zjistim kolik je hodin
    int t; // casova jednotka v sekundach
    int Time_Interval; // pro prepocet na index
  
    t = (int) SecondsBetween (LastMeas, now); // vemu rozdil v sekundach
    
    if (t < 0 ) t = 1; // osetreni prechodu na novy den
     
    thermostat.UpdateSensor (MonthOf (Date()), HourOf (Time()), t); 
    // aktualizuji sensor
    thermostat.UpdateSystem (te_TIMER);
    // aktualizuji system
    Room_Tempetature->Caption = FloatToStrF (thermostat.GetRoomTemperature (),
      ffGeneral, 3, 3);
    // zobrazim teplotu v mistnosti
    Status->Caption = thermostat.GetSystemStatus();
    // a aktualni status

    Time_Interval = (24*3600) / MAX_LOG;
    // Time interval je promenna ktera ma v sobe ulozen krok zaznamniku
    // dat v sekundach

    t = (HourOf (Time()) * 3600 + MinuteOf (Time()) * 60 + SecondOf (Time()))
        /Time_Interval;
    // spocitam hodnotu aktualniho indexu - ze znameho casu a delky intervalu
    RecordToday [t].RoomTemp = thermostat.GetRoomTemperature ();
    RecordToday [t].Status = thermostat.GetSystemStatusEx ();
    // a zalogujeme teplotu a stav systemu

    t = HourOf (Time ());
    // kolik je hodin ?
    if ( Setpoints [2] > 0 )
    // je nastaveno topeni po hodine
    {
      thermostat.SetHeatSetpoint(Setpoints[t]);
      //upravim setpoint
    }
    else if ( Setpoints [1] > 0 )
    // nebo den /noc
    {
      if ((t < 6)||(t > 18))
      {
        thermostat.SetHeatSetpoint(Setpoints[1]);
        // noc
      }
      else
      {
        thermostat.SetHeatSetpoint(Setpoints[0]);
        // den
      }
    }
    else
    {
      thermostat.SetHeatSetpoint(Setpoints[0]);
      // nebo jen single
    }

    Heat_Label->Caption = FloatToStrF (thermostat.GetHeatSetpoint (),
      ffGeneral, 3, 3);
    // jeste upravim zobrazeny udaj

    LastMeas = now; // aktualizuji posledni mereni
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormActivate(TObject *Sender)
// reakce na otevreni hlavniho formulare
// po aktivaci hlavniho okna je nutno nastavit teplotu podle nastaveni
{
    LastMeas = Time (); // zjistime kolik je hodin

    thermostat.UpdateSystem (te_INIT);
    // inicializace stavoveho automatu

    Simulate_data ();
    // nacucame z prstu nejaka data

    Room_Tempetature->Caption = FloatToStrF (thermostat.GetRoomTemperature (),
      ffGeneral, 3, 3);
    // zobrazeni teploty v mistnosti
    Status->Caption = thermostat.GetSystemStatus();
    // zobrazeni aktualniho statusu
    Setpoints [0] = thermostat.GetHeatSetpoint ();
    // setpoint si natahnu do seznamu
    Heat_Label->Caption = FloatToStrF (Setpoints [0],
      ffGeneral, 3, 3);
    // zobrazeni aktualniho setpointu
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Graphs1Click(TObject *Sender)
{
  Graphs = new TGraphs(this);
  // radsi dynamicky - to zaruci uplnou inicializaci dat a zedne mrtvoly
  Graphs->ShowModal();
  // modalne
  delete (Graphs);
  // a uklid
}
//---------------------------------------------------------------------------

