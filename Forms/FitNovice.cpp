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

#include "FitNovice.h"

#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "Expression.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "ParseLine.h"
#include "EVariableError.h"
#include "EExpressionError.h"
#include "GRA_cartesianCurve.h"
#include "GRA_cartesianAxes.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_colorControl.h"
#include "GRA_fontControl.h"
#include "GRA_window.h"
#include "GRA_drawableText.h"
#include "GRA_arrow3.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TFitNoviceForm *FitNoviceForm;

__fastcall TFitNoviceForm::TFitNoviceForm( TComponent* Owner ) : TForm(Owner)
{
  FillOutForm();
  //
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "FitNoviceForm", "Top", 25 );
  Left = ini->ReadInteger( "FitNoviceForm", "Left", 600 );
  //
  DataVecComboBox->ItemIndex = DataVecComboBox->Items->IndexOf(
    ini->ReadString("FitNoviceForm","DataVector","") );
  if( DataVecComboBox->ItemIndex >= 0 )
  {
    dVecName_ = ExString( DataVecComboBox->Items->Strings[DataVecComboBox->ItemIndex].c_str() );
    AnsiString tmp( dVecName_.c_str() );
    ExpressionLabel->Caption =
      AnsiString("Fit expression:  ")+tmp.SetLength(std::min(tmp.Length(),10))+" = ";
    IndepVecComboBox->Enabled = true;
    IndepVecLabel->Enabled = true;
    IndepVecComboBox->ItemIndex = IndepVecComboBox->Items->IndexOf(
      ini->ReadString("FitNoviceForm","IndepVector","") );
    if( IndepVecComboBox->ItemIndex >= 0 )
    {
      iVecName_ = ExString( IndepVecComboBox->Items->Strings[IndepVecComboBox->ItemIndex].c_str() );
      //
      IVMinEdit->Enabled = true;
      IVMaxEdit->Enabled = true;
      IVMinLabel->Enabled = true;
      IVMaxLabel->Enabled = true;
      double imin, imax;
      iVec_ = NVariableTable::GetTable()->GetVariable(iVecName_);
      iVec_->GetData().GetMinMax( imin, imax );
      IVMinEdit->Text = AnsiString(imin);
      IVMaxEdit->Text = AnsiString(imax);
      //
      ErrorVecLabel->Enabled = true;
      ErrorVecComboBox->Enabled = true;
      ErrorVecComboBox->ItemIndex = ErrorVecComboBox->Items->IndexOf(
        ini->ReadString("FitNoviceForm","ErrorVector","") );
      if( ErrorVecComboBox->ItemIndex >= 0 )
        eVecName_ = ExString( ErrorVecComboBox->Items->Strings[ErrorVecComboBox->ItemIndex].c_str() );
      //
      FitTypeComboBox->Enabled = true;
      FitTypeLabel->Enabled = true;
      FitTypeComboBox->ItemIndex = FitTypeComboBox->Items->IndexOf(
        ini->ReadString("FitNoviceForm","Type","") );
      if( FitTypeComboBox->ItemIndex >= 0 )
      {
        FitTypeChange( static_cast<TObject*>(0) );
        if( FitTypeComboBox->ItemIndex!=2 || FitTypeComboBox->ItemIndex!=3 || FitTypeComboBox->ItemIndex!=4 )
        {
          NumberComboBox->ItemIndex = ini->ReadInteger("FitNoviceForm","Number",-1);
          NumberChange( static_cast<TObject*>(0) );
        }
        ExpressionRichEdit->Text = ini->ReadString( "FitNoviceForm", "Expression", "" );
        UpdateExpression();
        int nparams = ini->ReadInteger( "FitNoviceForm", "Nparams", 0 );
        ParameterStringGrid->RowCount = nparams+1;
        for( int i=0; i<nparams; ++i )
        {
          ParameterStringGrid->Cells[0][i+1] = AnsiString("a")+AnsiString(i);
          ParameterStringGrid->Cells[1][i+1] =
            ini->ReadString( "FitNoviceForm", AnsiString("StartValue")+AnsiString(i), "" );
          ParameterStringGrid->Cells[2][i+1] = "";
        }
        if( nparams > 0 )
        {
          FixedCheckBox1->Enabled = true;
          FixedCheckBox1->Visible = true;
          FixedCheckBox1->Checked = ini->ReadBool( "FitNoviceForm", "Fixed1", false );
          if( nparams > 1 )
          {
            FixedCheckBox2->Enabled = true;
            FixedCheckBox2->Visible = true;
            FixedCheckBox2->Checked = ini->ReadBool( "FitNoviceForm", "Fixed2", false );
            if( nparams > 2 )
            {
              FixedCheckBox3->Enabled = true;
              FixedCheckBox3->Visible = true;
              FixedCheckBox3->Checked = ini->ReadBool( "FitNoviceForm", "Fixed3", false );
              if( nparams > 3 )
              {
                FixedCheckBox4->Enabled = true;
                FixedCheckBox4->Visible = true;
                FixedCheckBox4->Checked = ini->ReadBool( "FitNoviceForm", "Fixed4", false );
                if( nparams > 4 )
                {
                  FixedCheckBox5->Enabled = true;
                  FixedCheckBox5->Visible = true;
                  FixedCheckBox5->Checked = ini->ReadBool( "FitNoviceForm", "Fixed5", false );
                  if( nparams > 5 )
                  {
                    FixedCheckBox6->Enabled = true;
                    FixedCheckBox6->Visible = true;
                    FixedCheckBox6->Checked = ini->ReadBool( "FitNoviceForm", "Fixed6", false );
                    if( nparams > 6 )
                    {
                      FixedCheckBox7->Enabled = true;
                      FixedCheckBox7->Visible = true;
                      FixedCheckBox7->Checked = ini->ReadBool( "FitNoviceForm", "Fixed7", false );
                      if( nparams > 7 )
                      {
                        FixedCheckBox8->Enabled = true;
                        FixedCheckBox8->Visible = true;
                        FixedCheckBox8->Checked = ini->ReadBool( "FitNoviceForm", "Fixed8", false );
                      }
                    }
                  }
                }
              }
            }
          }
        }
        TestFitButton->Enabled = true;
        DoFitButton->Enabled = true;
      }
    }
  }
  LegendXEdit->Text = ini->ReadString( "FitNoviceForm", "Xloc", "53.0" );
  LegendYEdit->Text = ini->ReadString( "FitNoviceForm", "Yloc", "89.0" );
  delete ini;
}

__fastcall TFitNoviceForm::~TFitNoviceForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "FitNoviceForm", "Top", Top );
  ini->WriteInteger( "FitNoviceForm", "Left", Left );
  int idx = DataVecComboBox->ItemIndex;
  AnsiString tmp;
  if( idx >= 0 )tmp = DataVecComboBox->Items->Strings[idx];
  ini->WriteString( "FitNoviceForm", "DataVector", tmp );
  idx = IndepVecComboBox->ItemIndex;
  tmp = "";
  if( idx >= 0 )tmp = IndepVecComboBox->Items->Strings[idx];
  ini->WriteString( "FitNoviceForm", "IndepVector", tmp );
  idx = ErrorVecComboBox->ItemIndex;
  tmp = "";
  if( idx >= 0 )tmp = ErrorVecComboBox->Items->Strings[idx];
  ini->WriteString( "FitNoviceForm", "ErrorVector", tmp );
  idx = FitTypeComboBox->ItemIndex;
  tmp = "";
  if( idx >= 0 )tmp = FitTypeComboBox->Items->Strings[idx];
  ini->WriteString( "FitNoviceForm", "Type", tmp );
  ini->WriteInteger( "FitNoviceForm", "Number", NumberComboBox->ItemIndex );
  ini->WriteString( "FitNoviceForm", "Expression", ExpressionRichEdit->Text );
  int nRows = ParameterStringGrid->RowCount;
  ini->WriteInteger( "FitNoviceForm", "Nparams", nRows-1 );
  for( int i=1; i<nRows; ++i )
  {
    ini->WriteString( "FitNoviceForm", AnsiString("StartValue")+AnsiString(i-1), ParameterStringGrid->Cells[1][i] );
  }
  if( nRows > 1 )
  {
    ini->WriteBool( "FitNoviceForm", "Fixed1", FixedCheckBox1->Checked );
    if( nRows > 2 )
    {
      ini->WriteBool( "FitNoviceForm", "Fixed2", FixedCheckBox2->Checked );
      if( nRows > 3 )
      {
        ini->WriteBool( "FitNoviceForm", "Fixed3", FixedCheckBox3->Checked );
        if( nRows > 4 )
        {
          ini->WriteBool( "FitNoviceForm", "Fixed4", FixedCheckBox4->Checked );
          if( nRows > 5 )
          {
            ini->WriteBool( "FitNoviceForm", "Fixed5", FixedCheckBox5->Checked );
            if( nRows > 6 )
            {
              ini->WriteBool( "FitNoviceForm", "Fixed6", FixedCheckBox6->Checked );
              if( nRows > 7 )
              {
                ini->WriteBool( "FitNoviceForm", "Fixed7", FixedCheckBox7->Checked );
                if( nRows > 8 )
                  ini->WriteBool( "FitNoviceForm", "Fixed8", FixedCheckBox8->Checked );
              }
            }
          }
        }
      }
    }
  }
  ini->WriteString( "FitNoviceForm", "Xloc", LegendXEdit->Text );
  ini->WriteString( "FitNoviceForm", "Yloc", LegendYEdit->Text );
  delete ini;
}

