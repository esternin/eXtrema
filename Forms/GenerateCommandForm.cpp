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

#include "GenerateCommandForm.h"

#include "NumericVariable.h"
#include "EVariableError.h"
#include "UsefulFunctions.h"
#include "ExGlobals.h"
#include "ExString.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TGenerateCmndForm *GenerateCmndForm;

__fastcall TGenerateCmndForm::TGenerateCmndForm( TComponent* Owner )
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "GenerateCmndForm", "Top", 453 );
  Left = ini->ReadInteger( "GenerateCmndForm", "Left", 443 );
  Height = ini->ReadInteger( "GenerateCmndForm", "Height", 239 );
  Width = ini->ReadInteger( "GenerateCmndForm", "Width", 468 );
  NameEdit->Text = ini->ReadString( "GenerateCmndForm", "Name", "" );
  MinEdit->Text = ini->ReadString( "GenerateCmndForm", "Min", "" );
  IncEdit->Text = ini->ReadString( "GenerateCmndForm", "Inc", "" );
  MaxEdit->Text = ini->ReadString( "GenerateCmndForm", "Max", "" );
  NumEdit->Text = ini->ReadString( "GenerateCmndForm", "Number", "" );
  ChoicesRadioGroup->ItemIndex = ini->ReadInteger( "GenerateCmndForm", "Choice", 1 );
  delete ini;
  SetUp();
}

__fastcall TGenerateCmndForm::~TGenerateCmndForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "GenerateCmndForm", "Top", Top );
  ini->WriteInteger( "GenerateCmndForm", "Left", Left );
  ini->WriteInteger( "GenerateCmndForm", "Height", Height );
  ini->WriteInteger( "GenerateCmndForm", "Width", Width );
  ini->WriteString( "GenerateCmndForm", "Name", NameEdit->Text );
  ini->WriteString( "GenerateCmndForm", "Min", MinEdit->Text );
  ini->WriteString( "GenerateCmndForm", "Inc", IncEdit->Text );
  ini->WriteString( "GenerateCmndForm", "Max", MaxEdit->Text );
  ini->WriteString( "GenerateCmndForm", "Number", NumEdit->Text );
  ini->WriteInteger( "GenerateCmndForm", "Choice", ChoicesRadioGroup->ItemIndex );
  delete ini;
}

void __fastcall TGenerateCmndForm::DefaultsClick( TObject *Sender )
{
  NameEdit->Clear();
  MinEdit->Clear();
  IncEdit->Clear();
  MaxEdit->Clear();
  NumEdit->Clear();
  ChoicesRadioGroup->ItemIndex = 1;
  SetUp();
  //
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM GENERATE,DEFAULTS" );
}

