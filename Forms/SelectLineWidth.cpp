/*
Copyright (C) 2005 Joseph L. Chuma, TRIUMF

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ExGlobals.h"
#include "MainGraphicsWindow.h"
#include "GRA_window.h"
#include "GRA_setOfCharacteristics.h"

#include "SelectLineWidth.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TSelectLineWidthForm *SelectLineWidthForm;

__fastcall TSelectLineWidthForm::TSelectLineWidthForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "SelectLineWidthForm", "Top", 220 );
  this->Left = ini->ReadInteger( "SelectLineWidthForm", "Left", 850 );
  delete ini;
  //
  LineWidthComboBox->ItemIndex = static_cast<GRA_intCharacteristic*>(
      ExGlobals::GetGraphWindow()->GetGeneralCharacteristics()->Get("LINEWIDTH"))->Get() - 1;
}

__fastcall TSelectLineWidthForm::~TSelectLineWidthForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "SelectLineWidthForm", "Top", this->Top );
  ini->WriteInteger( "SelectLineWidthForm", "Left", this->Left );
  delete ini;
}

void __fastcall TSelectLineWidthForm::CloseClick(TObject *Sender)
{
  Close();
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TSelectLineWidthForm::LineWidthChange(TObject *Sender)
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  int w = LineWidthComboBox->ItemIndex + 1;
  static_cast<GRA_intCharacteristic*>(gw->GetGeneralCharacteristics()->Get("LINEWIDTH"))->Set( w );
  static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVELINEWIDTH"))->Set( w );
  static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLLINEWIDTH"))->Set( w );
  //
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TSelectLineWidthForm::FormClose(TObject *Sender, TCloseAction &Action)
{ Action = caFree; }

void __fastcall TSelectLineWidthForm::FormClose2(TObject *Sender, bool &CanClose)
{ SelectLineWidthForm = 0; }

// end of file
