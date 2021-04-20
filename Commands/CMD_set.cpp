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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "CMD_set.h"

#include "ExGlobals.h"
#include "ExString.h"
#include "ParseLine.h"
#include "ParseToken.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EGraphicsError.h"
#include "ESyntaxError.h"
#include "GRA_window.h"
#include "GRA_colorControl.h"
#include "GRA_color.h"
#include "GRA_fontControl.h"
#include "GRA_font.h"
#include "CMD_read.h"
#include "UsefulFunctions.h"
#include "TextVariable.h"
#include "NumericVariable.h"
#include "Script.h"
#include "GRA_setOfCharacteristics.h"
#include "MainGraphicsWindow.h"
#include "GRA_page.h"

CMD_set CMD_set::cmd_set_;

double CMD_set::GetValue( ExString const &keyword )
{
  ParseLine const *p = GetParseLine();
  ExString command( "SET " );
  command += keyword;
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    double d;
    NumericVariable::GetScalar( p->GetString(valueIndex_), keyword.c_str(), d );
    return d;
  }
  catch (EVariableError const &e)
  {
    throw ECommandError( command.c_str(), e.what() );
  }
}

GRA_color *CMD_set::GetColor( ExString const &keyword )
{
  ParseLine const *p = GetParseLine();
  ExString colorName;
  ExString command( "SET " );
  command += keyword + ": ";
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    TextVariable::GetVariable( p->GetString(valueIndex_), true, colorName );
  }
  catch (EVariableError const &e)
  {
    throw ECommandError( command, e.what() );
  }
  GRA_color *color = GRA_colorControl::GetColor( colorName );
  if( color )return color;
  double d;
  try
  {
    NumericVariable::GetScalar( p->GetString(valueIndex_), "", d );
  }
  catch (EVariableError const &e)
  {
    throw ECommandError( command, e.what() );
  }
  return GRA_colorControl::GetColor( static_cast<int>(d) );
}

GRA_font *CMD_set::GetFont( ExString const &keyword )
{
  ParseLine const *p = GetParseLine();
  ExString fontName;
  ExString command( "SET " );
  command += keyword;
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    TextVariable::GetVariable( p->GetString(valueIndex_), true, fontName );
  }
  catch (EVariableError const &e)
  {
    throw ECommandError( command, e.what() );
  }
  return GRA_fontControl::GetFont( fontName );
}

ExString CMD_set::GetString( ExString const &keyword )
{
  ParseLine const *p = GetParseLine();
  ExString label;
  AddToStackLine( p->GetString(valueIndex_) );
  try
  {
    TextVariable::GetVariable( p->GetString(valueIndex_), false, label );
  }
  catch (EVariableError const &e)
  {
    ExString command( "SET " );
    command += keyword;
    throw ECommandError( command, e.what() );
  }
  return label;
}

void CMD_set::Execute( ParseLine const *p )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  SetUp( p );
  //
  if( p->GetNumberOfTokens()>=2 && !p->IsString(1) )
    throw ECommandError("SET","expecting keyword");
  bool multipleEntries = false;
  if( p->GetNumberOfTokens() == 1 )
  {
    if( ExGlobals::NotInaScript() )throw ECommandError("SET","expecting keyword");
    multipleEntries = true;
  }
  //