void __fastcall TFitNoviceForm::FillOutForm()
{
  UpdateVectors( static_cast<TObject*>(DataVecComboBox) );
  UpdateVectors( static_cast<TObject*>(IndepVecComboBox) );
  UpdateVectors( static_cast<TObject*>(ErrorVecComboBox) );
  //
  FitTypeComboBox->Items->Add( "Legendre polynomials" );
  FitTypeComboBox->Items->Add( "Polynomial" );
  FitTypeComboBox->Items->Add( "Gaussian" );
  FitTypeComboBox->Items->Add( "Least squares line" );
  FitTypeComboBox->Items->Add( "Constant" );
  FitTypeComboBox->Items->Add( "Custom" );
  //
  ParameterStringGrid->Cells[0][0] = AnsiString("parameters");
  ParameterStringGrid->Cells[1][0] = AnsiString("start values");
  ParameterStringGrid->Cells[2][0] = AnsiString("fit results");
  FixedCheckBox1->Checked = false;
  FixedCheckBox1->Enabled = false;
  FixedCheckBox1->Visible = false;
  FixedCheckBox2->Checked = false;
  FixedCheckBox2->Enabled = false;
  FixedCheckBox2->Visible = false;
  FixedCheckBox3->Checked = false;
  FixedCheckBox3->Enabled = false;
  FixedCheckBox3->Visible = false;
  FixedCheckBox4->Checked = false;
  FixedCheckBox4->Enabled = false;
  FixedCheckBox4->Visible = false;
  FixedCheckBox5->Checked = false;
  FixedCheckBox5->Enabled = false;
  FixedCheckBox5->Visible = false;
  FixedCheckBox6->Checked = false;
  FixedCheckBox6->Enabled = false;
  FixedCheckBox6->Visible = false;
  FixedCheckBox7->Checked = false;
  FixedCheckBox7->Enabled = false;
  FixedCheckBox7->Visible = false;
  FixedCheckBox8->Checked = false;
  FixedCheckBox8->Enabled = false;
  FixedCheckBox8->Visible = false;
  //
  DataVecComboBox->Enabled = true;
  DataVecLabel->Enabled = true;
  IndepVecComboBox->Enabled = false;
  IndepVecLabel->Enabled = false;
  IVMinEdit->Enabled = false;
  IVMaxEdit->Enabled = false;
  IVMinLabel->Enabled = false;
  IVMaxLabel->Enabled = false;
  ErrorVecLabel->Enabled = false;
  ErrorVecComboBox->Enabled = false;
  FitTypeComboBox->Enabled = false;
  FitTypeLabel->Enabled = false;
  NumberComboBox->Enabled = false;
  NumberComboBox->Visible = false;
  NumberLabel->Enabled = false;
  NumberLabel->Visible = false;
  TestFitButton->Enabled = false;
  DoFitButton->Enabled = false;
  ExpressionLabel->Enabled = true;
  ExpressionRichEdit->Enabled = true;
  //
  eVecName_.clear();
  pStartValues_.clear();
}

void __fastcall TFitNoviceForm::UpdateVectors( TObject *Sender )
{
  TComboBox *cBox = static_cast<TComboBox*>(Sender);
  cBox->Clear();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int entries = nvTable->Entries();
  for( int i=0; i<entries; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry(i);
    if( nv->GetData().GetNumberOfDimensions() == 1 )cBox->Items->Add( nv->GetName().c_str() );
  }
  cBox->ItemIndex = -1;
}

void __fastcall TFitNoviceForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TFitNoviceForm::FormCloseQuery( TObject *Sender, bool &CanClose )
{ FitNoviceForm = 0; }

void __fastcall TFitNoviceForm::CloseButtonClick( TObject *Sender )
{
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM FIT,CLOSE" );
  Close();
  FitNoviceForm = 0;
}

void __fastcall TFitNoviceForm::ClearResults()
{
  ChiSquareEdit->Text = "";
  ConfidenceLevelEdit->Text = "";
  DegFreeEdit->Text = "";
  for( int i=1; i<ParameterStringGrid->RowCount; ++i )ParameterStringGrid->Cells[2][i] = "";
}

void __fastcall TFitNoviceForm::FitTypeChange( TObject *Sender )
{
  switch ( FitTypeComboBox->ItemIndex )
  {
    case 0: // Legendre polynomials
    {
      NumberLabel->Caption = "Degree of Polynomial";
      NumberLabel->Left = 15;
      NumberLabel->Visible = true;
      NumberLabel->Enabled = true;
      NumberComboBox->Visible = true;
      NumberComboBox->Enabled = true;
      NumberComboBox->ItemIndex = -1;
      int nitems = NumberComboBox->Items->Count;
      if( nitems == 8 )NumberComboBox->Items->Delete(7);
      ParameterStringGrid->RowCount = 2;
      ParameterStringGrid->Cells[0][1] = "";
      ParameterStringGrid->Cells[1][1] = "";
      ExpressionRichEdit->Text = "";
      ExpressionRichEdit->ReadOnly = true;
      ClearResults();
      FixedCheckBox2->Visible = false;
      FixedCheckBox3->Visible = false;
      FixedCheckBox4->Visible = false;
      FixedCheckBox5->Visible = false;
      FixedCheckBox6->Visible = false;
      FixedCheckBox7->Visible = false;
      FixedCheckBox8->Visible = false;
      break;
    }
    case 1: // polynomials
    {
      NumberLabel->Caption = "Degree of Polynomial";
      NumberLabel->Left = 15;
      NumberLabel->Visible = true;
      NumberLabel->Enabled = true;
      NumberComboBox->Visible = true;
      NumberComboBox->Enabled = true;
      NumberComboBox->ItemIndex = -1;
      int nitems = NumberComboBox->Items->Count;
      if( nitems == 8 )NumberComboBox->Items->Delete(7);
      ParameterStringGrid->RowCount = 2;
      ParameterStringGrid->Cells[0][1] = "";
      ParameterStringGrid->Cells[1][1] = "";
      ExpressionRichEdit->Text = "";
      ExpressionRichEdit->ReadOnly = true;
      ClearResults();
      FixedCheckBox2->Visible = false;
      FixedCheckBox3->Visible = false;
      FixedCheckBox4->Visible = false;
      FixedCheckBox5->Visible = false;
      FixedCheckBox6->Visible = false;
      FixedCheckBox7->Visible = false;
      FixedCheckBox8->Visible = false;
      break;
    }
    case 2: // Gaussian
    {
      NumberLabel->Visible = false;
      NumberLabel->Enabled = false;
      NumberComboBox->Visible = false;
      NumberComboBox->Enabled = false;
      //
      ParameterStringGrid->RowCount = 4;
      ParameterStringGrid->Cells[0][1] = "a0";
      ParameterStringGrid->Cells[1][1] = "1.0";
      ParameterStringGrid->Cells[0][2] = "a1";
      ParameterStringGrid->Cells[1][2] = "1.0";
      ParameterStringGrid->Cells[0][3] = "a2";
      ParameterStringGrid->Cells[1][3] = "1.0";
      FixedCheckBox1->Enabled = true;
      FixedCheckBox1->Visible = true;
      FixedCheckBox1->Checked = false;
      FixedCheckBox2->Enabled = true;
      FixedCheckBox2->Visible = true;
      FixedCheckBox2->Checked = false;
      FixedCheckBox3->Enabled = true;
      FixedCheckBox3->Visible = true;
      FixedCheckBox3->Checked = false;
      FixedCheckBox4->Visible = false;
      FixedCheckBox5->Visible = false;
      FixedCheckBox6->Visible = false;
      FixedCheckBox7->Visible = false;
      FixedCheckBox8->Visible = false;
      //
      ExpressionRichEdit->ReadOnly = true;
      UpdateExpression();
      TestFitButton->Enabled = true;
      DoFitButton->Enabled = true;
      ClearResults();
      break;
    }
    case 3: // least squares line
    {
      NumberLabel->Visible = false;
      NumberLabel->Enabled = false;
      NumberComboBox->Visible = false;
      NumberComboBox->Enabled = false;
      //
      ParameterStringGrid->RowCount = 3;
      ParameterStringGrid->Cells[0][1] = "a0";
      ParameterStringGrid->Cells[1][1] = "1.0";
      ParameterStringGrid->Cells[2][1] = "";
      ParameterStringGrid->Cells[0][2] = "a1";
      ParameterStringGrid->Cells[1][2] = "1.0";
      ParameterStringGrid->Cells[2][2] = "";
      FixedCheckBox1->Enabled = true;
      FixedCheckBox1->Visible = true;
      FixedCheckBox1->Checked = false;
      FixedCheckBox2->Enabled = true;
      FixedCheckBox2->Visible = true;
      FixedCheckBox2->Checked = false;
      FixedCheckBox3->Visible = false;
      FixedCheckBox4->Visible = false;
      FixedCheckBox5->Visible = false;
      FixedCheckBox6->Visible = false;
      FixedCheckBox7->Visible = false;
      FixedCheckBox8->Visible = false;
      //
      ExpressionRichEdit->ReadOnly = true;
      UpdateExpression();
      TestFitButton->Enabled = true;
      DoFitButton->Enabled = true;
      ClearResults();
      break;
    }
    case 4: // constant
    {
      NumberLabel->Visible = false;
      NumberLabel->Enabled = false;
      NumberComboBox->Visible = false;
      NumberComboBox->Enabled = false;
      //
      ParameterStringGrid->RowCount = 2;
      ParameterStringGrid->Cells[0][1] = "a0";
      ParameterStringGrid->Cells[1][1] = "1.0";
      FixedCheckBox1->Enabled = true;
      FixedCheckBox1->Visible = true;
      FixedCheckBox1->Checked = false;
      FixedCheckBox2->Visible = false;
      FixedCheckBox3->Visible = false;
      FixedCheckBox4->Visible = false;
      FixedCheckBox5->Visible = false;
      FixedCheckBox6->Visible = false;
      FixedCheckBox7->Visible = false;
      FixedCheckBox8->Visible = false;
      //
      ExpressionRichEdit->ReadOnly = true;
      UpdateExpression();
      TestFitButton->Enabled = true;
      DoFitButton->Enabled = true;
      ClearResults();
      break;
    }
    case 5: // Custom
    {
      NumberLabel->Caption = "Number of parameters";
      NumberLabel->Left = 10;
      NumberLabel->Visible = true;
      NumberLabel->Enabled = true;
      NumberComboBox->Visible = true;
      NumberComboBox->Enabled = true;
      NumberComboBox->ItemIndex = -1;
      int nitems = NumberComboBox->Items->Count;
      if( nitems == 7 )NumberComboBox->Items->Add("8");
      ParameterStringGrid->RowCount = 2;
      ParameterStringGrid->Cells[0][1] = "";
      ParameterStringGrid->Cells[1][1] = "";
      ExpressionRichEdit->ReadOnly = false;
      ExpressionRichEdit->Text = "";
      ClearResults();
      FixedCheckBox2->Visible = false;
      FixedCheckBox3->Visible = false;
      FixedCheckBox4->Visible = false;
      FixedCheckBox5->Visible = false;
      FixedCheckBox6->Visible = false;
      FixedCheckBox7->Visible = false;
      FixedCheckBox8->Visible = false;
      break;
    }
  }
  pStartValues_.clear();
}

