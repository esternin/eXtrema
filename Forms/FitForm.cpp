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

#include "UsefulFunctions.h"
#include "extremaMain.h"
#include "Expression.h"
#include "NumericData.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EExpressionError.h"
#include "ECommandError.h"
#include "EVariableError.h"
//
#include "Htmlhelp.h"
//
#include "FitForm.h"
//
#pragma package(smart_init)
#pragma resource "*.dfm"

TFitCommandForm *FitCommandForm;

__fastcall TFitCommandForm::TFitCommandForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "FitForm", "Top", 248 );
  this->Left = ini->ReadInteger( "FitForm", "Left", 351 );
  this->Height = ini->ReadInteger( "FitForm", "Height", 399 );
  this->Width = ini->ReadInteger( "FitForm", "Width", 783 );
  delete ini;
  //
  FitTypeRadioGroup->ItemIndex = 0; // normal type fit
  ITMAXEdit->Text = AnsiString("51");
  TOLERANCEEdit->Text = AnsiString("0.00001");
  ParameterStringGrid->Cells[0][0] = AnsiString("parameter");
  ParameterStringGrid->Cells[1][0] = AnsiString("value");
}

__fastcall TFitCommandForm::~TFitCommandForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "FitForm", "Top", this->Top );
  ini->WriteInteger( "FitForm", "Left", this->Left );
  ini->WriteInteger( "FitForm", "Height", this->Height );
  ini->WriteInteger( "FitForm", "Width", this->Width );
  delete ini;
}

void __fastcall TFitCommandForm::SetDefaults(TObject *Sender)
{
  CHISQCheckBox->Checked = false;
  CHISQEdit->Text = AnsiString("");
  CLCheckBox->Checked = false;
  CLEdit->Text = AnsiString("");
  CORRMATCheckBox->Checked = false;
  COVMATCheckBox->Checked = false;
  E1CheckBox->Checked = false;
  E2CheckBox->Checked = false;
  VARNAMESCheckBox->Checked = false;
  FREECheckBox->Checked = false;
  FREEEdit->Text = AnsiString("");
  RESETCheckBox->Checked = false;
  ZEROSCheckBox->Checked = false;
  ITMAXEdit->Text = AnsiString("51");
  TOLERANCEEdit->Text = AnsiString("0.00001");
  UPDATEEdit->Text = AnsiString("");
  DataVectorEdit->Text = AnsiString("");
  WeightVectorEdit->Text = AnsiString("");
  ExpressionEdit->Text = AnsiString("");
  ParameterStringGrid->RowCount = 2;
  ParameterStringGrid->Cells[0][1] = AnsiString("");
  ParameterStringGrid->Cells[1][1] = AnsiString("");
}

void __fastcall TFitCommandForm::SetFitType(TObject *Sender)
{ WeightVectorEdit->Enabled = FitTypeRadioGroup->ItemIndex==0 ? true : false; }

