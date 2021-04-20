#include "ProjectHeader.h"
#pragma hdrstop

#include "DigitizePNG.h"
#include "PNGDisplay.h"
#include "DigitizeInfo.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TDigitizeInfoForm *DigitizeInfoForm;

__fastcall TDigitizeInfoForm::TDigitizeInfoForm(TComponent* Owner) : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "DigitizeInfoForm", "Top", 400 );
  this->Left = ini->ReadInteger( "DigitizeInfoForm", "Left", 550 );
  this->Height = ini->ReadInteger( "DigitizeInfoForm", "Height", 200 );
  this->Width = ini->ReadInteger( "DigitizeInfoForm", "Width", 500 );
  delete ini;
  InstructionLabel1->Caption = AnsiString("Click with the left mouse button on a location");
  InstructionLabel2->Caption = AnsiString("on the PNG drawing where you know the coordinates");
  InstructionLabel3->Caption = AnsiString("then enter the coordinates on the pop-up form");
  n_ = 0;
}

__fastcall TDigitizeInfoForm::~TDigitizeInfoForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "DigitizeInfoForm", "Top", this->Top );
  ini->WriteInteger( "DigitizeInfoForm", "Left", this->Left );
  ini->WriteInteger( "DigitizeInfoForm", "Height", this->Height );
  ini->WriteInteger( "DigitizeInfoForm", "Width", this->Width );
  delete ini;
  DigitizeInfoForm = NULL;
}

void __fastcall TDigitizeInfoForm::FormClose(TObject *Sender, TCloseAction &Action)
{ Action = caFree; }

void __fastcall TDigitizeInfoForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{ DigitizeInfoForm = NULL; }

void __fastcall TDigitizeInfoForm::AbortClick(TObject *Sender)
{
  n_==3 ? DigitizePNGForm->StartDigitizing() : DigitizePNGForm->AbortDigitizing();
  DigitizePNGForm->Enabled = true;
  Close();
}

void __fastcall TDigitizeInfoForm::Next()
{
  switch (n_++)
  {
    case 0:
      InstructionLabel1->Caption = "Next, click with the left mouse button on another";
      InstructionLabel2->Caption = "location on the PNG drawing where you know the coordinates";
      PNGForm->StartPicking();
      PNGForm->SetFocus();
      break;
    case 1:
      InstructionLabel1->Caption = "Finally, click with the left mouse button on a";
      InstructionLabel2->Caption = "non-colinear location on the PNG drawing where you know the coordinates";
      PNGForm->StartPicking();
      PNGForm->SetFocus();
      break;
    case 2:
      InstructionLabel1->Caption = "Now close this form and then digitize by clicking";
      InstructionLabel2->Caption = "with the left mouse button on the PNG drawing";
      InstructionLabel3->Caption = "";
      AbortButton->Caption = "Close";
      break;
    case 3:
      DigitizePNGForm->Enabled = true;
      DigitizePNGForm->StartDigitizing();
      Close();
  }
}

// end of file