void __fastcall TFitNoviceForm::NumberChange( TObject *Sender )
{
  int number = NumberComboBox->ItemIndex + 1;
  int nparams = FitTypeComboBox->ItemIndex==5 ? number : number+1;
  ParameterStringGrid->RowCount = nparams + 1;
  for( int i=0; i<nparams; ++i )
  {
    ParameterStringGrid->Cells[0][i+1] = AnsiString("a")+IntToStr(i);
    ParameterStringGrid->Cells[1][i+1] = "0.0";
  }
  FixedCheckBox1->Enabled = false;
  FixedCheckBox1->Visible = false;
  FixedCheckBox2->Enabled = false;
  FixedCheckBox2->Visible = false;
  FixedCheckBox3->Enabled = false;
  FixedCheckBox3->Visible = false;
  FixedCheckBox4->Enabled = false;
  FixedCheckBox4->Visible = false;
  FixedCheckBox5->Enabled = false;
  FixedCheckBox5->Visible = false;
  FixedCheckBox6->Enabled = false;
  FixedCheckBox6->Visible = false;
  FixedCheckBox7->Enabled = false;
  FixedCheckBox7->Visible = false;
  FixedCheckBox8->Enabled = false;
  FixedCheckBox8->Visible = false;
  if( nparams > 0 )
  {
    FixedCheckBox1->Enabled = true;
    FixedCheckBox1->Visible = true;
  }
  if( nparams > 1 )
  {
    FixedCheckBox2->Enabled = true;
    FixedCheckBox2->Visible = true;
  }
  if( nparams > 2 )
  {
    FixedCheckBox3->Enabled = true;
    FixedCheckBox3->Visible = true;
  }
  if( nparams > 3 )
  {
    FixedCheckBox4->Enabled = true;
    FixedCheckBox4->Visible = true;
  }
  if( nparams > 4 )
  {
    FixedCheckBox5->Enabled = true;
    FixedCheckBox5->Visible = true;
  }
  if( nparams > 5 )
  {
    FixedCheckBox6->Enabled = true;
    FixedCheckBox6->Visible = true;
  }
  if( nparams > 6 )
  {
    FixedCheckBox7->Enabled = true;
    FixedCheckBox7->Visible = true;
  }
  if( nparams > 7 )
  {
    FixedCheckBox8->Enabled = true;
    FixedCheckBox8->Visible = true;
  }
  UpdateExpression();
  TestFitButton->Enabled = true;
  DoFitButton->Enabled = true;
  pStartValues_.clear();
  ClearResults();
}

void __fastcall TFitNoviceForm::DataVecChange( TObject *Sender )
{
  dVecName_ = ExString( DataVecComboBox->Items->Strings[DataVecComboBox->ItemIndex].c_str() );
  IndepVecComboBox->Enabled = true;
  IndepVecLabel->Enabled = true;
  pStartValues_.clear();
  AnsiString tmp( DataVecComboBox->Items->Strings[DataVecComboBox->ItemIndex] );
  ExpressionLabel->Caption =
    AnsiString("Fit expression:  ")+tmp.SetLength(std::min(tmp.Length(),10))+" = ";
  ClearResults();
}

void __fastcall TFitNoviceForm::IndepVecChange( TObject *Sender )
{
  iVecName_ = ExString( IndepVecComboBox->Items->Strings[IndepVecComboBox->ItemIndex].c_str() );
  //
  IVMinEdit->Enabled = true;
  IVMaxEdit->Enabled = true;
  IVMinLabel->Enabled = true;
  IVMaxLabel->Enabled = true;
  double imin, imax;
  iVec_ = NVariableTable::GetTable()->GetVariable(iVecName_);
  iVec_->GetData().GetMinMax( imin, imax );
  IVMinEdit->Text = AnsiString(imin);
  IVMaxEdit->Text = AnsiString(imax);
  //
  if( FitTypeComboBox->ItemIndex >= 0 )
  {
    if( FitTypeComboBox->ItemIndex==2 || FitTypeComboBox->ItemIndex==3 ||
        FitTypeComboBox->ItemIndex==4 )UpdateExpression();
    else
    {
      if( NumberComboBox->ItemIndex >= 0 )UpdateExpression();
    }
  }
  ErrorVecLabel->Enabled = true;
  ErrorVecComboBox->Enabled = true;
  FitTypeComboBox->Enabled = true;
  FitTypeLabel->Enabled = true;
  pStartValues_.clear();
  ClearResults();
}

