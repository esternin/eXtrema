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
#include <limits>

#include "TextVariable.h"
#include "EVariableError.h"
#include "TVariableTable.h"
#include "NumericData.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "ExGlobals.h"
#include "EExpressionError.h"
#include "Expression.h"

TextVariable::TextVariable( wxString const &name ) : Variable(name)
{ TVariableTable::GetTable()->AddEntry(this); }

TextVariable::TextVariable( TextVariable const &rhs ) : Variable(rhs)
{ data_ = rhs.data_; }

TextVariable &TextVariable::operator=( TextVariable const &rhs )
{
  if( this != &rhs )
  {
    Variable::CopyStuff( rhs );
    data_ = rhs.data_;
  }
  return *this;
}

TextData &TextVariable::GetData()
{ return data_; }

void TextVariable::SetData( TextData const &data )
{ data_ = data; }

void TextVariable::GetVariable( wxString const &name, bool flag, wxString &data )
{
  // if flag == false:  if the name is reserved, return the uppercase
  //                    version of name
  // if flag == true:   if the name cannot be evaluated, simply return
  //                    the name without throwing an error
  //                    (useful for filenames)
  //
  if( name.empty() )throw EVariableError(wxT("variable name is blank"));
  //
  if( !flag )
  {
    try
    {
      CheckReservedName( name );
    }
    catch ( EVariableError &e )
    {
      data = name.Upper();
      return;
    }
  }
  Expression expr( name );
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    if( flag )
    {
      data = name;
      return;
    }
    throw EVariableError( wxString(e.what(),wxConvUTF8) );
  }
  if( expr.IsCharacter() )
  {
    data = expr.GetFinalString();
  }
  else if( expr.IsNumeric() )
  {
    if( flag )
    {
      data = name;
      return;
    }
    throw EVariableError( name+wxT(" is not character type") );
  }
}

std::ostream &operator<<( std::ostream &out, TextVariable const &tv )
{
  wxString closer;
  switch ( tv.data_.GetNumberOfDimensions() )
  {
    case 0: // scalar
    {
      out << "<textscalar name=\"" << tv.name_.mb_str(wxConvUTF8) << "\">\n"
          << "<string><![CDATA[" << tv.data_.GetScalarValue().mb_str(wxConvUTF8) << "]]></string>\n";
      closer = wxT("</textscalar>\n");
      break;
    }
    case 1: // vector
    {
      std::size_t size = const_cast<TextVariable&>(tv).data_.GetData().size();
      out << "<textvector name=\"" << tv.name_.mb_str(wxConvUTF8) << "\" size=\"" << size << "\">\n";
      for( std::size_t i=0; i<size; ++i )
        out << "<string><![CDATA[" << const_cast<TextVariable&>(tv).data_.GetData(i).mb_str(wxConvUTF8)
            << "]]></string>\n";
      closer = wxT("</textvector>\n");
      break;
    }
  }
  out << "<history size=\"" << tv.history_.size() << "\">\n";
  std::deque<wxString>::const_reverse_iterator hEnd = tv.history_.rend();
  for( std::deque<wxString>::const_reverse_iterator i=tv.history_.rbegin(); i!=hEnd; ++i )
    out << "<string><![CDATA[" << (*i).mb_str(wxConvUTF8) << "]]></string>\n";
  return out << "</history>\n<origin><![CDATA[" << tv.GetOrigin().mb_str(wxConvUTF8)
             << "]]></origin>\n" << closer.mb_str(wxConvUTF8);
}

