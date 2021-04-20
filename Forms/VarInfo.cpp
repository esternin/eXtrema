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

#include "VarInfo.h"
#include "VarShow.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TVarInfoForm *VarInfoForm;

__fastcall TVarInfoForm::TVarInfoForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "VarInfoForm", "Top", 308 );
  this->Left = ini->ReadInteger( "VarInfoForm", "Left", 409 );
  this->Height = ini->ReadInteger( "VarInfoForm", "Height", 329 );
  this->Width = ini->ReadInteger( "VarInfoForm", "Width", 553 );
  delete ini;
}

__fastcall TVarInfoForm::~TVarInfoForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "VarInfoForm", "Top", this->Top );
  ini->WriteInteger( "VarInfoForm", "Left", this->Left );
  ini->WriteInteger( "VarInfoForm", "Height", this->Height );
  ini->WriteInteger( "VarInfoForm", "Width", this->Width );
  delete ini;
}

void __fastcall TVarInfoForm::DisplayInfo( AnsiString name )
{
  name_ = name;
  DisplayInfoWork();
}

void __fastcall TVarInfoForm::DisplayInfoWork()
{
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( name_.c_str() );
  TextVariable *tv = TVariableTable::GetTable()->GetVariable( name_.c_str() );
  if( nv )
  {
    NumericData nd( nv->GetData() );
    AnsiString type;
    switch ( nd.GetNumberOfDimensions() )
    {
      case 0:
        type = "scalar";
        break;
      case 1:
        type = "vector";
        break;
      case 2:
        type = "matrix";
        break;
      case 3:
        type = "tensor";
        break;
    }
    AnsiString s( nv->GetName().c_str() );
    s += AnsiString(" is a ") + type;
    VarInfoMemo->Lines->Add( s );
    char c[100];
    if( nd.GetNumberOfDimensions() == 0 )
    {
      if( nd.IsFit() )
      {
        s = "allowed to vary in a fit";
        VarInfoMemo->Lines->Add( s );
      }
      sprintf( c, "value = %g", nd.GetScalarValue() );
      VarInfoMemo->Lines->Add( AnsiString(c) );
    }
    else if( nd.GetNumberOfDimensions() == 1 )
    {
      sprintf( c, "length = %d", nd.GetDimMag(0) );
      VarInfoMemo->Lines->Add( AnsiString(c) );
    }
    else if( nv->GetData().GetNumberOfDimensions() == 2 )
    {
      sprintf( c, "number of rows = %d, number of columns = %d",
               nd.GetDimMag(0), nd.GetDimMag(1) );
      VarInfoMemo->Lines->Add( AnsiString(c) );
    }
    else if( nv->GetData().GetNumberOfDimensions() == 3 )
    {
      sprintf( c, "number of rows = %d, number of columns = %d, number of planes = %d",
               nd.GetDimMag(0), nd.GetDimMag(1), nd.GetDimMag(2) );
      VarInfoMemo->Lines->Add( AnsiString(c) );
    }
    VarInfoMemo->Lines->Add( AnsiString("History (most recent at the top):") );
    VarInfoMemo->Lines->Add( AnsiString("-----------------------------------------") );
    std::deque<ExString> nvH( nv->GetHistory() );
    std::size_t size = nvH.size();
    for( std::size_t i=0; i<size; ++i )VarInfoMemo->Lines->Add( AnsiString( nvH[i].c_str() ) );
  }
  else if( tv )
  {
    AnsiString type;
    TextData td( tv->GetData() );
    if( td.GetNumberOfDimensions() == 0 )     type = "text scalar";
    else if( td.GetNumberOfDimensions() == 1 )type = "text vector";
    AnsiString s( tv->GetName().c_str() );
    s += " is a " + type;
    VarInfoMemo->Lines->Add( s );
    char c[100];
    if( td.GetNumberOfDimensions() == 0 )
    {
      sprintf( c, "number of characters = %d", td.GetScalarValue().length() );
      VarInfoMemo->Lines->Add( AnsiString(c) );
      VarInfoMemo->Lines->Add( AnsiString( td.GetScalarValue().c_str() ) );
    }
    else if( tv->GetData().GetNumberOfDimensions() == 1 )
    {
      sprintf( c, "number of strings = %d", td.GetData().size() );
      VarInfoMemo->Lines->Add( AnsiString(c) );
    }
    VarInfoMemo->Lines->Add( AnsiString("History (most recent at the top):") );
    VarInfoMemo->Lines->Add( AnsiString("-----------------------------------------") );
    std::deque<ExString> tvH( tv->GetHistory() );
    std::size_t size = tvH.size();
    for( std::size_t i=0; i<size; ++i )VarInfoMemo->Lines->Add( AnsiString( tvH[i].c_str() ) );
  }
  else
    VarInfoMemo->Lines->Add( AnsiString("variable no longer exists") );
}

void __fastcall TVarInfoForm::RefreshInfo(TObject *Sender)
{
  VarInfoMemo->Clear();
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( name_.c_str() );
  TextVariable *tv = TVariableTable::GetTable()->GetVariable( name_.c_str() );
  if( nv || tv )DisplayInfoWork();
  else          VarInfoMemo->Lines->Add( AnsiString("variable no longer exists") );
}

void __fastcall TVarInfoForm::CloseClick(TObject *Sender)
{
  Close();
  VarInfoForm = 0;
}

void __fastcall TVarInfoForm::CloseForm()
{
  Close();
  VarInfoForm = 0;
}

void __fastcall TVarInfoForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TVarInfoForm::FormClose2(TObject *Sender,bool &CanClose)
{ VarInfoForm = 0; }

// end of file