void __fastcall TGenerateCmndForm::ApplyClick( TObject *Sender )
{
  if( NameEdit->Text.IsEmpty() )
  {
    Application->MessageBox( "please specify an output vector", "Fatal Error", MB_OK );
    return;
  }
  ExString name( NameEdit->Text.c_str() );
  double vMin, vMax, vInc;
  int vNum;
  std::vector<double> x;
  if( ChoicesRadioGroup->ItemIndex == 0 ) // random generation
  {
    if( MinEdit->Text.IsEmpty() || MaxEdit->Text.IsEmpty() || NumEdit->Text.IsEmpty() )
    {
      Application->MessageBox("minimum, maximum and number of values must be provided","Fatal Error",MB_OK);
      return;
    }
    try
    {
      NumericVariable::GetScalar( ExString(MinEdit->Text.c_str()), "minimum value", vMin );
      NumericVariable::GetScalar( ExString(MaxEdit->Text.c_str()), "maximum value", vMax );
      double d;
      NumericVariable::GetScalar( ExString(NumEdit->Text.c_str()), "number of values", d );
      vNum = static_cast<int>(d);
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Fatal Error", MB_OK );
      return;
    }
    if( vNum < 0 )
    {
      Application->MessageBox( "number of values < 0", "Fatal Error", MB_OK );
      return;
    }
    else if( vNum == 0 )
    {
      Application->MessageBox( "number of values = 0", "Fatal Error", MB_OK );
      return;
    }
    double delv = vMax - vMin;
    x.reserve( vNum );
    for( int i = 0; i < vNum; ++i )
    {
      double rdum = UsefulFunctions::GetSimpleRandomNumber();
      x.push_back( rdum*delv+vMin );
    }
  }
  else  // not random numbers
  {
    switch( ChoicesRadioGroup->ItemIndex == 1 )
    {
     case 1:
      if( MinEdit->Text.IsEmpty() || IncEdit->Text.IsEmpty() || MaxEdit->Text.IsEmpty() )
      {
        Application->MessageBox( "minimum, increment, and maximum must be provided", "Fatal Error", MB_OK );
        return;
      }
      break;
     case 2:
      if( MinEdit->Text.IsEmpty() || IncEdit->Text.IsEmpty() || NumEdit->Text.IsEmpty() )
      {
        Application->MessageBox("minimum, increment, and number of values must be provided","Fatal Error",MB_OK);
        return;
      }
      break;
     case 3:
      if( MinEdit->Text.IsEmpty() || MaxEdit->Text.IsEmpty() || NumEdit->Text.IsEmpty() )
      {
        Application->MessageBox("minimum, maximum, and number of values must be provided","Fatal Error",MB_OK);
        return;
      }
      break;
    }
    try
    {
      NumericVariable::GetScalar( ExString(MinEdit->Text.c_str()), "minimum value", vMin );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Fatal Error", MB_OK );
      return;
    }
    if( ChoicesRadioGroup->ItemIndex == 1 || ChoicesRadioGroup->ItemIndex == 2 )
    {
      try
      {
        NumericVariable::GetScalar( ExString(IncEdit->Text.c_str()), "increment", vInc );
      }
      catch( EVariableError &e )
      {
        Application->MessageBox( e.what(), "Fatal Error", MB_OK );
        return;
      }
      if( vInc == 0.0 )
      {
        Application->MessageBox( "increment = 0", "Fatal Error", MB_OK );
        return;
      }
    }
    if( ChoicesRadioGroup->ItemIndex == 1 || ChoicesRadioGroup->ItemIndex == 3 )
    {
      try
      {
        NumericVariable::GetScalar( ExString(MaxEdit->Text.c_str()), "maximum value", vMax );
      }
      catch( EVariableError &e )
      {
        Application->MessageBox( e.what(), "Fatal Error", MB_OK );
        return;
      }
    }
    if( ChoicesRadioGroup->ItemIndex == 2 || ChoicesRadioGroup->ItemIndex == 3 )
    {
      try
      {
        double d;
        NumericVariable::GetScalar( ExString(NumEdit->Text.c_str()), "number of values", d );
        vNum = static_cast<int>(d);
      }
      catch( EVariableError &e )
      {
        Application->MessageBox( e.what(), "Fatal Error", MB_OK );
        return;
      }
      if( vNum == 0 )
      {
        Application->MessageBox( "number of values = 0", "Fatal Error", MB_OK );
        return;
      }
      if( vNum < 0 )
      {
        Application->MessageBox( "number of values < 0", "Fatal Error", MB_OK );
        return;
      }
    }
    if( ChoicesRadioGroup->ItemIndex == 1 )
    {
      vNum = static_cast<int>((vMax-vMin)/vInc) + 1;
      if( vNum == 0 )
      {
        Application->MessageBox( "number of values = 0", "Fatal Error", MB_OK );
        return;
      }
      if( vNum < 0 )
      {
        Application->MessageBox( "number of values < 0", "Fatal Error", MB_OK );
        return;
      }
      x.reserve( vNum );
      for( int i = 0; i < vNum; ++i )x.push_back( vMin+i*vInc );
      if( fabs(x[vNum-1]-vMax) > fabs(vMax*0.000001) )
      {
        std::string s( "GENERATE: calculated maximum = " );
        char c[50];
        sprintf( c, "%g", x[vNum-1] );
        s += std::string(c) + std::string(", given maximum = ");
        sprintf( c, "%g", vMax );
        s += std::string(c);
        Application->MessageBox( s.c_str(), "Warning", MB_OK );
      }
    }
    else if( ChoicesRadioGroup->ItemIndex == 2 )
    {
      x.reserve( vNum );
      for( int i = 0; i < vNum; ++i )x.push_back( vMin+i*vInc );
    }
    else if( ChoicesRadioGroup->ItemIndex == 3 )
    {
      vNum > 1 ? vInc = (vMax-vMin)/(vNum-1) : vInc = 0.0;
      x.reserve( vNum );
      for( int i = 0; i < vNum-1; ++i )x.push_back( vMin+i*vInc );
      x.push_back( vMax );
    }
  }
  int order = 0;
  if( ChoicesRadioGroup->ItemIndex > 0 ) // not random
  {
    vInc>0.0 ? order=1 : order=2;
  }
  try
  {
    NumericVariable::PutVariable( name, x, order, "from GENERATE VECTOR gui" );
  }
  catch( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    return;
  }
  //
  if( ExGlobals::StackIsOn() )
  {
    ExString line( "FORM GENERATE," );
    line += ExString(NameEdit->Text.c_str()) + "," + ExString(ChoicesRadioGroup->ItemIndex) +
            "," + ExString(MinEdit->Text.c_str()) + "," + ExString(IncEdit->Text.c_str()) +
            "," + ExString(MaxEdit->Text.c_str()) + "," + ExString(NumEdit->Text.c_str()) +
            ",APPLY";
    ExGlobals::WriteStack( line );
  }
}

