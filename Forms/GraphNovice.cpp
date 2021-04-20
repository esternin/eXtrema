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

#include "GraphNovice.h"

#include "ExGlobals.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "GRA_cartesianCurve.h"
#include "GRA_cartesianAxes.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_colorControl.h"
#include "GRA_window.h"
#include "GRA_drawableObject.h"
#include "UsefulFunctions.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TGraphNoviceForm *GraphNoviceForm;

__fastcall TGraphNoviceForm::TGraphNoviceForm( TComponent* Owner ) : TForm(Owner)
{
  FillOutForm();
  //
  int const foo[] = {0,1,14,2,3,4,5,6,15,7,8,16,9,17,10,18,11,12,13};
  symArray_.assign( foo, foo+(sizeof foo/sizeof *foo) );
  //
  color_ = 0;
  //
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "GraphNoviceForm", "Top", 25 );
  Left = ini->ReadInteger( "GraphNoviceForm", "Left", 600 );
  Height = ini->ReadInteger( "GraphNoviceForm", "Height", 230 );
  Width = ini->ReadInteger( "GraphNoviceForm", "Width", 495 );
  //
  DepVarComboBox->ItemIndex = DepVarComboBox->Items->IndexOf(
    ini->ReadString("GraphNoviceForm","DependentVector","") );
  //
  IndepVarComboBox->ItemIndex = IndepVarComboBox->Items->IndexOf(
    ini->ReadString("GraphNoviceForm","IndependentVector","") );
  if( IndepVarComboBox->ItemIndex > 0 )
  {
    ExString iVecName = ExString( IndepVarComboBox->Items->Strings[IndepVarComboBox->ItemIndex].c_str() );
    IVMinEdit->Enabled = true;
    IVMaxEdit->Enabled = true;
    IVMinLabel->Enabled = true;
    IVMaxLabel->Enabled = true;
    double imin, imax;
    NumericVariable *iVec = NVariableTable::GetTable()->GetVariable(iVecName);
    iVec->GetData().GetMinMax( imin, imax );
    IVMinEdit->Text = AnsiString(imin);
    IVMaxEdit->Text = AnsiString(imax);
  }
  ErrorsComboBox->ItemIndex = ErrorsComboBox->Items->IndexOf(
    ini->ReadString("GraphNoviceForm","ErrorVector","") );
  //
  int ps = ini->ReadInteger( "GraphNoviceForm", "Plotsymbol", 18 );
  PlotSymbolComboBox->ItemIndex = -1;
  int size = symArray_.size();
  for( int i=0; i<size; ++i )
  {
    if( abs(ps) == symArray_[i] )
    {
      PlotSymbolComboBox->ItemIndex = i;
      break;
    }
  }
  ConnectCheckBox->Checked = (ps>=0);
  //
  delete ini;
}

__fastcall TGraphNoviceForm::~TGraphNoviceForm()
{
  //int const symArray_[19] = {0,1,14,2,3,4,5,6,15,7,8,16,9,17,10,18,11,12,13};
  //
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "GraphNoviceForm", "Top", Top );
  ini->WriteInteger( "GraphNoviceForm", "Left", Left );
  ini->WriteInteger( "GraphNoviceForm", "Height", Height );
  ini->WriteInteger( "GraphNoviceForm", "Width", Width );
  //
  int idx = DepVarComboBox->ItemIndex;
  AnsiString tmp;
  if( idx >= 0 )tmp = DepVarComboBox->Items->Strings[idx];
  ini->WriteString( "GraphNoviceForm", "DependentVector", tmp );
  //
  idx = IndepVarComboBox->ItemIndex;
  tmp = "";
  if( idx >= 0 )tmp = IndepVarComboBox->Items->Strings[idx];
  ini->WriteString( "GraphNoviceForm", "IndependentVector", tmp );
  //
  idx = ErrorsComboBox->ItemIndex;
  tmp = "";
  if( idx >= 0 )tmp = ErrorsComboBox->Items->Strings[idx];
  ini->WriteString( "GraphNoviceForm", "ErrorVector", tmp );
  //
  int ps = symArray_[PlotSymbolComboBox->ItemIndex];
  if( !ConnectCheckBox->Checked )ps *= -1;
  ini->WriteInteger( "GraphNoviceForm", "Plotsymbol", ps );
  //
  delete ini;
}

void __fastcall TGraphNoviceForm::FillOutForm()
{
  DepVarComboBox->Clear();
  IndepVarComboBox->Clear();
  ErrorsComboBox->Clear();
  IndepVarComboBox->Items->Add( "<none>" );
  ErrorsComboBox->Items->Add( "<none>" );
  NVariableTable *nvTable = NVariableTable::GetTable();
  int entries = nvTable->Entries();
  for( int i=0; i<entries; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry(i);
    if( nv->GetData().GetNumberOfDimensions() == 1 )
    {
      AnsiString name( nv->GetName().c_str() );
      DepVarComboBox->Items->Add( name );
      IndepVarComboBox->Items->Add( name );
      ErrorsComboBox->Items->Add( name );
    }
  }
  DepVarComboBox->ItemIndex = -1;
  IndepVarComboBox->ItemIndex = 0;
  ErrorsComboBox->ItemIndex = 0;
  PlotSymbolComboBox->ItemIndex = 15;
  ConnectCheckBox->Enabled = true;
  IVMinEdit->Enabled = false;
  IVMaxEdit->Enabled = false;
  IVMinLabel->Enabled = false;
  IVMaxLabel->Enabled = false;
}

