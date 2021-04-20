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

#include "Expression.h"
#include "EVariableError.h"
#include "EExpressionError.h"
#include "TextVariable.h"
#include "TVariableTable.h"

#include "NumericVariable.h"

std::ostream &operator<<( std::ostream &out, NumericVariable const &nv )
{
  ExString closer;
  switch ( nv.data_.GetNumberOfDimensions() )
  {
    case 0: // scalar
    {
      out << "<scalar name=\"" << nv.name_ << "\"";
      nv.data_.IsFixed() ? out << " type=\"FIXED\">\n" : out << " type=\"FIT\">\n";
      closer = "</scalar>\n";
      break;
    }
    case 1: // vector
    {
      out << "<vector name=\"" << nv.name_ << "\" size=\""
          << nv.data_.GetDimMag(0) << "\"";
      if( nv.data_.IsUnordered() )     out << " order=\"UNORDERED\">\n";
      else if( nv.data_.IsAscending() )out << " order=\"ASCENDING\">\n";
      else                             out << " order=\"DESCENDING\">\n";
      closer = "</vector>\n";
      break;
    }
    case 2: // matrix
    {
      out << "<matrix name=\"" << nv.name_ << "\""
          << " rows=\"" << nv.data_.GetDimMag(0) << "\" columns=\""
          << nv.data_.GetDimMag(1) << "\">\n";
      closer = "</matrix>\n";
      break;
    }
    case 3: // tensor
    {
      out << "<tensor name=\"" << nv.name_ << "\"";
      out << " rows=\"" << nv.data_.GetDimMag(0) << "\" columns=\""
          << nv.data_.GetDimMag(1) << "\" planes=\""
          << nv.data_.GetDimMag(2) << "\">\n";
      closer = "</tensor>\n";
      break;
    }
  }
  out << nv.data_;
  out << "<history size=\"" << nv.history_.size() << "\">\n";
  std::deque<ExString>::const_reverse_iterator hEnd = nv.history_.rend();
  for( std::deque<ExString>::const_reverse_iterator i=nv.history_.rbegin(); i!=hEnd; ++i )
    out << "<string><![CDATA[" << *i << "]]></string>\n";
  return out << "</history>\n" << "<origin><![CDATA[" << nv.GetOrigin() << "]]></origin>\n" << closer;
}

void NumericVariable::GetVariable( ExString const &name, int &numberOfDimensions,
                                   double &scalarValue, std::vector<double> &data,
                                   int *dimSizes )
{
  // input:   name is the variable name to test
  //
  // output:  numberOfDimensions is 0 if scalar, 1 if vector, 2 if matrix, 3 if tensor
  //          scalarValue  used if numberOfDimensions = 0
  //          data         used if numberOfDimensions > 0
  //          dimSizes[0]  first dimension size,  if numberOfDimensions = 1 or 2
  //          dimSizes[1]  second dimension size, if numberOfDimensions = 2 or 3
  //          dimSizes[2]  third dimension size,  if numberOfDimensions = 3
  //
  if( name.empty() )throw EVariableError("name is blank");
  Expression expr( name );
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    throw EVariableError( e.what() );
  }
  if( expr.IsCharacter() )
    throw EVariableError("character expression found where numeric expected");
  //
  NumericData nd( expr.GetFinalAnswer() );
  numberOfDimensions = nd.GetNumberOfDimensions();
  data.assign( nd.GetData().begin(), nd.GetData().end() );
  for( std::size_t i=0; i<3; ++i )dimSizes[i] = nd.GetDimMag(i);
  if( numberOfDimensions == 0 )scalarValue = nd.GetScalarValue();
}

void NumericVariable::GetMatrix( ExString const &name, char const *description,
                                 std::vector<double> &data, std::size_t &nRow,
                                 std::size_t &nCol )
{
  int numberOfDimensions;
  double scalarValue;
  int dimSizes[3];
  try
  {
    GetVariable( name, numberOfDimensions, scalarValue, data, dimSizes );
  }
  catch( EVariableError &e )
  {
    throw EVariableError( ExString(description)+"\n"+e.what() );
  }
  switch (numberOfDimensions)
  {
    case 2:
    {
      nRow = dimSizes[0];
      nCol = dimSizes[1];
      break;
    }
    default:
    {
      ExString const varType[4] =
          { ExString("scalar"), ExString("vector"), ExString("matrix"), ExString("tensor") };
      ExString message( "expecting a matrix, but " );
      message += name + ExString(" is a ") + varType[numberOfDimensions];
      throw EVariableError( message );
    }
  }
}

