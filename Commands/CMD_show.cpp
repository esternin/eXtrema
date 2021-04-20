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
#include "EVariableError.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"

#include "CMD_show.h"

CMD_show CMD_show::cmd_show_;

CMD_show::CMD_show() : Command( "SHOW" )
{
  AddQualifier( "ALL", false );
  AddQualifier( "SCALARS", false );
  AddQualifier( "VECTORS", false );
  AddQualifier( "MATRICES", false );
  AddQualifier( "TENSORS", false );
  AddQualifier( "TEXT", false );
  AddQualifier( "FIT", true );
  AddQualifier( "VARY", true );
  AddQualifier( "FIXED", true );
  AddQualifier( "ORDERED", true );
}

void CMD_show::Execute( ParseLine const *p )
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
  int nTokens = p->GetNumberOfTokens();
  qualifiers["ALL"] = !(qualifiers["SCALARS"] || qualifiers["VECTORS"] ||
                        qualifiers["MATRICES"] || qualifiers["TENSORS"] ||
                        qualifiers["TEXT"] );
  NVariableTable *nvTable = NVariableTable::GetTable();
  TVariableTable *tvTable = TVariableTable::GetTable();
  int nvEntries = nvTable->Entries();
  int tvEntries = tvTable->Entries();
  std::vector<int> indexNV, indexTV;
  if( nTokens == 1 )
  {
    for( int i=0; i<nvEntries; ++i )indexNV.push_back( i );
    for( int i=0; i<tvEntries; ++i )indexTV.push_back( i );
  }
  else
  {
    for( int j=1; j<nTokens; ++j )
    {
      if( !p->IsString(j) )throw ECommandError( "SHOW", "expecting variable names" );
      AddToStackLine( p->GetString(j) );
      GetWild( p->GetString(j), indexNV, indexTV );
    }
  }
  int numericVars = indexNV.size();
  int textVars = indexTV.size();
  std::size_t maxTextName = 4;
  std::size_t maxNumberOfStrings = 0;
  std::size_t maxNumberOfChars = 0;
  if( textVars > 0 )
  {
    for( int i=0; i<textVars; ++i )
    {
      TextVariable *tv = tvTable->GetEntry( i );
      std::size_t len = tv->GetName().size();
      if( maxTextName < len )maxTextName = len;
      TextData td( tv->GetData() );
      if( td.GetNumberOfDimensions() == 0 )
      {
        len = td.GetScalarValue().size();
        if( maxNumberOfChars < len )maxNumberOfChars = len;
      }
      else
      {
        std::size_t vlen = td.GetData().size();
        if( maxNumberOfStrings < vlen )maxNumberOfStrings = vlen;
        for( std::size_t j=0; j<vlen; ++j )
        {
          std::size_t len = td.GetData(j).size();
          if( maxNumberOfChars < len )maxNumberOfChars = len;
        }
      }
    }
    maxNumberOfChars =
      static_cast<int>(log10(static_cast<double>(maxNumberOfChars))) + 1;
    if( maxNumberOfStrings > 0 )maxNumberOfStrings =
      static_cast<int>(log10(static_cast<double>(maxNumberOfStrings))) + 1;
  }
  std::size_t maxVarNameLength = 6;  // minimum variable name length is 6
  std::size_t maxScalarValueLength = 5;
  std::size_t maxVecLength = 6;
  std::size_t maxMatrixRows = 5;
  std::size_t maxMatrixColumns = 8;
  char c[50];
  for( int i=0; i<numericVars; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( indexNV[i] );
    NumericData nd( nv->GetData() );
    std::size_t len = nv->GetName().size();
    if( maxVarNameLength < len )maxVarNameLength = len;
    switch ( nd.GetNumberOfDimensions() )
    {
      case 0:
      {
        double value = nd.GetScalarValue();
        ExString s = value>=0.0 ? ExString(" ")+ExString(value) : ExString(value);
        maxScalarValueLength = std::max( maxScalarValueLength, s.size() );
      }
      case 1:
      {
        maxVecLength = std::max( maxVecLength, ExString(nd.GetDimMag(0)).size() );
      }
      case 2:
      {
        maxMatrixRows = std::max( maxMatrixRows, ExString(nd.GetDimMag(0)).size() );
        maxMatrixColumns = std::max( maxMatrixColumns, ExString(nd.GetDimMag(1)).size() );
      }
    }
  }
  bool noVariablesFound = true;
  int nhistory = ExGlobals::GetNHistory();

  bool fit = qualifiers["FIT"];
  if( !qualifiers["VARY"] )fit = false;

  if( numericVars > 0 )
  {
    if( qualifiers["ALL"] || qualifiers["SCALARS"] )
    {
      bool first = true;
      for( int i=0; i<numericVars; ++i )
      {
        NumericVariable *nv = nvTable->GetEntry( indexNV[i] );
        NumericData nd( nv->GetData() );
        if( nd.GetNumberOfDimensions()==0 &&
            ( (nd.IsFit()&&fit) || (nd.IsFixed()&&qualifiers["FIXED"]) ) )
        {
          noVariablesFound = false;
          if( first )
          {
            ExString title;
            if( maxVarNameLength > 6 )title += ExString( maxVarNameLength-6, ' ' );
            title += ExString( "scalar = value" );
            if( maxScalarValueLength > 5 )title += ExString( maxScalarValueLength-5, ' ' );
            title += ExString( " type  history (most recent at the top)" );
            ExGlobals::WriteOutput( ExString(title.size(),'-') );
            ExGlobals::WriteOutput( title );
            ExGlobals::WriteOutput( ExString(title.size(),'-') );
            first = false;
          }
          ExString line;
          ExString name( nv->GetName() );
          std::size_t len = name.size();
          if( maxVarNameLength > len )line += ExString( maxVarNameLength-len, ' ' );
          line += name + ExString( " = " );
          double value = nd.GetScalarValue();
          ExString s = value>=0.0 ? ExString(" ")+ExString(value) : ExString(value);
          len = s.size();
          if( maxScalarValueLength > len )line += ExString( maxScalarValueLength-len, ' ' );
          line += s;
          line += nd.IsFit() ? ExString(" fit   ") : ExString(" fixed ");
          std::deque<ExString> history;
          history.assign( nv->GetHistory().begin(), nv->GetHistory().end() );
          std::size_t hSize = history.size();
          std::size_t nHistory;
          nhistory < 0 ? nHistory = hSize :
                         nHistory = std::min(hSize,static_cast<std::size_t>(nhistory));
          line += history[0];
          ExGlobals::WriteOutput( line );
          if( nHistory > 1 )
          {
            line.erase();
            ExString blanks( maxVarNameLength+3+maxScalarValueLength+7, ' ' );
            for( std::size_t j=1; j<nHistory; ++j )
            {
              line = blanks + history[j];
              ExGlobals::WriteOutput( line );
            }
          }
        }
      }
    }
  }
  if( qualifiers["ALL"] || qualifiers["VECTORS"] )
  {
    bool first = true;
    for( int i=0; i<numericVars; ++i )
    {
      NumericVariable *nv = nvTable->GetEntry( indexNV[i] );
      NumericData nd( nv->GetData() );
      if( nd.GetNumberOfDimensions() == 1 )
      {
        noVariablesFound = false;
        if( first )
        {
          ExString title;
          if( maxVarNameLength > 6 )
          {
            ExString blanks( maxVarNameLength-6, ' ' );
            title += blanks;
          }
          title += ExString( "vector" );
          if( maxVecLength > 6 )title += ExString( maxVecLength-6, ' ' );
          title += ExString( " length order      history (most recent at the top)" );
          ExGlobals::WriteOutput( ExString(title.size(),'-') );
          ExGlobals::WriteOutput( title );
          ExGlobals::WriteOutput( ExString(title.size(),'-') );
          first = false;
        }
        ExString line;
        ExString name( nv->GetName() );
        std::size_t len = name.size();
        if( maxVarNameLength > len )line += ExString( maxVarNameLength-len, ' ' );
        line += name;
        len = nd.GetDimMag(0);
        sprintf( c, " %*d", maxVecLength,len );
        line += ExString( c );
        if( nd.IsAscending() )
          line += ExString(" ascending  ");
        else if( nd.IsDescending() )
          line += ExString(" descending ");
        else if( nd.IsUnordered() )
          line += ExString(" unordered  ");
        std::deque<ExString> history( nv->GetHistory() );
        std::size_t hSize = history.size();
        std::size_t nHistory = nhistory<0 ? hSize : std::min(hSize,static_cast<std::size_t>(nhistory));
        line += history[0];
        ExGlobals::WriteOutput( line );
        if( nHistory > 1 )
        {
          line.erase();
          ExString blanks(maxVarNameLength+17+maxVecLength,' ');
          for( std::size_t j=1; j<nHistory; ++j )
          {
            line = blanks + history[j];
            ExGlobals::WriteOutput( line );
          }
        }
      }
    }
  }
  if( qualifiers["ALL"] || qualifiers["MATRICES"] )
  {
    bool first = true;
    for( int i=0; i<numericVars; ++i )
    {
      NumericVariable *nv = nvTable->GetEntry( indexNV[i] );
      NumericData nd( nv->GetData() );
      if( nd.GetNumberOfDimensions() == 2 )
      {
        noVariablesFound = false;
        if( first )
        {
          ExString title;
          if( maxVarNameLength > 6 )title += ExString( maxVarNameLength-6, ' ' );
          title += ExString( "matrix " );
          if( maxMatrixRows > 5 )title += ExString( maxMatrixRows-5, ' ' );
          title += ExString( " #rows" );
          if( maxMatrixColumns > 8 )title += ExString( maxMatrixColumns-8, ' ' );
          title += ExString( " #columns history (most recent at the top)" );
          ExGlobals::WriteOutput( ExString(title.size(),'-') );
          ExGlobals::WriteOutput( title );
          ExGlobals::WriteOutput( ExString(title.size(),'-') );
          first = false;
        }
        ExString line;
        ExString name( nv->GetName() );
        std::size_t len = name.size();
        if( maxVarNameLength > len )line += ExString( maxVarNameLength-len, ' ' );
        line += name;
        int nr = nd.GetDimMag(0);
        int nc = nd.GetDimMag(1);
        sprintf( c, "%*d ", maxMatrixRows, nr );
        ExString sr( c );
        sprintf( c, "%*d ", maxMatrixColumns, nc );
        ExString sc( c );
        line += sr + sc;
        std::deque<ExString> history( nv->GetHistory() );
        std::size_t hSize = history.size();
        std::size_t nHistory = nhistory<0 ? hSize : std::min(hSize,static_cast<std::size_t>(nhistory));
        line += history[0];
        ExGlobals::WriteOutput( line );
        if( nHistory > 1 )
        {
          line.erase();
          ExString blanks( maxVarNameLength+maxMatrixRows+maxMatrixColumns+9, ' ' );
          for( std::size_t j=1; j<nHistory; ++j )
          {
            line = blanks + history[j];
            ExGlobals::WriteOutput( line );
          }
        }
      }
    }
  }
  if( textVars > 0 )
  {
    if( qualifiers["ALL"] || qualifiers["TEXT"] )
    {
      ExString blanksSave;
      if( maxNumberOfStrings < 3 )blanksSave.assign( maxNumberOfStrings+3, ' ' );
      bool first = true;
      for( int i=0; i<textVars; ++i )
      {
        TextVariable *tv = tvTable->GetEntry( indexTV[i] );
        TextData td( tv->GetData() );
        noVariablesFound = false;
        if( first )
        {
          ExString title;
          if( maxTextName > 6 )title += ExString( maxTextName-4, ' ' );
          title += ExString( "text" );
          if( maxNumberOfStrings > 0 )
          {
            if( maxNumberOfStrings > 5 )title += ExString( maxNumberOfStrings-5, ' ' );
            title += " index";
          }
          if( maxNumberOfChars > 6 )title += ExString( maxNumberOfChars-6, ' ' );
          title += ExString(" length contents");
          ExGlobals::WriteOutput( ExString(title.size(),'-') );
          ExGlobals::WriteOutput( title );
          ExGlobals::WriteOutput( ExString(title.size(),'-') );
          first = false;
        }
        ExString line;
        ExString name( tv->GetName() );
        std::size_t len = name.size();
        if( maxTextName > len )line += ExString( maxTextName-len, ' ' );
        line += name;
        if( td.GetNumberOfDimensions() == 0 )
        {
          if( maxNumberOfStrings > 0 )
          {
            line += ExString( maxNumberOfStrings+3, ' ' );
            if( maxNumberOfStrings < 3 )line += blanksSave;
          }
          ExString value( td.GetScalarValue() );
          sprintf( c, " [%*d] ", maxNumberOfChars, td.GetScalarValue().size() );
          line += ExString(c) + value;
          ExGlobals::WriteOutput( line );
          line.erase();
        }
        else
        {
          std::size_t vLen = td.GetData().size();
          sprintf( c, "  [%*d]", maxNumberOfStrings, 1 );
          line += ExString(c);
          if( maxNumberOfStrings < 3 )line += blanksSave;
          ExString value( td.GetData(0) );
          sprintf( c, "[%*d] ", maxNumberOfChars, value.size() );
          line += ExString(c) + value;
          ExGlobals::WriteOutput( line );
          line.erase();
          for( std::size_t j=2; j<=vLen; ++j )
          {
            line.assign( ExString(maxTextName,' ') );
            sprintf( c, "  [%*d]", maxNumberOfStrings, j );
            line += ExString( c );
            if( maxNumberOfStrings < 3 )line += blanksSave;
            ExString value( td.GetData(j-1) );
            sprintf( c, "[%*d] ", maxNumberOfChars, value.size() );
            line += ExString(c) + value;
            ExGlobals::WriteOutput( line );
            line.erase();
          }
        }
      }
    }
  }
  if( noVariablesFound )
  {
    ExGlobals::WriteOutput( ExString(18,'-') );
    ExGlobals::WriteOutput( "no variables found" );
    ExGlobals::WriteOutput( ExString(18,'-') );
  }
}