void __fastcall TGenerateCmndForm::ChoicesClick( TObject *Sender )
{ SetUp(); }

void __fastcall TGenerateCmndForm::SetUp()
{
  MaxEdit->Enabled = true;
  MaxLabel->Enabled = true;
  IncEdit->Enabled = true;
  IncLabel->Enabled = true;
  NumEdit->Enabled = true;
  NumLabel->Enabled = true;
  switch( ChoicesRadioGroup->ItemIndex )
  {
   case 0:
   case 3:
    IncEdit->Enabled = false;
    IncLabel->Enabled = false;
    break;
   case 1:
    NumEdit->Enabled = false;
    NumLabel->Enabled = false;
    break;
   case 2:
    MaxEdit->Enabled = false;
    MaxLabel->Enabled = false;
    break;
  }
}

void __fastcall TGenerateCmndForm::CloseClick( TObject *Sender )
{
  Close();
  GenerateCmndForm = 0;
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM GENERATE,CLOSE" );
}

void __fastcall TGenerateCmndForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TGenerateCmndForm::FormClose2( TObject *Sender, bool &CanClose )
{ GenerateCmndForm = 0; }

void TGenerateCmndForm::Set( ExString const &name, int choice, double min, double inc,
                             double max, int number, ExString const &action )
{
  NameEdit->Text = name.c_str();
  ChoicesRadioGroup->ItemIndex = choice;
  MinEdit->Text = AnsiString(min);
  IncEdit->Text = AnsiString(inc);
  MaxEdit->Text = AnsiString(max);
  NumEdit->Text = AnsiString(number);
  SetUp();
  if( action == "CLOSE" )
  {
    CloseClick( static_cast<TObject*>(0) );
  }
  else if( action == "DEFAULTS" )
  {
    DefaultsClick( static_cast<TObject*>(0) );
  }
  else if( action == "APPLY" )
  {
    ApplyClick( static_cast<TObject*>(0) );
  }
}

// end of file
