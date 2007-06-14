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
#include <cmath>

#include "CMD_show.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"

CMD_show *CMD_show::cmd_show_ = 0;

CMD_show::CMD_show() : Command( wxT("SHOW") )
{
  AddQualifier( wxT("ALL"), false );
  AddQualifier( wxT("SCALARS"), false );
  AddQualifier( wxT("VECTORS"), false );
  AddQualifier( wxT("MATRICES"), false );
  AddQualifier( wxT("TENSORS"), false );
  AddQualifier( wxT("TEXT"), false );
  AddQualifier( wxT("FIT"), true );
  AddQualifier( wxT("VARY"), true );
  AddQualifier( wxT("FIXED"), true );
  AddQualifier( wxT("ORDERED"), true );
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
  qualifiers[wxT("ALL")] = !(qualifiers[wxT("SCALARS")] || qualifiers[wxT("VECTORS")] ||
                        qualifiers[wxT("MATRICES")] || qualifiers[wxT("TENSORS")] ||
                        qualifiers[wxT("TEXT")] );
  NVariableTable *nvTable = NVariableTable::GetTable();
  TVariableTable *tvTable = TVariableTable::GetTable();
  int nvEntries = nvTable->Entries();
  int tvEntries = tvTable->Entries();
  std::vector<int> indexNV, indexTV;
  wxString command( wxT("SHOW") );
  if( nTokens == 1 )
  {
    for( int i=0; i<nvEntries; ++i )indexNV.push_back( i );
    for( int i=0; i<tvEntries; ++i )indexTV.push_back( i );
  }
  else
  {
    for( int j=1; j<nTokens; ++j )
    {
      if( !p->IsString(j) )throw ECommandError( command+wxT("expecting variable names") );
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
  wxChar c[50];
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
        wxString s;
        if( value >= 0.0 )s << wxT(" ") << value;
        else              s << value;
        maxScalarValueLength = std::max( maxScalarValueLength, s.size() );
      }
      case 1:
      {
        wxString s;
        maxVecLength = std::max( maxVecLength, (s<<nd.GetDimMag(0)).size() );
      }
      case 2:
      {
        wxString s;
        maxMatrixRows = std::max( maxMatrixRows, (s<<nd.GetDimMag(0)).size() );
        s.clear();
        maxMatrixColumns = std::max( maxMatrixColumns, (s<<nd.GetDimMag(1)).size() );
      }
    }
  }
  bool noVariablesFound = true;
  int nhistory = ExGlobals::GetNHistory();

  bool fit = qualifiers[wxT("FIT")];
  if( !qualifiers[wxT("VARY")] )fit = false;

  if( numericVars > 0 )
  {
    if( qualifiers[wxT("ALL")] || qualifiers[wxT("SCALARS")] )
    {
      bool first = true;
      for( int i=0; i<numericVars; ++i )
      {
        NumericVariable *nv = nvTable->GetEntry( indexNV[i] );
        NumericData nd( nv->GetData() );
        if( nd.GetNumberOfDimensions()==0 &&
            ( (nd.IsFit() && fit) || (nd.IsFixed() && qualifiers[wxT("FIXED")])) )
        {
          noVariablesFound = false;
          if( first )
          {
            wxString title;
            if( maxVarNameLength > 6 )title += wxString( maxVarNameLength-6, ' ' );
            title += wxString( wxT("scalar = value") );
            if( maxScalarValueLength > 5 )title += wxString( maxScalarValueLength-5, ' ' );
            title += wxString( wxT(" type  history (most recent at the top)") );
            ExGlobals::WriteOutput( wxString(title.size(),'-') );
            ExGlobals::WriteOutput( title );
            ExGlobals::WriteOutput( wxString(title.size(),'-') );
            first = false;
          }
          wxString line;
          wxString name( nv->GetName() );
          std::size_t len = name.size();
          if( maxVarNameLength > len )line += wxString( maxVarNameLength-len, ' ' );
          line += name + wxString( wxT(" = ") );
          double value = nd.GetScalarValue();
          wxString s;
          if( value >= 0.0 )s << wxT(" ") << value;
          else              s << value;
          len = s.size();
          if( maxScalarValueLength > len )line += wxString( maxScalarValueLength-len, ' ' );
          line += s;
          line += nd.IsFit() ? wxString(wxT(" fit   ")) : wxString(wxT(" fixed "));
          std::deque<wxString> history( nv->GetHistory() );
          std::size_t hSize = history.size();
          std::size_t nHistory;
          nhistory < 0 ? nHistory = hSize :
                         nHistory = std::min(hSize,static_cast<std::size_t>(nhistory));
          line += history[0];
          ExGlobals::WriteOutput( line );
          if( nHistory > 1 )
          {
            line.erase();
            wxString blanks( maxVarNameLength+3+maxScalarValueLength+7, ' ' );
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
  if( qualifiers[wxT("ALL")] || qualifiers[wxT("VECTORS")] )
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
          wxString title;
          if( maxVarNameLength > 6 )
          {
            wxString blanks( maxVarNameLength-6, ' ' );
            title += blanks;
          }
          title += wxString( wxT("vector") );
          if( maxVecLength > 6 )title += wxString( maxVecLength-6, ' ' );
          title += wxString( wxT(" length order      history (most recent at the top)") );
          ExGlobals::WriteOutput( wxString(title.size(),'-') );
          ExGlobals::WriteOutput( title );
          ExGlobals::WriteOutput( wxString(title.size(),'-') );
          first = false;
        }
        wxString line;
        wxString name( nv->GetName() );
        std::size_t len = name.size();
        if( maxVarNameLength > len )line += wxString( maxVarNameLength-len, ' ' );
        line += name;
        len = nd.GetDimMag(0);
        ::wxSnprintf( c, 50, wxT(" %*d"), maxVecLength, len );
        line += wxString( c );
        if( nd.IsAscending() )
          line += wxString(wxT(" ascending  "));
        else if( nd.IsDescending() )
          line += wxString(wxT(" descending "));
        else if( nd.IsUnordered() )
          line += wxString(wxT(" unordered  "));
        std::deque<wxString> history( nv->GetHistory() );
        std::size_t hSize = history.size();
        std::size_t nHistory = nhistory<0 ? hSize : std::min(hSize,static_cast<std::size_t>(nhistory));
        line += history[0];
        ExGlobals::WriteOutput( line );
        if( nHistory > 1 )
        {
          line.erase();
          wxString blanks(maxVarNameLength+17+maxVecLength,' ');
          for( std::size_t j=1; j<nHistory; ++j )
          {
            line = blanks + history[j];
            ExGlobals::WriteOutput( line );
          }
        }
      }
    }
  }
  if( qualifiers[wxT("ALL")] || qualifiers[wxT("MATRICES")] )
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
          wxString title;
          if( maxVarNameLength > 6 )title += wxString( maxVarNameLength-6, ' ' );
          title += wxString( wxT("matrix ") );
          if( maxMatrixRows > 5 )title += wxString( maxMatrixRows-5, ' ' );
          title += wxString( wxT(" #rows") );
          if( maxMatrixColumns > 8 )title += wxString( maxMatrixColumns-8, ' ' );
          title += wxString( wxT(" #columns history (most recent at the top)") );
          ExGlobals::WriteOutput( wxString(title.size(),'-') );
          ExGlobals::WriteOutput( title );
          ExGlobals::WriteOutput( wxString(title.size(),'-') );
          first = false;
        }
        wxString line;
        wxString name( nv->GetName() );
        std::size_t len = name.size();
        if( maxVarNameLength > len )line += wxString( maxVarNameLength-len, ' ' );
        line += name;
        int nr = nd.GetDimMag(0);
        int nc = nd.GetDimMag(1);
        ::wxSnprintf( c, 50, wxT("%*d "), maxMatrixRows, nr );
        wxString sr( c );
        ::wxSnprintf( c, 50, wxT("%*d "), maxMatrixColumns, nc );
        wxString sc( c );
        line += sr + sc;
        std::deque<wxString> history( nv->GetHistory() );
        std::size_t hSize = history.size();
        std::size_t nHistory = nhistory<0 ? hSize : std::min(hSize,static_cast<std::size_t>(nhistory));
        line += history[0];
        ExGlobals::WriteOutput( line );
        if( nHistory > 1 )
        {
          line.erase();
          wxString blanks( maxVarNameLength+maxMatrixRows+maxMatrixColumns+9, ' ' );
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
    if( qualifiers[wxT("ALL")] || qualifiers[wxT("TEXT")] )
    {
      wxString blanksSave;
      if( maxNumberOfStrings < 3 )blanksSave.assign( maxNumberOfStrings+3, ' ' );
      bool first = true;
      for( int i=0; i<textVars; ++i )
      {
        TextVariable *tv = tvTable->GetEntry( indexTV[i] );
        TextData td( tv->GetData() );
        noVariablesFound = false;
        if( first )
        {
          wxString title;
          if( maxTextName > 6 )title += wxString( maxTextName-4, ' ' );
          title += wxString( wxT("text") );
          if( maxNumberOfStrings > 0 )
          {
            if( maxNumberOfStrings > 5 )title += wxString( maxNumberOfStrings-5, ' ' );
            title += wxT(" index");
          }
          if( maxNumberOfChars > 6 )title += wxString( maxNumberOfChars-6, ' ' );
          title += wxString(wxT(" length contents"));
          ExGlobals::WriteOutput( wxString(title.size(),'-') );
          ExGlobals::WriteOutput( title );
          ExGlobals::WriteOutput( wxString(title.size(),'-') );
          first = false;
        }
        wxString line;
        wxString name( tv->GetName() );
        std::size_t len = name.size();
        if( maxTextName > len )line += wxString( maxTextName-len, ' ' );
        line += name;
        if( td.GetNumberOfDimensions() == 0 )
        {
          if( maxNumberOfStrings > 0 )
          {
            line += wxString( maxNumberOfStrings+3, ' ' );
            if( maxNumberOfStrings < 3 )line += blanksSave;
          }
          wxString value( td.GetScalarValue() );
          ::wxSnprintf( c, 50, wxT(" [%*d] "), maxNumberOfChars, td.GetScalarValue().size() );
          line += wxString(c) + value;
          ExGlobals::WriteOutput( line );
          line.erase();
        }
        else
        {
          std::size_t vLen = td.GetData().size();
          ::wxSnprintf( c, 50, wxT("  [%*d]"), maxNumberOfStrings, 1 );
          line += wxString(c);
          if( maxNumberOfStrings < 3 )line += blanksSave;
          wxString value( td.GetData(0) );
          ::wxSnprintf( c, 50, wxT("[%*d] "), maxNumberOfChars, value.size() );
          line += wxString(c) + value;
          ExGlobals::WriteOutput( line );
          line.erase();
          for( std::size_t j=2; j<=vLen; ++j )
          {
            line.assign( wxString(maxTextName,' ') );
            ::wxSnprintf( c, 50, wxT("  [%*d]"), maxNumberOfStrings, j );
            line += wxString( c );
            if( maxNumberOfStrings < 3 )line += blanksSave;
            wxString value( td.GetData(j-1) );
            ::wxSnprintf( c, 50, wxT("[%*d] "), maxNumberOfChars, value.size() );
            line += wxString(c) + value;
            ExGlobals::WriteOutput( line );
            line.erase();
          }
        }
      }
    }
  }
  if( noVariablesFound )
  {
    ExGlobals::WriteOutput( wxString(18,wxT('-')) );
    ExGlobals::WriteOutput( wxT("no variables found") );
    ExGlobals::WriteOutput( wxString(18,wxT('-')) );
  }
}

void CMD_show::GetWild( wxString &s, std::vector<int> &indexNV, std::vector<int> &indexTV )
{
  s.UpperCase();
  int ls = s.size();
  wxChar wild = '*';
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
    wxString name( nv->GetName() );
    int lnm = name.size();
    if( i1 == 0 )                      // *x  or  *x*  or  *x*y
    {
      if( i2 == ls-1 )                 // *x*
      {
        wxString tmp( s.substr(1,ls-2) );
        idx = name.find_first_of( tmp );
        flag = ( idx>=0 );
      }
      else if( i2>=0 && i2<ls-1 )      // *x*y
      {
        wxString tmp( s.substr(1,i2-1) );
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
    wxString name( tv->GetName() );
    int lnm = name.size();
    if( i1 == 0 )                      // *x  or  *x*  or  *x*y
    {
      if( i2 == ls-1 )                 // *x*
      {
        wxString tmp( s.substr(1,ls-2) );
        idx = name.find_first_of( tmp );
        flag = ( idx>=0 );
      }
      else if( i2>=0 && i2<ls-1 )      // *x*y
      {
        wxString tmp( s.substr(1,i2-1) );
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