TOP:
  if( multipleEntries )
  {
    ExString line;
NEXTLINE:
    try
    {
      if( ExGlobals::GetScript()->GetNextLine(line) ) // end of file found
      {
        ExGlobals::StopScript();
        return;
      }
    }
    catch( runtime_error const &e )
    {
      ExGlobals::StopAllScripts();
      throw ECommandError( "SET", e.what() );
    }
    if( ExGlobals::GetEcho() )ExGlobals::WriteOutput( line );
    if( line.empty() || line==" " )return;
    if( line[0] == Script::GetCommentCharacter() )goto NEXTLINE;
    line = ExString("SET ")+line;
    ParseLine p2( line );
    try
    {
      p2.ParseIt();
    }
    catch( ESyntaxError const &e )
    {
      throw ECommandError( "SET", e.what() );
    }
    const_cast<ParseLine*>(p)->operator=( const_cast<ParseLine const>(p2) );
  }
  AddToStackLine( p->GetString(1) );
  //
  ExString temp( p->GetString(1).UpperCase() );
  //
  double value;
  bool percent = (temp[0] == '%');
  ExString keyword;
  percent ? keyword.assign( temp,1,temp.length()-1 ) :
            keyword.assign( temp,0,temp.length() );
  valueIndex_ = 2;
  ExString command("SET ");
  command += keyword;
  if( p->GetNumberOfTokens()<3 && keyword!="LEGENDFRAME" )throw ECommandError( command, "expecting value" );
  if( p->IsEqualSign(2) )
  {
    valueIndex_ = 3;
    if( p->GetNumberOfTokens() < 4 )throw ECommandError( command, "expecting value" );
  }
  if( keyword=="PLOTSYMBOL" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError const &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_intCharacteristic *plotsymbol =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOL"));
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
      throw ECommandError( command, "scalar or vector expected" );
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="PLOTSYMBOLSIZE" || keyword=="CHARSZ" )
  {
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_sizeCharacteristic *plotsymbolsize =
        static_cast<GRA_sizeCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLSIZE"));
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
      throw ECommandError( command, "must be scalar or vector" );
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="PLOTSYMBOLANGLE" || keyword=="CHARA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_angleCharacteristic *plotsymbolangle =
        static_cast<GRA_angleCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLANGLE"));
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
      throw ECommandError( command, "must be scalar or vector" );
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="PLOTSYMBOLLINEWIDTH" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_intCharacteristic *plotsymbollinewidth =
        static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLLINEWIDTH"));
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
      throw ECommandError( command, "must be scalar or vector" );
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="PLOTSYMBOLCOLOR" || keyword=="PLOTSYMBOLCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    ExString colorName;
    try
    {
      TextVariable::GetVariable( p->GetString(valueIndex_), true, colorName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_colorCharacteristic *plotsymbolcolor =
        static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLCOLOR"));
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
        NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command, e.what() );
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
        throw ECommandError( command, "expecting scalar or vector" );
      }
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="PCHAR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    int ndim;
    double dvalue;
    std::vector<double> data;
    int dimSizes[3];
    try
    {
      NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_intCharacteristic *plotsymbol =
        static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOL"));
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
      throw ECommandError( command, "scalar or vector expected" );
    }
    pcharSymbol_ = p->GetString(valueIndex_);
    AddToStackLine( p->GetString(valueIndex_) );
    pcharSize_.erase();
    pcharColor_.erase();
    pcharAngle_.erase();
    if( p->GetNumberOfTokens() >= valueIndex_+2 )
    {
      try
      {
        std::vector<double>().swap( data );
        NumericVariable::GetVariable( p->GetString(valueIndex_+1), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command, e.what() );
      }
      GRA_sizeCharacteristic *plotsymbolsize =
          static_cast<GRA_sizeCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLSIZE"));
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
        throw ECommandError( command,"scalar or vector expected for size scale factor" );
      }
      pcharSize_ = p->GetString(valueIndex_+1);
      AddToStackLine( p->GetString(valueIndex_+1) );
      if( p->GetNumberOfTokens() >= valueIndex_+3 )
      {
        try
        {
          std::vector<double>().swap( data );
          NumericVariable::GetVariable( p->GetString(valueIndex_+2), ndim, dvalue, data, dimSizes );
        }
        catch (EVariableError &e)
        {
          throw ECommandError( command, e.what() );
        }
        GRA_colorCharacteristic *plotsymbolcolor =
            static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLCOLOR"));
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
          throw ECommandError( command, "scalar or vector expected for color" );
        }
        pcharColor_ = p->GetString(valueIndex_+2);
        AddToStackLine( p->GetString(valueIndex_+2) );
        if( p->GetNumberOfTokens() >= valueIndex_+4 )
        {
          try
          {
            std::vector<double>().swap( data );
            NumericVariable::GetVariable( p->GetString(valueIndex_+3), ndim, dvalue, data, dimSizes );
          }
          catch (EVariableError &e)
          {
            throw ECommandError( command, e.what() );
          }
          GRA_angleCharacteristic *plotsymbolangle =
              static_cast<GRA_angleCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLANGLE"));
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
            throw ECommandError( command, "scalar or vector expected for angles" );
          }
          pcharAngle_ = p->GetString(valueIndex_+3);
          AddToStackLine( p->GetString(valueIndex_+3) );
        }
      }
    }
  }
  else if( keyword=="HISTOGRAMTYPE" || keyword=="HISTYP" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("HISTOGRAMTYPE"))->
          Set(static_cast<int>(GetValue(keyword)));
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="COLOR" || keyword=="COLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      GRA_color *color = GetColor( keyword );
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVECOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSCOLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetTextCharacteristics()->Get("COLOR"))->
          Set( color );
      static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLECOLOR"))->
          Set( color );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "ARROWHEADWIDTH" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    double temp;
    try
    {
      temp = GetValue(keyword);
      static_cast<GRA_doubleCharacteristic*>(gw->GetGeneralCharacteristics()->Get("ARROWHEADWIDTH"))->Set(temp);
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "ARROWHEADLENGTH" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    double temp;
    try
    {
      temp = GetValue(keyword);
      static_cast<GRA_doubleCharacteristic*>(gw->GetGeneralCharacteristics()->Get("ARROWHEADLENGTH"))->Set(temp);
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="BACKGROUNDCOLOR" || keyword=="BACKGROUNDCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    ExString colorName;
    try
    {
      TextVariable::GetVariable( p->GetString(valueIndex_), true, colorName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    //
    // first check to see if it is a named color, such as RED
    //
    GRA_color *bgcolor = GRA_colorControl::GetColor( colorName );
    if( bgcolor )      // it was a recognized name
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDCOLOR"))->
        Set( bgcolor );
      static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDFILE"))->
        Set( ExString("") );
      GRA_page *page = MainGraphicsForm->GetPage();
      page->EraseWindows();
      page->DrawGraphWindows( ExGlobals::GetScreenOutput() );
    }
    else             // it was not a recognized name
    {
      int ndim;
      double dvalue;
      std::vector<double> data;
      int dimSizes[3];
      try
      {
        NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command, e.what() );
      }
      if( ndim == 0 )      // scalar
      {
        int ivalue = static_cast<int>(dvalue);
        GRA_colorCharacteristic *bgchar =
          static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDCOLOR"));
        ivalue==0 ? bgchar->Set(reinterpret_cast<GRA_color*>(0)) :
                    bgchar->Set(GRA_colorControl::GetColor(ivalue));
        static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDFILE"))->
          Set( ExString("") );
        GRA_page *page = MainGraphicsForm->GetPage();
        page->EraseWindows();
        page->DrawGraphWindows( ExGlobals::GetScreenOutput() );
      }
      else
      {
        throw ECommandError( command, "expecting scalar value" );
      }
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="BACKGROUNDFILE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      ExString bgfile( GetString(keyword) );
      if( bgfile == " " )
      {
        static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDFILE"))->
          Set( ExString("") );
        static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDCOLOR"))->
          Set( reinterpret_cast<GRA_color*>(0) );
      }
      else
      {
        FILE *f = fopen(bgfile.c_str(),"rb");
        if( !f )
        {
          throw ECommandError( command, "unable to open PNG file" );
        }
        static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDFILE"))->
          Set( bgfile );
        static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDCOLOR"))->
          Set( reinterpret_cast<GRA_color*>(0) );
      }
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_page *page = MainGraphicsForm->GetPage();
    page->EraseWindows();
    page->DrawGraphWindows( ExGlobals::GetScreenOutput() );
  }
  else if( keyword=="COLORMAPFILE" || keyword=="COLOURMAPFILE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    ExString fileName;
    try
    {
      TextVariable::GetVariable( p->GetString(valueIndex_), true, fileName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    try
    {
      GRA_colorControl::SetFile( fileName );
    }
    catch (EGraphicsError &e)
    {
      throw ECommandError( command, e.what() );
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="COLORMAP" || keyword=="COLOURMAP" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    if( p->GetNumberOfTokens() == valueIndex_+1 )
    {
      ExString name;
      try
      {
        TextVariable::GetVariable( p->GetString(valueIndex_), true, name );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command, e.what() );
      }
      try
      {
        GRA_colorControl::SetColorMap( name );
      }
      catch (EGraphicsError &e)
      {
        throw ECommandError( command, e.what() );
      }
      AddToStackLine( p->GetString(valueIndex_) );
    }
    else
    {
      if( p->GetNumberOfTokens() != valueIndex_+3 )throw ECommandError( command, "expecting 3 vectors" );
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
        NumericVariable::GetVariable( p->GetString(valueIndex_), ndim1, dvalue1, data1, dimSizes1 );
        NumericVariable::GetVariable( p->GetString(valueIndex_+1), ndim2, dvalue2, data2, dimSizes2 );
        NumericVariable::GetVariable( p->GetString(valueIndex_+2), ndim3, dvalue3, data3, dimSizes3 );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command, e.what() );
      }
      if( ndim1!=1 || ndim2!=1 || ndim3!=1 )
        throw ECommandError( command, "must be 3 vectors" );
      if( data1.size()!=data2.size() || data2.size()!=data3.size() )
        throw ECommandError( command, "R, G, and B vectors must be the same size" );
      std::size_t size = data1.size();
      GRA_colorMap *tmp2 = new GRA_colorMap("USERDEFINED");
      for( std::size_t i=0; i<size; ++i )
      {
        tmp2->AddColor( new GRA_color( static_cast<int>(data1[i]),
                                       static_cast<int>(data2[i]),
                                       static_cast<int>(data3[i]) ) );
      }
      GRA_colorControl::SetColorMapUser( tmp2 );
      AddToStackLine( p->GetString(valueIndex_)+ExString(" ")+
                      p->GetString(valueIndex_+1)+ExString(" ")+
                      p->GetString(valueIndex_+2) );
    }
  }
  else if( keyword=="CURVECOLOR" || keyword=="CURVECOLOUR" ||
           keyword=="LINECOLOR"  || keyword=="LINECOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVECOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="AREAFILLCOLOR" || keyword=="AREAFILLCOLOUR" ||
           keyword=="FILLCOLOR" || keyword=="FILLCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    ExString colorName;
    try
    {
      TextVariable::GetVariable( p->GetString(valueIndex_), true, colorName );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    GRA_colorCharacteristic *areafillcolor =
      static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("AREAFILLCOLOR"));
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
        NumericVariable::GetVariable( p->GetString(valueIndex_), ndim, dvalue, data, dimSizes );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command, e.what() );
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
        throw ECommandError( command, "expecting scalar or vector" );
      }
    }
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword=="LINETYPE" || keyword=="LINTYP" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
      static_cast<GRA_intCharacteristic*>(gw->GetGeneralCharacteristics()->Get("GRIDLINETYPE"))->
          Set( static_cast<int>(value) );
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVELINETYPE"))->
          Set( static_cast<int>(value) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="CURVELINETYPE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVELINETYPE"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="GRIDLINETYPE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetGeneralCharacteristics()->Get("GRIDLINETYPE"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="LINEWIDTH" || keyword=="LINTHK" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
      int w = static_cast<int>(value+0.5);
      static_cast<GRA_intCharacteristic*>(
        gw->GetGeneralCharacteristics()->Get("LINEWIDTH"))->Set( w );
      static_cast<GRA_intCharacteristic*>(
        gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLLINEWIDTH"))->Set( w );
      static_cast<GRA_intCharacteristic*>(
        gw->GetDataCurveCharacteristics()->Get("CURVELINEWIDTH"))->Set( w );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="CURVELINEWIDTH" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVELINEWIDTH"))->
          Set(static_cast<int>(GetValue(keyword)+0.5));
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XAXISLINEWIDTH" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LINEWIDTH"))->
          Set(static_cast<int>(GetValue(keyword)+0.5));
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YAXISLINEWIDTH" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LINEWIDTH"))->
          Set(static_cast<int>(GetValue(keyword)+0.5));
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARAXISLINEWIDTH" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("LINEWIDTH"))->
          Set(static_cast<int>(GetValue(keyword)+0.5));
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XAXISCOLOR" || keyword=="XAXISCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YAXISCOLOR" || keyword=="YAXISCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARAXISCOLOR" || keyword=="POLARAXISCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XLABELFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELFONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLABELFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELFONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARLABELFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELFONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XLABELCOLOR" || keyword=="XLABELCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLABELCOLOR" || keyword=="YLABELCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARLABELCOLOR" || keyword=="POLARLABELCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XNUMBERSFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSFONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YNUMBERSFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSFONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNUMBERSFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSFONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XNUMBERSCOLOR" || keyword=="XNUMBERSCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YNUMBERSCOLOR" || keyword=="YNUMBERSCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNUMBERSCOLOR" || keyword=="POLARNUMBERSCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSCOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARORIGINX" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get("XORIGIN"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARORIGINY" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get("YORIGIN"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNAXES" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NAXES"))->
          Set(static_cast<int>(GetValue(keyword)+0.5));
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARAXISLENGTH" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARAXISANGLE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARCLOCKWISE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("CLOCKWISE"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARCOMPASSLABELS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("COMPASSLABELS"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="XLOWERAXIS" || keyword=="XLAXIS" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOWERAXIS"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError const &e)
    {
      throw;
    }
  }
  else if( keyword=="XUPPERAXIS" || keyword=="XUAXIS" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get("UPPERAXIS"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLOWERAXIS" || keyword=="YLAXIS" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOWERAXIS"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YUPPERAXIS" || keyword=="YUAXIS" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get("UPPERAXIS"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XLOWERWINDOW" || keyword=="XLWIND" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("XLOWERWINDOW"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XUPPERWINDOW" || keyword=="XUWIND" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("XUPPERWINDOW"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLOWERWINDOW" || keyword=="YLWIND" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("YLOWERWINDOW"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YUPPERWINDOW" || keyword=="YUWIND" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("YUPPERWINDOW"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="GRAPHBOX" || keyword=="BOX" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetGeneralCharacteristics()->Get("GRAPHBOX"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XNUMBERSON" || keyword == "XNUMBERS")
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YNUMBERSON" || keyword == "YNUMBERS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARNUMBERSON" || keyword == "POLARNUMBERS")
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XNUMBEROFDIGITS" || keyword=="NXDIG" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBEROFDIGITS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YNUMBEROFDIGITS" || keyword=="NYDIG" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBEROFDIGITS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNUMBEROFDIGITS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBEROFDIGITS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XNUMBEROFDECIMALS" || keyword=="NXDEC" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBEROFDECIMALS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YNUMBEROFDECIMALS" || keyword=="NYDEC" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBEROFDECIMALS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNUMBEROFDECIMALS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBEROFDECIMALS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XNUMBERSHEIGHT" || keyword=="XNUMSZ" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSHEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YNUMBERSHEIGHT" || keyword=="YNUMSZ" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSHEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNUMBERSHEIGHT" || keyword=="POLARNUMSZ" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSHEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XIMAGTICLENGTH" || keyword=="XITICL" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("IMAGTICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YIMAGTICLENGTH" || keyword=="YITICL" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("IMAGTICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARIMAGTICLENGTH" || keyword=="POLARITICL" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("IMAGTICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XIMAGTICANGLE" || keyword=="XITICA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("IMAGTICANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YIMAGTICANGLE" || keyword=="YITICA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("IMAGTICANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARIMAGTICANGLE" || keyword=="POLARITICA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("IMAGTICANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XNUMBERSANGLE" || keyword=="XNUMA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YNUMBERSANGLE" || keyword=="YNUMA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNUMBERSANGLE" || keyword=="POLARNUMA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XPOWER" || keyword=="XPOW" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("POWER"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YPOWER" || keyword=="YPOW" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("POWER"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARPOWER" || keyword=="POLARPOW" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("POWER"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XPOWERAUTO" || keyword=="XPAUTO" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("POWERAUTO"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YPOWERAUTO" || keyword=="YPAUTO" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("POWERAUTO"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARPOWERAUTO" || keyword=="POLARPAUTO" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("POWERAUTO"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XLABEL" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABEL"))->
          Set( GetString(keyword) );
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELON"))->
          Set( true );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
  }
  else if( keyword == "YLABEL" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABEL"))->
          Set( GetString(keyword) );
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELON"))->
          Set( true );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
  }
  else if( keyword == "POLARLABEL" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABEL"))->
          Set( GetString(keyword) );
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELON"))->
          Set( true );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
  }
  else if( keyword=="XLABELHEIGHT" || keyword=="XLABSZ" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELHEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLABELHEIGHT" || keyword=="YLABSZ" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELHEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARLABELHEIGHT" || keyword=="POLARLABSZ" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELHEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XLABELON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YLABELON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARLABELON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XLARGETICLENGTH" || keyword=="XTICL" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LARGETICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLARGETICLENGTH" || keyword=="YTICL" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LARGETICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARLARGETICLENGTH" || keyword=="POLARTICL" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("LARGETICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XSMALLTICLENGTH" || keyword=="XTICS" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("SMALLTICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YSMALLTICLENGTH" || keyword=="YTICS" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("SMALLTICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARSMALLTICLENGTH" || keyword=="POLARTICS" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("SMALLTICLENGTH"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XTICANGLE" || keyword=="XTICA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("TICANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YTICANGLE" || keyword=="YTICA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("TICANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARTICANGLE" || keyword=="POLARTICA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("TICANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XFORCECROSS" || keyword=="XCROSS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("FORCECROSS"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YFORCECROSS" || keyword=="YCROSS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("FORCECROSS"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XMIN" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    double temp;
    try
    {
      temp = GetValue(keyword);
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("MIN"))->Set(temp);
    }
    catch (ECommandError &e)
    {
      throw;
    }
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("VIRTUALMIN"))->Set(temp);
  }
  else if( keyword == "YMIN" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    double temp;
    try
    {
      temp = GetValue(keyword);
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("MIN"))->Set(temp);
    }
    catch (ECommandError &e)
    {
      throw;
    }
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("VIRTUALMIN"))->Set(temp);
  }
  else if( keyword == "XMAX" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    double temp;
    try
    {
      temp = GetValue(keyword);
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("MAX"))->Set(temp);
    }
    catch (ECommandError &e)
    {
      throw;
    }
    static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("VIRTUALMAX"))->Set(temp);
  }
  else if( keyword == "YMAX" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    double temp;
    try
    {
      temp = GetValue(keyword);
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("MAX"))->Set(temp);
    }
    catch (ECommandError &e)
    {
      throw;
    }
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("VIRTUALMAX"))->Set(temp);
  }
  else if( keyword == "POLARMAX" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    double temp;
    try
    {
      temp = GetValue(keyword);
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("MAX"))->Set(temp);
    }
    catch (ECommandError &e)
    {
      throw;
    }
    static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("VIRTUALMAX"))->Set(temp);
  }
  else if( keyword=="XVIRTUALMIN" || keyword=="XVMIN" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("VIRTUALMIN"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YVIRTUALMIN" || keyword=="YVMIN" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("VIRTUALMIN"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XVIRTUALMAX" || keyword=="XVMAX" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("VIRTUALMAX"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YVIRTUALMAX" || keyword=="YVMAX" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("VIRTUALMAX"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARVIRTUALMAX" || keyword=="POLARVMAX" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("VIRTUALMAX"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XNLINCS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NLINCS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNLINCS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NLINCS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="NLXINC" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    if( value < 0.0 )
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("DROPFIRSTNUMBER"))->
          Set( true );
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("DROPLASTNUMBER"))->
          Set( true );
      value *= -1;
    }
    static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NLINCS"))->
        Set( static_cast<int>(value) );
  }
  else if( keyword=="YNLINCS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NLINCS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="NLYINC" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    if( value < 0.0 )
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("DROPFIRSTNUMBER"))->
          Set( true );
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("DROPLASTNUMBER"))->
          Set( true );
      value *= -1;
    }
    static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NLINCS"))->
        Set( static_cast<int>(value) );
  }
  else if( keyword=="XNSINCS" || keyword=="NSXINC" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NSINCS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YNSINCS" || keyword=="NSYINC" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NSINCS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARNSINCS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NSINCS"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XTICSON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("TICSON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YTICSON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("TICSON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARTICSON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("TICSON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XTICSBOTHSIDES" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("TICSBOTHSIDES"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YTICSBOTHSIDES" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("TICSBOTHSIDES"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARTICSBOTHSIDES" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("TICSBOTHSIDES"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XTICTP" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    bool ls = (static_cast<int>(value) == 2);
    static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("TICSBOTHSIDES"))->
        Set( ls );
  }
  else if( keyword == "YTICTP" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    bool ls = (static_cast<int>(value) == 2);
    static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("TICSBOTHSIDES"))->
        Set( ls );
  }
  else if( keyword == "XAXISON" || keyword == "XAXIS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YAXISON" || keyword == "YAXIS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARAXISON" || keyword == "POLARAXIS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XGRID" || keyword=="NXGRID" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("GRID"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YGRID" || keyword=="NYGRID" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("GRID"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARGRID" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("GRID"))->
          Set( static_cast<int>(GetValue(keyword)) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XAXISANGLE" || keyword=="XAXISA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YAXISANGLE" || keyword=="YAXISA" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XLOGBASE" || keyword=="XLOG" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOGBASE"))->
          Set( value );
      //gw->GetReplot()->SetXLogBase( value );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XLOGSTYLE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOGSTYLE"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLOGBASE" || keyword=="YLOG" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOGBASE"))->
          Set( value );
      //gw->GetReplot()->SetYLogBase( value );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YLOGSTYLE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOGSTYLE"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XZERO" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("ZERO"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YZERO" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("ZERO"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XMOD" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("MOD"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YMOD" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("MOD"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARMOD" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("MOD"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XLEADINGZEROS" || keyword=="XLEADZ" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LEADINGZEROS"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YLEADINGZEROS" || keyword=="YLEADZ" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LEADINGZEROS"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLARLEADINGZEROS" || keyword=="POLARLEADZ" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("LEADINGZEROS"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XOFFSET" || keyword=="XOFF" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("OFFSET"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YOFFSET" || keyword=="YOFF" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("OFFSET"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="POLAROFFSET" || keyword=="POLAROFF" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("OFFSET"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XDROPFIRSTNUMBER" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("DROPFIRSTNUMBER"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YDROPFIRSTNUMBER" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("DROPFIRSTNUMBER"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARDROPFIRSTNUMBER" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("DROPFIRSTNUMBER"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "XDROPLASTNUMBER" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("DROPLASTNUMBER"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "YDROPLASTNUMBER" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("DROPLASTNUMBER"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "POLARDROPLASTNUMBER" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("DROPLASTNUMBER"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="EXTENSION" || keyword=="SCRIPTEXTENSION" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    if( !p->IsString(valueIndex_) )throw ECommandError( command, "expecting string value" );
    ExGlobals::SetScriptExtension( p->GetString(valueIndex_) );
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword == "STACKEXTENSION" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    if( !p->IsString(valueIndex_) )throw ECommandError( command, "expecting string value" );
    ExGlobals::SetStackExtension( p->GetString(valueIndex_) );
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword == "COMMENT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    if( !p->IsString(valueIndex_) )throw ECommandError( command, "expecting string value" );
    Script::SetCommentCharacter( p->GetString(valueIndex_).at(0) );
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword == "PARAMETER" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    if( !p->IsString(valueIndex_) )throw ECommandError( command, "expecting string value" );
    Script::SetParameterCharacter( p->GetString(valueIndex_).at(0) );
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword == "ERRORFILL" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      CMD_read::Definition()->SetErrorFill( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "ORIENTATION" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    if( !p->IsString(valueIndex_) )throw ECommandError( command, "expecting string value" );
    ExString orient( p->GetString(valueIndex_) );
    orient.ToUpper();
    //
    if( orient != ExString("LANDSCAPE") && orient != ExString("PORTRAIT") )
      throw ECommandError( command, "expecting LANDSCAPE or PORTRAIT" );
    AddToStackLine( p->GetString(valueIndex_) );
    double ar = ExGlobals::GetAspectRatio();
    bool landscape = (ar < 1.0);
    if( orient == "LANDSCAPE" )
    {
      if( !landscape )ExGlobals::ChangeAspectRatio( 1./ar );
    }
    else if( orient == "PORTRAIT" )
    {
      if( landscape )ExGlobals::ChangeAspectRatio( 1./ar );
    }
  }
  else if( keyword=="FONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    GRA_font *font;
    try
    {
      font = GetFont(keyword);
    }
    catch (ECommandError &e)
    {
      throw;
    }
    static_cast<GRA_fontCharacteristic*>(gw->GetTextCharacteristics()->Get("FONT"))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELFONT"))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELFONT"))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSFONT"))->Set(font);
    static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSFONT"))->Set(font);
  }
  else if( keyword=="TEXTFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetTextCharacteristics()->Get("FONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="TEXTCOLOR" || keyword=="TEXTCOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetTextCharacteristics()->Get("COLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "TEXTINTERACTIVE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get("INTERACTIVE"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="TEXTALIGN" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    int tj = static_cast<int>(value);
    if( tj<1 || tj>12 )throw ECommandError( command, "TEXTALIGN must be <= 12 and >= 1" );
    static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get("ALIGNMENT"))->Set( tj );
  }
  else if( keyword=="CURSOR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    int tj = static_cast<int>(value);
    if( abs(tj)<1 || abs(tj)>12 )throw ECommandError( command, "necessary condition: 1 <= |CURSOR| <= 12" );
    if( tj < 0 )
    {
      tj *= -1;
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get("INTERACTIVE"))->
          Set( false );
    }
    else
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get("INTERACTIVE"))->
          Set( true );
    }
    static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get("ALIGNMENT"))->Set( tj );
  }
  else if( keyword=="TEXTHEIGHT" || keyword=="TXTHIT" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetTextCharacteristics()->Get("HEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="TEXTANGLE" || keyword=="TXTANG" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_angleCharacteristic*>(gw->GetTextCharacteristics()->Get("ANGLE"))->
          Set( GetValue(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="XTEXTLOCATION" || keyword=="XLOC" )
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get("XLOCATION"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="YTEXTLOCATION" || keyword=="YLOC")
  {
    try
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get("YLOCATION"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "WINDOWSIZE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = fabs( GetValue( keyword ) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
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
  else if( keyword == "ASPECTRATIO" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = fabs( GetValue( keyword ) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    ExGlobals::ChangeAspectRatio( value );
  }
  else if( keyword == "AUTOSCALE" )
  {
    // autoscale =  0  means OFF
    //           = -1  means COMMENSURATE
    //           =  1  means ON,  = 4  means ON \VIRTUAL
    //           =  2  means X,   = 5  means X \VIRTUAL
    //           =  3  means Y,   = 6  means Y \VIRTUAL
    //
    if( percent )throw ECommandError( command, "% is meaningless" );
    ExString name;
    try
    {
      TextVariable::GetVariable( p->GetString(valueIndex_), true, name );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
    name.ToUpper();
    //
    bool isVirtual = false;
    ParseToken *t = p->GetToken(valueIndex_);
    if( t->GetNumberOfQualifiers() > 0 )
    {
      if( t->GetQualifier(0)->find("VIRTUAL") == 0 )
      {
        isVirtual = true;
      }
      else
      {
        throw ECommandError( command, "VIRTUAL is the only valid qualifier" );
      }
    }
    GRA_stringCharacteristic *autoscale =
      static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get("AUTOSCALE"));
    bool wasCommensurate = ( autoscale->Get() == "COMMENSURATE" );
    if( name == "COMMENSURATE" )wasCommensurate = false;
    if( name!="OFF" && name!="ON" && name!="COMMENSURATE" && name!="X" && name!="Y" )
      throw ECommandError( command, "valid autoscale keywords: OFF, ON, COMMENSURATE, X, Y" );
    if( wasCommensurate )
    {
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOWERAXIS"))->
          Set( 10.0, true );
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOWERAXIS"))->
          Set( 10.0, true );
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get("UPPERAXIS"))->
          Set( 90.0, true );
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get("UPPERAXIS"))->
          Set( 90.0, true );
    }
    if( name=="OFF" || name=="X" )
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NLINCS"))->Set( 0 );
    if( name=="OFF" || name=="Y" )
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NLINCS"))->Set( 0 );
    //
    if( isVirtual )
    {
      if( name == "ON" )name = "VIRTUAL";
      else              name += "VIRTUAL";
    }
    autoscale->Set(name);
    //
    AddToStackLine( p->GetString(valueIndex_) );
  }
  else if( keyword == "TENSION" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    ExGlobals::SetTension( value );
  }
  else if( keyword == "LEGENDSIZE" ) // contour / density plot legend size
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get("LEGENDSIZE"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("ON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDENTRYLINEON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("ENTRYLINEON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDFRAMEON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDFRAME" )
  {
    if( p->GetNumberOfTokens() == 6 )
    {
      double xlo, ylo, xhi, yhi;
      ExString p2( p->GetString(valueIndex_) );
      ExString p3( p->GetString(valueIndex_+1) );
      ExString p4( p->GetString(valueIndex_+2) );
      ExString p5( p->GetString(valueIndex_+3) );
      AddToStackLine( p2 );
      AddToStackLine( p3 );
      AddToStackLine( p4 );
      AddToStackLine( p5 );
      try
      {
        NumericVariable::GetScalar( p2, "x-coordinate of legend frame lower left corner", xlo );
        NumericVariable::GetScalar( p3, "y-coordinate of legend frame lower left corner", ylo );
        NumericVariable::GetScalar( p4, "x-coordinate of legend frame upper right corner", xhi );
        NumericVariable::GetScalar( p5, "y-coordinate of legend frame upper right corner", yhi );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "SET LEGENDFRAME", e.what() );
      }
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEXLO"))->
        Set(xlo,percent);
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEYLO"))->
        Set(ylo,percent);
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEXHI"))->
        Set(xhi,percent);
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEYHI"))->
        Set(yhi,percent);
    }
    else MainGraphicsForm->SetupLegendFrame();
  }
  else if( keyword=="LEGENDFRAMECOLOR" || keyword=="LEGENDFRAMECOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMECOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDTRANSPARENCY" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TRANSPARENCY"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDSYMBOLS" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      int u = static_cast<int>(GetValue(keyword));
      if( u<0 )throw ECommandError(command,"number of symbols < 0");
      static_cast<GRA_intCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("SYMBOLS"))->
          Set( u );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDTITLEON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLEON"))->
          Set( static_cast<bool>(static_cast<int>(GetValue(keyword))) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDTITLE" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_stringCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLE"))->
          Set( GetString(keyword) );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command, e.what() );
    }
  }
  else if( keyword == "LEGENDTITLEHEIGHT" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLEHEIGHT"))->
          Set( GetValue(keyword), percent );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="LEGENDTITLECOLOR" || keyword=="LEGENDTITLECOLOUR" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLECOLOR"))->
          Set( GetColor(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "LEGENDTITLEFONT" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      static_cast<GRA_fontCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLEFONT"))->
          Set( GetFont(keyword) );
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword=="NHISTORY" || keyword=="SHOWHISTORY" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    ExGlobals::SetNHistory( static_cast<int>(value) );
  }
  else if( keyword=="MAXHISTORY" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    ExGlobals::SetMaxHistory( std::max(2,static_cast<int>(value)) );
  }
  else if( keyword == "CONTOURLABELHEIGHT" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(
        gw->GetGeneralCharacteristics()->Get("CONTOURLABELHEIGHT"))->
          Set(GetValue(keyword),percent);
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "CONTOURLABELSEPARATION" )
  {
    try
    {
      static_cast<GRA_sizeCharacteristic*>(
        gw->GetGeneralCharacteristics()->Get("CONTOURLABELSEPARATION"))->
          Set(GetValue(keyword),percent);
    }
    catch (ECommandError &e)
    {
      throw;
    }
  }
  else if( keyword == "ECHOON" )
  {
    if( percent )throw ECommandError( command, "% is meaningless" );
    try
    {
      value = GetValue( keyword );
    }
    catch (ECommandError &e)
    {
      throw;
    }
    ExGlobals::SetEcho( static_cast<int>(value)!=0 );
  }
  else
  {
    ExString tmp( "unknown keyword: " );
    tmp += keyword;
    throw ECommandError( command, tmp.c_str() );
  }
  if( multipleEntries )goto TOP;
}

// end of file
