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
#include "GRA_window.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_intCharacteristic.h"

#include "MainGraphicsWindow.h"

#include "HistogramType.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

THistogramTypeForm *HistogramTypeForm;

__fastcall THistogramTypeForm::THistogramTypeForm(TComponent* Owner)
  : TForm(Owner)
{
  GRA_intCharacteristic *histType = static_cast<GRA_intCharacteristic*>(
    ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics()->Get("HISTOGRAMTYPE"));
  HistogramTypeRadioGroup->ItemIndex = histType->Get();
}

void __fastcall THistogramTypeForm::CloseForm(TObject *Sender)
{
  Close();
  //MainGraphicsForm->SetHistogramTypeFormNull();
}

void __fastcall THistogramTypeForm::SetDefaultsClick(TObject *Sender)
{
  GRA_intCharacteristic *histType = static_cast<GRA_intCharacteristic*>(
      ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics()->Get("HISTOGRAMTYPE"));
  histType->Set( 0 );
  HistogramTypeRadioGroup->ItemIndex = 0;
}

void __fastcall THistogramTypeForm::ApplyClick(TObject *Sender)
{
  GRA_intCharacteristic *histType = static_cast<GRA_intCharacteristic*>(
      ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics()->Get("HISTOGRAMTYPE"));
  histType->Set(HistogramTypeRadioGroup->ItemIndex);
}

// end of file