void __fastcall TFitNoviceForm::UpdateExpression()
{
  dVecName_ = DataVecComboBox->Items->Strings[DataVecComboBox->ItemIndex].c_str();
  //
  iVecName_ = IndepVecComboBox->Items->Strings[IndepVecComboBox->ItemIndex].c_str();
  //
  int fitTypeIndex = FitTypeComboBox->ItemIndex;
  int numberIndex = -1;
  fitExpression_.clear();
  fitTitle_.clear();
  switch ( fitTypeIndex )
  {
    case 0: // Legendre polynomials
    {
      numberIndex = NumberComboBox->ItemIndex;
      for( int i=0; i<=numberIndex; ++i )
      {
        ExString pName( ParameterStringGrid->Cells[0][i+1].c_str() );
        fitExpression_ += pName + "*LEGENDRE(" + ExString(i) + "," + iVecName_ + ")+";
        fitTitle_ += pName + "*P<_>" + ExString(i) + "<^>(" + iVecName_ +")+";
      }
      fitExpression_ += ExString(ParameterStringGrid->Cells[0][numberIndex+2].c_str()) +
                        "*LEGENDRE(" +ExString(numberIndex+1) + "," + iVecName_ + ")";
      fitTitle_ += ExString(ParameterStringGrid->Cells[0][numberIndex+2].c_str()) +
                   "*P<_>" +ExString(numberIndex+1) + "<^>(" + iVecName_ + ")";
      break;
    }
    case 1: // polynomial
    {
      numberIndex = NumberComboBox->ItemIndex;
      fitExpression_ = ExString( ParameterStringGrid->Cells[0][1].c_str() ) + "+";
      fitTitle_ = ExString( ParameterStringGrid->Cells[0][1].c_str() ) + "+";
      fitExpression_ += ExString(ParameterStringGrid->Cells[0][2].c_str()) + "*" + iVecName_;
      fitTitle_ += ExString(ParameterStringGrid->Cells[0][2].c_str()) + "*" + iVecName_;
      for( int i=2; i<=numberIndex; ++i )
      {
        ExString pName( ParameterStringGrid->Cells[0][i+1].c_str() );
        fitExpression_ += ExString("+") + pName + "*" + iVecName_ + "^" + ExString(i);
        fitTitle_ += ExString("+") + pName + "*" + iVecName_ + "<^>" + ExString(i) + "<_>";
      }
      if( numberIndex > 0 )
      {
        fitExpression_ += ExString("+") +
                          ExString(ParameterStringGrid->Cells[0][numberIndex+2].c_str()) +
                          "*" + iVecName_ + "^" + ExString(numberIndex+1);
        fitTitle_ += ExString("+") +
                     ExString(ParameterStringGrid->Cells[0][numberIndex+2].c_str()) +
                     "*" + iVecName_ + "<^>" + ExString(numberIndex+1) + "<_>";
      }
      break;
    }
    case 2: // Gaussian
    {
      fitExpression_ = ExString( ParameterStringGrid->Cells[0][1].c_str() ) +
                       "*exp(-("+iVecName_+"-"+
                       ParameterStringGrid->Cells[0][2].c_str()+")^2/"+
                       ParameterStringGrid->Cells[0][3].c_str()+"^2)";
      fitTitle_ = ExString( ParameterStringGrid->Cells[0][1].c_str() ) +
                  "*exp(" + iVecName_ + "-" +
                  ParameterStringGrid->Cells[0][2].c_str() + ")<^>2<_>/"+
                  ParameterStringGrid->Cells[0][3].c_str()+"<^>2<_>)";
      break;
    }
    case 3: // least squares line
    {
      fitExpression_ = ExString( ParameterStringGrid->Cells[0][1].c_str() ) + "+" +
                       ExString(ParameterStringGrid->Cells[0][2].c_str()) + "*" + iVecName_;
      fitTitle_ = ExString( ParameterStringGrid->Cells[0][1].c_str() ) + "+" +
                  ExString(ParameterStringGrid->Cells[0][2].c_str()) + "*" + iVecName_;
      break;
    }
    case 4: // constant
    {
      fitExpression_ = ExString( ParameterStringGrid->Cells[0][1].c_str() );
      fitTitle_ = ExString( ParameterStringGrid->Cells[0][1].c_str() );
      break;
    }
    case 5: // Custom
    {
      fitExpression_ = ExpressionRichEdit->Text.c_str();
      fitTitle_ = fitExpression_;
      break;
    }
  }
  ExpressionRichEdit->Text = AnsiString( fitExpression_.c_str() );
}

void __fastcall TFitNoviceForm::CreateParameters()
{
  int nParams = ParameterStringGrid->RowCount - 1;
  bool fixed[8];
  fixed[0] = FixedCheckBox1->Checked;
  fixed[1] = FixedCheckBox2->Checked;
  fixed[2] = FixedCheckBox3->Checked;
  fixed[3] = FixedCheckBox4->Checked;
  fixed[4] = FixedCheckBox5->Checked;
  fixed[5] = FixedCheckBox6->Checked;
  fixed[6] = FixedCheckBox7->Checked;
  fixed[7] = FixedCheckBox8->Checked;
  for( int i=1; i<=nParams; ++i )
  {
    ExString name( ParameterStringGrid->Cells[0][i].c_str() );
    double startValue;
    try
    {
      startValue = ParameterStringGrid->Cells[1][i].ToDouble();
    }
    catch (EConvertError &e)
    {
      AnsiString tmp("invalid start value entered for parameter ");
      tmp += IntToStr(i);
      Application->MessageBox( tmp.c_str(), "Error", MB_OK );
      return;
    }
    NVariableTable::GetTable()->RemoveEntry( name );
    NumericVariable *nv = NumericVariable::PutVariable( name, startValue, pvStr_ );
    if( !fixed[i-1] )nv->GetData().SetFit();
    else
      ParameterStringGrid->Cells[2][i] = ParameterStringGrid->Cells[1][i];
  }
}