wxString TextVariable::GetString( NumericData const *nda, NumericData const *ndb )
{
  wxString aString, finalString;
  if( nda )      // there is an array index
  {
    if( data_.GetNumberOfDimensions() == 0 )throw EVariableError(
      wxString(wxT("text scalar variable "))+name_+wxString(wxT(" cannot have an array index")) );
    if( nda->GetNumberOfDimensions() > 0 )throw EVariableError(
      wxString(wxT("text array variable "))+name_+wxString(wxT(" array index must be scalar")) );
    //
    int i = static_cast<int>( nda->GetScalarValue() );
    if( i > static_cast<int>( data_.GetData().size() ) )
      throw EVariableError( wxString(wxT("array index > maximum for "))+name_ );
    if( i <= 0 )
      throw EVariableError( wxString(wxT("array index <= 0 for "))+name_ );
    aString = data_.GetData( i-1 );
  }
  else
  {
    aString = data_.GetScalarValue();
  }
  if( ndb )    // there are character indices
  {
    if( ndb->GetNumberOfDimensions() == 0 ) // scalar character index
    {
      int jval = static_cast<int>( ndb->GetScalarValue() );
      if( jval <= 0 )
        throw EVariableError( wxString(wxT("character index <= 0 for "))+name_ );
      if( jval > static_cast<int>(aString.length()) )
        throw EVariableError( wxString(wxT("character index > maximum for "))+name_ );
      finalString = aString[jval-1];
    }
    else if( ndb->GetNumberOfDimensions() == 1 )
    {
      for( unsigned int k=0; k<ndb->GetDimMag(0); ++k )
      {
        int jval = static_cast<int>( ndb->GetData(k) );
        if( jval <= 0 )
          throw EVariableError( wxString(wxT("character index <= 0 for "))+name_ );
        if( jval > static_cast<int>(aString.length()) )
          throw EVariableError( wxString(wxT("character index > maximum for "))+name_ );
        finalString += aString[jval-1];
      }
    }
    else
      throw EVariableError(wxT("character index can only be scalar or vector"));
  }
  else               // there is no character index
    finalString = aString;
  return finalString;
}

void TextVariable::GetIndices( wxString &name, int &numberOfIndices,
                               std::vector<double> *indexVecs, bool *indexIsScalar )
{
  std::size_t dum = name.find( wxT("//") );
  if( dum != name.npos )
  {
    // See if the variable name needs to be constructed, i.e.,
    // `A'//RCHAR(I) could be `A3' if I=3.  This is done by default, if
    // the name contains "//", but be aware that the expansion continues,
    // e.g. if A3=`X2', then if you entered A3=5,  you would get X2=5.
    // This also means that you can enter `A'//RCHAR(I)=`testing' since
    // the output variable name will be constructed.
    //
    Expression expr( name );
    try
    {
      expr.Evaluate();
    }
    catch ( EExpressionError &e )
    {
      throw EVariableError( wxString(e.what(),wxConvUTF8) );
    }
    if( expr.IsCharacter() )name = expr.GetFinalString();
  }
  wxString simpleName;
  try
  {
    simpleName = SimpleName( name );
    CheckReservedName( simpleName );
  }
  catch( EVariableError &e )
  {
    throw;
  }
  int simpleLen = simpleName.size();
  int indexLen = name.size() - simpleLen;
  numberOfIndices = 0;
  if( indexLen == 0 ) // variable had no indices
  {
    name = simpleName;
    return;
  }
  wxString indices( name.substr(simpleLen,indexLen) );
  //
  // the first character of indices must be (, [, or {
  //
  int bracketCntr = 0;
  int start;
  for( int i=0; i<indexLen; ++i )
  {
    if( indices[i]=='(' || indices[i]=='{' || indices[i]=='[' )
    {
      ++bracketCntr;
      start = i+1;
    }
    if( indices[i]==')' || indices[i]=='}' || indices[i]==']' )
    {
      if( bracketCntr-- == 1 )
      {
        wxString idx = wxString(wxT("[")) + wxString(indices.substr(start,i-start)) + wxString(wxT("]"));
        if( numberOfIndices == 0 ) // working on first set of indices
        {                          // could be array or character index
          ++numberOfIndices;
          Expression expr( idx );
          try
          {
            expr.Evaluate();
          }
          catch ( EExpressionError &e )
          {
            throw EVariableError( wxString(e.what(),wxConvUTF8) );
          }
          if( expr.IsCharacter() )throw EVariableError( wxT("character expression in an index") );
          NumericData nd( expr.GetFinalAnswer() );
          //
          int ndim = nd.GetNumberOfDimensions();
          if( ndim == 0 )
          {
            if( static_cast<int>(nd.GetScalarValue()) < 0 )
              throw EVariableError( wxString(wxT("string variable index < 0: "))+idx );
            indexVecs[0].push_back( nd.GetScalarValue() );
            indexIsScalar[0] = true;
          }
          else if( ndim == 1 )
          {
            std::size_t size = nd.GetData().size();
            for( std::size_t i=0; i<size; ++i )
            {
              if( nd.GetData().at(i) < 0 )
                throw EVariableError( wxString(wxT("string variable index < 0: "))+idx );
            }
            indexVecs[0].assign( nd.GetData().begin(), nd.GetData().end() );
            indexIsScalar[0] = false;
          }
          else
            throw EVariableError( wxString(wxT("string variable index must be scalar or vector: "))+idx );
        }
        else if( numberOfIndices == 1 ) // working on second set of indices
        {                               // must be character index
          ++numberOfIndices;
          Expression expr( idx );
          try
          {
            expr.Evaluate();
          }
          catch ( EExpressionError &e )
          {
            throw EVariableError( wxString(e.what(),wxConvUTF8) );
          }
          //
          if( expr.IsCharacter() )throw EVariableError( wxT("character expression in an index") );
          NumericData nd( expr.GetFinalAnswer() );
          //
          int ndim = nd.GetNumberOfDimensions();
          if( ndim == 0 )
          {
            if( static_cast<int>(nd.GetScalarValue()) < 0 )
              throw EVariableError( wxString(wxT("string variable character index < 0: "))+idx );
            indexVecs[1].push_back( nd.GetScalarValue() );
            indexIsScalar[1] = true;
          }
          else if( ndim == 1 )
          {
            std::size_t size = nd.GetData().size();
            for( std::size_t i=0; i<size; ++i )
            {
              if( static_cast<int>(nd.GetData().at(i)) < 0 )
                throw EVariableError( wxString(wxT("string variable character index < 0: "))+idx );
            }
            indexVecs[1].assign( nd.GetData().begin(), nd.GetData().end() );
            indexIsScalar[1] = false;
          }
          else
            throw EVariableError( wxString(wxT("string variable index must be scalar or vector: "))+idx );
        }
        else
          throw EVariableError( wxString(wxT("string variable has too many indices: "))+idx );
      }
    }
  }
  if( bracketCntr != 0 )
    throw EVariableError( wxString(wxT("unmatched brackets in index: "))+name );
  name = simpleName;
}