void __fastcall TGraphNoviceForm::DrawGraphClick( TObject *Sender )
{
  GRA_window *currentWindow = ExGlobals::GetGraphWindow();
  //
  GRA_setOfCharacteristics *generalC = currentWindow->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataCurveC = currentWindow->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *xAxisC = currentWindow->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = currentWindow->GetYAxisCharacteristics();
  //
  std::vector<double> x, y, xe1, ye1, xe2, ye2;
  int depIndex = DepVarComboBox->ItemIndex;
  int indepIndex = IndepVarComboBox->ItemIndex;
  int errIndex = ErrorsComboBox->ItemIndex;
  //
  if( depIndex == -1 )
  {
    Application->MessageBox( "a dependent variable vector must be chosen", "Error", MB_OK );
    return;
  }
  AnsiString depName( DepVarComboBox->Text );
  try
  {
    NumericVariable::GetVector( depName.c_str(), "dependent variable", y );
  }
  catch( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  AnsiString indepName;
  double minRange, maxRange;
  if( indepIndex < 1 ) // no x vector chosen from the list
  {
    std::size_t size = y.size();
    for( std::size_t i=0; i<size; ++i )x.push_back( static_cast<double>(i+1) );
    minRange = 1;
    maxRange = size;
  }
  else                 // x was chosen from the list
  {
    indepName = IndepVarComboBox->Items->Strings[indepIndex];
    try
    {
      NumericVariable::GetVector( indepName.c_str(), "independent variable", x );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    try
    {
      minRange = IVMinEdit->Text.ToDouble();
    }
    catch (EConvertError &e)
    {
      Application->MessageBox( "invalid value entered for min", "Error", MB_OK );
      return;
    }
    try
    {
      maxRange = IVMaxEdit->Text.ToDouble();
    }
    catch (EConvertError &e)
    {
      Application->MessageBox( "invalid value entered for max", "Error", MB_OK );
      return;
    }
  }
  AnsiString errorName;
  if( errIndex > 0 ) // an error vector was chosen from the list
  {
    errorName = ErrorsComboBox->Items->Strings[errIndex];
    try
    {
      NumericVariable::GetVector( errorName.c_str(), "error vector", ye1 );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  std::size_t num = std::min( x.size(), y.size() );
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( !ye1.empty() )
  {
    num = std::min( num, ye1.size() );
    if( num < x.size() )x.resize( num );
    if( num < y.size() )y.resize( num );
    if( num < ye1.size() )ye1.resize( num );
  }
  std::vector<double> newX, newY, newE;
  for( std::size_t i=0; i<num; ++i )
  {
    if( x[i]>=minRange && x[i]<=maxRange )
    {
      newX.push_back( x[i] );
      newY.push_back( y[i] );
      if( !ye1.empty() )newE.push_back( ye1[i] );
    }
  }
  if( newX.empty() )
  {
    Application->MessageBox( "there are no data values within the specified independent vector range", "Error", MB_OK );
    return;
  }
  num = newX.size();
  --color_;
  if( color_ == -6 )--color_; // to not use yellow (which is hard to see)
  static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Set( true );
  //
  int ps = symArray_[PlotSymbolComboBox->ItemIndex];
  if( !ConnectCheckBox->Checked )ps *= -1;
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("PLOTSYMBOL"))->Set( ps );
  //
  if( ps!=0 && num>=200 )
    static_cast<GRA_sizeCharacteristic*>(dataCurveC->Get("PLOTSYMBOLSIZE"))->SetAsPercent( 0.5 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("PLOTSYMBOLCOLOR"))->Set(
    GRA_colorControl::GetColor(color_) );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get("CURVECOLOR"))->Set(
    GRA_colorControl::GetColor(color_) );
  //
  if( !indepName.IsEmpty() )
  {
    static_cast<GRA_stringCharacteristic*>(xAxisC->Get("LABEL"))->Set( indepName.c_str() );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Set( true );
  }
  else static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Set( false );
  //
  static_cast<GRA_stringCharacteristic*>(yAxisC->Get("LABEL"))->Set( depName.c_str() );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("LABELON"))->Set( true );
  //
  std::vector<double> x2(2), y2(2);
  UsefulFunctions::MinMax( newX, 0, num, x2[0], x2[1] );
  y2[0] = std::numeric_limits<double>::max();
  y2[1] = -y2[0];
  if( !newE.empty() )
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < newY[i]+newE[i] )y2[1] = newY[i]+newE[i];
      if( y2[0] > newY[i]-newE[i] )y2[0] = newY[i]-newE[i];
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < newY[i] )y2[1] = newY[i];
      if( y2[0] > newY[i] )y2[0] = newY[i];
    }
  }
  GRA_cartesianAxes *cartesianAxes = 0;
  try
  {
    cartesianAxes = new GRA_cartesianAxes(x2,y2,false,false);
    cartesianAxes->Make();
  }
  catch (EGraphicsError &e)
  {
    delete cartesianAxes;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  cartesianAxes->Draw( ExGlobals::GetScreenOutput() );
  currentWindow->AddDrawableObject( cartesianAxes );
  //
  GRA_cartesianCurve *cartesianCurve = 0;
  try
  {
    cartesianCurve = new GRA_cartesianCurve(newX,newY,xe1,newE,xe2,ye2,false);
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    delete cartesianCurve;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
  currentWindow->AddDrawableObject( cartesianCurve );
  ExGlobals::ReplotCurrentWindow();
  //
  if( ExGlobals::StackIsOn() )
  {
    ExString line( "FORM GRAPH," );
    int ps = symArray_[PlotSymbolComboBox->ItemIndex];
    if( !ConnectCheckBox->Checked )ps *= -1;
    line += ExString(depName.c_str()) + "," + indepName.c_str() + "," +
            IVMinEdit->Text.c_str() + "," + IVMaxEdit->Text.c_str() +
            "," + errorName.c_str() + "," + ExString(ps) + ",DRAW";
    ExGlobals::WriteStack( line );
  }
}

void __fastcall TGraphNoviceForm::ClearClick( TObject *Sender )
{
  ExGlobals::ClearGraphicsMonitor();
  ExGlobals::ClearWindows();
  color_ = 0;
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM GRAPH,CLEAR" );
}

void __fastcall TGraphNoviceForm::CloseClick( TObject *Sender )
{
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM GRAPH,CLOSE" );
  Close();
  GraphNoviceForm = 0;
}

void __fastcall TGraphNoviceForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TGraphNoviceForm::FormCloseQuery( TObject *Sender, bool &CanClose )
{ GraphNoviceForm = 0; }

void __fastcall TGraphNoviceForm::UpdateVars( TObject *Sender )
{
  TComboBox *cBox = static_cast<TComboBox*>(Sender);
  cBox->Clear();
  if( cBox==IndepVarComboBox || cBox==ErrorsComboBox )cBox->Items->Add( "<none>" );
  NVariableTable *nvTable = NVariableTable::GetTable();
  int entries = nvTable->Entries();
  for( int i=0; i<entries; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry(i);
    if( nv->GetData().GetNumberOfDimensions() == 1 )cBox->Items->Add( nv->GetName().c_str() );
  }
  cBox->ItemIndex = -1;
}

void __fastcall TGraphNoviceForm::IndepVarChange(TObject *Sender)
{
  if( IndepVarComboBox->ItemIndex == 0 )
  {
    IVMinEdit->Enabled = false;
    IVMaxEdit->Enabled = false;
    IVMinLabel->Enabled = false;
    IVMaxLabel->Enabled = false;
    return;
  }
  ExString iVecName( IndepVarComboBox->Items->Strings[IndepVarComboBox->ItemIndex].c_str() );
  //
  IVMinEdit->Enabled = true;
  IVMaxEdit->Enabled = true;
  IVMinLabel->Enabled = true;
  IVMaxLabel->Enabled = true;
  double imin, imax;
  NumericVariable *iVec = NVariableTable::GetTable()->GetVariable(iVecName);
  iVec->GetData().GetMinMax( imin, imax );
  IVMinEdit->Text = AnsiString(imin);
  IVMaxEdit->Text = AnsiString(imax);
}

void __fastcall TGraphNoviceForm::PlotSymbolChange( TObject *Sender )
{
  ConnectCheckBox->Enabled = (PlotSymbolComboBox->ItemIndex != 0);
}

void TGraphNoviceForm::Set( ExString const &depName, ExString const &indepName,
                            double idmin, double idmax, ExString const &errorName,
                            int ps, ExString const &action )
{
  DepVarComboBox->ItemIndex = DepVarComboBox->Items->IndexOf( depName.c_str() );
  IndepVarComboBox->ItemIndex = IndepVarComboBox->Items->IndexOf( indepName.c_str() );
  IVMinEdit->Enabled = true;
  IVMaxEdit->Enabled = true;
  IVMinLabel->Enabled = true;
  IVMaxLabel->Enabled = true;
  IVMinEdit->Text = AnsiString(idmin);
  IVMaxEdit->Text = AnsiString(idmax);
  ErrorsComboBox->ItemIndex = ErrorsComboBox->Items->IndexOf( errorName.c_str() );
  PlotSymbolComboBox->ItemIndex = -1;
  int size = symArray_.size();
  for( int i=0; i<size; ++i )
  {
    if( abs(ps) == symArray_[i] )
    {
      PlotSymbolComboBox->ItemIndex = i;
      break;
    }
  }
  ConnectCheckBox->Checked = (ps>=0);
  if( action == "CLEAR" )
  {
    ClearClick( static_cast<TObject*>(0) );
  }
  else if( action == "DRAW" )
  {
    DrawGraphClick( static_cast<TObject*>(0) );
  }
  else if( action == "CLOSE" )
  {
    CloseClick( static_cast<TObject*>(0) );
  }
}
// end of file