void NumericVariable::GetVector( ExString const &name, char const *description,
                                 std::vector<double> &data )
{
  int numberOfDimensions;
  double scalarValue;
  int dimSizes[3];
  try
  {
    GetVariable( name, numberOfDimensions, scalarValue, data, dimSizes );
  }
  catch( EVariableError &e )
  {
    throw EVariableError( ExString(description)+"\n"+e.what() );
  }
  switch (numberOfDimensions)
  {
    case 1:
      break;
    case 0:
      data.push_back( scalarValue );
      break;
    default:
    {
      ExString const varType[4] =
          { ExString("scalar"), ExString("vector"), ExString("matrix"), ExString("tensor") };
      ExString message( "expecting a vector, but " );
      message += name + ExString(" is a ") + varType[numberOfDimensions];
      throw EVariableError( message );
    }
  }
}

void NumericVariable::GetScalar( ExString &name, char const *description, double &value )
{
  // returns the scalar value as a double (in value) and as a string (in name)
  //
  int numberOfDimensions;
  double scalarValue;
  int dimSizes[3];
  std::vector<double> data;
  try
  {
    GetVariable( name, numberOfDimensions, scalarValue, data, dimSizes );
  }
  catch( EVariableError &e )
  {
    throw EVariableError( ExString(description)+"\n"+e.what() );
  }
  switch (numberOfDimensions)
  {
    case 0:
    {
      value = scalarValue;
      //name = ExString( value );
      break;
    }
    default:
    {
      ExString const varType[4] =
          { ExString("scalar"), ExString("vector"), ExString("matrix"), ExString("tensor") };
      ExString message( "expecting a scalar, but " );
      message += name + ExString(" is a ") + varType[numberOfDimensions];
      throw EVariableError( message );
    }
  }
}

