/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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
#include <string>

#include "wx/datetime.h"

#include "FCN_date.h"
#include "EExpressionError.h"

FCN_date *FCN_date::fcn_date_ = 0;

void FCN_date::TextScalarEval( int j, std::vector<wxString> &sStack ) const
{
  wxDateTime now = wxDateTime::Now();
  if( GetArgumentCounter() == 1 )
  {

    //std::cout << "sStack[" << j << "]=|" << sStack[j].mb_str(wxConvUTF8) << "|\n";

    wxString format, order;
    wxChar seperator;
    int nd=0, nm=0, ny=0;
    try
    {
      order = ParseInputString( sStack[j], seperator, nd, nm, ny );

      //std::cout << "order=|" << order.mb_str(wxConvUTF8) << "|\n";

      AddToFormat( format, order.at(0), nd, nm, ny );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    if( order.size() > 1 )
    {
      format += seperator;
      try
      {
        AddToFormat( format, order.at(1), nd, nm, ny );
      }
      catch (EExpressionError &e)
      {
        throw;
      }
      if( order.size() > 2 )
      {
        format += seperator;
        try
        {
          AddToFormat( format, order.at(2), nd, nm, ny );
        }
        catch (EExpressionError &e)
        {
          throw;
        }
      }
    }
    
    //std::cout << "format=|" << format.mb_str(wxConvUTF8) << "|\n";

    sStack[j] = now.Format( format.c_str() );
  }
  else // no arguments
  {
    sStack.push_back( now.Format(wxT("%d/%m/%Y")) );
  }
}

wxString FCN_date::ParseInputString( wxString const &input_in, wxChar &seperator,
                                     int &nd, int &nm, int &ny ) const
{
  std::string input( input_in.Upper().mb_str(wxConvUTF8) );
  input += '\n';
  //
  // 'D' = 0,  'M' = 1,  'Y' = 2, other = 3, linefeed = 4
  int const classes[128] =
  {
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
  };
  int const stateTable[10][5] =
  {
      // D   M   Y  othr end
      {  1,  2,  3, -1,  -2 }, // 0
      {  4, -3, -4,  7, 100 }, // 1
      { -5,  5, -6,  8, 100 }, // 2
      { -7, -8,  6,  9, 100 }, // 3
      {  4, -3, -4,  7, 100 }, // 4
      { -5,  5, -6,  8, 100 }, // 5
      { -7, -8,  6,  9, 100 }, // 6
      { -9,  2,  3,-12, -13 }, // 7
      {  1,-10,  3,-12, -13 }, // 8
      {  1,  2,-11,-12, -13 }  // 9
  };
  int nsep = 0;
  bool ddone=false, mdone=false, ydone=false;
  int currentState=0, newState=0;
  int i = -1;
  int errNo = 0;
  wxString order;
  for( ;; )
  {
    
    //std::cout << "newState=" << newState << "\n";
    
    switch (newState)
    {
      case 0:
      {
        break;
      }
      case 1:
      {
        if( ddone )currentState = -9;
        else ++nd;
        order += wxT("d");
        break;
      }
      case 2:
      {
        if( mdone )currentState = -10;
        else ++nm;
        order += wxT("m");
        break;
      }
      case 3:
      {
        if( ydone )currentState = -11;
        else ++ny;
        order += wxT("y");
        break;
      }
      case 4:
      {
        ++nd;
        break;
      }
      case 5:
      {
        ++nm;
        break;
      }
      case 6:
      {
        ++ny;
        break;
      }
      case 7:
      {
        if( ++nsep > 2 )currentState = -12;
        else seperator = input.at(i);

        //std::cout << "7: seperator=|" << seperator << "|, " << toascii(seperator) << "\n";

        ddone = true;
        break;
      }
      case 8:
      {
        if( ++nsep > 2 )currentState = -12;
        else seperator = input.at(i);

        //std::cout << "8: seperator=|" << seperator << "|, " << toascii(seperator) << "\n";

        mdone = true;
        break;
      }
      case 9:
      {
        if( ++nsep > 2 )currentState = -12;
        else seperator = input.at(i);

        //std::cout << "9: seperator=|" << seperator << "|, " << toascii(seperator) << "\n";

        ydone = true;
        break;
      }
    }
    if( currentState < 0 )
    {
      errNo = abs(currentState);
      break;
    }
    currentState = newState;
    int iascii = toascii( input.at(++i) );
    newState = stateTable[currentState][classes[iascii]];
    
    //std::cout << "iascii=" << iascii << ", currentState=" << currentState 
    //          << ", newState=" << newState << "\n";

    if( newState == 100 )break;
    if( newState < 0 )
    {
      errNo = abs(newState);
      break;
    }
  }
  switch (errNo)
  {
    case 1:
      throw EExpressionError( wxT("DATE: seperator cannot begin a format") );
    case 2:
      throw EExpressionError( wxT("DATE: blank format") );
    case 3:
      throw EExpressionError( wxT("DATE: m immediately following d") );
    case 4:
      throw EExpressionError( wxT("DATE: y immediately following d") );
    case 5:
      throw EExpressionError( wxT("DATE: d immediately following m") );
    case 6:
      throw EExpressionError( wxT("DATE: y immediately following m") );
    case 7:
      throw EExpressionError( wxT("DATE: d immediately following y") );
    case 8:
      throw EExpressionError( wxT("DATE: m immediately following y") );
    case 9:
      throw EExpressionError( wxT("DATE: only one day specifier section allowed") );
    case 10:
      throw EExpressionError( wxT("DATE: only one month specifier section allowed") );
    case 11:
      throw EExpressionError( wxT("DATE: only one year specifier section allowed") );
    case 12:
      throw EExpressionError( wxT("DATE: seperator must be a single character") );
    case 13:
      throw EExpressionError( wxT("DATE: seperator cannot end a format") );
  }
  return order;
}

void FCN_date::AddToFormat( wxString &format, wxChar c, int nd, int nm, int ny ) const
{
  if( c == wxT('d') )
  {
    switch (nd)
    {
      case 1:
        format += wxT("%#d");
        break;
      case 2:
        format += wxT("%d");
        break;
      case 3:
        format += wxT("%a");
        break;
      case 4:
        format += wxT("%A");
        break;
      default:
        throw EExpressionError( wxT("DATE: valid day indicators are d dd ddd dddd") );
    }
  }
  else if( c == wxT('m') )
  {
    switch (nm)
    {
      case 1:
        format += wxT("%#m");
        break;
      case 2:
        format += wxT("%m");
        break;
      case 3:
        format += wxT("%b");
        break;
      case 4:
        format += wxT("%B");
        break;
      default:
        throw EExpressionError( wxT("DATE: valid month indicators are m mm mmm mmmm") );
    }
  }
  else if( c == wxT('y') )
  {
    switch (ny)
    {
      case 2:
        format += wxT("%y");
        break;
      case 4:
        format += wxT("%Y");
        break;
      default:
        throw EExpressionError( wxT("DATE: valid year indicators are yy yyyy") );
    }
  }
}
