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
#include "wx/wx.h"

#include "CMD_destroy.h"
#include "ExGlobals.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "Expression.h"

CMD_destroy *CMD_destroy::cmd_destroy_ = 0;
 
CMD_destroy::CMD_destroy() : Command( wxT("DESTROY") )
{
  AddQualifier( wxT("ALL"), false );
  AddQualifier( wxT("SCALARS"), false );
  AddQualifier( wxT("VECTORS"), false );
  AddQualifier( wxT("MATRICES"), false );
  AddQualifier( wxT("TENSORS"), false );
  AddQualifier( wxT("TEXT"), false );
  AddQualifier( wxT("EXPAND"), false );
  AddQualifier( wxT("MESSAGES"), true );
}

void CMD_destroy::Execute( ParseLine const *p )
{
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
  if( qualifiers[wxT("ALL")] )
  {
    qualifiers[wxT("SCALARS")] = true;
    qualifiers[wxT("VECTORS")] = true;
    qualifiers[wxT("MATRICES")] = true;
    qualifiers[wxT("TENSORS")] = true;
    qualifiers[wxT("TEXT")] = true;
  }
  bool output = qualifiers[wxT("MESSAGES")] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers[wxT("MESSAGES")] )output = false;
  //
  std::vector<NumericVariable*> nvs;
  std::vector<TextVariable*> tvs;
  std::vector<wxString> nvNames, tvNames, sNames;
  //
  int nTokens = p->GetNumberOfTokens();
  for( int j=1; j<nTokens; ++j )
  {
    if( p->IsNull(j) || p->IsNumeric(j) || p->IsEqualSign(j) )
    {
      wxString s(wxT("parameter "));
      s << j << wxT(" is invalid");
      throw ECommandError( command+s );
    }
    if( output && p->GetToken(j)->GetNumberOfQualifiers()!=0 )
      ExGlobals::WarningMessage( wxT("DESTROY command: there are no valid parameter qualifiers") );
    wxString name( p->GetString(j) );
    wxString uName( name.Upper() ); // convert to upper case
    //
    if( uName == wxT("*") )
    {
      qualifiers[wxT("SCALARS")] = true;
      qualifiers[wxT("VECTORS")] = true;
      qualifiers[wxT("MATRICES")] = true;
      qualifiers[wxT("TENSORS")] = true;
      qualifiers[wxT("TEXT")] = true;
      continue;
    }
    else if( uName == wxT("*S") )
    {
      qualifiers[wxT("SCALARS")] = true;
      continue;
    }
    else if( uName == wxT("*V") )
    {
      qualifiers[wxT("VECTORS")] = true;
      continue;
    }
    else if( uName == wxT("*M") )
    {
      qualifiers[wxT("MATRICES")] = true;
      continue;
    }
    else if( uName == wxT("*T") )
    {
      qualifiers[wxT("TEXT")] = true;
      continue;
    }
    //
    // See if the variable name needs to be constructed,
    // i.e., `A'//RCHAR(I) could be `A3' if I=3
    //
    if( qualifiers[wxT("EXPAND")] )
    {
      Expression expr( name );
      try
      {
        expr.Evaluate();
      }
      catch ( EExpressionError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( expr.IsCharacter() )name = expr.GetFinalString();
    }
    wxString sName( Variable::SimpleName(name) );
    NumericVariable *nVar = NVariableTable::GetTable()->GetVariable( sName );
    if( nVar )
    {
      nvNames.push_back( name.Upper() );
      nvs.push_back( nVar );
    }
    else
    {
      TextVariable *tVar = TVariableTable::GetTable()->GetVariable( sName );
      if( !tVar )
      {
        if( output )ExGlobals::WarningMessage(
          wxString(wxT("DESTROY command: variable "))+sName+wxT(" does not exist") );
        continue;
      }
      tvNames.push_back( name.Upper() );
      tvs.push_back( tVar );
    }
    sNames.push_back( sName );
  }
  std::size_t nvSize = nvs.size();
  std::size_t tvSize = tvs.size();
  if( !nvSize && !tvSize &&
      !(qualifiers[wxT("SCALARS")]||qualifiers[wxT("VECTORS")]||qualifiers[wxT("MATRICES")]||
        qualifiers[wxT("TENSORS")]||qualifiers[wxT("TEXT")]) )return;
  //
  // delete scalars or parts of vectors, matrices, tensors, text variables
  //
  if( qualifiers[wxT("SCALARS")] )
  {
    NVariableTable::GetTable()->RemoveScalars();
    if( output )ExGlobals::WriteOutput( wxT("all scalars are gone") );
  }
  if( qualifiers[wxT("VECTORS")] )
  {
    NVariableTable::GetTable()->RemoveVectors();
    if( output )ExGlobals::WriteOutput( wxT("all vectors are gone") );
  }
  if( qualifiers[wxT("MATRICES")] )
  {
    NVariableTable::GetTable()->RemoveMatrices();
    if( output )ExGlobals::WriteOutput( wxT("all matrices are gone") );
  }
  if( qualifiers[wxT("TENSORS")] )
  {
    NVariableTable::GetTable()->RemoveTensors();
    if( output )ExGlobals::WriteOutput( wxT("all tensors are gone") );
  }
  if( qualifiers[wxT("TEXT")] )
  {
    TVariableTable::GetTable()->ClearTable();
    if( output )ExGlobals::WriteOutput( wxT("all text variables are gone") );
  }
  for( std::size_t i=0; i<nvSize; ++i )
  {
    if( !NVariableTable::GetTable()->Contains(sNames[i]) )
    {
      nvs[i] = 0;
      continue;
    }
    switch ( nvs[i]->GetData().GetNumberOfDimensions() )
    {
      case 0:
        if( sNames[i] != nvNames[i] )
          throw ECommandError( command+wxT("indices not allowed on scalars") );
        NVariableTable::GetTable()->RemoveEntry( nvs[i] );
        nvs[i] = 0;
        if( output )ExGlobals::WriteOutput( wxString(wxT("scalar "))+sNames[i]+wxT(" is gone") );
        break;
      case 1:
      {
        if( sNames[i] == nvNames[i] ) // no indices
        {
          NVariableTable::GetTable()->RemoveEntry( nvs[i] );
          nvs[i] = 0;
          if( output )ExGlobals::WriteOutput( wxString(wxT("vector "))+sNames[i]+wxT(" is gone") );
        }
        else                            // indices were used
        {
          try
          {
            if( nvs[i]->DeletePartial(nvNames[i],p->GetInputLine()) )
            {
              NVariableTable::GetTable()->RemoveEntry( nvs[i] );
              nvs[i] = 0;
              if( output )ExGlobals::WriteOutput( wxString(wxT("vector "))+sNames[i]+wxT(" is gone") );
            }
          }
          catch (EVariableError &e)
          {
            throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
          }
        }
        break;
      }
      case 2:
        if( sNames[i] != nvNames[i] )
          throw ECommandError( command+wxT("indices not allowed on matrices") );
        NVariableTable::GetTable()->RemoveEntry( nvs[i] );
        nvs[i] = 0;
        if( output )ExGlobals::WriteOutput( wxString(wxT("matrix "))+sNames[i]+wxT(" is gone") );
        break;
      case 3:
        if( sNames[i] != nvNames[i] )
          throw ECommandError( command+wxT("indices not allowed on tensors") );
        NVariableTable::GetTable()->RemoveEntry( nvs[i] );
        nvs[i] = 0;
        if( output )ExGlobals::WriteOutput( wxString(wxT("tensor "))+sNames[i]+wxT(" is gone") );
        break;
    }
  }
  //
  // now do the text variables
  //
  for( std::size_t i=0; i<tvSize; ++i )
  {
    if( sNames[i] == tvNames[i] ) // no indices
    {
      TVariableTable::GetTable()->RemoveEntry( tvs[i] );
      tvs[i] = 0;
      if( output )ExGlobals::WriteOutput( wxString(wxT("text variable "))+sNames[i]+wxT(" is gone") );
    }
    else
    {
      try
      {
        if( tvs[i]->DeletePartial(tvNames[i],p->GetInputLine()) )
        {
          TVariableTable::GetTable()->RemoveEntry( tvs[i] );
          tvs[i] = 0;
          if( output )ExGlobals::WriteOutput( wxString(wxT("text variable "))+sNames[i]+wxT(" is gone") );
        }
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
  }
}

// end of file