void CMD_show::GetWild( ExString &s, std::vector<int> &indexNV, std::vector<int> &indexTV )
{
  s.ToUpper();
  int ls = s.size();
  char wild = '*';
  //
  // the only possibilities allowed are:   *x, *x*, *x*y, x*y, x*
  //
  int i1 = s.find_first_of( wild );
  int i2;
  if( i1 == 0 )           // wild is first character in string, e.g., *x
  {
    i2 = s.substr(2,ls-2).find_first_of( wild );
    //
    // i2 >= 0 --> there is another wild in string, e.g., *x*y
    // i2 <  0 --> only first character is wild, e.g., *x
    //
    i2 >= 0 ? i2 += 2 : i2 = -1;
  }
  else if( i1 > 0 )       // wild is in string, but not first, e.g., x*y or x*
  {
    i2 = i1;
    i1 = -1;
  }
  else                    // no wild in string
  {
    i1 = -1;
    i2 = -1;
  }
  int idx;
  NVariableTable *nvTable = NVariableTable::GetTable();
  int nvEntries = nvTable->Entries();
  for( int i=0; i<nvEntries; ++i )
  {
    bool flag = false;
    NumericVariable *nv = nvTable->GetEntry( i );
    ExString name( nv->GetName() );
    int lnm = name.size();
    if( i1 == 0 )                      // *x  or  *x*  or  *x*y
    {
      if( i2 == ls-1 )                 // *x*
      {
        ExString tmp( s.substr(1,ls-2) );
        idx = name.find_first_of( tmp );
        flag = ( idx>=0 );
      }
      else if( i2>=0 && i2<ls-1 )      // *x*y
      {
        ExString tmp( s.substr(1,i2-1) );
        idx = name.find_first_of( tmp );
        flag = ( idx>=0 && name.substr(lnm-ls+i2+1,ls-i2-1)==s.substr(i2+1,ls-i2-1) );
      }
      else if( i2 == -1 )              // *x
      {
        flag = ( name.substr(lnm-ls+1,ls-1)==s.substr(1,ls-1) );
      }
    }
    else if( i1 == -1 )                // x*y  or  x*  or  x
    {
      if( i2 == ls-1 )                 // x*
      {
        flag = ( name.substr(0,i2)==s.substr(0,i2) );
      }
      else if( i2>=0 && i2<ls-1 )      // x*y
      {
        flag = ( name.substr(0,i2)==s.substr(0,i2) &&
                 name.substr(lnm-ls+i2+1,ls-i2-1)==s.substr(i2+1,ls-i2-1) );
      }
      else if( i2 == -1 )              // no wild card
      {
        flag = ( name==s );
      }
    }
    if( flag )indexNV.push_back( i );
  }
  TVariableTable *tvTable = TVariableTable::GetTable();
  int tvEntries = tvTable->Entries();
  for( int i=0; i<tvEntries; ++i )
  {
    bool flag = false;
    TextVariable *tv = tvTable->GetEntry( i );
    ExString name( tv->GetName() );
    int lnm = name.size();
    if( i1 == 0 )                      // *x  or  *x*  or  *x*y
    {
      if( i2 == ls-1 )                 // *x*
      {
        ExString tmp( s.substr(1,ls-2) );
        idx = name.find_first_of( tmp );
        flag = ( idx>=0 );
      }
      else if( i2>=0 && i2<ls-1 )      // *x*y
      {
        ExString tmp( s.substr(1,i2-1) );
        idx = name.find_first_of( tmp );
        flag = ( idx>=0 && name.substr(lnm-ls+i2+1,ls-i2-1)==s.substr(i2+1,ls-i2-1) );
      }
      else if( i2 == -1 )              // *x
      {
        flag = ( name.substr(lnm-ls+1,ls-1)==s.substr(1,ls-1) );
      }
    }
    else if( i1 == -1 )                // x*y  or  x*  or  x
    {
      if( i2 == ls-1 )                 // x*
      {
        flag = ( name.substr(0,i2)==s.substr(0,i2) );
      }
      else if( i2>=0 && i2<ls-1 )      // x*y
      {
        flag = ( name.substr(0,i2)==s.substr(0,i2) &&
                 name.substr(lnm-ls+i2+1,ls-i2-1)==s.substr(i2+1,ls-i2-1) );
      }
      else if( i2 == -1 )              // no wild card
      {
        flag = ( name==s );
      }
    }
    if( flag )indexTV.push_back( i );
  }
}

// end of file
