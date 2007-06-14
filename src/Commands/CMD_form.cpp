/*
Copyright (C) 2006 Joseph L. Chuma, TRIUMF

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
#include "wx/wx.h"

#include "CMD_form.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "AnalysisWindow.h"
#include "VisualizationWindow.h"
#include "FitForm.h"
#include "GraphForm.h"
#include "ExecuteDialog.h"
#include "TextForm.h"
#include "GenerateVectorForm.h"

CMD_form *CMD_form::cmd_form_ = 0;

CMD_form::CMD_form() : Command( wxT("FORM") )
{
  AddQualifier( wxT("NOVICE"), true );
}

void CMD_form::Execute( ParseLine const *p )
{
  // open one of the gui forms
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  wxString command( Name()+wxT(": ") );
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError( command+wxT("expecting a form name") );
  wxString form( p->GetString(1) );
  form.UpperCase();
  ExGlobals::RemoveQuotes( form );
  AddToStackLine( form );
  int icnt = 2;
  if( form == wxT("FIT") )
  {
    // FORM FIT,CLOSE
    // FORM FIT,'CLEAR GRAPHICS'
    // FORM FIT,dataVec, indepVec, idmin, idmax, errorVec, fitType, nparam, expression,
    //          fixedVec, startVec, xloc, yloc, action
    //   where action can be one of: CLOSE, CLEAR GRAPHICS, TEST THE FIT, or DO THE FIT
    //
    command += wxT("FIT ");
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting data vector name or CLOSE or CLEAR GRAPHICS") );
    wxString dataName( p->GetString(icnt) );
    dataName.UpperCase();
    ExGlobals::RemoveQuotes( dataName );
    AnalysisWindow *analysisWindow = ExGlobals::GetAnalysisWindow();
    FitForm *fitForm = analysisWindow->GetFitForm();
    if( dataName == wxT("CLOSE") )
    {
      if( fitForm )
      {
        fitForm->Close();
        analysisWindow->ZeroFit();
      }
      return;
    }
    else if( dataName == wxT("CLEAR GRAPHICS") )
    {
      if( fitForm )fitForm->Raise();
      else
      {
        fitForm = new FitForm( analysisWindow );
        fitForm->Show();
        analysisWindow->SetFitForm( fitForm );
      }
      fitForm->ClearGraphics();
      return;
    }
    NumericVariable *dataVec = NVariableTable::GetTable()->GetVariable(dataName);
    if( !dataVec )
      throw ECommandError( command+wxT("data vector ")+dataName+wxT(" does not exist") );
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting independent vector name") );
    wxString indepName( p->GetString(icnt) );
    NumericVariable *indepVec = NVariableTable::GetTable()->GetVariable(indepName);
    if( !indepVec )
      throw ECommandError( command+wxT("independent vector ")+indepName+wxT(" does not exist") );
    AddToStackLine( p->GetString(icnt) );
    double idmin, idmax;
    indepVec->GetData().GetMinMax( idmin, idmax );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting indep. vector range minimum") );
    if( p->IsNull(icnt) )
    {
      AddToStackLine( idmin );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), wxT("indep. vector range minimum"), idmin );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting indep. vector range maximum") );
    if( p->IsNull(icnt) )
    {
      AddToStackLine( idmax );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), wxT("indep. vector range maximum"), idmax );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting error vector name or null") );
    wxString errorName;
    NumericVariable *errorVec = 0;
    if( p->IsString(icnt) )
    {
      errorName = p->GetString(icnt);
      errorVec = NVariableTable::GetTable()->GetVariable(errorName);
      if( !errorVec )
        throw ECommandError( command+wxT("error vector")+errorName+wxT(" does not exist") );
      AddToStackLine( p->GetString(icnt) );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting type of fit") );
    wxString typeName( p->GetString(icnt) );
    typeName.UpperCase();
    ExGlobals::RemoveQuotes( typeName );
    if( typeName!=wxT("LEGENDRE POLYNOMIALS") && typeName!=wxT("POLYNOMIAL") && typeName!=wxT("GAUSSIAN") &&
        typeName!=wxT("LEAST SQUARES LINE") && typeName!=wxT("CONSTANT") && typeName!=wxT("CUSTOM") )
      throw ECommandError( command+wxT("invalid type of fit") );
    AddToStackLine( p->GetString(icnt) );
    //
    int nParams = 0;
    ++icnt;
    if( typeName == wxT("CUSTOM") )
    {
      if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
        throw ECommandError( command+wxT("expecting number of fit parameters") );
      double fitp;
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), wxT("number of fit parameters"), fitp );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      nParams = static_cast<int>(fitp);
      if( nParams > 8 || nParams < 1 )
        throw ECommandError( command+wxT("number of fit parameters must be > 0 and <= 8") );
      AddToStackLine( p->GetString(icnt) );
      typeName = wxT("Custom");
    }
    else if( typeName == wxT("LEGENDRE POLYNOMIALS") )
    {
      if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
        throw ECommandError( command+wxT("expecting degree of polynomial") );
      double deg;
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), wxT("degree of polynomial"), deg );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      nParams = static_cast<int>(deg)+1;
      if( nParams > 8 || nParams < 1 )
        throw ECommandError( command+wxT("degree of polynomial must be > 0 and <= 7") );
      AddToStackLine( p->GetString(icnt) );
      typeName = wxT("Legendre polynomials");
    }
    else if( typeName == wxT("POLYNOMIAL") )
    {
      if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
        throw ECommandError( command+wxT("expecting degree of polynomial") );
      double deg;
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), wxT("degree of polynomial"), deg );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      nParams = static_cast<int>(deg)+1;
      if( nParams > 8 || nParams < 1 )
        throw ECommandError( command+wxT("degree of polynomial must be > 0 and <= 7") );
      AddToStackLine( p->GetString(icnt) );
      typeName = wxT("Polynomial");
    }
    else if( typeName == wxT("GAUSSIAN") )
    {
      nParams = 3;
      AddToStackLine();
      typeName = wxT("Gaussian");
    }
    else if( typeName == wxT("LEAST SQUARES LINE") )
    {
      nParams = 2;
      AddToStackLine();
      typeName = wxT("Least squares line");
    }
    else if( typeName == wxT("CONSTANT") )
    {
      nParams = 1;
      AddToStackLine();
      typeName = wxT("Constant");
    }
    ++icnt;
    wxString expression;
    if( typeName == wxT("Custom") )
    {
      if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
        throw ECommandError( command+wxT("expecting fit expression") );
      expression = p->GetString(icnt);
      ExGlobals::RemoveQuotes( expression );
      AddToStackLine( p->GetString(icnt) );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting fixed vector") );
    std::vector<double> fixed;
    try
    {
      NumericVariable::GetVector( p->GetString(icnt), wxT("fixed vector"), fixed );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( static_cast<int>(fixed.size()) != nParams )
      throw ECommandError( command+wxT("fixed vector length must equal number of parameters") );
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting starting values vector") );
    std::vector<double> start;
    try
    {
      NumericVariable::GetVector( p->GetString(icnt), wxT("fixed vector"), start );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( static_cast<int>(start.size()) != nParams )
      throw ECommandError( command+wxT("starting values vector length must equal number of parameters") );
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
      throw ECommandError( command+wxT("expecting %x legend location"));
    double xloc;
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), wxT("%x legend location"), xloc );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
      throw ECommandError( command+wxT("expecting %y legend location") );
    double yloc;
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), wxT("%y legend location"), yloc );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()>icnt && !p->IsString(icnt) && !p->IsNull(icnt) )
      throw ECommandError( command+wxT("expecting fit actionwxT(") );
    wxString action;
    if( p->GetNumberOfTokens()<=icnt || p->IsNull(icnt) )AddToStackLine();
    else
    {
      action = p->GetString(icnt).Upper();
      ExGlobals::RemoveQuotes( action );
      if( action!=wxT("TEST THE FIT") && action!=wxT("CLEAR GRAPHICS") &&
          action!=wxT("DO THE FIT") && action!=wxT("CLOSE") )
        throw ECommandError( command+wxT("invalid type of action") );
      AddToStackLine( p->GetString(icnt) );
    }
    if( fitForm )fitForm->Raise();
    else
    {
      fitForm = new FitForm( analysisWindow );
      fitForm->Show();
      analysisWindow->SetFitForm( fitForm );
    }
    fitForm->Set( dataName, indepName, idmin, idmax, errorName, typeName, nParams,
                  expression, fixed, start, xloc, yloc, action );
  }
  else if( form == wxT("GRAPH") )
  {
    // FORM GRAPH,CLOSE
    // FROM GRAPH,CLEAR
    // FORM GRAPH, depVecName, indepVecName, idmin, idmax, errorVecName, plotsymbol, DRAW
    //
    command += wxT("GRAPH ");
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting dependent vector name or CLOSE or CLEAR") );
    wxString depName( p->GetString(icnt) );
    depName.UpperCase();
    VisualizationWindow *visualizationWindow = ExGlobals::GetVisualizationWindow();
    GraphForm *graphForm = visualizationWindow->GetGraphForm();
    if( depName == wxT("CLOSE") )
    {
      if( graphForm )
      {
        graphForm->Close();
        visualizationWindow->ZeroGraphForm();
      }
      return;
    }
    else if( depName == wxT("CLEAR") )
    {
      if( graphForm )graphForm->Raise();
      else
      {
        graphForm = new GraphForm( visualizationWindow );
        graphForm->Show();
        visualizationWindow->SetGraphForm( graphForm );
      }
      graphForm->ClearGraphics();
      return;
    }
    NumericVariable *depVec = NVariableTable::GetTable()->GetVariable(depName);
    if( !depVec )
      throw ECommandError( command+wxT("dependent vector ")+depName+wxT(" does not exist") );
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    double idmin, idmax;
    wxString indepName;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting independent vector name") );
    if( p->IsString(icnt) )
    {
      indepName = p->GetString(icnt);
      NumericVariable *indepVec = NVariableTable::GetTable()->GetVariable(indepName);
      if( !indepVec )
        throw ECommandError( command+wxT("independent vector ")+indepName+wxT(" does not exist") );
      AddToStackLine( p->GetString(icnt) );
      indepVec->GetData().GetMinMax( idmin, idmax );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting indep. vector range minimum") );
    if( p->IsNull(icnt) )
    {
      if( indepName.empty() )AddToStackLine();
      else                   AddToStackLine( idmin );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), wxT("indep. vector range minimum"), idmin );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting indep. vector range maximum") );
    if( p->IsNull(icnt) )
    {
      if( indepName.empty() )AddToStackLine();
      else                   AddToStackLine( idmax );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), wxT("indep. vector range maximum"), idmax );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting error vector name or null") );
    wxString errorName;
    if( p->IsString(icnt) )
    {
      errorName = p->GetString(icnt);
      NumericVariable *errorVec = NVariableTable::GetTable()->GetVariable(errorName);
      if( !errorVec )
        throw ECommandError( command+wxT("error vector")+errorName+wxT(" does not exist") );
      AddToStackLine( p->GetString(icnt) );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
      throw ECommandError( command+wxT("expecting plotsymbol code") );
    int ps;
    try
    {
      double tmp;
      NumericVariable::GetScalar( p->GetString(icnt), wxT("plotsymbol code"), tmp );
      ps = static_cast<int>(tmp);
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()>icnt && !p->IsString(icnt) && !p->IsNull(icnt) )
      throw ECommandError( command+wxT("expecting graph action") );
    wxString action;
    if( p->GetNumberOfTokens()<=icnt || p->IsNull(icnt) )AddToStackLine();
    else
    {
      action = p->GetString(icnt).Upper();
      ExGlobals::RemoveQuotes( action );
      if( action!=wxT("DRAW") && action!=wxT("CLEAR") && action!=wxT("CLOSE") )
        throw ECommandError( command+wxT("invalid type of action") );
      AddToStackLine( p->GetString(icnt) );
    }
    if( graphForm )graphForm->Raise();
    else
    {
      graphForm = new GraphForm( visualizationWindow );
      graphForm->Show();
      visualizationWindow->SetGraphForm( graphForm );
    }
    graphForm->Set( depName, indepName, idmin, idmax, errorName, ps, action );
  }
  else if( form == wxT("EXECUTE") )
  {
    // FORM EXECUTE,CLOSE
    // FORM EXECUTE, filename, parameters, APPLY
    //
    command += wxT("EXECUTE ");
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting filename or CLOSE") );
    wxString fileName( p->GetString(icnt) );
    ExGlobals::RemoveQuotes( fileName );
    AnalysisWindow *analysisWindow = ExGlobals::GetAnalysisWindow();
    ExecuteDialog *executeDialog = analysisWindow->GetExecuteDialog();
    if( fileName.Upper() == wxT("CLOSE") )
    {
      AddToStackLine( wxT("CLOSE") );
      if( executeDialog )
      {
        executeDialog->Close();
        analysisWindow->ZeroExecuteDialog();
      }
      return;
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    wxString parameters;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting parameters") );
    if( p->IsString(icnt) )
    {
      parameters = p->GetString(icnt);
      AddToStackLine( p->GetString(icnt) );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting execute action") );
    wxString action( p->GetString(icnt).Upper() );
    ExGlobals::RemoveQuotes( action );
    if( action!=wxT("APPLY") && action!=wxT("CLOSE") )
      throw ECommandError( command+wxT("invalid type of action") );
    AddToStackLine( p->GetString(icnt) );
    if( executeDialog )executeDialog->Raise();
    else
    {
      executeDialog = new ExecuteDialog( analysisWindow );
      executeDialog->Show();
      analysisWindow->SetExecuteDialog( executeDialog );
    }
    executeDialog->Set( fileName, parameters, action );
  }
  else if( form == wxT("TEXT") )
  {
    // FORM TEXT,CLOSE
    // FORM TEXT,ERASE
    // FORM TEXT,'text',psc,xloc,yloc,alignment,action
    //  where action can be one of: "CLOSE", "ERASE", "DRAW"
    //
    command += wxT("TEXT ");
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting text string or CLOSE or ERASE") );
    wxString text( p->GetString(icnt) );
    ExGlobals::RemoveQuotes( text );
    VisualizationWindow *visualizationWindow = ExGlobals::GetVisualizationWindow();
    TextForm *textForm = visualizationWindow->GetTextForm();
    if( text.Upper() == wxT("CLOSE") )
    {
      AddToStackLine( wxT("CLOSE") );
      if( textForm )
      {
        textForm->Close();
        visualizationWindow->ZeroTextForm();
      }
      return;
    }
    else if( text.Upper() == wxT("ERASE") )
    {
      AddToStackLine( wxT("ERASE") );
      if( textForm )textForm->Raise();
      else
      {
        textForm = new TextForm( visualizationWindow );
        textForm->Show();
        visualizationWindow->SetTextForm( textForm );
      }
      textForm->Erase();
      return;
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError( command+wxT("expecting interactive flag") );
    int psc = static_cast<int>( p->GetNumeric(icnt) );
    AddToStackLine( psc );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting x location") );
    double xloc = p->IsNumeric(icnt) ? p->GetNumeric(icnt) : 50.0;
    AddToStackLine( xloc );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError( command+wxT("expecting y location") );
    double yloc = p->IsNumeric(icnt) ? p->GetNumeric(icnt) : 50.0;
    AddToStackLine( yloc );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError( command+wxT("expecting alignment code") );
    int alignment = static_cast<int>( p->GetNumeric(icnt) );
    AddToStackLine( alignment );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting text action") );
    wxString action( p->GetString(icnt).Upper() );
    ExGlobals::RemoveQuotes( action );    
    if( action!=wxT("CLOSE") && action!=wxT("ERASE") && action!=wxT("DRAW") )
      throw ECommandError( command+wxT("text action must be one of CLOSE or ERASE or DRAW") );
    AddToStackLine( action );
    if( textForm )textForm->Raise();
    else
    {
      textForm = new TextForm( visualizationWindow );
      textForm->Show();
      visualizationWindow->SetTextForm( textForm );
    }
    textForm->Set( text, psc, xloc, yloc, alignment, action );
  }
  else if( form == wxT("GENERATE") )
  {
    // FORM GENERATE,CLOSE
    // FORM GENERATE,DEFAULTS
    // FORM GENERATE,name,choice,min,inc,max,number,action
    //  where action can be one of: "CLOSE", "DEFAULTS", "APPLY"
    //
    command += wxT("GENERATE ");
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting vector name or CLOSE or DEFAULTS") );
    wxString name( p->GetString(icnt) );
    AnalysisWindow *analysisWindow = ExGlobals::GetAnalysisWindow();
    GenerateVectorForm *generateVectorForm = analysisWindow->GetGenerateVectorForm();
    if( name.Upper() == wxT("CLOSE") )
    {
      AddToStackLine( wxT("CLOSE") );
      if( generateVectorForm )
      {
        generateVectorForm->Close();
        analysisWindow->ZeroGenerateVector();
      }
      return;
    }
    else if( name.Upper() == wxT("DEFAULTS") )
    {
      AddToStackLine( wxT("DEFAULTS") );
      if( generateVectorForm )generateVectorForm->Raise();
      else
      {
        generateVectorForm = new GenerateVectorForm( analysisWindow );
        generateVectorForm->Show();
        analysisWindow->SetGenerateVectorForm( generateVectorForm );
      }
      generateVectorForm->Defaults();
      return;
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError( command+wxT("expecting choice code") );
    int choice = static_cast<int>( p->GetNumeric(icnt) );
    AddToStackLine( choice );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError( command+wxT("expecting min value") );
    double min = p->GetNumeric(icnt);
    AddToStackLine( min );
    //
    double inc = 0.0;
    double max = 0.0;
    int number = 0;
    switch ( choice )
    {
      case 0:  // min,,max,number (random)
      case 3:  // min,,max,number
      {
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
          throw ECommandError( command+wxT("expecting increment value") );
        if( p->IsNumeric(icnt) )inc = p->GetNumeric(icnt);
        AddToStackLine( inc );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError( command+wxT("expecting max value") );
        max = p->IsNumeric(icnt);
        AddToStackLine( max );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError( command+wxT("expecting number of elements") );
        number = static_cast<int>( p->GetNumeric(icnt) );
        AddToStackLine( number );
        //
        break;
      }
      case 1:  // min,inc,max,
      {
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError( command+wxT("expecting increment value") );
        inc = p->GetNumeric(icnt);
        AddToStackLine( inc );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError( command+wxT("expecting max value") );
        max = p->IsNumeric(icnt);
        AddToStackLine( max );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
          throw ECommandError( command+wxT("expecting number of elements") );
        if( p->IsNumeric(icnt) )number = static_cast<int>( p->GetNumeric(icnt) );
        AddToStackLine( number );
        //
        break;
      }
      case 2:  // min,inc,,number
      {
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError( command+wxT("expecting increment value") );
        inc = p->GetNumeric(icnt);
        AddToStackLine( inc );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
          throw ECommandError( command+wxT("expecting max value") );
        if( p->IsNumeric(icnt) )max = p->IsNumeric(icnt);
        AddToStackLine( max );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError( command+wxT("expecting number of elements") );
        number = static_cast<int>( p->GetNumeric(icnt) );
        AddToStackLine( number );
        //
        break;
      }
      default:
        throw ECommandError( command+wxT("invalid choice code: expecting 0, 1, 2, or 3") );
    }
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting text action") );
    wxString action( p->GetString(icnt).Upper() );
    ExGlobals::RemoveQuotes( action );    
    if( action!=wxT("CLOSE") && action!=wxT("DEFAULTS") && action!=wxT("APPLY") )
      throw ECommandError( command+wxT("text action must be one of CLOSE or DEFAULTS or APPLY") );
    AddToStackLine( action );
    if( generateVectorForm )generateVectorForm->Raise();
    else
    {
      generateVectorForm = new GenerateVectorForm( analysisWindow );
      generateVectorForm->Show();
      analysisWindow->SetGenerateVectorForm( generateVectorForm );
    }
    generateVectorForm->Set( name, choice, min, inc, max, number, action );
  }
  else throw ECommandError( command+wxT("unknown FORM type") );
}

// end of file