void __fastcall TFitNoviceForm::TestFitButtonClick( TObject *Sender )
{
  CreateParameters();
  //
  eVecName_.clear();
  if( ErrorVecComboBox->ItemIndex >= 0 )
    eVecName_ = ExString( ErrorVecComboBox->Items->Strings[ErrorVecComboBox->ItemIndex].c_str() );
  //
  int nParams = ParameterStringGrid->RowCount - 1;
  pStartValues_.push_back( std::vector<double>(nParams) );
  for( int i=0; i<nParams; ++i )
  {
    try
    {
      pStartValues_.back().at(i) = ParameterStringGrid->Cells[1][i+1].ToDouble();
    }
    catch (EConvertError &e)
    {
      ExString tmp("invalid start value entered for parameter ");
      tmp += ExString(i);
      Application->MessageBox( tmp.c_str(), "Error", MB_OK );
      return;
    }
  }
  //
  GRA_window *currentWindow = ExGlobals::GetGraphWindow();
  //
  GRA_setOfCharacteristics *generalC = currentWindow->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataCurveC = currentWindow->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *xAxisC = currentWindow->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = currentWindow->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *textC = currentWindow->GetTextCharacteristics();
  //
  GRA_setOfCharacteristics generalCsave( *generalC );
  GRA_setOfCharacteristics dataCurveCsave( *dataCurveC );
  GRA_setOfCharacteristics xAxisCsave( *xAxisC );
  GRA_setOfCharacteristics yAxisCsave( *yAxisC );
  GRA_setOfCharacteristics textCsave( *textC );
  //
  currentWindow->Clear();
  currentWindow->Erase();
  //currentWindow->SetDefaults();
  std::vector<double> y, x, xe1, ye1, xe2, ye2;
  try
  {
    NumericVariable::GetVector( dVecName_.c_str(), "data variable", y );
    NumericVariable::GetVector( iVecName_.c_str(), "independent variable", x );
    if( !eVecName_.empty() )
      NumericVariable::GetVector( eVecName_.c_str(), "error variable", ye1 );
  }
  catch( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  std::size_t num = std::min( x.size(), y.size() );
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( !eVecName_.empty() )
  {
    num = std::min( num, ye1.size() );
    if( num < x.size() )x.resize( num );
    if( num < y.size() )y.resize( num );
    if( num < ye1.size() )ye1.resize( num );
  }
  //
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("PLOTSYMBOL"))->Set( -8 );
  if( num >= 200 )
    static_cast<GRA_sizeCharacteristic*>(dataCurveC->Get("PLOTSYMBOLSIZE"))->SetAsPercent( 0.5 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("PLOTSYMBOLCOLOR"))->Set(
    GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Set(
    GRA_colorControl::GetColor("BLACK") );
  //
  static_cast<GRA_stringCharacteristic*>(xAxisC->Get("LABEL"))->Set( iVecName_ );
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Set( true );
  //
  static_cast<GRA_stringCharacteristic*>(yAxisC->Get("LABEL"))->Set( dVecName_ );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("LABELON"))->Set( true );
  //
  static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Set( true );
  //
  std::vector<double> x2(2), y2(2);
  UsefulFunctions::MinMax( x, 0, num, x2[0], x2[1] );
  y2[0] = std::numeric_limits<double>::max();
  y2[1] = -y2[0];
  if( !ye1.empty() )
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i]+ye1[i] )y2[1] = y[i]+ye1[i];
      if( y2[0] > y[i]-ye1[i] )y2[0] = y[i]-ye1[i];
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i] )y2[1] = y[i];
      if( y2[0] > y[i] )y2[0] = y[i];
    }
  }
  GRA_cartesianAxes *cartesianAxes = new GRA_cartesianAxes(x2,y2,false,false);
  GRA_cartesianCurve *cartesianCurve = new GRA_cartesianCurve(x,y,xe1,ye1,xe2,ye2);
  try
  {
    cartesianAxes->Make();
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete cartesianAxes;
    delete cartesianCurve;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  cartesianAxes->Draw( ExGlobals::GetScreenOutput() );
  cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
  currentWindow->AddDrawableObject( cartesianAxes );
  currentWindow->AddDrawableObject( cartesianCurve );
  ye1.erase( ye1.begin(), ye1.end() );
  //
  static_cast<GRA_intCharacteristic*>(textC->Get("ALIGNMENT"))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(textC->Get("ANGLE"))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(textC->Get("COLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(textC->Get("FONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(textC->Get("INTERACTIVE"))->Set( false );
  double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->GetAsPercent();
  double xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->GetAsPercent();
  static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsPercent( xlaxis );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yuaxis+1 );
  //
  ExString title( "FIT: " );
  title += dVecName_ + " = " + fitTitle_;
  GRA_drawableText *dt = new GRA_drawableText( title );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  static_cast<GRA_intCharacteristic*>(textC->Get("ALIGNMENT"))->Set( 3 ); // lower right
  static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->SetAsPercent( 2.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsPercent( 98.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( 97.5 );
  dt = new GRA_drawableText( DateTimeToStr(Now()).c_str() );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( dVecName_ );
  if( !nv->GetOrigin().empty() )
  {
    static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( 94.5 );
    dt = new GRA_drawableText( nv->GetOrigin() );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete dt;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    currentWindow->AddDrawableObject( dt );
    ExGlobals::GetScreenOutput()->Draw( dt );
  }
  static_cast<GRA_intCharacteristic*>(textC->Get("ALIGNMENT"))->Set( 1 ); // lower left
  static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->SetAsPercent( 3.0 );
  //
  // re-evaluate the last expression
  //
  nv = NVariableTable::GetTable()->GetVariable( iVecName_ );
  std::vector<double> iVec( nv->GetData().GetData() );
  double imin, imax;
  nv->GetData().GetMinMax( imin, imax );
  int newNum = 200;
  double inc = (imax-imin)/(newNum-1);
  std::vector<double> newX( newNum );
  for( int i=0; i<newNum-1; ++i )newX[i] = imin+i*inc;
  newX[newNum-1] = imax;
  nv->GetData().SetData( newX );
  nv->GetData().SetDimMag( 0, newX.size() );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsPercent( xlaxis+3 );
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("PLOTSYMBOL"))->Set( 0 );
  static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->SetAsPercent( 2.0 );
  double yloc = yuaxis;
  int nTest = pStartValues_.size();
  int icolor = -1;
  for( int i=0; i<nTest; ++i )
  {
    for( int j=0; j<nParams; ++j )
    {
      ExString pName( ParameterStringGrid->Cells[0][j+1].c_str() );
      NVariableTable::GetTable()->GetVariable(pName)->GetData().SetScalarValue( pStartValues_[i][j] );
    }
    Expression expr( fitExpression_ );
    try
    {
      expr.Evaluate();
    }
    catch ( EExpressionError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      nv->GetData().SetData( iVec );
      nv->GetData().SetDimMag( 0, iVec.size() );
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    std::vector<double> newY( expr.GetFinalAnswer().GetData() );
    if( newY.size() == 1 )newY.insert( newY.begin()+1, newX.size()-1, newY[0] );
    //
    --icolor;
    if( icolor == -6 )--icolor; // to not use yellow (which is hard to see)
    GRA_color *color = GRA_colorControl::GetColor( icolor );
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Set( color );
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get("CURVELINETYPE"))->Set( i+2 );
    cartesianCurve = new GRA_cartesianCurve(newX,newY,xe1,ye1,xe2,ye2);
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      nv->GetData().SetData( iVec );
      nv->GetData().SetDimMag( 0, iVec.size() );
      delete cartesianCurve;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
    currentWindow->AddDrawableObject( cartesianCurve );
    //
    static_cast<GRA_colorCharacteristic*>(textC->Get("COLOR"))->Set( color );
    yloc -= 3.0;
    static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
    ExString nameList;
    for( int j=0; j<nParams-1; ++j )
    {
      nameList += ExString( ParameterStringGrid->Cells[0][j+1].c_str() ) +
                  ExString( " = " ) + ExString( pStartValues_[i][j] ) + ExString( ", " );
    }
    nameList += ExString( ParameterStringGrid->Cells[0][nParams].c_str() ) +
                ExString( " = " ) + ExString( pStartValues_[i][nParams-1] );
    GRA_drawableText *dt = new GRA_drawableText( nameList );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      nv->GetData().SetData( iVec );
      nv->GetData().SetDimMag( 0, iVec.size() );
      delete dt;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    currentWindow->AddDrawableObject( dt );
    ExGlobals::GetScreenOutput()->Draw( dt );
  }
  nv->GetData().SetData( iVec );
  nv->GetData().SetDimMag( 0, iVec.size() );
  //
  ExGlobals::ReplotCurrentWindow();
  //
  *generalC = generalCsave;
  *dataCurveC = dataCurveCsave;
  *xAxisC = xAxisCsave;
  *yAxisC = yAxisCsave;
  *textC = textCsave;
  //
  if( ExGlobals::StackIsOn() )
  {
    ExString line( "FORM FIT," );
    int number = NumberComboBox->ItemIndex + 1;
    int nparams = FitTypeComboBox->ItemIndex==5 ? number : number+1;
    ExString fixed( "[" );
    ExString start( "[" );
    if( nparams > 0 )
    {
      fixed += FixedCheckBox1->Checked ? "1" : "0";
      start += ParameterStringGrid->Cells[1][1].c_str();
    }
    if( nparams > 1 )
    {
      fixed += FixedCheckBox2->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][2].c_str();
    }
    if( nparams > 2 )
    {
      fixed += FixedCheckBox3->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][3].c_str();
    }
    if( nparams > 3 )
    {
      fixed += FixedCheckBox4->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][4].c_str();
    }
    if( nparams > 4 )
    {
      fixed += FixedCheckBox5->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][5].c_str();
    }
    if( nparams > 5 )
    {
      fixed += FixedCheckBox6->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][6].c_str();
    }
    if( nparams > 6 )
    {
      fixed += FixedCheckBox7->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][7].c_str();
    }
    if( nparams > 7 )
    {
      fixed += FixedCheckBox8->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][8].c_str();
    }
    fixed += "]";
    start += "]";
    line += dVecName_ + "," + iVecName_ + "," + IVMinEdit->Text.c_str() + "," +
            IVMaxEdit->Text.c_str() + "," + eVecName_ + ",'" + FitTypeComboBox->Text.c_str() +
            "'," + NumberComboBox->Text.c_str() + ",'" + ExpressionRichEdit->Text.c_str() +
            "'," + fixed + "," + start + "," + LegendXEdit->Text.c_str() + "," +
            LegendYEdit->Text.c_str() + ",'TEST THE FIT'";
    ExGlobals::WriteStack( line );
  }
}

