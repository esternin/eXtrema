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

#include "FCN_time.h"
#include "EExpressionError.h"

FCN_time *FCN_time::fcn_time_ = 0;

void FCN_time::TextScalarEval( int j, std::vector<wxString> &sStack ) const
{
  wxDateTime now = wxDateTime::Now();
  if( GetArgumentCounter() == 1 )
  {
    wxString format, order;
    wxChar seperator;
    int nh=0, nm=0, ns=0;
    int ampm=0;  // 0 -> not there, 1 -> lower case, 2 -> upper case
    order = ParseInputString( sStack[j], seperator, nh, nm, ns, ampm );
    AddToFormat( format, order.at(0), nh, nm, ns, ampm );
    if( order.size() > 1 )
    {
      format += seperator;
      AddToFormat( format, order.at(1), nh, nm, ns, ampm );
      if( order.size() > 2 )
      {
        format += seperator;
        AddToFormat( format, order.at(2), nh, nm, ns, ampm );
      }
    }
    if( ampm == 1 )format += wxT(" %P");
    else if( ampm == 2 )format += wxT(" %p");
    sStack[j] = now.Format( format.c_str() );
  }
  else // no arguments
  {
    sStack.push_back( now.Format(wxT("%H:%M:%S")) );
  }
}

wxString FCN_time::ParseInputString( wxString const &input_in, wxChar &seperator,
                                     int &nh, int &nm, int &ns, int &ampm ) const
{
  std::string input( input_in.Upper().mb_str(wxConvUTF8) );

  if( input == "T" )
  {
    ampm = 1;
    nh = 2;
    nm = 2;
    seperator = wxT(':');
    return wxT("hm");
  }
  else if( input == "TT" )
  {
    ampm = 2;
    nh = 2;
    nm = 2;
    ns = 2;
    seperator = wxT(':');
    return wxT("hms");
  }
  int size = input.size();
  if( size>6 )
  {
    if( input.substr(size-6,6)==" AM/PM" )
    {
      ampm = 1;
      input = input.substr(0,size-6);
    }
  }
  input += '\n';
  //
  // 'H' = 0,  'N' = 1,  'S' = 2, other = 3, linefeed = 4
  int const classes[128] =
  {
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 1, 3,
      3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
  };
  int const stateTable[10][5] =
  {
      // H   N   S  othr end
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
  bool hdone=false, mdone=false, sdone=false;
  int currentState=0, newState=0;
  int i = -1;
  int errNo = 0;
  wxString order;
  for( ;; )
  {
    switch (newState)
    {
      case 0:
      {
        break;
      }
      case 1:
      {
        if( hdone )currentState = -9;
        else ++nh;
        order += wxT("h");
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
        if( sdone )currentState = -11;
        else ++ns;
        order += wxT("s");
        break;
      }
      case 4:
      {
        ++nh;
        break;
      }
      case 5:
      {
        ++nm;
        break;
      }
      case 6:
      {
        ++ns;
        break;
      }
      case 7:
      {
        if( ++nsep > 2 )currentState = -12;
        else seperator = input.at(i);
        hdone = true;
        break;
      }
      case 8:
      {
        if( ++nsep > 2 )currentState = -12;
        else seperator = input.at(i);
        mdone = true;
        break;
      }
      case 9:
      {
        if( ++nsep > 2 )currentState = -12;
        else seperator = input.at(i);
        sdone = true;
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
      throw EExpressionError( wxT("TIME: seperator cannot begin a format") );
    case 2:
      throw EExpressionError( wxT("TIME: blank format") );
    case 3:
      throw EExpressionError( wxT("TIME: n immediately following h") );
    case 4:
      throw EExpressionError( wxT("TIME: s immediately following h") );
    case 5:
      throw EExpressionError( wxT("TIME: h immediately following n") );
    case 6:
      throw EExpressionError( wxT("TIME: s immediately following n") );
    case 7:
      throw EExpressionError( wxT("TIME: h immediately following s") );
    case 8:
      throw EExpressionError( wxT("TIME: n immediately following s") );
    case 9:
      throw EExpressionError( wxT("TIME: only one hour specifier section allowed") );
    case 10:
      throw EExpressionError( wxT("TIME: only one minute specifier section allowed") );
    case 11:
      throw EExpressionError( wxT("TIME: only one second specifier section allowed") );
    case 12:
      throw EExpressionError( wxT("TIME: seperator must be a single character") );
    case 13:
      throw EExpressionError( wxT("TIME: seperator cannot end a format") );
  }
  return order;
}

void FCN_time::AddToFormat( wxString &format, wxChar c,
                            int nh, int nm, int ns, int ampm ) const
{
  if( c == wxT('h') )
  {
    switch (nh)
    {
      case 1:
        format += ampm > 0 ? wxT("%#I") : wxT("%#H");
        break;
      case 2:
        format += ampm > 0 ? wxT("%I") : wxT("%H");
        break;
      default:
        throw EExpressionError( wxT("TIME: valid hour indicators are h hh") );
    }
  }
  else if( c == wxT('m') )
  {
    switch (nm)
    {
      case 1:
        format += wxT("%#M");
        break;
      case 2:
        format += wxT("%M");
        break;
      default:
        throw EExpressionError( wxT("TIME: valid minute indicators are m mm") );
    }
  }
  else if( c == wxT('s') )
  {
    switch (ns)
    {
      case 1:
        format += wxT("%#S");
        break;
      case 2:
        format += wxT("%S");
        break;
      default:
        throw EExpressionError( wxT("TIME: valid second indicators are s ss") );
    }
  }
}