bool TextVariable::DeletePartial( wxString &name, wxString const &inputLine )
{
  int numberOfIndices;
  std::vector<double> indexVecs[2];
  bool indexIsScalar[2];
  GetIndices( name, numberOfIndices, indexVecs, indexIsScalar );
  //
  bool allGone = false;
  if( numberOfIndices == 1 ) // one set of indices were used
  {
    if( data_.GetNumberOfDimensions() == 0 )  // scalar text variable
    {
      wxString s( data_.GetScalarValue() );
      int sLen = static_cast<int>(s.size());
      if( indexIsScalar[0] )  // destroy one character
      {
        int charIndex = static_cast<int>(indexVecs[0][0]);
        if( charIndex > sLen )
          throw EVariableError( wxString(wxT("character index > maximum ("))+name+wxT(")") );
        if( charIndex < 1 )
          throw EVariableError( wxString(wxT("character index < 1 ("))+name+wxT(")") );
        s.erase( charIndex-1 );
      }
      else  // destroy multiple characters
      {
        wxString t;
        int nValues = static_cast<int>(indexVecs[0].size());
        for( int j=1; j<=sLen; ++j )
        {
          bool remove = false;
          for( int k=0; k<nValues; ++k )
          {
            int charIndex = static_cast<int>(indexVecs[0][k]);
            if( charIndex > sLen )
              throw EVariableError( wxString(wxT("character index > maximum ("))+name+wxT(")") );
            if( charIndex < 1 )
              throw EVariableError( wxString(wxT("character index < 1 ("))+name+wxT(")") );
            if( j == charIndex )
            {
              remove = true;
              break;
            }
          }
          if( !remove )t.append( 1, s[j-1] );
        }
        s = t;
      }
      if( s.empty() )
      {
        allGone = true;
      }
      else
      {
        data_.SetScalarValue( s );
        AddToHistory( inputLine );
      }
    }
    else  // array text variable with one index
    {
      std::vector<wxString> data( data_.GetData() );
      int finalIndex = static_cast<int>(data.size());
      if( indexIsScalar[0] )  // destroy one string
      {
        int arrayIndex = static_cast<int>(indexVecs[0][0]);
        if( arrayIndex > finalIndex )
          throw EVariableError( wxString(wxT("text array index > maximum ("))+name+wxT(")") );
        if( arrayIndex < 1 )
          throw EVariableError( wxString(wxT("text array index < 1 ("))+name+wxT(")") );
        data.erase( data.begin()+arrayIndex-1 );
      }
      else  // destroy multiple strings from array text variable
      {
        std::vector<wxString> tmp;
        int nValues = static_cast<int>(indexVecs[0].size());
        int const fI = finalIndex;
        for( int j=1; j<=fI; ++j )
        {
          bool remove = false;
          for( int k=0; k<nValues; ++k )
          {
            int index = static_cast<int>(indexVecs[0][k]);
            if( index > fI )
              throw EVariableError( wxString(wxT("text array index > maximum ("))+name+wxT(")") );
            if( index < 1 )
              throw EVariableError( wxString(wxT("text array index < 1 ("))+name+wxT(")") );
            if( j == index )
            {
              remove = true;
              --finalIndex;
              break;
            }
          }
          if( !remove )tmp.push_back( data[j-1] );
        }
        data.assign( tmp.begin(), tmp.end() );
      }
      if( data.empty() )
      {
        allGone = true;
      }
      else
      {
        data_.SetData( data );
        AddToHistory( inputLine );
      }
    }
  }
  else if( numberOfIndices == 2 )
  {
    if( data_.GetNumberOfDimensions() == 0 )  // scalar text variable
    {
      ExGlobals::WarningMessage(
        wxString(wxT("array index not allowed on text scalar variables ("))+name+wxT(")") );
      return false;
    }
    std::vector<wxString> data( data_.GetData() );
    int finalIndex = static_cast<int>(data.size())+1;
    //
    if( indexIsScalar[0] )  // destroy characters in one string
    {
      int arrayIndex = static_cast<int>(indexVecs[0][0]);
      if( arrayIndex > finalIndex )
        throw EVariableError( wxString(wxT("text array index > maximum ("))+name+wxT(")") );
      if( arrayIndex < 1 )
        throw EVariableError( wxString(wxT("text array index < 1 ("))+name+wxT(")") );
      wxString s( data[arrayIndex-1] );
      int sLen = static_cast<int>( s.size() );
      if( indexIsScalar[1] )  // destroy one character in one string
      {
        int charIndex = static_cast<int>(indexVecs[1][0]);
        if( charIndex > sLen )
          throw EVariableError( wxString(wxT("character index > maximum ("))+name+wxT(")") );
        if( charIndex < 1 )
          throw EVariableError( wxString(wxT("character index < 1 ("))+name+wxT(")") );
        s.erase( charIndex-1 );
      }
      else // destroy multiple characters in one string
      {
        wxString t;
        int nValues = static_cast<int>(indexVecs[1].size());
        for( int j=1; j<=sLen; ++j )
        {
          bool remove = false;
          for( int k=0; k<nValues; ++k )
          {
            int charIndex = static_cast<int>(indexVecs[1][k]);
            if( charIndex > sLen )
              throw EVariableError( wxString(wxT("character index > maximum ("))+name+wxT(")") );
            if( charIndex < 1 )
              throw EVariableError( wxString(wxT("character index < 1 ("))+name+wxT(")") );
            if( j == charIndex )
            {
              remove = true;
              break;
            }
          }
          if( !remove )t.append( 1, s[j-1] );
        }
        s = t;
      }
      if( s.empty() )
        data.erase( data.begin()+arrayIndex-1 );
      else
        data[arrayIndex-1].assign( s );
      if( data.empty() )
      {
        allGone = true;
      }
      else
      {
        data_.SetData( data );
        AddToHistory( inputLine );
      }
    }
    else  // destroy one or more characters in multiple strings
    {
      int nArrayIndices = static_cast<int>(indexVecs[0].size());
      int const fI = finalIndex;
      for( int j=1; j<=fI; ++j )
      {
        wxString s( data[j-1] );
        int sLen = static_cast<int>( s.size() );
        for( int k=0; k<nArrayIndices; ++k )
        {
          int arrayIndex = static_cast<int>(indexVecs[0][k]);
          if( arrayIndex > fI )
            throw EVariableError( wxString(wxT("text array index > maximum ("))+name+wxT(")") );
          if( arrayIndex < 1 )
            throw EVariableError( wxString(wxT("text array index < 1 ("))+name+wxT(")") );
          if( j == arrayIndex )     // found a string to work on
          {
            if( indexIsScalar[1] )  // destroy one character in this string
            {
              int charIndex = static_cast<int>(indexVecs[1][0]);
              if( charIndex > sLen )
                throw EVariableError( wxString(wxT("character index > maximum ("))+name+wxT(")") );
              if( charIndex < 1 )
                throw EVariableError( wxString(wxT("character index < 1 ("))+name+wxT(")") );
              s.erase( charIndex-1 );
            }
            else // destroy multiple characters in this string
            {
              wxString t;
              int nCharIndices = static_cast<int>(indexVecs[1].size());
              for( int jj=1; jj<=sLen; ++jj )
              {
                bool removeChar = false;
                for( int kk=0; kk<nCharIndices; ++kk )
                {
                  int charIndex = static_cast<int>(indexVecs[1][kk]);
                  if( charIndex > sLen )
                    throw EVariableError( wxString(wxT("character index > maximum ("))+name+wxT(")") );
                  if( charIndex < 1 )
                    throw EVariableError( wxString(wxT("character index < 1 ("))+name+wxT(")") );
                  if( jj == charIndex )
                  {
                    removeChar = true;
                    break;
                  }
                }
                if( !removeChar )t.append( 1, s[jj-1] );
              }
              s = t;
            }
            if( s.empty() )break;
          }
        }
        if( s.empty() )data.erase( data.begin()+j-1 );
      }
      if( data.empty() )
      {
        allGone = true;
      }
      else
      {
        data_.SetData( data );
        AddToHistory( inputLine );
      }
    }
  }
  return allGone;
}