void __fastcall TFitNoviceForm::DoFitButtonClick( TObject *Sender )
{
  CreateParameters();
  pStartValues_.clear();
  //
  dVecName_ = DataVecComboBox->Items->Strings[DataVecComboBox->ItemIndex].c_str();
  iVecName_ = IndepVecComboBox->Items->Strings[IndepVecComboBox->ItemIndex].c_str();
  eVecName_.clear();
  if( ErrorVecComboBox->ItemIndex >= 0 )
    eVecName_ = ExString( ErrorVecComboBox->Items->Strings[ErrorVecComboBox->ItemIndex].c_str() );
  //
  ExString command("FIT");
  int ntmax = 51;
  double tolerance = 0.00001;
  //
  std::vector<double> xVecSave, xVec, yVec, errorVec;
  try
  {
    NumericVariable::GetVector( dVecName_, "data variable", yVec );
    NumericVariable::GetVector( iVecName_, "independent variable", xVecSave );
    if( !eVecName_.empty() )
      NumericVariable::GetVector( eVecName_, "data error variable", errorVec );
  }
  catch( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  xVec.assign( xVecSave.begin(), xVecSave.end() );
  //
  std::size_t num = std::min( xVec.size(), yVec.size() );
  if( num < xVec.size() )xVec.resize( num );
  if( num < yVec.size() )yVec.resize( num );
  if( eVecName_.empty() )errorVec.assign( num, 1.0 );
  else
  {
    num = std::min( num, errorVec.size() );
    if( num < xVec.size() )xVec.resize( num );
    if( num < yVec.size() )yVec.resize( num );
    if( num < errorVec.size() )errorVec.resize( num );
  }
  try
  {
    minRange_ = IVMinEdit->Text.ToDouble();
  }
  catch (EConvertError &e)
  {
    Application->MessageBox( "invalid value entered for min", "Error", MB_OK );
    return;
  }
  try
  {
    maxRange_ = IVMaxEdit->Text.ToDouble();
  }
  catch (EConvertError &e)
  {
    Application->MessageBox( "invalid value entered for max", "Error", MB_OK );
    return;
  }
  std::vector<double> newX, newY, newE;
  for( std::size_t i=0; i<num; ++i )
  {
    if( xVec[i]>=minRange_ && xVec[i]<=maxRange_ )
    {
      newX.push_back( xVec[i] );
      newY.push_back( yVec[i] );
      newE.push_back( errorVec[i] );
    }
  }
  if( newX.empty() )
  {
    Application->MessageBox( "there are no data values within the independent vector range", "Error", MB_OK );
    return;
  }
  num = newX.size();
  NumericVariable *nvX = NVariableTable::GetTable()->GetVariable( iVecName_ );
  nvX->GetData().SetData( newX );
  nvX->GetData().SetDimMag( 0, newX.size() );
  //
  // decode the expression
  //
  Expression expr( fitExpression_ );
  expr.SetIsaFit();
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  std::size_t const nparam = expr.GetNumberOfFitParameters();
  if( nparam == 0 )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    Application->MessageBox( "there are no fitting parameters in the expression", "Error", MB_OK );
    return;
  }
  if( num <= nparam )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    Application->MessageBox( "number of data points <= number of parameters", "Error", MB_OK );
    return;
  }
  //
  // do the fit
  //
  std::vector<double> pp( nparam, 0.0 );
  //
  // initial parameter values
  //
  for( std::size_t i=0; i<nparam; ++i )pp[i] = expr.GetFitParameterValue( i );
  //
  std::vector<double> p1( pp );    // save original parameter values
  std::vector<double> pSave( pp );
  std::vector<double> e1( nparam ), e2( nparam );
  std::vector< std::vector<double> > errmat( nparam );
  for( std::size_t i=0; i<nparam; ++i )errmat[i].resize( nparam );
  nFree_ = 0;
  bool zeros = false;
  bool output = true;
  bool poisson = false;
  for( std::size_t i=0; i<num; ++i )
    newE[i] = ( newE[i]==0.0 ? 1.e6 : 1.0/(newE[i]*newE[i]) );
  bool marquardt = false;
  try
  {
    UsefulFunctions::LeastSquaresFit( &expr, dVecName_, newY, newE, e1, e2, pp, pSave,
                                      ntmax, tolerance, chisq_, confidenceLevel_, errmat,
                                      output, zeros, poisson, marquardt, nFree_ );
  }
  catch ( EExpressionError &e )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, p1[i] );
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  catch (...)
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, p1[i] );
    Application->MessageBox( "unknown error", "Error", MB_OK );
    return;
  }
  //
  nvX->GetData().SetData( xVecSave );
  nvX->GetData().SetDimMag( 0, xVecSave.size() );
  //
  // the fit was successful
  // re-evaluate the last expression and make new variable using the
  // calculated parameter values
  //
  Expression expr2( fitExpression_ );
  try
  {
    expr2.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  //
  // the fit is re-evaluated at every data point,
  // not just the points within the independent vector range
  //
  NumericData nd( expr2.GetFinalAnswer() );
  ExString yfit( dVecName_+"$UPDATE" );
  try
  {
    switch ( nd.GetNumberOfDimensions() )
    {
      case 0:
        NumericVariable::PutVariable( yfit, nd.GetScalarValue(), pvStr_ );
        break;
      case 1:
        NumericVariable::PutVariable( yfit, nd.GetData(), 0, pvStr_ );
        break;
      case 2:
        NumericVariable::PutVariable( yfit, nd.GetData(), nd.GetDimMag(0), nd.GetDimMag(1), pvStr_ );
    }
  }
  catch ( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  pvStr_ = ExString("FIT: ") + dVecName_ + " = " + fitExpression_;
  //
  // make residual vector with xNew.size() values
  //
  std::vector<double> residual;
  std::size_t xvSize = xVec.size();
  for( std::size_t i=0; i<xvSize; ++i )
  {
    if( xVec[i]>=minRange_ && xVec[i]<=maxRange_ )residual.push_back( nd.GetData()[i]-yVec[i] );
  }
  try
  {
    NumericVariable::PutVariable( "FIT$RESIDUAL", residual, 0, pvStr_ );
    NumericVariable::PutVariable( "FIT$XMIN", minRange_, pvStr_ );
    NumericVariable::PutVariable( "FIT$XMAX", maxRange_, pvStr_ );
    NumericVariable::PutVariable( "FIT$XRANGE", newX, 0, pvStr_ );
  }
  catch ( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  //
  // update the parameters and definition table after a successful fit
  //
  pStrings_.assign( expr.GetFitParameterNames().begin(), expr.GetFitParameterNames().end() );
  TextVariable::PutVariable( "FIT$VAR", pStrings_, pvStr_ );
  for( std::size_t i=0; i<nparam; ++i )
  {
    expr.SetFitParameterValue( i, pp[i], pvStr_ );
    int j = ParameterStringGrid->Cols[0]->IndexOf(AnsiString(pStrings_[i].c_str()));
    ParameterStringGrid->Cells[2][j] = AnsiString(pp[i]);
    //
    pStrings_[i] += ExString(" = ") + ExString::GetNiceNumber(pp[i]) +
      "  <pm> " + ExString::GetNiceNumber(e1[i]) + "  ( ";
    if( pp[i] == 0.0 )pStrings_[i] += ExString("0.0% )");
    else pStrings_[i] += ExString::GetNiceNumber(fabs(e1[i]/pp[i]*100),6,2) + "% )";
  }
  std::vector<double> corr( nparam*nparam, 0.0 );
  for( std::size_t i = 0; i < nparam; ++i )
  {
    for( std::size_t j = 0; j <= i; ++j )
    {
      corr[j+i*nparam] = 1.0;
      if( errmat[j][i] == 0.0 )corr[j+i*nparam] = 0.0;
      if( errmat[j][j]*errmat[i][i] > 0.0 )
      {
        corr[j+i*nparam] = errmat[j][i]/sqrt(errmat[j][j]*errmat[i][i]);
        corr[i+j*nparam] = corr[j+i*nparam];
      }
    }
    try
    {
      NumericVariable::PutVariable( "FIT$CORR", corr, nparam, nparam, pvStr_ );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  std::vector<double> cov( nparam*nparam, 0.0 );
  for( std::size_t i=0; i<nparam; ++i )
  {
    for( std::size_t j=0; j<nparam; ++j )cov[j+i*nparam] = errmat[j][i];
  }
  ChiSquareEdit->Text = AnsiString( chisq_ );
  ConfidenceLevelEdit->Text = AnsiString( confidenceLevel_ ) + AnsiString('\%');
  DegFreeEdit->Text = AnsiString( static_cast<int>(nFree_) );
  try
  {
    NumericVariable::PutVariable( "FIT$COVM", cov, nparam, nparam, pvStr_ );
    NumericVariable::PutVariable( "FIT$E1", e1, 0, pvStr_ );
    NumericVariable::PutVariable( "FIT$E2", e2, 0, pvStr_ );
    NumericVariable::PutVariable( "FIT$CHISQ", chisq_, pvStr_ );
    NumericVariable::PutVariable( "FIT$CL", confidenceLevel_, pvStr_ );
    NumericVariable::PutVariable( "FIT$FREE", static_cast<double>(nFree_), pvStr_ );
  }
  catch ( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  PlotFit();
}

void __fastcall TFitNoviceForm::PlotFit()
{
  GRA_window *currentWindow = ExGlobals::GetGraphWindow();
  //
  GRA_setOfCharacteristics *generalC = currentWindow->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataCurveC = currentWindow->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *xAxisC = currentWindow->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = currentWindow->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *textC = currentWindow->GetTextCharacteristics();
  //
  GRA_setOfCharacteristics generalCsave( *generalC );
  GRA_setOfCharacteristics dataCurveCsave( *dataCurveC );
  GRA_setOfCharacteristics xAxisCsave( *xAxisC );
  GRA_setOfCharacteristics yAxisCsave( *yAxisC );
  GRA_setOfCharacteristics textCsave( *textC );
  //
  currentWindow->Clear();
  currentWindow->Erase();
  std::vector<double> y, x, xe1, ye1, xe2, ye2;
  try
  {
    NumericVariable::GetVector( dVecName_.c_str(), "data variable", y );
    NumericVariable::GetVector( iVecName_.c_str(), "independent variable", x );
    if( !eVecName_.empty() )
      NumericVariable::GetVector( eVecName_.c_str(), "error variable", ye1 );
  }
  catch( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  std::size_t num = std::min( x.size(), y.size() );
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( !eVecName_.empty() )
  {
    num = std::min( num, ye1.size() );
    if( num < x.size() )x.resize( num );
    if( num < y.size() )y.resize( num );
    if( num < ye1.size() )ye1.resize( num );
  }
  double imin, imax;
  UsefulFunctions::MinMax( x, 0, num, imin, imax );
  //
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("PLOTSYMBOL"))->Set( -8 );
  if( num >= 200 )
    static_cast<GRA_sizeCharacteristic*>(dataCurveC->Get("PLOTSYMBOLSIZE"))->SetAsPercent( 0.5 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_stringCharacteristic*>(xAxisC->Get("LABEL"))->Set( iVecName_ );
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Set( true );
  static_cast<GRA_stringCharacteristic*>(yAxisC->Get("LABEL"))->Set( dVecName_ );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("LABELON"))->Set( true );
  //
  static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Set( true );
  //
  if( imin<minRange_ || imax>maxRange_ )
  {
    std::vector<GRA_color*> psym;
    for( std::size_t i=0; i<num; ++i )
      psym.push_back( ((minRange_<=x[i])&&(x[i]<=maxRange_)) ?
        GRA_colorControl::GetColor("PURPLE") : GRA_colorControl::GetColor("BLACK") );
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("PLOTSYMBOLCOLOR"))->Set( psym );
  }
  else
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("PLOTSYMBOLCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  //
  std::vector<double> x2(2), y2(2);
  x2[0] = imin;
  x2[1] = imax;
  y2[0] = std::numeric_limits<double>::max();
  y2[1] = -y2[0];
  if( !ye1.empty() )
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i]+ye1[i] )y2[1] = y[i]+ye1[i];
      if( y2[0] > y[i]-ye1[i] )y2[0] = y[i]-ye1[i];
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i] )y2[1] = y[i];
      if( y2[0] > y[i] )y2[0] = y[i];
    }
  }
  GRA_cartesianAxes *cartesianAxes = 0;
  GRA_cartesianCurve *cartesianCurve = 0;
  try
  {
    cartesianAxes = new GRA_cartesianAxes(x2,y2,false,false);
    cartesianCurve = new GRA_cartesianCurve(x,y,xe1,ye1,xe2,ye2);
    cartesianAxes->Make();
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete cartesianAxes;
    delete cartesianCurve;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  cartesianAxes->Draw( ExGlobals::GetScreenOutput() );
  cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
  currentWindow->AddDrawableObject( cartesianAxes );
  currentWindow->AddDrawableObject( cartesianCurve );
  //
  if( imin<minRange_ || imax>maxRange_ )
  {
    std::vector<double> ytmp, xtmp, xe1t, ye1t, xe2t, ye2t;
    double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MIN"))->Get();
    double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MAX"))->Get();
    double ylogBase = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("LOGBASE"))->Get();
    xtmp.push_back( minRange_ );
    xtmp.push_back( minRange_ );
    if( ylogBase > 1.0 )
    {
      ymin = pow(ylogBase,ymin);
      ymax = pow(ylogBase,ymax);
    }
    ytmp.push_back( ymin );
    ytmp.push_back( ymax );
    GRA_color *color = static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Get();
    int linetype = static_cast<GRA_intCharacteristic*>(dataCurveC->Get("CURVELINETYPE"))->Get();
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Set(
      GRA_colorControl::GetColor("PURPLE") );
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get("CURVELINETYPE"))->Set( 5 );  // 5
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get("PLOTSYMBOL"))->Set( 0 );
    try
    {
      cartesianCurve = new GRA_cartesianCurve(xtmp,ytmp,xe1t,ye1t,xe2t,ye2t);
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete cartesianCurve;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
    currentWindow->AddDrawableObject( cartesianCurve );
    xtmp[0] = maxRange_;
    xtmp[1] = maxRange_;
    try
    {
      cartesianCurve = new GRA_cartesianCurve(xtmp,ytmp,xe1t,ye1t,xe2t,ye2t);
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete cartesianCurve;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
    currentWindow->AddDrawableObject( cartesianCurve );
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Set( color );
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get("CURVELINETYPE"))->Set( linetype );
  }
  ye1.erase( ye1.begin(), ye1.end() );
  //
  // re-evaluate the last expression
  //
  ExString fitDataVec( dVecName_+"$FIT" );
  ExString fitIndepVec( iVecName_+"$FIT" );
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( iVecName_ );
  std::vector<double> iVec( nv->GetData().GetData() );
  nv->GetData().GetMinMax( imin, imax );
  int newNum = 500;
  double inc = (imax-imin)/(newNum-1);
  std::vector<double> newX( newNum );
  for( int i=0; i<newNum-1; ++i )newX[i] = imin+i*inc;
  newX[newNum-1] = imax;
  nv->GetData().SetData( newX );
  nv->GetData().SetDimMag( 0, newX.size() );
  Expression expr( fitExpression_ );
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    nv->GetData().SetData( iVec );
    nv->GetData().SetDimMag( 0, iVec.size() );
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  std::vector<double> newY( expr.GetFinalAnswer().GetData() );
  if( newY.size() == 1 )newY.resize( newX.size(), newY[0] );
  nv->GetData().SetData( iVec );
  nv->GetData().SetDimMag( 0, iVec.size() );
  try
  {
    NumericVariable::PutVariable( fitIndepVec, newX, 0, pvStr_ );
    NumericVariable::PutVariable( fitDataVec, newY, 0, pvStr_ );
  }
  catch ( EVariableError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("PLOTSYMBOL"))->Set( 0 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Set(
    GRA_colorControl::GetColor(-2) );
  try
  {
    cartesianCurve = new GRA_cartesianCurve(newX,newY,xe1,ye1,xe2,ye2);
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete cartesianCurve;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
  currentWindow->AddDrawableObject( cartesianCurve );
  //
  double const ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("LOWERAXIS"))->GetAsPercent();
  double const xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->GetAsPercent();
  double const yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->GetAsPercent();
  double const xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->GetAsPercent();
  //
  static_cast<GRA_intCharacteristic*>(textC->Get("ALIGNMENT"))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(textC->Get("ANGLE"))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(textC->Get("COLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(textC->Get("FONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(textC->Get("INTERACTIVE"))->Set( false );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsPercent( xlaxis );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yuaxis+1 );
  //
  ExString title( "FIT: " );
  title += dVecName_ + " = " + fitTitle_;
  GRA_drawableText *dt = new GRA_drawableText( title );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  static_cast<GRA_intCharacteristic*>(textC->Get("ALIGNMENT"))->Set( 3 ); // lower right
  static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->SetAsPercent( 2.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsPercent( 98.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( 97.5 );
  dt = new GRA_drawableText( DateTimeToStr(Now()).c_str() );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  nv = NVariableTable::GetTable()->GetVariable( dVecName_ );
  if( !nv->GetOrigin().empty() )
  {
    static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( 94.5 );
    dt = new GRA_drawableText( nv->GetOrigin() );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete dt;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    currentWindow->AddDrawableObject( dt );
    ExGlobals::GetScreenOutput()->Draw( dt );
  }
  //
  double xloc, yloc;
  try
  {
    xloc = LegendXEdit->Text.ToDouble();
  }
  catch (EConvertError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    Application->MessageBox( "invalid value entered for legend x location", "Error", MB_OK );
    return;
  }
  try
  {
    yloc = LegendYEdit->Text.ToDouble();
  }
  catch (EConvertError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    Application->MessageBox( "invalid value entered for legend y location", "Error", MB_OK );
    return;
  }
  //
  std::size_t nparams = pStrings_.size();
  static_cast<GRA_intCharacteristic*>(textC->Get("ALIGNMENT"))->Set( 1 ); // lower left
  static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsPercent( xloc );
  for( std::size_t i=0; i<nparams; ++i )
  {
    yloc -= 4.0;
    static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
    dt = new GRA_drawableText( pStrings_[i] );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete dt;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    currentWindow->AddDrawableObject( dt );
    ExGlobals::GetScreenOutput()->Draw( dt );
  }
  yloc -= 6.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
  ExString str( "Number of data points = " );
  str += ExString(num);
  dt = new GRA_drawableText( str );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
  AnsiString astr = AnsiString("Fit range: ") + AnsiString(minRange_) +
        " to " + AnsiString(maxRange_);
  dt = new GRA_drawableText( astr.c_str() );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
  str = ExString("Degrees of freedom = ") + ExString(nFree_);
  dt = new GRA_drawableText( str );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
  str = ExString("Total <chi><^>2<_> = ") + ExString::GetNiceNumber(chisq_) +
        ",   <chi><^>2<_>/df = " + ExString::GetNiceNumber(chisq_/nFree_);
  dt = new GRA_drawableText( str );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
  str = ExString("Confidence level = ") + ExString::GetNiceNumber(confidenceLevel_,6,2) + "%";
  dt = new GRA_drawableText( str );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  currentWindow->AddDrawableObject( dt );
  ExGlobals::GetScreenOutput()->Draw( dt );
  //
  ExGlobals::ReplotCurrentWindow();
  //
  *generalC = generalCsave;
  *dataCurveC = dataCurveCsave;
  *xAxisC = xAxisCsave;
  *yAxisC = yAxisCsave;
  *textC = textCsave;
  //
  if( ExGlobals::StackIsOn() )
  {
    ExString line( "FORM FIT," );
    int number = NumberComboBox->ItemIndex + 1;
    int nparams = FitTypeComboBox->ItemIndex==5 ? number : number+1;
    ExString fixed( "[" );
    ExString start( "[" );
    if( nparams > 0 )
    {
      fixed += FixedCheckBox1->Checked ? "1" : "0";
      start += ParameterStringGrid->Cells[1][1].c_str();
    }
    if( nparams > 1 )
    {
      fixed += FixedCheckBox2->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][2].c_str();
    }
    if( nparams > 2 )
    {
      fixed += FixedCheckBox3->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][3].c_str();
    }
    if( nparams > 3 )
    {
      fixed += FixedCheckBox4->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][4].c_str();
    }
    if( nparams > 4 )
    {
      fixed += FixedCheckBox5->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][5].c_str();
    }
    if( nparams > 5 )
    {
      fixed += FixedCheckBox6->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][6].c_str();
    }
    if( nparams > 6 )
    {
      fixed += FixedCheckBox7->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][7].c_str();
    }
    if( nparams > 7 )
    {
      fixed += FixedCheckBox8->Checked ? ";1" : ";0";
      start += ExString(";") + ParameterStringGrid->Cells[1][8].c_str();
    }
    fixed += "]";
    start += "]";
    line += dVecName_ + "," + iVecName_ + "," + IVMinEdit->Text.c_str() + "," +
            IVMaxEdit->Text.c_str() + "," + eVecName_ + ",'" + FitTypeComboBox->Text.c_str() +
            "'," + NumberComboBox->Text.c_str() + ",'" + ExpressionRichEdit->Text.c_str() +
            "'," + fixed + "," + start + "," + LegendXEdit->Text.c_str() + "," +
            LegendYEdit->Text.c_str() + ",'DO THE FIT'";
    ExGlobals::WriteStack( line );
  }
}