void __fastcall TFitCommandForm::UPDATEButtonClick(TObject *Sender)
{
  if( UPDATEEdit->Text.IsEmpty() )
  {
    Application->MessageBox( "please specify an output vector", "Error", MB_OK );
    return;
  }
  if( expression_.empty() )
  {
    Application->MessageBox( "you must do a fit first", "Error", MB_OK );
    return;
  }
  ExString updateVector( UPDATEEdit->Text.c_str() );
  //
  // re-evaluate the last expression
  //
  Expression expr( expression_ );
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  if( expr.IsCharacter() )
  {
    Application->MessageBox( "expecting numeric expression", "Error", MB_OK );
    return;
  }
  NumericData nd( expr.GetFinalAnswer() );
  try
  {
    switch ( nd.GetNumberOfDimensions() )
    {
      case 0:
        NumericVariable::PutVariable( updateVector, nd.GetScalarValue(), "FIT Gui" );
        break;
      case 1:
        NumericVariable::PutVariable( updateVector, nd.GetData(), 0, "FIT Gui" );
        break;
      case 2:
        NumericVariable::PutVariable( updateVector, nd.GetData(), nd.GetDimMag(0),
                                      nd.GetDimMag(1), "FIT Gui" );
    }
  }
  catch ( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
}

void __fastcall TFitCommandForm::DoTheFitClick(TObject *Sender)
{
  ExString command("FIT");
  std::vector<double> weightVec;
  bool userWeights = false;
  bool zeros( ZEROSCheckBox->Checked );
  if( WeightVectorEdit->Text.IsEmpty() )
  {
    zeros = false;
  }
  else
  {
    ExString weightVector( WeightVectorEdit->Text.c_str() );
    try
    {
      NumericVariable::GetVector( weightVector, "weight vector", weightVec );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    userWeights = true;
  }
  int ntmax;
  if( !ITMAXEdit->Text.IsEmpty() )
  {
    int len = ITMAXEdit->GetTextLen();
    // don't use auto_ptr since delete [] might have problems
    char *c = new char[++len];
    ITMAXEdit->GetTextBuf( c, len );
    ExString s( c );
    delete [] c;
    try
    {
      ntmax = s.ToInt();
    }
    catch (...)
    {
      ITMAXEdit->Clear();
      ITMAXEdit->Text = AnsiString( "" );
      s += " is an invalid value for maximum number of iterations";
      Application->MessageBox( s.c_str(), "Error", MB_OK );
      return;
    }
    if( ntmax < 2 )
    {
      ITMAXEdit->Clear();
      ITMAXEdit->Text = AnsiString( "" );
      Application->MessageBox( "maximum number of iterations must be >= 2",
                               "Error", MB_OK );
      return;
    }
  }
  else
  {
    ntmax = 51;
  }
  double tolerance;
  if( !TOLERANCEEdit->Text.IsEmpty() )
  {
    int len = TOLERANCEEdit->GetTextLen();
    char *c = new char[++len];
    TOLERANCEEdit->GetTextBuf( c, len );
    ExString s( c );
    delete [] c;
    try
    {
      tolerance = s.ToDouble();
    }
    catch (...)
    {
      TOLERANCEEdit->Clear();
      TOLERANCEEdit->Text = AnsiString( "" );
      s += " is an invalid value for tolerance";
      Application->MessageBox( s.c_str(), "Error", MB_OK );
      return;
    }
    if( tolerance <= 0.0 )
    {
      TOLERANCEEdit->Clear();
      TOLERANCEEdit->Text = AnsiString( "" );
      Application->MessageBox( "tolerance must be > 0", "Error", MB_OK );
      return;
    }
  }
  else
  {
    tolerance = 0.00001;
  }
  //
  // extract the vector from the left hand side of the "="
  //
  std::vector<double> yVec;
  ExString dataVector( DataVectorEdit->Text.c_str() );
  try
  {
    NumericVariable::GetVector( dataVector, "left hand side of equation", yVec );
  }
  catch( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  if( userWeights )
  {
    if( weightVec.size() != yVec.size() )
    {
      Application->MessageBox(
        "weight vector has different length than dependent variable",
        "Error", MB_OK );
      return;
    }
  }
  else
  {
    weightVec.assign( yVec.size(), 1.0 );
  }
  //
  // decode the expression
  //
  if( ExpressionEdit->Text.IsEmpty() )
  {
    Application->MessageBox( "must enter an expression for fitting",
                             "Error", MB_OK );
    return;
  }
  expression_ = ExString( ExpressionEdit->Text.c_str() );
  Expression expr( expression_ );
  expr.SetIsaFit();
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  std::size_t const nparam = expr.GetNumberOfFitParameters();
  if( nparam == 0 )
  {
    Application->MessageBox( "no fitting parameters in the expression",
                             "Error", MB_OK );
    return;
  }
  if( yVec.size() < nparam )
  {
    Application->MessageBox( "number of data points < number of parameters",
                             "Error", MB_OK );
    return;
  }
  //
  // do the least squares fit
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
  double confidenceLevel=0.0, chisq=0.0;
  std::size_t nfree=0;
  bool output=false;
  bool poisson=(FitTypeRadioGroup->ItemIndex==1);
  bool marquardt = false;
  try
  {
    UsefulFunctions::LeastSquaresFit( &expr, dataVector, yVec, weightVec, e1, e2,
                                      pp, pSave, ntmax, tolerance, chisq, confidenceLevel,
                                      errmat, output, zeros, poisson, marquardt, nfree );
  }
  catch ( EExpressionError &e )
  {
    if( RESETCheckBox->Checked ) // reset original parameter values
    {
      for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, p1[i] );
    }
    else        // update the parameters after an unsuccessful fit
    {
      for( std::size_t i=0; i<nparam; ++i )
        expr.SetFitParameterValue( i, pSave[i], "FIT GUI" ); // also update variable history
    }
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  //
  // update the parameters and definition table after a successful fit
  //
  Application->MessageBox( "Fit successful!", "Success", MB_OK );
  //
  for( std::size_t i = 0; i < nparam; ++i )
  {
    expr.SetFitParameterValue( i, pp[i], "FIT GUI" );
    ParameterStringGrid->Cells[0][i+1] = AnsiString(expr.GetFitParameterName(i).c_str());
    ParameterStringGrid->Cells[1][i+1] = AnsiString(pp[i]);
    //
    // array text variable fit$var contains the names of the fit parameters
    //
  }
  if( VARNAMESCheckBox->Checked )
    TextVariable::PutVariable( "FIT$VAR", expr.GetFitParameterNames(), "FIT GUI" );
  //
  // output correlation matrix if corrmat modifier specified
  //
  if( CORRMATCheckBox->Checked )
  {
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
    }
    try
    {
      NumericVariable::PutVariable( "FIT$CORR", corr, nparam, nparam, "FIT GUI" );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  //
  // output covariance matrix if covmat modifier specified
  //
  if( COVMATCheckBox->Checked )
  {
    std::vector<double> cov( nparam*nparam, 0.0 );
    for( std::size_t i = 0; i < nparam; ++i )
    {
      for( std::size_t j = 0; j < nparam; ++j )
      {
        cov[j+i*nparam] = errmat[j][i];
      }
    }
    try
    {
      NumericVariable::PutVariable( "FIT$COVM", cov, nparam, nparam, "FIT GUI" );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  //
  // output e1 error vector if e1 modifier specified
  //
  if( E1CheckBox->Checked )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$E1", e1, 0, "FIT GUI" );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  //
  // output e2 error vector if e2 modifier specified
  //
  if( E2CheckBox->Checked )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$E2", e2, 0, "FIT GUI" );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  //
  // output chisq if chisq modifier specified
  //
  CHISQEdit->Text = AnsiString( chisq );
  if( CHISQCheckBox->Checked )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$CHISQ", chisq, "FIT GUI" );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  //
  // output confidenceLevel if CONFIDENCELEVEL qualifier specified
  //
  CLEdit->Text = AnsiString( confidenceLevel ) + AnsiString('\%');
  if( CLCheckBox->Checked )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$CL", confidenceLevel, "FIT GUI" );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  //
  // output degrees of freedom if free modifier specified
  //
  FREEEdit->Text = AnsiString( static_cast<int>(nfree) );
  if( FREECheckBox->Checked )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$FREE", static_cast<double>(nfree), "FIT GUI" );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
}

void __fastcall TFitCommandForm::HelpClick(TObject *Sender)
{
  std::ifstream tmp;
  AnsiString path( ExtractFilePath(Application->ExeName) );
  AnsiString helpFile = path + MainForm->GetHelpFileName();
  tmp.open( helpFile.c_str(), ios_base::in );
  if( tmp )
  {
    tmp.close();
    helpFile += AnsiString("::/html/FitCommand/fitform.htm");
    HtmlHelp( MainForm->Handle, helpFile.c_str(), HH_DISPLAY_TOPIC, NULL );
  }
  else
  {
    AnsiString s( "unable to open help file: " );
    s += helpFile;
    Application->MessageBox( s.c_str(), "ERROR", MB_OK );
  }
}

void __fastcall TFitCommandForm::CloseClick(TObject *Sender)
{ Close(); FitCommandForm=0; }

void __fastcall TFitCommandForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TFitCommandForm::FormClose2(TObject *Sender,bool &CanClose)
{ FitCommandForm = 0; }

// end of file