TextVariable *TextVariable::PutVariable( wxString name, wxString const &result,
                                         wxString const &inputLine, bool addToHistory )
{
  // name       = name of the text variable without indices
  // result     = characters to add to the text variable
  // inputLine  = the input line that led to this method being called
  //
  int numberOfIndices;
  std::vector<double> indexVecs[2];
  bool indexIsScalar[2];
  GetIndices( name, numberOfIndices, indexVecs, indexIsScalar );
  //
  // if it is a numeric variable, delete it
  //
  NVariableTable::GetTable()->RemoveEntry( name );
  //
  TextVariable *tv = TVariableTable::GetTable()->GetVariable( name );
  bool preExisting = (tv!=0);
  //
  bool arrayIndexUsed = false, characterIndexUsed = false;
  bool scalarCharacterIndex = true;
  int arrayIndex = 0, characterIndex = 0;
  //
  if( numberOfIndices == 2 )
  {
    arrayIndexUsed = true;
    characterIndexUsed = true;
    if( indexIsScalar[0] )
      arrayIndex = static_cast<int>(indexVecs[0][0]);
    else
      throw EVariableError(wxString(wxT("Error in "))+inputLine+wxString(wxT(": array index must be scalar")));
    if( indexIsScalar[1] )
    {
      characterIndex = static_cast<int>(indexVecs[1][0]);
      scalarCharacterIndex = true;
    }
    else
    {
      characterIndex = static_cast<int>(indexVecs[1][0]);
      std::size_t size = indexVecs[1].size();
      for( std::size_t i=0; i<size; ++i )
      {
        int j = static_cast<int>(indexVecs[1][i]);
        if( j > characterIndex )characterIndex = j;
      }
      scalarCharacterIndex = false;
    }
    if( preExisting && tv->data_.GetNumberOfDimensions()==0 ) // pre-existing scalar
    {
      TVariableTable::GetTable()->RemoveEntry( tv );
      tv = 0;
      preExisting = false;
    }
  }
  else if( numberOfIndices == 1 )
  {
    characterIndexUsed = true;
    if( preExisting )
    {
      if( tv->data_.GetNumberOfDimensions() == 1 )
      {
        arrayIndexUsed = true;
        characterIndexUsed = false;
      }
      else
      {
        if( result.size() > 1 )
        {
          arrayIndexUsed = true;
          characterIndexUsed = false;
        }
      }
    }
    else
    {
      if( result.size() > 1 )
      {
        arrayIndexUsed = true;
        characterIndexUsed = false;
      }
    }
    if( indexIsScalar[0] )
    {
      if( characterIndexUsed )
      {
        characterIndex = static_cast<int>(indexVecs[0][0]);
        scalarCharacterIndex = true;
      }
      else
        arrayIndex = static_cast<int>(indexVecs[0][0]);
    }
    else  // vector index used
    {
      if( characterIndexUsed )
      {
        characterIndex = static_cast<int>(indexVecs[0][0]);
        std::size_t size = indexVecs[0].size();
        for( std::size_t i=0; i<size; ++i )
        {
          int j = static_cast<int>(indexVecs[0][i]);
          if( j > characterIndex )characterIndex = j;
        }
        scalarCharacterIndex = false;
      }
      else
        throw EVariableError(wxString(wxT("Error in "))+inputLine+wxString(wxT(": array index must be scalar")));
    }
  }
  //
  // input is always a single string
  // which is why an array index must be a scalar
  //
  if( arrayIndexUsed )    // output variable had an array index so it will be a text vector
  {
    if( preExisting && tv->data_.GetNumberOfDimensions()==0 ) // output variable exists
    {                                                         // and is a text scalar
      TVariableTable::GetTable()->RemoveEntry( tv );
      tv = 0;
      preExisting = false;
    }
    if( preExisting )                   // output variable already exists so it must be a vector
    {
      for( int i=tv->data_.GetData().size(); i<arrayIndex-1; ++i )tv->data_.AddString(wxString(wxT(" ")));
      if( characterIndexUsed )          // there are character indices
      {
        int oldLength = tv->data_.GetData()[arrayIndex-1].size();
        int smax = std::max( characterIndex, oldLength );
        wxString s( smax, wxT(' ') );
        s.replace( 0, oldLength, tv->data_.GetData()[arrayIndex-1] );
        if( scalarCharacterIndex )
          s.replace( characterIndex-1, 1, 1, result.c_str()[0] );
        else
        {
          std::size_t j = 0;
          std::size_t size = indexVecs[1].size();
          std::size_t rsize = result.size();
          for( std::size_t i=0; i<size && j<rsize; ++i, ++j )
            s.replace( static_cast<int>(indexVecs[1][i])-1, 1, 1, result.c_str()[j] );
        }
        tv->data_.GetData()[arrayIndex-1].assign( s );
      }
      else                              // no character indices
      {
        if( arrayIndex-1 < static_cast<signed int>(tv->data_.GetData().size()) )
          tv->data_.GetData()[arrayIndex-1].assign( result );
        else
          tv->data_.GetData().push_back( result );
      }
    }
    else                                // output variable does not exist
    {
      tv = new TextVariable( name );
      tv->data_.SetNumberOfDimensions( 1 );
      for( int i=0; i<arrayIndex-1; ++i )tv->data_.AddString( wxString(wxT(" ")) );
      if( characterIndexUsed )          // there are character indices
      {
        wxString s( characterIndex-1, wxT(' ') );
        if( scalarCharacterIndex )
          s.replace( characterIndex-1, 1, 1, result.c_str()[0] );
        else
        {
          std::size_t j = 0;
          std::size_t size = indexVecs[0].size();
          for( std::size_t i=0; i<size && j<result.size(); ++i, ++j )
            s.replace( static_cast<int>(indexVecs[0][i])-1, 1, 1, result.c_str()[j] );
        }
        tv->data_.AddString( s );
      }
      else                              // no character indices
        tv->data_.AddString( result );
    }
  }
  else                    // output variable had no array indices
  {
    if( preExisting && tv->data_.GetNumberOfDimensions()==1 ) // output variable exists
    {                                                         // and is a text vector
      TVariableTable::GetTable()->RemoveEntry( tv );
      tv = 0;
      preExisting = false;
    }
    if( preExisting )                   // output variable already exists
    {                                   // so it must be a scalar
      if( characterIndexUsed )          // there are character indices
      {
        wxString s( tv->data_.GetScalarValue() );
        int oldLength = s.size();
        int newLength = std::max( characterIndex, oldLength );
        if( newLength > oldLength )s.append( newLength-oldLength, ' ' );
        if( scalarCharacterIndex )
          s.replace( characterIndex-1, 1, 1, result.c_str()[0] );
        else
        {
          std::size_t j = 0;
          std::size_t size = indexVecs[0].size();
          for( std::size_t i=0; i<size && j<result.size(); ++i, ++j )
            s.replace( static_cast<int>(indexVecs[0][i])-1, 1, 1, result.c_str()[j] );
        }
        tv->data_.SetScalarValue( s );
      }
      else                              // no character indices used
      {
        tv->data_.SetScalarValue( result );
      }
    }
    else                                // output variable does not exist
    {
      tv = new TextVariable( name );
      tv->data_.SetNumberOfDimensions( 0 );
      if( characterIndexUsed )          // there are character indices
      {
        wxString s( characterIndex-1, wxT(' ') );
        if( scalarCharacterIndex )
          s.replace( characterIndex-1, 1, 1, result.c_str()[0] );
        else
        {
          std::size_t j = 0;
          std::size_t size = indexVecs[0].size();
          for( std::size_t i=0; i<size && j<result.size(); ++i,++j )
            s.replace( static_cast<int>(indexVecs[0][i])-1, 1, 1, result.c_str()[j] );
        }
        tv->data_.SetScalarValue( s );
      }
      else                              // no character indices used
        tv->data_.SetScalarValue( result );
    }
  }
  if( addToHistory )tv->AddToHistory( inputLine );
  return tv;
}