void __fastcall TFitNoviceForm::SelectCell( TObject *Sender, int ACol, int ARow, bool &CanSelect )
{
  CanSelect = (ACol==1); // can only modify the parameter start values
}

void __fastcall TFitNoviceForm::ClearButtonClick( TObject *Sender )
{
  ExGlobals::ClearGraphicsMonitor();
  ExGlobals::ClearWindows();
  pStartValues_.clear();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM FIT,'CLEAR GRAPHICS'" );
}

void __fastcall TFitNoviceForm::ErrorVecChange( TObject *Sender )
{
  eVecName_.clear();
  if( ErrorVecComboBox->ItemIndex >= 0 )
    eVecName_ = ExString( ErrorVecComboBox->Items->Strings[ErrorVecComboBox->ItemIndex].c_str() );
}

void __fastcall TFitNoviceForm::ExpressionEditChange( TObject *Sender )
{
  if( FitTypeComboBox->ItemIndex == 5 ) // custom type fit
  {
    fitExpression_ = ExpressionRichEdit->Text.c_str();
    fitTitle_ = fitExpression_;
  }
}

void __fastcall TFitNoviceForm::FormActivate( TObject *Sender )
{
  if( !iVecName_.empty() )
  {
    NumericVariable *tmp = NVariableTable::GetTable()->GetVariable(iVecName_);
    if( tmp != iVec_ ) // different variable with the same name
    {
      iVec_ = tmp;
      double imin, imax;
      iVec_->GetData().GetMinMax( imin, imax );
      IVMinEdit->Text = AnsiString(imin);
      IVMaxEdit->Text = AnsiString(imax);
    }
  }
}

