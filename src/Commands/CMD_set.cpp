/*
Copyright (C) 2005,...,2009 Joseph L. Chuma

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
#include <sstream>

#include "CMD_set.h"
#include "ECommandError.h"
#include "ESyntaxError.h"
#include "ParseLine.h"
#include "ParseToken.h"
#include "Script.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"
#include "EGraphicsError.h"
#include "GRA_window.h"
#include "GRA_axis.h"
#include "GRA_colorMap.h"
#include "GRA_colorControl.h"
#include "GRA_color.h"
#include "GRA_fontControl.h"
#include "GRA_font.h"
#include "CMD_read.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "VisualizationWindow.h"
#include "GraphicsPage.h"

CMD_set *CMD_set::cmd_set_ = 0;

CMD_set::CMD_set() : Command( wxT("SET") )
{}

double CMD_set::GetValue( wxString const &keyword )
{
  ParseLine const *p = GetParseLine();
  wxString command( Name()+wxT(": ") );
  command += keyword;
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    double d;
    NumericVariable::GetScalar( p->GetString(valueIndex_), keyword.c_str(), d );
    return d;
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
}

GRA_color *CMD_set::GetColor( wxString const &keyword )
{
  ParseLine const *p = GetParseLine();
  wxString colorName;
  wxString command( Name()+wxT(": ") );
  command += keyword + wxT(": ");
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    TextVariable::GetVariable( p->GetString(valueIndex_), true, colorName );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  GRA_color *color = GRA_colorControl::GetColor( colorName );
  if( color )return color;
  double d;
  try
  {
    NumericVariable::GetScalar( p->GetString(valueIndex_), wxT(""), d );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  return GRA_colorControl::GetColor( static_cast<int>(d) );
}

GRA_font *CMD_set::GetFont( wxString const &keyword )
{
  ParseLine const *p = GetParseLine();
  wxString fontName;
  wxString command( Name()+wxT(": ") );
  command += keyword;
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    TextVariable::GetVariable( p->GetString(valueIndex_), true, fontName );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  return GRA_fontControl::GetFont( fontName );
}

wxString CMD_set::GetString( wxString const &keyword )
{
  ParseLine const *p = GetParseLine();
  wxString label;
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    TextVariable::GetVariable( p->GetString(valueIndex_), false, label );
  }
  catch (EVariableError &e)
  {
    wxString command( Name()+wxT(": ") );
    command += keyword;
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  return label;
}

void CMD_set::Execute( ParseLine const *p_in )
{
  ParseLine p( *p_in );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  SetUp( &p );
  //
  wxString command( Name()+wxT(": ") );
  if( p.GetNumberOfTokens()>=2 && !p.IsString(1) )
    throw ECommandError( command+wxT("expecting keyword") );
  bool multipleEntries = false;
  if( p.GetNumberOfTokens() == 1 )
  {
    if( ExGlobals::NotInaScript() )throw ECommandError( command+wxT("expecting keyword") );
    multipleEntries = true;
    WriteStackLine();
  }
  //
TOP:
  if( multipleEntries )
  {
    wxString line;
NEXTLINE:
    try
    {
      if( ExGlobals::GetScript()->GetNextLine(line) ) // end of script file found
      {
        ExGlobals::StopScript();
        return;
      }
    }
    catch( std::runtime_error &e )
    {
      ExGlobals::StopAllScripts();
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( line.empty() || line==wxT(" ") )return;
    if( line.at(0) == Script::GetCommentCharacter() )goto NEXTLINE;
    line = wxString(wxT("SET "))+line;
    ParseLine p2( line );
    try
    {
      p2.ParseIt();
    }
    catch( ESyntaxError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    p = p2;
  }
  AddToStackLine( p.GetString(1) );
  //
  wxString temp( p.GetString(1).Upper() );
  //
  double value;
  bool percent = (temp.at(0) == wxT('%'));
  wxString keyword;
  percent ? keyword.assign( temp,1,temp.length()-1 ) :
            keyword.assign( temp,0,temp.length() );
  valueIndex_ = 2;
  //command += keyword;
  if( p.GetNumberOfTokens()<3 && keyword!=wxT("LEGENDFRAME") )
    throw ECommandError( command+wxT("expecting value") );
  if( p.IsEqualSign(2) )
  {
    valueIndex_ = 3;
    if( p.GetNumberOfTokens() < 4 )throw ECommandError( command+wxT("expecting value") );
  }
  if( keyword==wxT("PLOTSYMBOL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GRA_intCharacteristic *plotsymbol =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOL")));
    if( ndim == 0 )
    {
      plotsymbol->Set( static_cast<int>(dvalue) );
    }
    else if( ndim == 1 )
    {
      std::vector<int> idata;
      std::vector<double>::const_iterator dEnd = data.end();
      for( std::vector<double>::const_iterator i=data.begin(); i!=dEnd; ++i )
        idata.push_back( static_cast<int>(*i) );
      plotsymbol->Set( idata );
    }
    else
    {
      throw ECommandError( command+wxT("scalar or vector expected") );
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("PLOTSYMBOLSIZE") || keyword==wxT("CHARSZ") )
  {
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GRA_sizeCharacteristic *plotsymbolsize =
        static_cast<GRA_sizeCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLSIZE")));
    if( ndim == 0 )
    {
      plotsymbolsize->Set( dvalue, percent );
    }
    else if( ndim == 1 )
    {
      plotsymbolsize->Set( data, percent );
    }
    else
    {
      throw ECommandError( command+wxT("must be scalar or vector") );
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("PLOTSYMBOLANGLE") || keyword==wxT("CHARA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GRA_angleCharacteristic *plotsymbolangle =
        static_cast<GRA_angleCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLANGLE")));
    if( ndim == 0 )
    {
      plotsymbolangle->Set( dvalue );
    }
    else if( ndim == 1 )
    {
      plotsymbolangle->Set( data );
    }
    else
    {
      throw ECommandError( command+wxT("must be scalar or vector") );
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("PLOTSYMBOLLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GRA_intCharacteristic *plotsymbollinewidth =
        static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLLINEWIDTH")));
    if( ndim == 0 )
    {
      plotsymbollinewidth->Set( static_cast<int>(dvalue) );
    }
    else if( ndim == 1 )
    {
      std::vector<int> idata;
      std::vector<double>::const_iterator dEnd( data.end() );
      for( std::vector<double>::const_iterator i=data.begin(); i!=dEnd; ++i )
        idata.push_back( static_cast<int>(*i) );
      plotsymbollinewidth->Set( idata );
    }
    else
    {
      throw ECommandError( command+wxT("must be scalar or vector") );
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("PLOTSYMBOLCOLOR") || keyword==wxT("PLOTSYMBOLCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    wxString colorName;
    try
    {
      TextVariable::GetVariable( p.GetString(valueIndex_), true, colorName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GRA_colorCharacteristic *plotsymbolcolor =
        static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLCOLOR")));
    //
    // first check to see if it is a named color, such as RED
    //
    GRA_color *color = GRA_colorControl::GetColor(colorName);
    if( color ) // it was a recognized name
    {
      plotsymbolcolor->Set( color );
    }
    else             // it was not a recognized name
    {
      int ndim;
      double dvalue;
      std::vector<double> data;
      int dimSizes[3];
      try
      {
        NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndim == 0 )      // scalar
      {
        plotsymbolcolor->Set( GRA_colorControl::GetColor( static_cast<int>(dvalue) ) );
      }
      else if( ndim == 1 ) // vector
      {
        std::vector<GRA_color*> colors;
        for( int i=0; i<dimSizes[0]; ++i )
          colors.push_back( GRA_colorControl::GetColor( static_cast<int>(data[i]) ) );
        plotsymbolcolor->Set( colors );
      }
      else
      {
        throw ECommandError( command+wxT("expecting scalar or vector") );
      }
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("PCHAR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GRA_intCharacteristic *plotsymbol =
        static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOL")));
    if( ndim == 0 )
    {
      plotsymbol->Set( static_cast<int>(dvalue) );
    }
    else if( ndim == 1 )
    {
      std::vector<int> idata;
      std::vector<double>::const_iterator end( data.end() );
      for( std::vector<double>::const_iterator i=data.begin(); i!=end; ++i )
        idata.push_back( static_cast<int>(*i) );
      plotsymbol->Set( idata );
    }
    else
    {
      throw ECommandError( command+wxT("scalar or vector expected") );
    }
    pcharSymbol_ = p.GetString(valueIndex_);
    AddToStackLine( p.GetString(valueIndex_) );
    pcharSize_.erase();
    pcharColor_.erase();
    pcharAngle_.erase();
    if( p.GetNumberOfTokens() >= valueIndex_+2 )
    {
      try
      {
        std::vector<double>().swap( data );
        NumericVariable::GetVariable( p.GetString(valueIndex_+1), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      GRA_sizeCharacteristic *plotsymbolsize =
          static_cast<GRA_sizeCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLSIZE")));
      double oldValue = plotsymbolsize->GetAsWorld();
      if( ndim == 0 )
      {
        plotsymbolsize->SetAsWorld( dvalue*oldValue );
      }
      else if( ndim == 1 )
      {
        std::size_t dataSize = data.size();
        for( std::size_t i=0; i<dataSize; ++i )data[i] *= oldValue;
        plotsymbolsize->SetAsWorld( data );
      }
      else
      {
        throw ECommandError( command+wxT("scalar or vector expected for size scale factor") );
      }
      pcharSize_ = p.GetString(valueIndex_+1);
      AddToStackLine( p.GetString(valueIndex_+1) );
      if( p.GetNumberOfTokens() >= valueIndex_+3 )
      {
        try
        {
          std::vector<double>().swap( data );
          NumericVariable::GetVariable( p.GetString(valueIndex_+2), ndim, dvalue, data, dimSizes );
        }
        catch (EVariableError &e)
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        GRA_colorCharacteristic *plotsymbolcolor =
            static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLCOLOR")));
        if( ndim == 0 )
        {
          plotsymbolcolor->Set( GRA_colorControl::GetColor( static_cast<int>(dvalue) ) );
        }
        else if( ndim == 1 )
        {
          std::vector<GRA_color*> colors;
          for( int i=0; i<dimSizes[0]; ++i )
            colors.push_back( GRA_colorControl::GetColor( static_cast<int>(data[i]) ) );
          plotsymbolcolor->Set( colors );
        }
        else
        {
          throw ECommandError( command+wxT("scalar or vector expected for color") );
        }
        pcharColor_ = p.GetString(valueIndex_+2);
        AddToStackLine( p.GetString(valueIndex_+2) );
        if( p.GetNumberOfTokens() >= valueIndex_+4 )
        {
          try
          {
            std::vector<double>().swap( data );
            NumericVariable::GetVariable( p.GetString(valueIndex_+3), ndim, dvalue, data, dimSizes );
          }
          catch (EVariableError &e)
          {
            throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
          }
          GRA_angleCharacteristic *plotsymbolangle =
              static_cast<GRA_angleCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLANGLE")));
          if( ndim == 0 )
          {
            plotsymbolangle->Set( dvalue );
          }
          else if( ndim == 1 )
          {
            plotsymbolangle->Set( data );
          }
          else
          {
            throw ECommandError( command+wxT("scalar or vector expected for angles") );
          }
          pcharAngle_ = p.GetString(valueIndex_+3);
          AddToStackLine( p.GetString(valueIndex_+3) );
        }
      }
    }
  }
  else if( keyword==wxT("HISTOGRAMTYPE") || keyword==wxT("HISTYP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("HISTOGRAMTYPE")))->
        Set(static_cast<int>(GetValue(keyword)));
  }
  else if( keyword==wxT("COLOR") || keyword==wxT("COLOUR") )
  {
    if( percent )throw ECommandError( command+wxT(" % is meaningless") );
    GRA_color *color = GetColor( keyword );
    static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLCOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVECOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("AXISCOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("AXISCOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELCOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELCOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSCOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSCOLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("COLOR")))->
        Set( color );
    static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLECOLOR")))->
        Set( color );
  }
  else if( keyword==wxT("BACKGROUNDCOLOR") || keyword==wxT("BACKGROUNDCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT(" % is meaningless") );
    wxString colorName;
    try
    {
      TextVariable::GetVariable( p.GetString(valueIndex_), true, colorName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    //
    // first check to see if it is a named color, such as RED
    //
    GRA_color *bgcolor = GRA_colorControl::GetColor( colorName );
    if( bgcolor )      // it was a recognized name
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDCOLOR")))->
        Set( bgcolor );
      static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDFILE")))->
        Set( wxString(wxT("")) );
      //
      GraphicsPage *page = ExGlobals::GetGraphicsPage();
      GraphicsDC dc( page );
      page->EraseWindows();
      page->DrawGraphWindows( ExGlobals::GetGraphicsOutput(), dc );
    }
    else             // it was not a recognized name
    {
      int ndim;
      double dvalue;
      std::vector<double> data;
      int dimSizes[3];
      try
      {
        NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndim == 0 )      // scalar
      {
        int ivalue = static_cast<int>(dvalue);
        GRA_colorCharacteristic *bgchar =
          static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDCOLOR")));
        ivalue==0 ? bgchar->Set(reinterpret_cast<GRA_color*>(0)) :
                    bgchar->Set(GRA_colorControl::GetColor(ivalue));
        static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDFILE")))->
          Set( wxString(wxT("")) );
        //
        GraphicsPage *page = ExGlobals::GetGraphicsPage();
        GraphicsDC dc( page );
        page->EraseWindows();
        page->DrawGraphWindows( ExGlobals::GetGraphicsOutput(), dc );
      }
      else
      {
        throw ECommandError( command+wxT("expecting scalar value") );
      }
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("BACKGROUNDFILE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    try
    {
      wxString bgfile( GetString(keyword) );
      if( bgfile == wxT(" ") )
      {
        static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDFILE")))->
          Set( wxString(wxT("")) );
        static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDCOLOR")))->
          Set( reinterpret_cast<GRA_color*>(0) );
      }
      else
      {
        std::ifstream inStream;
        inStream.clear( std::ios::goodbit );
        inStream.open( bgfile.mb_str(wxConvUTF8), std::ios_base::in );
        if( !inStream.is_open() )throw ECommandError( command+wxT("could not open file: ")+bgfile );
        inStream.close();
        static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDFILE")))->
          Set( bgfile );
        static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("BACKGROUNDCOLOR")))->
          Set( reinterpret_cast<GRA_color*>(0) );
      }
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GraphicsPage *page = ExGlobals::GetGraphicsPage();
    GraphicsDC dc( page );
    page->EraseWindows();
    page->DrawGraphWindows( ExGlobals::GetGraphicsOutput(), dc );
  }
  else if( keyword==wxT("COLORMAPFILE") || keyword==wxT("COLOURMAPFILE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    wxString fileName;
    try
    {
      TextVariable::GetVariable( p.GetString(valueIndex_), true, fileName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    try
    {
      GRA_colorControl::SetFile( fileName );
    }
    catch (EGraphicsError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("COLORMAP") || keyword==wxT("COLOURMAP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    if( p.GetNumberOfTokens() == valueIndex_+1 )
    {
      wxString name;
      try
      {
        TextVariable::GetVariable( p.GetString(valueIndex_), true, name );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      try
      {
        GRA_colorControl::SetColorMap( name );
      }
      catch (EGraphicsError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p.GetString(valueIndex_) );
    }
    else
    {
      if( p.GetNumberOfTokens() != valueIndex_+3 )throw ECommandError( command+wxT("expecting 3 vectors") );
      int ndim1, ndim2, ndim3;
      double dvalue1, dvalue2, dvalue3;
      std::vector<double> data1;
      std::vector<double> data2;
      std::vector<double> data3;
      int dimSizes1[3];
      int dimSizes2[3];
      int dimSizes3[3];
      try
      {
        NumericVariable::GetVariable( p.GetString(valueIndex_), ndim1, dvalue1, data1, dimSizes1 );
        NumericVariable::GetVariable( p.GetString(valueIndex_+1), ndim2, dvalue2, data2, dimSizes2 );
        NumericVariable::GetVariable( p.GetString(valueIndex_+2), ndim3, dvalue3, data3, dimSizes3 );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndim1!=1 || ndim2!=1 || ndim3!=1 )
        throw ECommandError( command+wxT("must be 3 vectors") );
      if( data1.size()!=data2.size() || data2.size()!=data3.size() )
        throw ECommandError( command+wxT("R, G, and B vectors must be the same size") );
      std::size_t size = data1.size();
      GRA_colorMap *tmp2 = new GRA_colorMap(wxT("USERDEFINED"));
      for( std::size_t i=0; i<size; ++i )
      {
        tmp2->AddColor( new GRA_color( static_cast<int>(data1[i]),
                                       static_cast<int>(data2[i]),
                                       static_cast<int>(data3[i]) ) );
      }
      GRA_colorControl::SetColorMapUser( tmp2 );
      AddToStackLine( p.GetString(valueIndex_)+wxString(wxT(" "))+
                      p.GetString(valueIndex_+1)+wxString(wxT(" "))+
                      p.GetString(valueIndex_+2) );
    }
  }
  else if( keyword==wxT("CURVECOLOR") || keyword==wxT("CURVECOLOUR") ||
           keyword==wxT("LINECOLOR")  || keyword==wxT("LINECOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVECOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("AREAFILLCOLOR") || keyword==wxT("AREAFILLCOLOUR") ||
           keyword==wxT("FILLCOLOR") || keyword==wxT("FILLCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    wxString colorName;
    try
    {
      TextVariable::GetVariable( p.GetString(valueIndex_), true, colorName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    GRA_colorCharacteristic *areafillcolor =
      static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("AREAFILLCOLOR")));
    //
    // first check to see if it is a named color, such as RED
    //
    GRA_color *color = GRA_colorControl::GetColor( colorName );
    if( color )      // it was a recognized name
    {
      areafillcolor->Set( color );
    }
    else             // it was not a recognized name
    {
      int ndim;
      double dvalue;
      std::vector<double> data;
      int dimSizes[3];
      try
      {
        NumericVariable::GetVariable( p.GetString(valueIndex_), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndim == 0 )      // scalar
      {
        int ivalue = static_cast<int>(dvalue);
        ivalue==0 ? areafillcolor->Set(reinterpret_cast<GRA_color*>(0)) :
                    areafillcolor->Set(GRA_colorControl::GetColor(ivalue));
      }
      else if( ndim == 1 ) // vector
      {
        std::vector<GRA_color*> colors;
        for( int i=0; i<dimSizes[0]; ++i )colors.push_back( GRA_colorControl::GetColor( static_cast<int>(data[i]) ) );
        areafillcolor->Set( colors );
      }
      else
      {
        throw ECommandError( command+wxT("expecting scalar or vector") );
      }
    }
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("LINETYPE") || keyword==wxT("LINTYP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    static_cast<GRA_intCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("GRIDLINETYPE")))->
        Set( static_cast<int>(value) );
    static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVELINETYPE")))->
        Set( static_cast<int>(value) );
  }
  else if( keyword==wxT("CURVELINETYPE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVELINETYPE")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("GRIDLINETYPE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("GRIDLINETYPE")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("LINEWIDTH") || keyword==wxT("LINTHK") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    int w = static_cast<int>(value+0.5);
    static_cast<GRA_intCharacteristic*>(
      gw->GetGeneralCharacteristics()->Get(wxT("LINEWIDTH")))->Set( w );
    static_cast<GRA_intCharacteristic*>(
      gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLLINEWIDTH")))->Set( w );
    static_cast<GRA_intCharacteristic*>(
      gw->GetDataCurveCharacteristics()->Get(wxT("CURVELINEWIDTH")))->Set( w );
  }
  else if( keyword==wxT("CURVELINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVELINEWIDTH")))->
        Set(static_cast<int>(GetValue(keyword)+0.5));
  }
  else if( keyword==wxT("XAXISLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LINEWIDTH")))->
        Set(static_cast<int>(GetValue(keyword)+0.5));
  }
  else if( keyword==wxT("YAXISLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LINEWIDTH")))->
        Set(static_cast<int>(GetValue(keyword)+0.5));
  }
  else if( keyword==wxT("POLARAXISLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LINEWIDTH")))->
        Set(static_cast<int>(GetValue(keyword)+0.5));
  }
  else if( keyword==wxT("XAXISCOLOR") || keyword==wxT("XAXISCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("AXISCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("YAXISCOLOR") || keyword==wxT("YAXISCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("AXISCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("POLARAXISCOLOR") || keyword==wxT("POLARAXISCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("AXISCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("XLABELFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELFONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("YLABELFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELFONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("POLARLABELFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELFONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("XLABELCOLOR") || keyword==wxT("XLABELCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("YLABELCOLOR") || keyword==wxT("YLABELCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("POLARLABELCOLOR") || keyword==wxT("POLARLABELCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("XNUMBERSFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSFONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("YNUMBERSFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSFONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("POLARNUMBERSFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSFONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("XNUMBERSCOLOR") || keyword==wxT("XNUMBERSCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("YNUMBERSCOLOR") || keyword==wxT("YNUMBERSCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("POLARNUMBERSCOLOR") || keyword==wxT("POLARNUMBERSCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSCOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword==wxT("POLARORIGINX") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("XORIGIN")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARORIGINY") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("YORIGIN")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARNAXES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NAXES")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("POLARAXISLENGTH") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("AXISLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARAXISANGLE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("AXISANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("POLARCLOCKWISE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("CLOCKWISE")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("POLARCOMPASSLABELS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("COMPASSLABELS")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("XLOWERAXIS") || keyword==wxT("XLAXIS") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOWERAXIS")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("XUPPERAXIS") || keyword==wxT("XUAXIS") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("UPPERAXIS")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YLOWERAXIS") || keyword==wxT("YLAXIS") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOWERAXIS")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YUPPERAXIS") || keyword==wxT("YUAXIS") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("UPPERAXIS")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("XLOWERWINDOW") || keyword==wxT("XLWIND") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("XLOWERWINDOW")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("XUPPERWINDOW") || keyword==wxT("XUWIND") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("XUPPERWINDOW")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YLOWERWINDOW") || keyword==wxT("YLWIND") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("YLOWERWINDOW")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YUPPERWINDOW") || keyword==wxT("YUWIND") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("YUPPERWINDOW")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("GRAPHBOX") || keyword==wxT("BOX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("GRAPHBOX")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XNUMBERSON") || keyword == wxT("XNUMBERS"))
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YNUMBERSON") || keyword == wxT("YNUMBERS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("POLARNUMBERSON") || keyword == wxT("POLARNUMBERS"))
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("XNUMBEROFDIGITS") || keyword==wxT("NXDIG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBEROFDIGITS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("YNUMBEROFDIGITS") || keyword==wxT("NYDIG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBEROFDIGITS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("POLARNUMBEROFDIGITS") || keyword==wxT("NPDIG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBEROFDIGITS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("XNUMBEROFDECIMALS") || keyword==wxT("NXDEC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBEROFDECIMALS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("YNUMBEROFDECIMALS") || keyword==wxT("NYDEC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBEROFDECIMALS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("POLARNUMBEROFDECIMALS") || keyword==wxT("NPDEC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBEROFDECIMALS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("XNUMBERSHEIGHT") || keyword==wxT("XNUMSZ") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSHEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YNUMBERSHEIGHT") || keyword==wxT("YNUMSZ") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSHEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARNUMBERSHEIGHT") || keyword==wxT("POLARNUMSZ") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSHEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("XIMAGTICLENGTH") || keyword==wxT("XITICL") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("IMAGTICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YIMAGTICLENGTH") || keyword==wxT("YITICL") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("IMAGTICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARIMAGTICLENGTH") || keyword==wxT("PITICL") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("IMAGTICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("XIMAGTICANGLE") || keyword==wxT("XITICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("IMAGTICANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YIMAGTICANGLE") || keyword==wxT("YITICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("IMAGTICANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("POLARIMAGTICANGLE") || keyword==wxT("PITICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("IMAGTICANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XNUMBERSANGLE") || keyword==wxT("XNUMA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YNUMBERSANGLE") || keyword==wxT("YNUMA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("POLARNUMBERSANGLE") || keyword==wxT("PNUMA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XPOWER") || keyword==wxT("XPOW") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("POWER")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YPOWER") || keyword==wxT("YPOW") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("POWER")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("POLARPOWER") || keyword==wxT("PPOW") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("POWER")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XPOWERAUTO") || keyword==wxT("XPAUTO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("POWERAUTO")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("YPOWERAUTO") || keyword==wxT("YPAUTO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("POWERAUTO")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("POLARPOWERAUTO") || keyword==wxT("PPAUTO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("POWERAUTO")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XLABEL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABEL")))->
          Set( GetString(keyword) );
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELON")))->
          Set( true );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  else if( keyword == wxT("YLABEL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABEL")))->
          Set( GetString(keyword) );
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELON")))->
          Set( true );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  else if( keyword == wxT("POLARLABEL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABEL")))->
          Set( GetString(keyword) );
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELON")))->
          Set( true );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  else if( keyword==wxT("XLABELHEIGHT") || keyword==wxT("XLABSZ") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELHEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YLABELHEIGHT") || keyword==wxT("YLABSZ") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELHEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARLABELHEIGHT") || keyword==wxT("PLABSZ") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELHEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword == wxT("XLABELON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YLABELON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("POLARLABELON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("XLARGETICLENGTH") || keyword==wxT("XTICL") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LARGETICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YLARGETICLENGTH") || keyword==wxT("YTICL") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LARGETICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARLARGETICLENGTH") || keyword==wxT("PTICL") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LARGETICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("XSMALLTICLENGTH") || keyword==wxT("XTICS") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("SMALLTICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YSMALLTICLENGTH") || keyword==wxT("YTICS") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("SMALLTICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("POLARSMALLTICLENGTH") || keyword==wxT("PTICS") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("SMALLTICLENGTH")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("XTICANGLE") || keyword==wxT("XTICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("TICANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YTICANGLE") || keyword==wxT("YTICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("TICANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("POLARTICANGLE") || keyword==wxT("PTICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("TICANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XFORCECROSS") || keyword==wxT("XCROSS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("FORCECROSS")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("YFORCECROSS") || keyword==wxT("YCROSS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("FORCECROSS")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    double temp;
    temp = GetValue(keyword);
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("MIN")))->Set(temp);
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("VIRTUALMIN")))->Set(temp);
  }
  else if( keyword == wxT("YMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    double temp;
    temp = GetValue(keyword);
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("MIN")))->Set(temp);
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("VIRTUALMIN")))->Set(temp);
  }
  else if( keyword == wxT("XMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    double temp;
    temp = GetValue(keyword);
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("MAX")))->Set(temp);
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("VIRTUALMAX")))->Set(temp);
  }
  else if( keyword == wxT("YMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    double temp;
    temp = GetValue(keyword);
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("MAX")))->Set(temp);
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("VIRTUALMAX")))->Set(temp);
  }
  else if( keyword == wxT("POLARMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    double temp;
    temp = GetValue(keyword);
    static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("MAX")))->Set(temp);
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("VIRTUALMAX")))->Set(temp);
  }
  else if( keyword==wxT("XVIRTUALMIN") || keyword==wxT("XVMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("VIRTUALMIN")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YVIRTUALMIN") || keyword==wxT("YVMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("VIRTUALMIN")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XVIRTUALMAX") || keyword==wxT("XVMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("VIRTUALMAX")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YVIRTUALMAX") || keyword==wxT("YVMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("VIRTUALMAX")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("POLARVIRTUALMAX") || keyword==wxT("PVMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("VIRTUALMAX")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XNLINCS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NLINCS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("NLXINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    if( value < 0.0 )
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("DROPFIRSTNUMBER")))->
          Set( true );
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("DROPLASTNUMBER")))->
          Set( true );
      value *= -1;
    }
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NLINCS")))->
        Set( static_cast<int>(value) );
  }
  else if( keyword==wxT("YNLINCS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NLINCS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("NLYINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    if( value < 0.0 )
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("DROPFIRSTNUMBER")))->
          Set( true );
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("DROPLASTNUMBER")))->
          Set( true );
      value *= -1;
    }
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NLINCS")))->
        Set( static_cast<int>(value) );
  }
  else if( keyword==wxT("POLARNLINCS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NLINCS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("XNSINCS") || keyword==wxT("NSXINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NSINCS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("YNSINCS") || keyword==wxT("NSYINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NSINCS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("POLARNSINCS") || keyword==wxT("NSPINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NSINCS")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword == wxT("XTICSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("TICSON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YTICSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("TICSON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("POLARTICSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("TICSON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XTICSBOTHSIDES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("TICSBOTHSIDES")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YTICSBOTHSIDES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("TICSBOTHSIDES")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("POLARTICSBOTHSIDES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("TICSBOTHSIDES")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XTICTP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    bool ls = (static_cast<int>(value) == 2);
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("TICSBOTHSIDES")))->
        Set( ls );
  }
  else if( keyword == wxT("YTICTP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    bool ls = (static_cast<int>(value) == 2);
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("TICSBOTHSIDES")))->
        Set( ls );
  }
  else if( keyword == wxT("XAXISON") || keyword == wxT("XAXIS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("AXISON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YAXISON") || keyword == wxT("YAXIS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("AXISON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("POLARAXISON") || keyword == wxT("PAXIS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("AXISON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("XGRID") || keyword==wxT("NXGRID") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("GRID")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("YGRID") || keyword==wxT("NYGRID") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("GRID")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("POLARGRID") || keyword==wxT("NPGRID") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("GRID")))->
        Set( static_cast<int>(GetValue(keyword)) );
  }
  else if( keyword==wxT("XAXISANGLE") || keyword==wxT("XAXISA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("AXISANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YAXISANGLE") || keyword==wxT("YAXISA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("AXISANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XLOGBASE") || keyword==wxT("XLOG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOGBASE")))->
        Set( value );
    //gw->GetReplot()->SetXLogBase( value );
  }
  else if( keyword==wxT("YLOGBASE") || keyword==wxT("YLOG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOGBASE")))->
        Set( value );
    //gw->GetReplot()->SetYLogBase( value );
  }
  else if( keyword == wxT("XLOGSTYLE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOGSTYLE")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YLOGSTYLE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOGSTYLE")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XZERO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("ZERO")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YZERO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("ZERO")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XMOD") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("MOD")))->
        Set( GetValue(keyword) );
  }
  else if( keyword == wxT("YMOD") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("MOD")))->
        Set( GetValue(keyword) );
  }
  else if( keyword == wxT("POLARMOD") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("MOD")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XLEADINGZEROS") || keyword==wxT("XLEADZ") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LEADINGZEROS")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("YLEADINGZEROS") || keyword==wxT("YLEADZ") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LEADINGZEROS")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("POLARLEADINGZEROS") || keyword==wxT("PLEADZ") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LEADINGZEROS")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("XOFFSET") || keyword==wxT("XOFF") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("OFFSET")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("YOFFSET") || keyword==wxT("YOFF") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("OFFSET")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("POLAROFFSET") || keyword==wxT("POFF") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("OFFSET")))->
        Set( GetValue(keyword) );
  }
  else if( keyword == wxT("XDROPFIRSTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("DROPFIRSTNUMBER")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YDROPFIRSTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("DROPFIRSTNUMBER")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("POLARDROPFIRSTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("DROPFIRSTNUMBER")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("XDROPLASTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("DROPLASTNUMBER")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("YDROPLASTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("DROPLASTNUMBER")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("POLARDROPLASTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("DROPLASTNUMBER")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("EXTENSION") || keyword==wxT("SCRIPTEXTENSION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    if( !p.IsString(valueIndex_) )throw ECommandError( command+wxT("expecting string value") );
    ExGlobals::SetScriptExtension( p.GetString(valueIndex_) );
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword==wxT("STACKEXTENSION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    if( !p.IsString(valueIndex_) )throw ECommandError( command+wxT("expecting string value") );
    ExGlobals::SetStackExtension( p.GetString(valueIndex_) );
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword == wxT("COMMENT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    if( !p.IsString(valueIndex_) )throw ECommandError( command+wxT("expecting string value") );
    Script::SetCommentCharacter( p.GetString(valueIndex_).at(0) );
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword == wxT("PARAMETER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    if( !p.IsString(valueIndex_) )throw ECommandError( command+wxT("expecting string value") );
    Script::SetParameterCharacter( p.GetString(valueIndex_).at(0) );
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword == wxT("ERRORFILL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    CMD_read::Instance()->SetErrorFill( GetValue(keyword) );
  }
  else if( keyword == wxT("ORIENTATION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    if( !p.IsString(valueIndex_) )throw ECommandError( command+wxT("expecting string value") );
    wxString orient( p.GetString(valueIndex_) );
    orient.UpperCase();
    //
    if( orient != wxString(wxT("LANDSCAPE")) && orient != wxString(wxT("PORTRAIT")) )
      throw ECommandError( command+wxT("expecting LANDSCAPE or PORTRAIT") );
    AddToStackLine( p.GetString(valueIndex_) );
    double ar = ExGlobals::GetAspectRatio();
    bool landscape = (ar < 1.0);
    if( orient == wxT("LANDSCAPE") )
    {
      if( !landscape )ExGlobals::SetAspectRatio( 1./ar );
    }
    else if( orient == wxT("PORTRAIT") )
    {
      if( landscape )ExGlobals::SetAspectRatio( 1./ar );
    }
  }
  else if( keyword==wxT("FONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    GRA_font *font;
    font = GetFont(keyword);
    static_cast<GRA_fontCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("FONT")))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELFONT")))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELFONT")))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSFONT")))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSFONT")))->Set(font);
  }
  else if( keyword==wxT("TEXTFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("FONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("TEXTCOLOR") || keyword==wxT("TEXTCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("COLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword == wxT("TEXTINTERACTIVE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("INTERACTIVE")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword==wxT("TEXTALIGN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    int tj = static_cast<int>(value);
    if( tj<1 || tj>12 )throw ECommandError( command+wxT("TEXTALIGN must be <= 12 and >= 1") );
    static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("ALIGNMENT")))->Set( tj );
  }
  else if( keyword==wxT("CURSOR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    int tj = static_cast<int>(value);
    if( abs(tj)<1 || abs(tj)>12 )throw ECommandError( command+wxT("necessary condition: 1 <= |CURSOR| <= 12") );
    if( tj < 0 )
    {
      tj *= -1;
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("INTERACTIVE")))->
          Set( false );
    }
    else
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("INTERACTIVE")))->
          Set( true );
    }
    static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("ALIGNMENT")))->Set( tj );
  }
  else if( keyword==wxT("TEXTHEIGHT") || keyword==wxT("TXTHIT") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("HEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("TEXTANGLE") || keyword==wxT("TXTANG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_angleCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("ANGLE")))->
        Set( GetValue(keyword) );
  }
  else if( keyword==wxT("XTEXTLOCATION") || keyword==wxT("XLOC") )
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("XLOCATION")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("YTEXTLOCATION") || keyword==wxT("YLOC"))
  {
    static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("YLOCATION")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword == wxT("WINDOWSIZE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = fabs( GetValue( keyword ) );
    if( value <= 0.0001 )
    {
      ExGlobals::DefaultSize();
    }
    else
    {
      value = std::min(2.0,value);
      ExGlobals::ChangeSize( value );
    }
  }
  else if( keyword == wxT("ARROWHEADWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("ARROWHEADWIDTH")))->
        Set( GetValue(keyword) );
  }
  else if( keyword == wxT("ARROWHEADLENGTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_doubleCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("ARROWHEADLENGTH")))->
        Set( GetValue(keyword) );
  }
  else if( keyword == wxT("ASPECTRATIO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = fabs( GetValue( keyword ) );
    ExGlobals::SetAspectRatio( value );
    ExGlobals::GetVisualizationWindow()->ResetPages();
    ExGlobals::GetVisualizationWindow()->ResetWindows();
    //ExGlobals::GetVisualizationWindow()->DrawGraphWindows( ExGlobals::GetGraphicsOutput(), dc );
  }
  else if( keyword == wxT("AUTOSCALE") )
  {
    // autoscale =  0  means OFF
    //           = -1  means COMMENSURATE
    //           =  1  means ON,  = 4  means ON \VIRTUAL
    //           =  2  means X,   = 5  means X \VIRTUAL
    //           =  3  means Y,   = 6  means Y \VIRTUAL
    //
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    wxString name;
    try
    {
      TextVariable::GetVariable( p.GetString(valueIndex_), true, name );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    name.UpperCase();
    //
    bool isVirtual = false;
    ParseToken *t = p.GetToken(valueIndex_);
    if( t->GetNumberOfQualifiers() > 0 )
    {
      if( t->GetQualifier(0)->find(wxT("VIRTUAL")) == 0 )
      {
        isVirtual = true;
      }
      else
      {
        throw ECommandError( command+wxT("VIRTUAL is the only valid qualifier") );
      }
    }
    GRA_stringCharacteristic *autoscale =
      static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("AUTOSCALE")));
    bool wasCommensurate = ( autoscale->Get() == wxT("COMMENSURATE") );
    if( name == wxT("COMMENSURATE") )wasCommensurate = false;
    if( name!=wxT("OFF") && name!=wxT("ON") && name!=wxT("COMMENSURATE") && name!=wxT("X") && name!=wxT("Y") )
      throw ECommandError( command+wxT("valid autoscale keywords: OFF, ON, COMMENSURATE, X, Y") );
    if( wasCommensurate )
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOWERAXIS")))->
          Set( 10.0, true );
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOWERAXIS")))->
          Set( 10.0, true );
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("UPPERAXIS")))->
          Set( 90.0, true );
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("UPPERAXIS")))->
          Set( 90.0, true );
    }
    if( name==wxT("OFF") || name==wxT("X") )
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NLINCS")))->Set( 0 );
    if( name==wxT("OFF") || name==wxT("Y") )
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NLINCS")))->Set( 0 );
    //
    if( isVirtual )
    {
      if( name == wxT("ON") )name = wxT("VIRTUAL");
      else if( name==wxT("X") || name==wxT("Y") )name += wxT("VIRTUAL");
    }
    autoscale->Set(name);
    //
    AddToStackLine( p.GetString(valueIndex_) );
  }
  else if( keyword == wxT("TENSION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    ExGlobals::SetTension( value );
  }
  else if( keyword == wxT("LEGENDSIZE") ) // contour / density plot legend size
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("LEGENDSIZE")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword == wxT("LEGENDON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("ON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("LEGENDENTRYLINEON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("ENTRYLINEON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("LEGENDFRAMEON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("LEGENDFRAME") )
  {
    if( p.GetNumberOfTokens() == 6 )
    {
      double xlo, ylo, xhi, yhi;
      wxString p2( p.GetString(valueIndex_) );
      wxString p3( p.GetString(valueIndex_+1) );
      wxString p4( p.GetString(valueIndex_+2) );
      wxString p5( p.GetString(valueIndex_+3) );
      AddToStackLine( p2 );
      AddToStackLine( p3 );
      AddToStackLine( p4 );
      AddToStackLine( p5 );
      try
      {
        NumericVariable::GetScalar( p2, wxT("x-coordinate of legend frame lower left corner"), xlo );
        NumericVariable::GetScalar( p3, wxT("y-coordinate of legend frame lower left corner"), ylo );
        NumericVariable::GetScalar( p4, wxT("x-coordinate of legend frame upper right corner"), xhi );
        NumericVariable::GetScalar( p5, wxT("y-coordinate of legend frame upper right corner"), yhi );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEXLO")))->
        Set(xlo,percent);
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEYLO")))->
        Set(ylo,percent);
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEXHI")))->
        Set(xhi,percent);
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEYHI")))->
        Set(yhi,percent);
    }
    else
      ExGlobals::InteractiveLegendFrameSetup();
  }
  else if( keyword==wxT("LEGENDFRAMECOLOR") || keyword==wxT("LEGENDFRAMECOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMECOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword == wxT("LEGENDTRANSPARENCY") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TRANSPARENCY")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("LEGENDSYMBOLS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    int u = static_cast<int>(GetValue(keyword));
    if( u<0 )throw ECommandError(command+wxT("number of symbols < 0"));
    static_cast<GRA_intCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("SYMBOLS")))->
        Set( u );
  }
  else if( keyword == wxT("LEGENDTITLEON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLEON")))->
        Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
  }
  else if( keyword == wxT("LEGENDTITLE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLE")))->
          Set( GetString(keyword) );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  else if( keyword == wxT("LEGENDTITLEHEIGHT") )
  {
    static_cast<GRA_sizeCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLEHEIGHT")))->
        Set( GetValue(keyword), percent );
  }
  else if( keyword==wxT("LEGENDTITLECOLOR") || keyword==wxT("LEGENDTITLECOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLECOLOR")))->
        Set( GetColor(keyword) );
  }
  else if( keyword == wxT("LEGENDTITLEFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    static_cast<GRA_fontCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLEFONT")))->
        Set( GetFont(keyword) );
  }
  else if( keyword==wxT("MAXHISTORY") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    ExGlobals::SetMaxHistory( std::max(2,static_cast<int>(value)) );
  }
  else if( keyword==wxT("NHISTORY") || keyword==wxT("SHOWHISTORY") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    ExGlobals::SetNHistory( static_cast<int>(value) );
  }
  else if( keyword == wxT("CONTOURLABELHEIGHT") )
  {
    static_cast<GRA_sizeCharacteristic*>(
      gw->GetGeneralCharacteristics()->Get(wxT("CONTOURLABELHEIGHT")))->
        Set(GetValue(keyword),percent);
  }
  else if( keyword == wxT("CONTOURLABELSEPARATION") )
  {
    static_cast<GRA_sizeCharacteristic*>(
      gw->GetGeneralCharacteristics()->Get(wxT("CONTOURLABELSEPARATION")))->
        Set(GetValue(keyword),percent);
  }
  else if( keyword == wxT("ECHOON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless") );
    value = GetValue( keyword );
    ExGlobals::SetEcho( static_cast<int>(value)!=0 );
  }
  else
  {
    wxString tmp( wxT("unknown keyword: ") );
    tmp += keyword;
    throw ECommandError( command+tmp );
  }
  if( multipleEntries )goto TOP;
}

// end of file
