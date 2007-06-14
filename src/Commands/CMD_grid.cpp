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

#include "wx/wx.h"

#include "CMD_grid.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "GRA_thiessenTriangulation.h"
#include "ExGlobals.h"

CMD_grid *CMD_grid::cmd_grid_ = 0;

CMD_grid::CMD_grid() : Command( wxT("GRID") )
{
  AddQualifier( wxT("SIZE"), false );
  AddQualifier( wxT("XYOUT"), false );
  AddQualifier( wxT("BOUNDS"), false );
  AddQualifier( wxT("PATTERN"), false );
  AddQualifier( wxT("INDICES"), false );
  AddQualifier( wxT("INTERPOLATE"), true );
  AddQualifier( wxT("CHECKDUP"), false );
  AddQualifier( wxT("MESSAGES"), true );
}

void CMD_grid::Execute( ParseLine const *p )
{
  // GRID  x y z m
  // GRID\SIZE  size  x y z m
  // GRID\XYOUT  x y z m xout yout 
  // GRID\SIZE\XYOUT  size x y z m xout yout 
  // GRID\BOUNDS  x y z m minx maxx miny maxy
  // GRID\SIZE\BOUNDS  size x y z m minx maxx miny maxy
  // GRID\XYOUT\BOUNDS  x y z m xout yout minx maxx miny maxy
  // GRID\SIZE\XYOUT\BOUNDS  size x y z m xout yout minx maxx miny maxy
  // GRID\SIZE matrix { size } x y z { xout yout } { minx maxx miny maxy }
  //
  // GRID\PATTERN x y z m
  // GRID\PATTERN\XYOUT x y z m xout yout
  //
  // GRID\INDICES   x y z m
  // GRID\INDICES\XYOUT  x y z m  xout yout
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
  //
  int counter = 1;
  double size;
  if( qualifiers[wxT("SIZE")] )
  {
    if( qualifiers[wxT("PATTERN")] )
      throw ECommandError( command+wxT("\\SIZE incompatible with \\PATTERN") );
    if( qualifiers[wxT("INDICES")] )
      throw ECommandError( command+wxT("\\SIZE incompatible with \\INDICES") );
    if( p->GetNumberOfTokens() < 2 )
      throw ECommandError( command+wxT("expecting matrix output size") );
    try
    {
      NumericVariable::GetScalar( p->GetString(1), wxT("matrix output size"), size );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( size < 0.0 )throw ECommandError( command+wxT("output matrix size < 0") );
    if( size == 0.0 )throw ECommandError( command+wxT("output matrix size = 0") );
    AddToStackLine( p->GetString(1) );
    ++counter;
  }
  //
  if( p->GetNumberOfTokens() < 5 )
    throw ECommandError( command+wxT("expecting x, y and z vectors") );
  std::vector<double> x, y, z;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("x vector"), x );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("y vector"), y );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  if( qualifiers[wxT("MESSAGES")] && x.size()!=y.size() )
    ExGlobals::WarningMessage( wxT("GRID: x and y vectors have different lengths, using minimum") );
  std::size_t num = std::min( x.size(), y.size() );
  //
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("z vector"), z );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  if( qualifiers[wxT("MESSAGES")] && z.size()!=num )ExGlobals::WarningMessage(
   wxT("GRID: vectors have different lengths, using minimum") );
  num = std::min( num, z.size() );
  //
  if( qualifiers[wxT("INTERPOLATE")] && num<4 )
    throw ECommandError( command+wxT("grid interpolation requires at least 4 points") );
  //
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( command+wxT("expecting output matrix name") );
  wxString matrixName( p->GetString(counter++) );
  AddToStackLine( matrixName );
  //
  if( qualifiers[wxT("CHECKDUP")] )
  {
    std::vector<std::size_t> dup( num );
    for( std::size_t i=0; i<num-1; ++i )
    {
      for( std::size_t j=i+1; j<num; ++j )
      {
        if( x[i] == x[j] )
        {
          if( y[i] == y[j] )
          {
            if( qualifiers[wxT("MESSAGES")] )
            {
              wxString s( wxT("GRID: duplicate points found at indices ") );
              ExGlobals::WarningMessage( s << i+1 << wxT(" and ") << j+1 );
              dup.push_back( j );
            }
          }
        }
      }
    }
    if( dup.size() > 0 )
    {
      if( qualifiers[wxT("MESSAGES")] )
        ExGlobals::WarningMessage( wxString(wxT("GRID: "))+dup.size()+wxT(" duplicate points found") );
      for( std::size_t i=0; i<dup.size(); ++i )
      {
        std::size_t j = dup.size()-i-1;
        for( std::size_t k=dup[j]; k<num; ++k )
        {
          x[k-1] = x[k];
          y[k-1] = y[k];
          z[k-1] = z[k];
        }
      }
    }
    num -= num - dup.size();
  }
  if( qualifiers[wxT("INDICES")] ) // make matrix without interpolation with indices data
  {
    std::size_t ncol = static_cast<std::size_t>( x[0] );
    for( std::size_t i=0; i<num; ++i )
    {
      if( ncol < static_cast<std::size_t>(x[i]) )ncol = static_cast<std::size_t>(x[i]);
    }
    std::size_t nrow = static_cast<std::size_t>( y[0] );
    for( std::size_t i=0; i<num; ++i )
    {
      if( nrow < static_cast<std::size_t>(y[i]) )nrow = static_cast<std::size_t>(y[i]);
    }
    std::vector<double> xi, yi;
    wxString xoutName, youtName;
    if( qualifiers[wxT("XYOUT")] )
    {
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting xout vector") );
      xoutName = p->GetString(counter++);
      AddToStackLine( xoutName );
      //
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting yout vector") );
      youtName = p->GetString(counter++);
      AddToStackLine( youtName );
      //
      for( std::size_t i=0; i<nrow; ++i )yi.push_back( static_cast<double>(i+1) );
      for( std::size_t i=0; i<ncol; ++i )xi.push_back( static_cast<double>(i+1) );
    }
    std::vector<double> array( nrow*ncol, 0.0 );
    for( std::size_t i=0; i<num; ++i )
    {
      std::size_t jrow = static_cast<std::size_t>(y[i]-1);
      std::size_t jcol = static_cast<std::size_t>(x[i]-1);
      array[jrow+jcol*nrow] = z[i];
    }
    if( qualifiers[wxT("XYOUT")] )
    {
      NumericVariable::PutVariable( xoutName, xi, 1, p->GetInputLine() );
      NumericVariable::PutVariable( youtName, yi, 1, p->GetInputLine() );
    }
    NumericVariable::PutVariable( matrixName, array, nrow, ncol, p->GetInputLine() );
  }
  else if( qualifiers[wxT("PATTERN")] ) // make matrix without interpolation using a full data set
  {
    //
    // GRID\PATTERN X Y Z M
    // GRID\PATTERN\XYOUT X Y Z M XOUT YOUT
    //
    std::size_t ncol, nrow = 1;
    while( x[0] == x[nrow] )++nrow;
    bool invert = false;
    if( nrow == 1 )
    {
      invert = true;
      ncol = 1;
      while( y[0] == y[ncol] )++ncol;
      nrow = num / ncol;
      if( num!=nrow*ncol || nrow==1 || ncol==1 )
        throw ECommandError( command+wxT("non-interpolated matrix is not possible with this data") );
      std::vector<double> yi( nrow );
      for( std::size_t i=0; i<nrow; ++i )yi[i] = y[i*ncol];
      std::vector<double> xi( ncol );
      if( x[1] < x[0] )
      {
        for( std::size_t i=0; i<ncol; ++i )xi[i] = x[ncol-i-1];
        for( std::size_t i=0; i<ncol/2; ++i )
        {
          double xtmp = z[i];
          z[i] = z[ncol-i-1];
          z[ncol-i-1] = xtmp;
        }
      }
      else
      {
        for( std::size_t i=0; i<ncol; ++i )xi[i] = x[i];
      }
      for( std::size_t j=2; j<=nrow; ++j )
      {
        if( x[1+(j-1)*ncol] < x[(j-1)*ncol] )
        {
          for( std::size_t i=0; i<ncol/2; ++i )
          {
            double xtmp = z[i+(j-1)*ncol];
            z[i+(j-1)*ncol] = z[ncol-i-1+(j-1)*ncol];
            z[ncol-i-1+(j-1)*ncol] = xtmp;
          }
        }
      }
    }
    else
    {
      ncol = num / nrow;
      if( num != nrow*ncol )
        throw ECommandError( command+wxT("non-interpolated matrix is not possible with this data") );
      std::vector<double> xi( ncol );
      for( std::size_t i=0; i<ncol; ++i )xi[i] = x[i*nrow];
      std::vector<double> yi( nrow );
      if( y[1] < y[0] )
      {
        for( std::size_t i=0; i<nrow; ++i )yi[i] = y[nrow-i-1];
        for( std::size_t i=0; i<nrow/2; ++i )
        {
          double xtmp = z[i];
          z[i] = z[nrow-i-1];
          z[nrow-i-1] = xtmp;
        }
      }
      else
      {
        for( std::size_t i=0; i<nrow; ++i )yi[i] = y[i];
      }
      for( std::size_t j=2; j<=ncol; ++j )
      {
        if( y[1+(j-1)*nrow] < y[(j-1)*nrow] )
        {
          for( std::size_t i=0; i<nrow/2; ++i )
          {
            double xtmp = z[i+(j-1)*nrow];
            z[i+(j-1)*nrow] = z[nrow-i-1+(j-1)*nrow];
            z[nrow-i-1+(j-1)*nrow] = xtmp;
          }
        }
      }
    }
    std::vector<double> xi, yi;
    wxString xoutName, youtName;
    if( qualifiers[wxT("XYOUT")] )
    {
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting xout vector") );
      xoutName = p->GetString(counter++);
      AddToStackLine( xoutName );
      //
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting yout vector") );
      youtName = p->GetString(counter++);
      AddToStackLine( youtName );
      //
      for( std::size_t i=0; i<nrow; ++i )yi.push_back( static_cast<double>(i+1) );
      for( std::size_t i=0; i<ncol; ++i )xi.push_back( static_cast<double>(i+1) );
    }
    std::vector<double> array( nrow*ncol, 0.0 );
    if( invert )
    {
      std::size_t ii = 0;
      for( std::size_t i=0; i<nrow; ++i )
      {
        for( std::size_t j=0; j<ncol; ++j )array[i+j*nrow] = z[ii++];
      }
    }
    else
    {
      std::size_t ii = 0;
      for( std::size_t j=0; j<ncol; ++j )
      {
        for( std::size_t i=0; i<nrow; ++i )array[i+j*nrow] = z[ii++];
      }
    }
    if( qualifiers[wxT("XYOUT")] )
    {
      NumericVariable::PutVariable( xoutName, xi, 1, p->GetInputLine() );
      NumericVariable::PutVariable( youtName, yi, 1, p->GetInputLine() );
    }
    NumericVariable::PutVariable( matrixName, array, nrow, ncol, p->GetInputLine() );
  }
  else // make matrix using interpolation
  {
    std::size_t ncol = qualifiers[wxT("SIZE")] ? static_cast<std::size_t>(size) :
                                            static_cast<std::size_t>(5.0*sqrt(1.0*num));
    std::size_t nrow = ncol;
    std::vector<double> array( nrow*ncol, 0.0 );
    wxString xoutName, youtName;
    if( qualifiers[wxT("XYOUT")] )
    {
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting xout vector") );
      xoutName = p->GetString(counter++);
      AddToStackLine( xoutName );
      //
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting yout vector") );
      youtName = p->GetString(counter++);
      AddToStackLine( youtName );
    }
    //
    // if the xmin, xmax, ymin, and ymax were not entered as parameters
    // with the command then they will be taken from the minimum
    // and the maximum of the data
    //
    double xmin, xmax, ymin, ymax;
    if( qualifiers[wxT("BOUNDS")] )
    {
      if( p->GetNumberOfTokens() < counter+1 )
        throw ECommandError( command+wxT("expecting xmin") );
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), wxT("xmin"), xmin );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(counter++) );
      //
      if( p->GetNumberOfTokens() < counter+1 )throw ECommandError( command+wxT("expecting xmax") );
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), wxT("xmax"), xmax );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(counter++) );
      //
      if( p->GetNumberOfTokens() < counter+1 )throw ECommandError( command+wxT("expecting ymin") );
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), wxT("ymin"), ymin );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(counter++) );
      //
      if( p->GetNumberOfTokens() < counter+1 )throw ECommandError( command+wxT("expecting ymax") );
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), wxT("ymax"), ymax );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(counter++) );
    }
    else
    {
      xmin = x[0];
      xmax = xmin;
      ymin = y[0];
      ymax = ymin;
      for( std::size_t i=0; i<num; ++i )
      {
        if( xmin > x[i] )xmin = x[i];
        if( xmax < x[i] )xmax = x[i];
        if( ymin > y[i] )ymin = y[i];
        if( ymax < y[i] )ymax = y[i];
      }
    }
    double xinc = (xmax-xmin)/(ncol-1);
    double yinc = (ymax-ymin)/(ncol-1);
    std::vector<double> xi( ncol );
    std::vector<double> yi( ncol );
    for( std::size_t i=0; i<ncol-1; ++i )
    {
      xi[i] = xmin + i*xinc;
      yi[i] = ymin + i*yinc;
    }
    xi[ncol-1] = xmax;
    yi[ncol-1] = ymax;
    //
    try
    {
      GRA_thiessenTriangulation tt( x, y, z );
      tt.CreateMesh();
      double eps = 0.001;
      std::size_t nit = 128;
      tt.Gradients( eps, nit );
      tt.Interpolate( ncol, ncol, xi, yi, array );
    }
    catch (EExpressionError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( qualifiers[wxT("XYOUT")] )
    {
      NumericVariable::PutVariable( xoutName, xi, 1, p->GetInputLine() );
      NumericVariable::PutVariable( youtName, yi, 1, p->GetInputLine() );
    }
    NumericVariable::PutVariable( matrixName, array, ncol, ncol, p->GetInputLine() );
    if( qualifiers[wxT("MESSAGES")] )
    {
      ExGlobals::WriteOutput( wxT("grid completed successfully") );
      wxString s(wxT("matrix "));
      s << matrixName << wxT(" has ") << nrow << wxT(" rows and ") << ncol << wxT(" columns");
      ExGlobals::WriteOutput( s );
    }
  }
}

// end of file
