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
#include "VarInfo.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"

#include "VarShow.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TVarShowForm *VarShowForm;

__fastcall TVarShowForm::TVarShowForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "VarShowForm", "Top", 282 );
  this->Left = ini->ReadInteger( "VarShowForm", "Left", 271 );
  this->Height = ini->ReadInteger( "VarShowForm", "Height", 277 );
  this->Width = ini->ReadInteger( "VarShowForm", "Width", 768 );
  delete ini;
  //
  VarInfoForm = 0;
  FillStringGrid();
}

__fastcall TVarShowForm::~TVarShowForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "VarShowForm", "Top", this->Top );
  ini->WriteInteger( "VarShowForm", "Left", this->Left );
  ini->WriteInteger( "VarShowForm", "Height", this->Height );
  ini->WriteInteger( "VarShowForm", "Width", this->Width );
  delete ini;
}

void __fastcall TVarShowForm::SetRows()
{
  NVariableTable *nvTable = NVariableTable::GetTable();
  int nRows = VarShowStringGrid->RowCount;
  int scalarCntr = 0;
  int vectorCntr = 0;
  int matrixCntr = 0;
  int tensorCntr = 0;
  int end = nvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( i );
    AnsiString name( nv->GetName().c_str() );
    if( nv->GetData().GetNumberOfDimensions() == 0 )
    {
      if( ++scalarCntr > nRows-1 )++nRows;
    }
    else if( nv->GetData().GetNumberOfDimensions() == 1 )
    {
      if( ++vectorCntr > nRows-1 )++nRows;
    }
    else if( nv->GetData().GetNumberOfDimensions() == 2 )
    {
      if( ++matrixCntr > nRows-1 )++nRows;
    }
    else if( nv->GetData().GetNumberOfDimensions() == 3 )
    {
      if( ++tensorCntr > nRows-1 )++nRows;
    }
  }
  TVariableTable *tvTable = TVariableTable::GetTable();
  int tscalarCntr = 0;
  int tvectorCntr = 0;
  end = tvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    TextVariable *tv = tvTable->GetEntry( i );
    AnsiString name( tv->GetName().c_str() );
    if( tv->GetData().GetNumberOfDimensions() == 0 )
    {
      if( ++tscalarCntr > nRows-1 )++nRows;
    }
    else if( tv->GetData().GetNumberOfDimensions() == 1 )
    {
      if( ++tvectorCntr > nRows-1 )++nRows;
    }
  }
  VarShowStringGrid->RowCount = nRows;
}

void __fastcall TVarShowForm::FillStringGrid()
{
  VarShowStringGrid->Cells[0][0] = "Scalars";
  VarShowStringGrid->Cells[1][0] = "Vectors";
  VarShowStringGrid->Cells[2][0] = "Matrices";
  VarShowStringGrid->Cells[3][0] = "Tensors";
  VarShowStringGrid->Cells[4][0] = "Character";
  SetRows();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int scalarCntr = 0;
  int vectorCntr = 0;
  int matrixCntr = 0;
  int tensorCntr = 0;
  int end = nvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( i );
    AnsiString name( nv->GetName().c_str() );
    if( nv->GetData().GetNumberOfDimensions() == 0 )
    {
      VarShowStringGrid->Cells[0][++scalarCntr] = name;
    }
    else if( nv->GetData().GetNumberOfDimensions() == 1 )
    {
      VarShowStringGrid->Cells[1][++vectorCntr] = name;
    }
    else if( nv->GetData().GetNumberOfDimensions() == 2 )
    {
      VarShowStringGrid->Cells[2][++matrixCntr] = name;
    }
    else if( nv->GetData().GetNumberOfDimensions() == 3 )
    {
      VarShowStringGrid->Cells[3][++tensorCntr] = name;
    }
  }
  TVariableTable *tvTable = TVariableTable::GetTable();
  int textCntr = 0;
  end = tvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    TextVariable *tv = tvTable->GetEntry( i );
    AnsiString name( tv->GetName().c_str() );
    if( tv->GetData().GetNumberOfDimensions() == 0 )
    {
      VarShowStringGrid->Cells[4][++textCntr] = name;
    }
    else if( tv->GetData().GetNumberOfDimensions() == 1 )
    {
      VarShowStringGrid->Cells[4][++textCntr] = name;
    }
  }
  VarShowStringGrid->Repaint();
}

void __fastcall TVarShowForm::RefreshStringGrid(TObject *Sender)
{ RefreshStringGrid(); }

void TVarShowForm::RefreshStringGrid()
{
  for( int i=0; i<5; ++i )VarShowStringGrid->Cols[i]->Clear();
  FillStringGrid();
}

void __fastcall TVarShowForm::DisplayInfo(TObject *Sender)
{
  int col = VarShowStringGrid->Col;
  int row = VarShowStringGrid->Row;
  AnsiString name = VarShowStringGrid->Cells[col][row];
  if( !name.IsEmpty() )
  {
    if( VarInfoForm )VarInfoForm->CloseForm();
    std::auto_ptr<TVarInfoForm> tmp( new TVarInfoForm(this) );
    VarInfoForm = tmp.release();
    VarInfoForm->DisplayInfo( name );
    VarInfoForm->Show();
  }
}

void __fastcall TVarShowForm::CloseClick(TObject *Sender)
{
  if( VarInfoForm )VarInfoForm->CloseForm();
  Close();
  VarShowForm = 0;
}

void __fastcall TVarShowForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TVarShowForm::FormClose2(TObject *Sender,bool &CanClose)
{
  if( VarInfoForm )VarInfoForm->CloseForm();
  VarShowForm = 0;
}

// end of file
