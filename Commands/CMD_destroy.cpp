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
#include "CMD_destroy.h"

CMD_destroy CMD_destroy::cmd_destroy_;
 
CMD_destroy::CMD_destroy() : Command( "DESTROY" )
{
  AddQualifier( "ALL", false );
  AddQualifier( "SCALARS", false );
  AddQualifier( "VECTORS", false );
  AddQualifier( "MATRICES", false );
  AddQualifier( "TENSORS", false );
  AddQualifier( "TEXT", false );
  AddQualifier( "EXPAND", false );
  AddQualifier( "MESSAGES", true );
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
  if( qualifiers["ALL"] )
  {
    qualifiers["SCALARS"] = true;
    qualifiers["VECTORS"] = true;
    qualifiers["MATRICES"] = true;
    qualifiers["TENSORS"] = true;
    qualifiers["TEXT"] = true;
  }
  bool output = qualifiers["MESSAGES"] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers["MESSAGES"] )output = false;
  //
  std::vector<NumericVariable*> nvs;
  std::vector<TextVariable*> tvs;
  std::vector<ExString> nvNames, tvNames, sNames;
  //
  int nTokens = p->GetNumberOfTokens();
  for( int j=1; j<nTokens; ++j )
  {
    if( p->IsNull(j) || p->IsNumeric(j) || p->IsEqualSign(j) )
      throw ECommandError( "DESTROY", ExString("parameter ")+ExString(j)+" is invalid" );
    if( output && p->GetToken(j)->GetNumberOfQualifiers()!=0 )
      ExGlobals::WarningMessage( "DESTROY command: there are no valid parameter qualifiers" );
    ExString name( p->GetString(j) );
    AddToStackLine( name );
    ExString uName( name.UpperCase() ); // convert to upper case
    //
    if( uName == "*" )
    {
      qualifiers["SCALARS"] = true;
      qualifiers["VECTORS"] = true;
      qualifiers["MATRICES"] = true;
      qualifiers["TENSORS"] = true;
      qualifiers["TEXT"] = true;
      continue;
    }
    else if( uName == "*S" )
    {
      qualifiers["SCALARS"] = true;
      continue;
    }
    else if( uName == "*V" )
    {
      qualifiers["VECTORS"] = true;
      continue;
    }
    else if( uName == "*M" )
    {
      qualifiers["MATRICES"] = true;
      continue;
    }
    else if( uName == "*T" )
    {
      qualifiers["TEXT"] = true;
      continue;
    }
    //
    // See if the variable name needs to be constructed,
    // i.e., `A'//RCHAR(I) could be `A3' if I=3
    //
    if( qualifiers["EXPAND"] )
    {
      Expression expr( name );
      try
      {
        expr.Evaluate();
      }
      catch ( EExpressionError &e )
      {
        throw ECommandError( "DESTROY", e.what() );
      }
      if( expr.IsCharacter() )name = expr.GetFinalString();
    }
    ExString sName( Variable::SimpleName(name) );
    NumericVariable *nVar = NVariableTable::GetTable()->GetVariable( sName );
    if( nVar )
    {
      nvNames.push_back( name.UpperCase() );
      nvs.push_back( nVar );
    }
    else
    {
      TextVariable *tVar = TVariableTable::GetTable()->GetVariable( sName );
      if( !tVar )
      {
        if( output )ExGlobals::WarningMessage(
          ExString("DESTROY command: variable ")+sName+" does not exist" );
        continue;
      }
      tvNames.push_back( name.UpperCase() );
      tvs.push_back( tVar );
    }
    sNames.push_back( sName );
  }
  std::size_t nvSize = nvs.size();
  std::size_t tvSize = tvs.size();
  if( !nvSize && !tvSize &&
      !(qualifiers["SCALARS"]||qualifiers["VECTORS"]||qualifiers["MATRICES"]||
        qualifiers["TENSORS"]||qualifiers["TEXT"]) )return;
  //
  // delete scalars or parts of vectors, matrices, tensors, text variables
  //
  if( qualifiers["SCALARS"] )
  {
    NVariableTable::GetTable()->RemoveScalars();
    if( output )ExGlobals::WriteOutput( "all scalars are gone" );
  }
  if( qualifiers["VECTORS"] )
  {
    NVariableTable::GetTable()->RemoveVectors();
    if( output )ExGlobals::WriteOutput( "all vectors are gone" );
  }
  if( qualifiers["MATRICES"] )
  {
    NVariableTable::GetTable()->RemoveMatrices();
    if( output )ExGlobals::WriteOutput( "all matrices are gone" );
  }
  if( qualifiers["TENSORS"] )
  {
    NVariableTable::GetTable()->RemoveTensors();
    if( output )ExGlobals::WriteOutput( "all tensors are gone" );
  }
  if( qualifiers["TEXT"] )
  {
    TVariableTable::GetTable()->ClearTable();
    if( output )ExGlobals::WriteOutput( "all text variables are gone" );
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
          throw ECommandError( "DESTROY", "indices not allowed on scalars" );
        NVariableTable::GetTable()->RemoveEntry( nvs[i] );
        nvs[i] = 0;
        if( output )ExGlobals::WriteOutput( ExString("scalar ")+sNames[i]+" is gone" );
        break;
      case 1:
      {
        if( sNames[i] == nvNames[i] ) // no indices
        {
          NVariableTable::GetTable()->RemoveEntry( nvs[i] );
          nvs[i] = 0;
          if( output )ExGlobals::WriteOutput( ExString("vector ")+sNames[i]+" is gone" );
        }
        else                            // indices were used
        {
          try
          {
            if( nvs[i]->DeletePartial(nvNames[i],p->GetInputLine()) )
            {
              NVariableTable::GetTable()->RemoveEntry( nvs[i] );
              nvs[i] = 0;
              if( output )ExGlobals::WriteOutput( ExString("vector ")+sNames[i]+" is gone" );
            }
          }
          catch (EVariableError &e)
          {
            throw ECommandError( "DESTROY", e.what() );
          }
        }
        break;
      }
      case 2:
        if( sNames[i] != nvNames[i] )
          throw ECommandError( "DESTROY", "indices not allowed on matrices" );
        NVariableTable::GetTable()->RemoveEntry( nvs[i] );
        nvs[i] = 0;
        if( output )ExGlobals::WriteOutput( ExString("matrix ")+sNames[i]+" is gone" );
        break;
      case 3:
        if( sNames[i] != nvNames[i] )
          throw ECommandError( "DESTROY", "indices not allowed on tensors" );
        NVariableTable::GetTable()->RemoveEntry( nvs[i] );
        nvs[i] = 0;
        if( output )ExGlobals::WriteOutput( ExString("tensor ")+sNames[i]+" is gone" );
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
      if( output )ExGlobals::WriteOutput( ExString("text variable ")+sNames[i]+" is gone" );
    }
    else
    {
      try
      {
        if( tvs[i]->DeletePartial(tvNames[i],p->GetInputLine()) )
        {
          TVariableTable::GetTable()->RemoveEntry( tvs[i] );
          tvs[i] = 0;
          if( output )ExGlobals::WriteOutput( ExString("text variable ")+sNames[i]+" is gone" );
        }
      }
      catch (EVariableError &e)
      {
        throw ECommandError( "DESTROY", e.what() );
      }
    }
  }
}

// end of file