void TFitNoviceForm::Set( ExString const &dataName, ExString const &indepName,
                          double idmin, double idmax, ExString const &errorName,
                          ExString const &typeName, int nParams,
                          ExString const &expression,
                          std::vector<double> &fixed, std::vector<double> &start,
                          double xloc, double yloc, ExString const &action )
{
  DataVecComboBox->ItemIndex = DataVecComboBox->Items->IndexOf( dataName.c_str() );
  dVecName_ = dataName.UpperCase();
  AnsiString tmp( dVecName_.c_str() );
  ExpressionLabel->Caption =
      AnsiString("Fit expression:  ")+tmp.SetLength(std::min(tmp.Length(),10))+" = ";
  IndepVecComboBox->Enabled = true;
  IndepVecLabel->Enabled = true;
  IndepVecComboBox->ItemIndex = IndepVecComboBox->Items->IndexOf( indepName.c_str() );
  iVecName_ = indepName.UpperCase();
  //
  IVMinEdit->Enabled = true;
  IVMaxEdit->Enabled = true;
  IVMinLabel->Enabled = true;
  IVMaxLabel->Enabled = true;
  IVMinEdit->Text = AnsiString(idmin);
  IVMaxEdit->Text = AnsiString(idmax);
  //
  ErrorVecLabel->Enabled = true;
  ErrorVecComboBox->Enabled = true;
  ErrorVecComboBox->ItemIndex = ErrorVecComboBox->Items->IndexOf( errorName.c_str() );
  eVecName_ = errorName.UpperCase();
  //
  FitTypeComboBox->Enabled = true;
  FitTypeLabel->Enabled = true;
  FitTypeComboBox->ItemIndex = FitTypeComboBox->Items->IndexOf( typeName.c_str() );
  FitTypeChange( static_cast<TObject*>(0) );
  if( FitTypeComboBox->ItemIndex == 0 || FitTypeComboBox->ItemIndex == 1 )
  {
    NumberComboBox->ItemIndex = nParams-2;
    NumberChange( static_cast<TObject*>(0) );
  }
  else if( FitTypeComboBox->ItemIndex == 5 )
  {
    NumberComboBox->ItemIndex = nParams-1;
    NumberChange( static_cast<TObject*>(0) );
  }
  ExpressionRichEdit->Text = expression.c_str();
  UpdateExpression();
  ParameterStringGrid->RowCount = nParams+1;
  for( int i=0; i<nParams; ++i )
  {
    ParameterStringGrid->Cells[0][i+1] = AnsiString("a")+AnsiString(i);
    ParameterStringGrid->Cells[1][i+1] = AnsiString( start[i] );
    ParameterStringGrid->Cells[2][i+1] = "";
  }
  FixedCheckBox1->Enabled = true;
  FixedCheckBox1->Visible = true;
  FixedCheckBox1->Checked = (fixed[0]!=0.0);
  if( nParams > 1 )
  {
    FixedCheckBox2->Enabled = true;
    FixedCheckBox2->Visible = true;
    FixedCheckBox2->Checked = (fixed[1]!=0.0);
    if( nParams > 2 )
    {
      FixedCheckBox3->Enabled = true;
      FixedCheckBox3->Visible = true;
      FixedCheckBox3->Checked = (fixed[2]!=0.0);
      if( nParams > 3 )
      {
        FixedCheckBox4->Enabled = true;
        FixedCheckBox4->Visible = true;
        FixedCheckBox4->Checked = (fixed[3]!=0.0);
        if( nParams > 4 )
        {
          FixedCheckBox5->Enabled = true;
          FixedCheckBox5->Visible = true;
          FixedCheckBox5->Checked = (fixed[4]!=0.0);
          if( nParams > 5 )
          {
            FixedCheckBox6->Enabled = true;
            FixedCheckBox6->Visible = true;
            FixedCheckBox6->Checked = (fixed[5]!=0.0);
            if( nParams > 6 )
            {
              FixedCheckBox7->Enabled = true;
              FixedCheckBox7->Visible = true;
              FixedCheckBox7->Checked = (fixed[6]!=0.0);
              if( nParams > 7 )
              {
                FixedCheckBox8->Enabled = true;
                FixedCheckBox8->Visible = true;
                FixedCheckBox8->Checked = (fixed[7]!=0.0);
              }
            }
          }
        }
      }
    }
  }
  TestFitButton->Enabled = true;
  DoFitButton->Enabled = true;
  LegendXEdit->Text = AnsiString( xloc );
  LegendYEdit->Text = AnsiString( yloc );
  if( action == "CLEAR GRAPHICS" )
  {
    ClearButtonClick( static_cast<TObject*>(0) );
  }
  else if( action == "TEST THE FIT" )
  {
    TestFitButtonClick( static_cast<TObject*>(0) );
  }
  else if( action == "DO THE FIT" )
  {
    DoFitButtonClick( static_cast<TObject*>(0) );
  }
  else if( action == "CLOSE" )
  {
    CloseButtonClick( static_cast<TObject*>(0) );
  }
}

//---------------------------------------------------------------------------

