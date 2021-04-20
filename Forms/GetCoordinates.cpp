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

#include "DigitizePNG.h"

#include "GetCoordinates.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TGetCoordinatesForm *GetCoordinatesForm;

__fastcall TGetCoordinatesForm::TGetCoordinatesForm(TComponent* Owner) : TForm(Owner)
{
}

void __fastcall TGetCoordinatesForm::OKClick(TObject *Sender)
{
  double x, y;
  if( !GetValues( x, y ) )return;
  DigitizePNGForm->SetPoints( x, y );
  Close();
}

bool TGetCoordinatesForm::GetValues( double &x, double &y )
{
  if( XEdit->Text.IsEmpty() )
  {
    Application->MessageBox("Please enter an x-coordinate value","ERROR",MB_OK);
    return false;
  }
  try
  {
    x = XEdit->Text.ToDouble();
  }
  catch (EConvertError &e)
  {
    Application->MessageBox("Invalid number entered as x-coordinate","ERROR",MB_OK);
    XEdit->Text = AnsiString("");
    return false;
  }
  if( YEdit->Text.IsEmpty() )
  {
    Application->MessageBox("Please enter a y-coordinate value","ERROR",MB_OK);
    return false;
  }
  try
  {
    y = YEdit->Text.ToDouble();
  }
  catch (EConvertError &e)
  {
    Application->MessageBox("Invalid number entered as y-coordinate","ERROR",MB_OK);
    YEdit->Text = AnsiString("");
    return false;
  }
  return true;
}
//---------------------------------------------------------------------------