TextVariable *TextVariable::PutVariable( wxString name,
                                         std::vector<wxString> const &results,
                                         wxString const &inputLine, bool addToHistory )
{
  // name       = name of the text variable without indices
  // results    = strings to add to the text variable
  // inputLine  = the input line that led to this method being called
  //
  int numberOfIndices;
  std::vector<double> indexVecs[2];
  bool indexIsScalar[2];
  GetIndices( name, numberOfIndices, indexVecs, indexIsScalar );
  //
  // if it is a numeric variable, delete it
  //
  NVariableTable::GetTable()->RemoveEntry( name );
  //
  TextVariable *tv = TVariableTable::GetTable()->GetVariable( name );
  bool preExisting = (tv != NULL);
  //
  bool arrayIndexUsed = false;
  bool characterIndexUsed = false;
  std::vector<double> arrayIndex;
  std::vector<double> characterIndex;
  int maxCharacterIndex = std::numeric_limits<int>::min();
  //
  if( numberOfIndices == 2 )
  {
    arrayIndexUsed = true;
    characterIndexUsed = true;
    arrayIndex.assign( indexVecs[0].begin(), indexVecs[0].end() );
    characterIndex.assign( indexVecs[1].begin(), indexVecs[1].end() );
    std::size_t size = characterIndex.size();
    for( std::size_t i=0; i<size; ++i )
      maxCharacterIndex = std::max( static_cast<int>(characterIndex[i]), maxCharacterIndex );
    if( preExisting && tv->data_.GetNumberOfDimensions()==0 )
    {
      TVariableTable::GetTable()->RemoveEntry( tv );
      tv = 0;
      preExisting = false;
    }
  }
  else if( numberOfIndices == 1 )
  {
    characterIndexUsed = true;
    if( preExisting )
    {
      if( tv->data_.GetNumberOfDimensions() == 1 )
      {
        arrayIndex.assign( indexVecs[0].begin(), indexVecs[0].end() );
        arrayIndexUsed = true;
        characterIndexUsed = false;
      }
      else // not a vector
      {
        if( results.size() > 1 )
        {
          arrayIndex.assign( indexVecs[0].begin(), indexVecs[0].end() );
          arrayIndexUsed = true;
          characterIndexUsed = false;
          TVariableTable::GetTable()->RemoveEntry( tv );
          tv = 0;
          preExisting = false;
        }
        else // results is a single string
        {
          characterIndex.assign( indexVecs[0].begin(), indexVecs[0].end() );
          std::size_t size = characterIndex.size();
          for( std::size_t i=0; i<size; ++i )
            maxCharacterIndex = std::max( static_cast<int>(characterIndex[i]), maxCharacterIndex );
        }
      }
    }
    else // not pre-existing
    {
      if( results.size() > 1 )
      {
        arrayIndex.assign( indexVecs[0].begin(), indexVecs[0].end() );
        arrayIndexUsed = true;
        characterIndexUsed = false;
      }
      else
      {
        characterIndex.assign( indexVecs[0].begin(), indexVecs[0].end() );
        std::size_t size = characterIndex.size();
        for( std::size_t i=0; i<size; ++i )
          maxCharacterIndex = std::max( static_cast<int>(characterIndex[i]), maxCharacterIndex );
      }
    }
  }
  if( arrayIndexUsed )    // output variable had an array index
  {                       // so it will be a text vector
    if( results.size()!=1 && results.size()!=arrayIndex.size() )
      throw EVariableError( wxT("array index length not equal to results length") );
    //
    if( preExisting )                   // output variable already exists
    {                                   // so it must be a vector
      std::size_t tvLength = tv->data_.GetData().size();
      std::size_t size = static_cast<std::size_t>(arrayIndex[0]-1);
      for( std::size_t i=tvLength; i<size; ++i )tv->data_.AddString( wxString(wxT(" ")) );
      size = arrayIndex.size();
      for( std::size_t i=0; i<size; ++i )
      {
        std::size_t j = static_cast<std::size_t>(arrayIndex[i]);
        wxString result;
        results.size()==1 ? result=results[0] : result=results[i];
        if( characterIndexUsed )
        {
          if( result.size() != characterIndex.size() )
           throw EVariableError( wxT("character index length not equal to result length") );
          //
          int smax = std::max( maxCharacterIndex,
                               static_cast<int>(tv->data_.GetData(j-1).size()) );
          wxString s( smax, wxT(' ') );
          s.replace( 0, tv->data_.GetData(j-1).size(), tv->data_.GetData(j-1) );
          std::size_t k = 0;
          std::size_t size = characterIndex.size();
          for( std::size_t m=0; m<size; ++m,++k )s[static_cast<int>(characterIndex[m])-1] = result[k];
          tv->data_.GetData(j-1).assign( s );
        }
        else // no character indices
        {
          if( j-1 < tv->data_.GetData().size() )
            tv->data_.GetData(j-1).assign( result );
          else
            tv->data_.AddString( result );
        }
        if( i < arrayIndex.size()-1 )
        {
          for( std::size_t k=j+1; k<static_cast<std::size_t>(arrayIndex[i+1]); ++k )
          {
            if( k-1 >= tvLength )tv->data_.AddString( wxString(wxT(" ")) );
          }
        }
      }
    }
    else // output variable does not exist
    {
      tv = new TextVariable( name );
      tv->data_.SetNumberOfDimensions( 1 );
      for( int i=0; i<static_cast<int>(arrayIndex[0])-1; ++i )tv->data_.AddString( wxString(wxT(" ")) );
      for( std::size_t i=0; i<arrayIndex.size(); ++i )
      {
        int j = static_cast<int>(arrayIndex[i]);
        wxString result;
        results.size()==1 ? result=results[0] : result=results[i];
        wxString s;
        if( characterIndexUsed )
        {
          if( result.size() != characterIndex.size() )
            throw EVariableError( wxT("character index length not equal to result length") );
          s = wxString( maxCharacterIndex, wxT(' ') );
          std::size_t k = 0;
          std::size_t size = characterIndex.size();
          for( std::size_t m=0; m<size; ++m,++k )
            s[static_cast<int>(characterIndex[m])-1] = result[k];
        }
        else // no character indices
          s = result;
        tv->data_.AddString( s );
        if( i < arrayIndex.size()-1 )
        {
          for( int k=j+1; k<static_cast<int>(arrayIndex[i+1]); ++k )
            tv->data_.AddString( wxString(wxT(" ")) );
        }
      }
    }
  }
  else // output variable had no array indices
  {
    if( preExisting )                   // output variable already exists
    {                                   // so it must be a scalar
      if( characterIndexUsed )          // there are character indices
      {
        if( results.size() != 1 )
         throw EVariableError( wxT("no array indices given but results is an array") );
        if( results[0].size() != characterIndex.size() )
         throw EVariableError( wxT("character index length not equal to result length") );
        //
        wxString s( tv->data_.GetScalarValue() );
        int oldLength = s.size();
        int newLength = std::max( maxCharacterIndex, oldLength );
        if( newLength > oldLength )s.append( newLength-oldLength, ' ' );
        std::size_t j = 0;
        std::size_t size = characterIndex.size();
        for( std::size_t i=0; i<size; ++i,++j )
          s.replace( static_cast<int>(characterIndex[i])-1, 1, 1, results[0].c_str()[j] );
        tv->data_.SetScalarValue( s );
      }
      else // no character indices used
      {
        if( results.size() == 1 )
        {
          TVariableTable::GetTable()->RemoveEntry( tv );
          tv = 0;
          preExisting = false;
          tv = new TextVariable( name );
          tv->data_.SetNumberOfDimensions( 0 );
          tv->data_.SetScalarValue( results[0] );
        }
        else
        {
          TVariableTable::GetTable()->RemoveEntry( tv );
          tv = 0;
          preExisting = false;
          tv = new TextVariable( name );
          tv->data_.SetNumberOfDimensions( 1 );
          std::size_t size = results.size();
          for( std::size_t i=0; i<size; ++i )tv->data_.AddString( results[i] );
        }
      }
    }
    else // output variable does not exist
    {
      tv = new TextVariable( name );
      if( characterIndexUsed )
      {
        if( results.size() != 1 )
         throw EVariableError( wxT("no array indices given but results is an array") );
        if( results[0].size() != characterIndex.size() )
         throw EVariableError( wxT("character index length not equal to result length") );
        //
        wxString s( maxCharacterIndex, wxT(' ') );
        std::size_t j = 0;
        std::size_t size = characterIndex.size();
        for( std::size_t i=0; i<size; ++i,++j )
          s.replace( static_cast<int>(characterIndex[i])-1, 1, 1, results[0].c_str()[j] );
        tv->data_.SetNumberOfDimensions( 0 );
        tv->data_.SetScalarValue( s );
      }
      else // no character indices used
      {
        if( results.size() == 1 )
        {
          tv->data_.SetNumberOfDimensions( 0 );
          tv->data_.SetScalarValue( results[0] );
        }
        else
        {
          tv->data_.SetNumberOfDimensions( 1 );
          std::size_t size = results.size();
          for( std::size_t i=0; i<size; ++i )tv->data_.AddString( results[i] );
        }
      }
    }
  }
  if( addToHistory )tv->AddToHistory( inputLine );
  return tv;
}

// end of file