bool NumericVariable::DeletePartial( ExString const &name, ExString const &inputLine )
{
  int numberOfIndices;
  std::vector<double> indexVecs[3];
  bool indexIsScalar[3];
  try
  {
    GetIndices( const_cast<ExString&>(name), numberOfIndices, indexVecs, indexIsScalar );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  std::vector<double> data( data_.GetData() );
  int size = data.size();
  if( indexIsScalar[0] )
  {
    int index = static_cast<int>(indexVecs[0][0]);
    if( index > size )
      throw EVariableError( ExString("vector index > maximum (")+name+")" );
    if( index < 1 )
      throw EVariableError( ExString("vector index < minimum (")+name+")" );
    data.erase( data.begin()+index-1 );
  }
  else  // vector index
  {
    std::vector<double> tmp;
    int nValues = static_cast<int>(indexVecs[0].size());
    for( int j=1; j<=size; ++j )
    {
      bool remove = false;
      for( int k=0; k<nValues; ++k )
      {
        int index = static_cast<int>(indexVecs[0][k]);
        if( index > size )
          throw EVariableError( ExString("vector index > maximum (")+name+")" );
        if( index < 1 )
          throw EVariableError( ExString("vector index < minimum (")+name+")" );
        if( j == index )
        {
          remove = true;
          break;
        }
      }
      if( !remove )tmp.push_back( data[j-1] );
    }
    data.assign( tmp.begin(), tmp.end() );
  }
  bool allGone = false;
  if( data.empty() )
  {
    allGone = true;
  }
  else
  {
    data_.SetData( data );
    data_.SetDimMag( 0, data.size() );
    AddToHistory( inputLine );
  }
  return allGone;
}

void NumericVariable::GetIndices( ExString &name, int &numberOfIndices,
                                  std::vector<double> *indexVecs, bool *indexIsScalar )
{
  // First see if the variable name needs to be constructed, i.e.,
  // `A'//RCHAR(I) could be `A3' if I=3.  This is done by default, if
  // the name contains "//", but be aware that the expansion continues,
  // e.g. if A3=`X2', then if you entered A3=5,  you would get X2=5.
  // This also means that you can enter `A'//RCHAR(I)=`testing' since
  // the output variable name will be constructed.
  //
  try
  {
    Expression expr( name );
    expr.Evaluate();
    if( expr.IsCharacter() )name = expr.GetFinalString();
  }
  catch ( EExpressionError &e ) {}
  //
  ExString simpleName;
  try
  {
    simpleName = SimpleName( name );
    CheckReservedName( simpleName );
  }
  catch ( EVariableError &e )
  {
    throw;
  }
  int nameLen = simpleName.length();
  int indexLen = name.length() - nameLen;
  int dimension = 0;
  numberOfIndices = 0;
  if( indexLen == 0 )
  {
    name = simpleName;
    return;
  }
  ExString indices( name.substr(nameLen,indexLen) );
  int cntr = 0;
  int start = 1;
  //
  if( indices[0] != '(' && indices[0] != '{' && indices[0] != '[' )
    throw EVariableError( "indices must begin with an opening bracket" );
  //
  for( int i=0; i<indexLen; ++i )
  {
    if( indices[i]=='(' || indices[i]=='{' || indices[i]=='[' )++cntr;
    if( indices[i]==')' || indices[i]=='}' || indices[i]==']' )
    {
      if( cntr-- == 1 )
      {
        if( dimension == 3 )throw EVariableError( "max number of dimensions = 3" );
        Expression expr( ExString("[")+indices.substr(start,i-start)+ExString("]") );
        try
        {
          expr.Evaluate();
        }
        catch ( EExpressionError &e )
        {
          throw EVariableError( e.what() );
        }
        if( expr.IsCharacter() )throw EVariableError( "character expression in an index" );
        //
        NumericData nd( expr.GetFinalAnswer() );
        int ndim = nd.GetNumberOfDimensions();
        if( ndim == 0 )
        {
          indexVecs[dimension].push_back( nd.GetScalarValue() );
          indexIsScalar[dimension] = true;
        }
        else if( ndim == 1 )
        {
          if( nd.GetData().empty() )
            throw EVariableError(ExString("variable index is an empty vector: ")+indices);
          indexVecs[dimension].assign( nd.GetData().begin(), nd.GetData().end() );
          indexIsScalar[dimension] = false;
        }
        else
          throw EVariableError(ExString("variable index must be scalar or vector: ")+indices);
      }
    }
    if( indices[i]==',' && cntr==1 )
    {
      if( dimension == 3 )throw EVariableError( "max number of dimensions = 3" );
      ExString idx( "[" );
      idx += indices.substr( start, i-start ) + ExString("]");
      Expression expr( idx );
      try
      {
        expr.Evaluate();
      }
      catch ( EExpressionError &e )
      {
        throw EVariableError( e.what() );
      }
      if( expr.IsCharacter() )throw EVariableError( "character expression in an index" );
      NumericData nd( expr.GetFinalAnswer() );
      int ndim = nd.GetNumberOfDimensions();
      if( ndim == 0 )
      {
        indexVecs[dimension].push_back( nd.GetScalarValue() );
        indexIsScalar[dimension] = true;
      }
      else if( ndim == 1 )
      {
        if( nd.GetData().empty() )
          throw EVariableError(ExString("variable index is an empty vector: ")+idx);
        indexVecs[dimension].assign( nd.GetData().begin(), nd.GetData().end() );
        indexIsScalar[dimension] = false;
      }
      else
        throw EVariableError(ExString("variable index must be scalar or vector: ")+idx);
      ++dimension;
      start = i+1;
    }
  }
  if( cntr != 0 )throw EVariableError(ExString("unmatched brackets in index: ")+name);
  numberOfIndices = dimension+1;
  name = simpleName;
}

NumericVariable *NumericVariable::GetPreExisting( ExString const &name, 
                                                  int numberOfIndices,
                                                  std::vector<double> *indexVecs,
                                                  bool *indexIsScalar,
                                                  int *nmin, int *nmax, int *imin, int *imax )
{
  try
  {
    TVariableTable::GetTable()->RemoveEntry( TVariableTable::GetTable()->GetVariable(name) );
  }
  catch (EExpressionError &e)
  {
    throw EVariableError( e.what() );
  }
  //
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( name );
  if( nv )
  {
    std::size_t nDims = nv->GetData().GetNumberOfDimensions();
    for( std::size_t i=0; i<nDims; ++i )
    {
      imin[i] = 0;
      imax[i] = nv->GetData().GetDimMag(i);
      nmin[i] = imin[i];
      nmax[i] = imax[i];
    }
  }
  for( int i=0; i<numberOfIndices; ++i )
  {
    if( indexIsScalar[i] )
    {
      nmin[i] = static_cast<int>(indexVecs[i][0]);
      nmax[i] = nmin[i];
    }
    else
    {
      nmin[i] = static_cast<int>(indexVecs[i][0]);
      nmax[i] = nmin[i];
      std::size_t size = indexVecs[i].size();
      for( std::size_t j=0; j<size; ++j )
      {
        int tmp = static_cast<int>(indexVecs[i][j]);
        if( tmp < nmin[i] )nmin[i] = tmp;
        if( tmp > nmax[i] )nmax[i] = tmp;
      }
    }
  }
  return nv;
}

NumericVariable *NumericVariable::PutVariable( ExString name,
                                               double dataValue,
                                               ExString const &inputLine,
                                               bool addToHistory )
{
  // input data was a scalar
  //
  int numberOfIndices;
  std::vector<double> indexVecs[3];
  bool indexIsScalar[3];
  try
  {
    GetIndices( name, numberOfIndices, indexVecs, indexIsScalar );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  //
  int nmin[3] = {0,0,0}, nmax[3] = {0,0,0};
  int imin[3] = {0,0,0}, imax[3] = {0,0,0};
  //
  NumericVariable *nv = 0;
  try
  {
    nv = GetPreExisting( name, numberOfIndices, indexVecs, indexIsScalar,
                         nmin, nmax, imin, imax );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  bool preExisting = (nv!=0);
  //
  if( numberOfIndices == 0 ) // new variable had no indices so it will be a scalar
  {                          // e.g. X = 3
    if( nv && nv->data_.GetNumberOfDimensions()!=0 )
    {
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    if( !nv )
    {
      nv = new NumericVariable( name );
      nv->data_.SetNumberOfDimensions( 0 );
      nv->data_.SetFixed();
    }
    nv->data_.SetScalarValue( dataValue );
  }
  else if( numberOfIndices == 1 ) // new variable indices indicates vector
  {                               // but data indicates scalar
    //                               e.g. X[5] = 3 or X[3:9] = 2
    if( nv && nv->data_.GetNumberOfDimensions()!=1 )
    {
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    if( nv )
    {
      int newpts = std::max(nmax[0],imax[0]);
      std::vector<double> v( newpts, 0.0 );
      nv->data_.SetOrderType( 0 ); // unordered
      nv->data_.SetDimMag( 0, newpts );
      for( int i=0; i<imax[0]; ++i )v[i] = nv->data_.GetData(i);
      if( indexIsScalar[0] )             // e.g., X[-2] = 3
      {
        v[nmin[0]-1] = dataValue;
      }
      else                                   // e.g. X[3:9] = 2
      {
        std::size_t size = indexVecs[0].size();
        for( std::size_t i=0; i<size; ++i )v[static_cast<int>(indexVecs[0][i])-1] = dataValue;
      }
      nv->data_.SetData( v );
    }
    else                          // new variable does not pre-exist
    {
      nv = new NumericVariable( name );
      nv->data_.SetNumberOfDimensions( 1 );
      std::vector<double> v( nmax[0], 0.0 );
      if( indexIsScalar[0] )           // e.g. X[-2] = 3
      {
        v[nmin[0]-1] = dataValue;
      }
      else                                 // e.g. X[-2:5] = 2
      {
        std::size_t size = indexVecs[0].size();
        for( std::size_t i=0; i<size; ++i )v[static_cast<int>(indexVecs[0][i])-1] = dataValue;
      }
      nv->data_.SetData( v );
      nv->data_.SetDimMag( 0, nmax[0] );
    }
  }
  else if( numberOfIndices == 2 )  // new variable indices indicates matrix
  {                                // but data indicates scalar
    //                                e.g. X[2,5] = 3, X[2,3:9] = 2
    if( nv && nv->data_.GetNumberOfDimensions()!=2 )
    {
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    if( nv ) // variable exists and is a matrix
    {
      int newpts0 = std::max(nmax[0],imax[0]);
      int newpts1 = std::max(nmax[1],imax[1]);
      std::vector<double> v( newpts0*newpts1, 0.0 );
      int size1 = nv->data_.GetDimMag(0);
      for( int j=0; j<imax[1]; ++j )
      {
        for( int i=0; i<imax[0]; ++i )v[i+j*newpts0] = nv->data_.GetData(i+j*size1);
      }
      nv->data_.SetDimMag( 0, newpts0 );
      nv->data_.SetDimMag( 1, newpts1 );
      if( indexIsScalar[0] )
      {
        if( indexIsScalar[1] )       // e.g. X[2,5] = 3
          v[nmin[0]-1+(nmin[1]-1)*newpts0] = dataValue;
        else                         // e.g. X[2,3:9] = 2
        {
          std::size_t size = indexVecs[1].size();
          for( std::size_t j=0; j<size; ++j )
            v[nmin[0]-1+(static_cast<int>(indexVecs[1][j])-1)*newpts0] = dataValue;
        }
      }
      else
      {
        if( indexIsScalar[1] )   // e.g. X[2:7,3] = 3
        {
          std::size_t size = indexVecs[0].size();
          for( std::size_t i=0; i<size; ++i )
            v[static_cast<int>(indexVecs[0][i])-1+(nmin[1]-1)*newpts0] = dataValue;
        }
        else                         // e.g. X[2:7,3:9] = 3
        {
          std::size_t sizej = indexVecs[1].size();
          for( std::size_t j=0; j<sizej; ++j )
          {
            std::size_t sizei = indexVecs[0].size();
            for( std::size_t i=0; i<sizei; ++i )
              v[static_cast<int>(indexVecs[0][i])-1+
                (static_cast<int>(indexVecs[1][j])-1)*newpts0] = dataValue;
          }
        }
      }
      nv->data_.SetData( v );
    }
    else // variable does not exist already
    {
      nv = new NumericVariable( name );
      nv->data_.SetNumberOfDimensions( 2 );
      nv->data_.SetDimMag( 0, nmax[0] );
      nv->data_.SetDimMag( 1, nmax[1] );
      std::vector<double> v( nmax[0]*nmax[1], 0.0 );
      if( indexIsScalar[0] )
      {
        if( indexIsScalar[1] )    // e.g. X[2,5] = 2
          v[nmin[0]-1+(nmin[1]-1)*nmax[0]] = dataValue;
        else                          // e.g. X[2,3:9] = 2
        {
          std::size_t size = indexVecs[1].size();
          for( std::size_t j=0; j<size; ++j )
            v[nmin[0]-1+(static_cast<int>(indexVecs[1][j])-1)*nmax[0]] = dataValue;
        }
      }
      else
      {
        if( indexIsScalar[1] )    // e.g. X[2:7,3] = 2
        {
          std::size_t size = indexVecs[0].size();
          for( std::size_t j=0; j<size; ++j )
            v[static_cast<int>(indexVecs[0][j])-1+(nmin[1]-1)*nmax[0]] = dataValue;
        }
        else                          // e.g. X[2:7,3:9] = 2
        {
          std::size_t sizej = indexVecs[1].size();
          for( std::size_t j=0; j<sizej; ++j )
          {
            std::size_t sizei = indexVecs[0].size();
            for( std::size_t i=0; i<sizei; ++i )
              v[static_cast<int>(indexVecs[0][i])-1+
                (static_cast<int>(indexVecs[1][j])-1)*nmax[0]] = dataValue;
          }
        }
      }
      nv->data_.SetData( v );
    }
  }
  if( preExisting )
  {
    if( addToHistory )nv->AddToHistory( inputLine );
  }
  else
    nv->AddToHistory( inputLine );
  return nv;
}

NumericVariable *NumericVariable::PutVariable( ExString name,
                                               std::vector<double> const &data, int order,
                                               ExString const &inputLine, bool addToHistory )
{
  // input data was a vector
  //
  int numberOfIndices;
  std::vector<double> indexVecs[3];
  bool indexIsScalar[3];
  try
  {
    GetIndices( name, numberOfIndices, indexVecs, indexIsScalar );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  //
  int nmin[3] = {0,0,0}, nmax[3] = {0,0,0};
  int imin[3] = {0,0,0}, imax[3] = {0,0,0};
  //
  NumericVariable *nv = 0;
  try
  {
    nv = GetPreExisting( name, numberOfIndices, indexVecs, indexIsScalar,
                         nmin, nmax, imin, imax );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  bool preExisting = (nv!=0);
  //
  std::size_t dataSize = data.size();
  //
  bool savedHistory = false;
  std::deque<ExString> history2;
  //
  if( numberOfIndices == 0 ) // new variable had no indices so it will be a vector
  {                          // e.g. x = [3:9]
    if( nv ) // variable exists already
    {
      if( nv->data_.GetNumberOfDimensions() == 1 ) // variable is a pre-existing vector
      {
        savedHistory = true;
        history2.assign( nv->history_.begin(), nv->history_.end() );
      }
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    nv = new NumericVariable( name );
    nv->data_.SetNumberOfDimensions( 1 );
    nv->data_.SetOrderType( order );
    nv->data_.SetData( data );
    nv->data_.SetDimMag( 0, dataSize );
  }
  else if( numberOfIndices == 1 )  // new variable indices indicates vector
  {                                // e.g. x[2:8] = [3:9]
    if( nv && nv->data_.GetNumberOfDimensions()!=1 )
    {
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    std::vector<double> v;
    int tmp;
    if( nv ) // variable exists and is a vector
    {
      int newpts = std::max(nmax[0],imax[0]);
      v.swap( std::vector<double>(newpts,0.0) );
      std::size_t nvSize = nv->data_.GetData().size();
      for( std::size_t i=0; i<nvSize; ++i )v[i] = nv->data_.GetData(i);
      nv->data_.SetDimMag( 0, newpts );
      tmp = 1;
    }
    else     // variable does not exist
    {
      nv = new NumericVariable( name );
      nv->data_.SetNumberOfDimensions( 1 );
      nv->data_.SetDimMag( 0, nmax[0] );
      v.swap( std::vector<double>(nmax[0],0.0) );
      tmp = nmin[0];
    }
    if( indexIsScalar[0] )
    {
      v[nmin[0]-1] = data[0];
    }
    else
    {
      std::size_t size = std::min( indexVecs[0].size(), dataSize );
      for( std::size_t i=0; i<size; ++i )v[static_cast<int>(indexVecs[0][i])-tmp] = data[i];
    }
    nv->data_.SetOrderType( 0 ); // unordered
    nv->data_.SetData( v );
  }
  else if( numberOfIndices == 2 )  // new variable indices indicates matrix
  {                                // e.g. x[2,4:10] = [3:9] or x[2:4,9] = [3:5]
    if( nv && nv->data_.GetNumberOfDimensions()!=2 )
    {
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    std::vector<double> v;
    if( nv ) // variable exists and is a matrix
    {
      int newpts0 = max(nmax[0],imax[0]);
      int newpts1 = max(nmax[1],imax[1]);
      v.swap( std::vector<double>(newpts0*newpts1,0.0) );
      for( int j=0; j<imax[1]; ++j )
      {
        for( int i=0; i<imax[0]; ++i )v[i+j*newpts0] =
                                          nv->data_.GetData(i+j*nv->data_.GetDimMag(0));
      }
      nv->data_.SetDimMag( 0, newpts0 );
      nv->data_.SetDimMag( 1, newpts1 );
      if( indexIsScalar[0] )
      {
        if( indexIsScalar[1] )
          v[nmin[0]-1+(nmin[1]-1)*newpts0] = data[0];
        else
        {
          std::size_t size = std::min( dataSize, indexVecs[1].size() );
          for( std::size_t j=0; j<size; ++j )
            v[nmin[0]-1+(static_cast<int>(indexVecs[1][j])-1)*newpts0] = data[j];
        }
      }
      else
      {
        if( indexIsScalar[1] )
        {
          std::size_t size = std::min( dataSize, indexVecs[0].size());
          for( std::size_t i=0; i<size; ++i )
            v[static_cast<int>(indexVecs[0][i])-1+(nmin[1]-1)*newpts0] = data[i];
        }
        else
        {
          std::size_t k = 0;
          std::size_t size1 = indexVecs[1].size();
          for( std::size_t j=0; j<size1; ++j )
          {
            std::size_t size0 = indexVecs[0].size();
            for( std::size_t i=0; i<size0; ++i )
            {
              v[static_cast<int>(indexVecs[0][i])-1+
                (static_cast<int>(indexVecs[1][j])-1)*newpts0] = data[k];
              if( k++ == dataSize-1 )break;
            }
            if( k == dataSize-1 )break;
          }
        }
      }
    }
    else  // variable does not exist already
    {
      nv = new NumericVariable( name );
      nv->data_.SetNumberOfDimensions( 2 );
      nv->data_.SetDimMag( 0, nmax[0] );
      nv->data_.SetDimMag( 1, nmax[1] );
      v.swap( std::vector<double>(nmax[0]*nmax[1],0.0) );
      if( indexIsScalar[0] )
      {
        if( indexIsScalar[1] )
        {
          v[nmin[0]-1+(nmin[1]-1)*nmax[0]] = data[0];
        }
        else
        {
          std::size_t size = std::min( dataSize, indexVecs[1].size() );
          for( std::size_t j=0; j<size; ++j )
            v[nmin[0]-1+(static_cast<int>(indexVecs[1][j])-1)*nmax[0]] = data[j];
        }
      }
      else
      {
        if( indexIsScalar[1] )
        {
          std::size_t size = std::min( dataSize, indexVecs[0].size());
          for( std::size_t i=0; i<size; ++i )
            v[static_cast<int>(indexVecs[0][i])-1+(nmin[1]-1)*nmax[0]] = data[i];
        }
        else
        {
          std::size_t k = 0;
          std::size_t size1 = indexVecs[1].size();
          for( std::size_t j=0; j<size1; ++j )
          {
            std::size_t size0 = indexVecs[0].size();
            for( std::size_t i=0; i<size0; ++i )
            {
              v[static_cast<int>(indexVecs[0][i])-1+
                (static_cast<int>(indexVecs[1][j])-1)*nmax[0]] = data[k];
              if( k++ == dataSize-1 )break;
            }
            if( k == dataSize-1 )break;
          }
        }
      }
    }
    nv->data_.SetData( v );
  }
  if( savedHistory )nv->history_.assign( history2.begin(), history2.end() );
  if( preExisting )
  {
    if( addToHistory )nv->AddToHistory( inputLine );
  }
  else
    nv->AddToHistory( inputLine );
  return nv;
}

NumericVariable *NumericVariable::PutVariable( ExString name,
                                               std::vector<double> &data,
                                               std::size_t firstDim, std::size_t secondDim,
                                               ExString const &inputLine, bool addToHistory )
{
  // input data was a matrix
  //
  int numberOfIndices;
  std::vector<double> indexVecs[3];
  bool indexIsScalar[3];
  try
  {
    GetIndices( name, numberOfIndices, indexVecs, indexIsScalar );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  //
  int nmin[3] = {0,0,0}, nmax[3] = {0,0,0};
  int imin[3] = {0,0,0}, imax[3] = {0,0,0};
  //
  NumericVariable *nv = GetPreExisting( name, numberOfIndices, 
                                        indexVecs, indexIsScalar,
                                        nmin, nmax, imin, imax );
  bool preExisting = (nv!=0);
  //
  std::size_t dataSize = data.size();
  //
  bool savedHistory = false;
  std::deque<ExString> history2;
  //
  if( numberOfIndices == 0 ) // new variable had no indices so it will be a matrix
  {                          // e.g. x = m[2:5,3:9]
    if( nv )
    {
      if( nv->data_.GetNumberOfDimensions() == 2 ) // variable is a matrix already
      {
        savedHistory = true;
        history2.assign( nv->history_.begin(), nv->history_.end() );
      }
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    nv = new NumericVariable( name );
    nv->data_.SetNumberOfDimensions(2);
    nv->data_.SetDimMag( 0, firstDim );
    nv->data_.SetDimMag( 1, secondDim );
    nv->data_.SetData( data );
  }
  else if( numberOfIndices == 1 ) // new variable indices indicates vector
  {                               // e.g. x[1:28] = m[2:5,3:9]
    if( nv && nv->data_.GetNumberOfDimensions()!=1 )
      NVariableTable::GetTable()->RemoveEntry(nv); // this deletes nv and sets it to zero
    if( nv ) // variable exists and is a vector
    {                                       // e.g. x[14:41] = m[2:5,3:9]
      std::size_t newpts0 = std::max( nmax[0], imax[0] );
      std::vector<double> v( newpts0, 0.0 );
      for( int i=0; i<imax[0]; ++i )v[i] = nv->data_.GetData(i);
      nv->data_.SetOrderType( 0 ); // unordered
      nv->data_.SetDimMag( 0, newpts0 );
      if( indexIsScalar[0] )             // e.g. x[-3] = m[2:5,3:9]
        v[nmin[0]-1] = data[0];
      else                                   // e.g. x[-14:13] = m[2:5,3:9]
      {
        std::size_t k = 0;
        for( std::size_t j=0; j<secondDim; ++j )
        {
          for( std::size_t i=0; i<firstDim; ++i )
          {
            v[static_cast<int>(indexVecs[0][k])-1] = data[i+j*firstDim];
            if( k++ == newpts0 )break;
          }
          if( k == newpts0 )break;
        }
      }
      nv->data_.SetData( v );
    }
    else // variable does not exist already
    {
      nv = new NumericVariable( name );
      nv->data_.SetOrderType( 0 ); // unordered
      nv->data_.SetNumberOfDimensions( 1 );
      nv->data_.SetDimMag( 0, nmax[0] );
      std::vector<double> v( nmax[0], 0.0 );
      if( indexIsScalar[0] )
        v[nmin[0]-1] = data[0];
      else
      {
        std::size_t k = 0;
        for( std::size_t j=0; j<secondDim; ++j )
        {
          for( std::size_t i=0; i<firstDim; ++i )
          {
            v[static_cast<int>(indexVecs[0][k])-1] = data[i+j*firstDim];
            if( k++ == dataSize )break;
          }
          if( k == dataSize )break;
        }
      }
      nv->data_.SetData( v );
    }
  }
  else if( numberOfIndices == 2 ) // new variable indices indicates matrix
  {                               // e.g. x[2:7,1:4] = m[2:5,3:8]
    if( nv && nv->data_.GetNumberOfDimensions()!=2 )
    {
      NVariableTable::GetTable()->RemoveEntry(nv);
      nv = 0;
    }
    if( nv ) // variable exists already and is a matrix
    {
      int newpts0 = std::max( nmax[0], imax[0] );
      int newpts1 = std::max( nmax[1], imax[1] );
      std::vector<double> v( newpts0*newpts1, 0.0 );
      int size1 = nv->data_.GetDimMag(0);
      for( int j=0; j<imax[1]; ++j )
      {
        for( int i=0; i<imax[0]; ++i )v[i+j*newpts0] = nv->data_.GetData(i+j*size1);
      }
      nv->data_.SetDimMag( 0, newpts0 );
      nv->data_.SetDimMag( 1, newpts1 );
      if( indexIsScalar[0] )
      {
        if( indexIsScalar[1] )      // e.g. x[2,1] = m[2:5,3:8]
          v[nmin[0]-1+(nmin[1]-1)*newpts0] = data[0];
        else
        {
          std::size_t k = 0;          // e.g. x[2,1:2] = m[2:5,3:8]
          std::size_t v1size = indexVecs[1].size();
          for( std::size_t j=0; j<secondDim; ++j )
          {
            for( std::size_t i=0; i<firstDim; ++i )
            {
              v[nmin[0]-1+(static_cast<int>(indexVecs[1][k])-1)*newpts0] = data[i+j*firstDim];
              if( k++ == v1size-1 )break;
            }
            if( k == v1size-1 )break;
          }
        }
      }
      else
      {
        if( indexIsScalar[1] )   // e.g. x[2:3,1] = m[2:5,3:8]
        {
          std::size_t k = 0;
          std::size_t v0size = indexVecs[0].size();
          for( std::size_t j=0; j<secondDim; ++j )
          {
            for( std::size_t i=0; i<firstDim; ++i )
            {
              v[static_cast<int>(indexVecs[0][k])-1+(nmin[1]-1)*newpts0] = data[i+j*firstDim];
              if( k++ == v0size-1 )break;
            }
            if( k == v0size-1 )break;
          }
        }
        else
        {
          std::size_t k = 0;                  // e.g. x[2:3,1:2] = m[2:5,3:8]
          std::size_t l = 0;
          std::size_t v0size = indexVecs[0].size();
          std::size_t v1size = indexVecs[1].size();
          for( std::size_t j=0; j<secondDim; ++j )
          {
            for( std::size_t i=0; i<firstDim; ++i )
            {
              v[static_cast<int>(indexVecs[0][k])-1+
                (static_cast<int>(indexVecs[1][l])-1)*newpts0] = data[i+j*firstDim];
              if( k == v0size-1 )
              {
                if( l++ == v1size-1 )break;
                k = -1;
              }
              ++k;
            }
            if( l == v1size-1 )break;
          }
        }
      }
      nv->data_.SetData( v );
    }
    else // variable does not exist already
    {
      nv = new NumericVariable( name );
      nv->data_.SetNumberOfDimensions( 2 );
      nv->data_.SetDimMag( 0, nmax[0] );
      nv->data_.SetDimMag( 1, nmax[1] );
      std::vector<double> v( nmax[0]*nmax[1], 0.0 );
      if( indexIsScalar[0] )
      {
        if( indexIsScalar[1] )     // e.g. x[2,1] = m[2:5,3:8]
          v[nmin[0]-1] = data[0];
        else
        {
          std::size_t k = 0;                   // e.g. x[2,1:2] = m[2:5,3:8]
          std::size_t v1size = indexVecs[1].size();
          for( std::size_t j=0; j<secondDim; ++j )
          {
            for( std::size_t i=0; i<firstDim; ++i )
            {
              v[nmin[0]-1+(static_cast<int>(indexVecs[1][k])-1)*nmax[0]] = data[i+j*firstDim];
              if( k++ == v1size-1 )break;
            }
            if( k == v1size-1 )break;
          }
        }
      }
      else
      {
        if( indexIsScalar[1] )   // e.g. x[-2:3,-1] = m[2:5,3:8]
        {
          std::size_t k = 0;
          std::size_t v0size = indexVecs[0].size();
          for( std::size_t j=0; j<secondDim; ++j )
          {
            for( std::size_t i=0; i<firstDim; ++i )
            {
              v[static_cast<int>(indexVecs[0][k])-1+(nmin[1]-1)*nmax[0]] = data[i+j*firstDim];
              if( k++ == v0size-1 )break;
            }
            if( k == v0size-1 )break;
          }
        }
        else
        {
          std::size_t k = 0;                 // e.g. x[-2:3,-1:2] = m[2:5,3:8]
          std::size_t l = 0;
          std::size_t v0size = indexVecs[0].size();
          std::size_t v1size = indexVecs[1].size();
          for( std::size_t j=0; j<secondDim; ++j )
          {
            for( std::size_t i=0; i<firstDim; ++i )
            {
              v[static_cast<int>(indexVecs[0][k])-1+
                (static_cast<int>(indexVecs[1][l])-1)*nmax[0]] = data[i+j*firstDim];
              if( k == v0size-1 )
              {
                if( l++ == v1size-1 )break;
                k = -1;
              }
              if( l == v1size-1 )break;
              ++k;
            }
          }
        }
      }
      nv->data_.SetData( v );
    }
  }
  if( savedHistory )nv->history_.assign( history2.begin(), history2.end() );
  if( preExisting )
  {
    if( addToHistory )nv->AddToHistory( inputLine );
  }
  else
  {
    nv->AddToHistory( inputLine );
  }
  return nv;
}

// end of file
