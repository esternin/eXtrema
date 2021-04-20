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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ParseLine.h"
#include "ECommandError.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "ESyntaxError.h"
#include "GRA_window.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "CMD_read.h"
#include "UsefulFunctions.h"
#include "CMD_set.h"
#include "ExString.h"
#include "Script.h"
#include "GRA_font.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_setOfCharacteristics.h"
#include "ExGlobals.h"

#include "CMD_get.h"

CMD_get CMD_get::cmd_get_;

void CMD_get::Execute( ParseLine const *p )
{
  SetUp( p );
  //
  if( p->GetNumberOfTokens()>=2 && !p->IsString(1) )
    throw ECommandError( "GET","expecting keyword" );
  bool multipleEntries = false;
  if( p->GetNumberOfTokens() == 1 )
  {
    if( ExGlobals::NotInaScript() )throw ECommandError("GET","expecting keyword");
    multipleEntries = true;
  }
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
    catch( runtime_error &e )
    {
      ExGlobals::StopAllScripts();
      throw ECommandError( "GET", e.what() );
    }
    if( ExGlobals::GetEcho() )ExGlobals::WriteOutput( line );
    if( line.empty() || line==ExString(" ") )return;
    if( line[0] == Script::GetCommentCharacter() )goto NEXTLINE;
    line = ExString("GET ")+line;
    ParseLine p2( line );
    try
    {
      p2.ParseIt();
    }
    catch( ESyntaxError &e )
    {
      throw ECommandError( "GET", e.what() );
    }
    const_cast<ParseLine*>(p)->operator=( const_cast<ParseLine const>(p2) );
  }
  AddToStackLine( p->GetString(1) );
  //
  if( p->GetNumberOfTokens()==3 && !p->IsString(2) )
    throw ECommandError( "GET","expecting output variable name" );
  //
  ExString temp( p->GetString(1).UpperCase() );
  ExString name;
  if( p->GetNumberOfTokens() == 3 )
  {
    name = p->GetString(2);
    AddToStackLine( p->GetString(2) );
  }
  bool percent = (temp[0]=='%');
  //
  double fvalue;
  ExString svalue;
  bool isaString = false;
  bool isaVector = false;
  std::vector<double> dblVec;
  std::vector<double>().swap( dblVec );
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  ExString keyword;
  percent ? keyword.assign(temp,1,temp.length()-1) : keyword.assign(temp,0,temp.length());
  //
  if( keyword=="ERRORFILL" || keyword=="ERRFILL" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with ERRORFILL" );
    fvalue = CMD_read::Definition()->GetErrorFill();
  }
  else if( keyword == "ORIENTATION" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with ORIENTATION" );
    (ExGlobals::GetAspectRatio() < 1.0) ? svalue="LANDSCAPE" : svalue="PORTRAIT";
    isaString = true;
  }
  else if( keyword=="COLORMAP" || keyword=="COLOURMAP" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with COLORMAPNAME" );
    svalue = GRA_colorControl::GetColorMapName();
    isaString = true;
  }
  else if( keyword=="COLORMAPSIZE" || keyword=="COLOURMAPSIZE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with COLORMAPSIZE" );
    fvalue = static_cast<double>(GRA_colorControl::GetColorMapSize());
  }
  else if( keyword=="BACKGROUNDCOLOR" || keyword=="BACKGROUNDCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with BACKGROUNDCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword == "BACKGROUNDFILE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with BACKGROUNDFILE" );
    GRA_stringCharacteristic *bgfile =
      static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get("BACKGROUNDFILE"));
    svalue = bgfile->Get();
    isaString = true;
  }
  else if( keyword == "ARROWHEADWIDTH" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with ARROWHEADWIDTH" );
    GRA_doubleCharacteristic *width =
      static_cast<GRA_doubleCharacteristic*>(gw->GetGeneralCharacteristics()->Get("ARROWHEADWIDTH"));
    fvalue = width->Get();
  }
  else if( keyword == "ARROWHEADLENGTH" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with ARROWHEADLENGTH" );
    GRA_doubleCharacteristic *length =
      static_cast<GRA_doubleCharacteristic*>(gw->GetGeneralCharacteristics()->Get("ARROWHEADLENGTH"));
    fvalue = length->Get();
  }
  else if( keyword=="EXTENSION" || keyword=="SCRIPTEXTENSION" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with SCRIPTEXTENSION" );
    svalue = ExGlobals::GetScriptExtension();
    isaString = true;
  }
  else if( keyword == "STACKEXTENSION" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with STACKEXTENSION" );
    svalue = ExGlobals::GetStackExtension();
    isaString = true;
  }
  else if( keyword == "COMMENT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with COMMENT" );
    svalue = Script::GetCommentCharacter();
    isaString = true;
  }
  else if( keyword == "PARAMETER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with PARAMETER" );
    svalue = Script::GetParameterCharacter();
    isaString = true;
  }
  else if( keyword == "TEXTINTERACTIVE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with TEXTINTERACTIVE" );
    GRA_boolCharacteristic *interactive =
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get("INTERACTIVE"));
    fvalue = static_cast<double>(static_cast<int>(interactive->Get()));
  }
  else if( keyword=="TEXTALIGN" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with TEXTALIGN" );
    GRA_intCharacteristic *alignment =
      static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get("ALIGNMENT"));
    fvalue = static_cast<double>(alignment->Get());
  }
  else if( keyword=="CURSOR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with CURSOR" );
    GRA_boolCharacteristic *interactive =
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get("INTERACTIVE"));
    interactive->Get() ? fvalue=1.0 : fvalue=-1.0;
    GRA_intCharacteristic *alignment =
      static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get("ALIGNMENT"));
    fvalue *= static_cast<double>(alignment->Get());
  }
  else if( keyword=="TEXTHEIGHT" || keyword=="TXTHIT" )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetTextCharacteristics()->Get("HEIGHT"));
    fvalue = height->Get( percent );
  }
  else if( keyword=="TEXTANGLE" || keyword=="TXTANG" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with TEXTANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetTextCharacteristics()->Get("ANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="XTEXTLOCATION" || keyword=="XLOC" )
  {
    GRA_distanceCharacteristic *location =
      static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get("XLOCATION"));
    fvalue = location->Get( percent );
  }
  else if( keyword=="YTEXTLOCATION" || keyword=="YLOC" )
  {
    GRA_distanceCharacteristic *location =
      static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get("YLOCATION"));
    fvalue = location->Get( percent );
  }
  else if( keyword=="TEXTFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with TEXTFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetTextCharacteristics()->Get("FONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword=="TEXTCOLOR" || keyword=="TEXTCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with TEXTCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetTextCharacteristics()->Get("COLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="PLOTSYMBOL" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with PLOTSYMBOL" );
    GRA_intCharacteristic *plotsymbol =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOL"));
    if( plotsymbol->IsVector() )
    {
      isaVector = true;
      std::vector<int> intVec( plotsymbol->Gets() );
      std::vector<int>::const_iterator vEnd = intVec.end();
      for( std::vector<int>::const_iterator i=intVec.begin(); i!=vEnd; ++i )
        dblVec.push_back( static_cast<double>(*i) );
    }
    else
      fvalue = static_cast<double>(plotsymbol->Get());
  }
  else if( keyword=="PCHAR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with PCHAR" );
    svalue = CMD_set::Definition()->GetPCHARsymbol() + " " +
             CMD_set::Definition()->GetPCHARsize() + " " +
             CMD_set::Definition()->GetPCHARcolor() + " " +
             CMD_set::Definition()->GetPCHARangle();
    isaString = true;
  }
  else if( keyword=="PLOTSYMBOLSIZE" || keyword=="CHARSZ" )
  {
    GRA_sizeCharacteristic *plotsymbolsize =
      static_cast<GRA_sizeCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLSIZE"));
    if( plotsymbolsize->IsVector() )
    {
      isaVector = true;
      std::vector<double> tmp( plotsymbolsize->Gets(percent) );
      dblVec.assign( tmp.begin(), tmp.end() );
    }
    else
    {
      fvalue = plotsymbolsize->Get( percent );
    }
  }
  else if( keyword=="PLOTSYMBOLANGLE" || keyword=="CHARA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with PLOTSYMBOLANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLANGLE"));
    if( angle->IsVector() )
    {
      isaVector = true;
      std::vector<double> tmp( angle->Gets() );
      dblVec.assign( tmp.begin(), tmp.end() );
    }
    else
      fvalue = angle->Get();
  }
  else if( keyword=="PLOTSYMBOLCOLOR" || keyword=="PLOTSYMBOLCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with PLOTSYMBOLCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLCOLOR"));
    if( color->IsVector() )
    {
      isaVector = true;
      std::vector<GRA_color*> colorVec( color->Gets() );
      std::vector<GRA_color*>::const_iterator vEnd = colorVec.end();
      for( std::vector<GRA_color*>::const_iterator i=colorVec.begin(); i!=vEnd; ++i )
        dblVec.push_back( static_cast<double>(GRA_colorControl::GetColorCode(*i)) );
    }
    else
      fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="PLOTSYMBOLLINEWIDTH" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with PLOTSYMBOLLINEWIDTH" );
    GRA_intCharacteristic *psLineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLLINEWIDTH"));
    fvalue = static_cast<double>(psLineWidth->Get());
  }
  else if( keyword=="CURVECOLOR" || keyword=="CURVECOLOUR" ||
           keyword=="LINECOLOR" || keyword=="LINECOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with CURVECOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVECOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="AREAFILLCOLOR" || keyword=="AREAFILLCOLOUR" ||
           keyword=="FILLCOLOR" || keyword=="FILLCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with AREAFILLCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("AREAFILLCOLOR"));
    if( color->IsVector() )
    {
      isaVector = true;
      std::vector<GRA_color*> colorVec( color->Gets() ); 
      std::vector<GRA_color*>::const_iterator vEnd = colorVec.end();
      for( std::vector<GRA_color*>::const_iterator i=colorVec.begin(); i!=vEnd; ++i )
        dblVec.push_back( static_cast<double>(GRA_colorControl::GetColorCode(*i)) );
    }
    else
      fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="GRIDLINETYPE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with GRIDLINETYPE" );
    GRA_intCharacteristic *gridLineType =
      static_cast<GRA_intCharacteristic*>(gw->GetGeneralCharacteristics()->Get("GRIDLINETYPE"));
    fvalue = static_cast<double>(gridLineType->Get());
  }
  else if( keyword=="CURVELINETYPE" || keyword=="LINTYP" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with CURVELINETYPE" );
    GRA_intCharacteristic *curveLineType =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVELINETYPE"));
    fvalue = static_cast<double>(curveLineType->Get());
  }
  else if( keyword=="CURVELINEWIDTH" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with CURVELINEWIDTH" );
    GRA_intCharacteristic *curveLineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVELINEWIDTH"));
    fvalue = static_cast<double>(curveLineWidth->Get());
  }
  else if( keyword=="XAXISLINEWIDTH" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XAXISLINEWIDTH" );
    GRA_intCharacteristic *lineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LINEWIDTH"));
    fvalue = static_cast<double>(lineWidth->Get());
  }
  else if( keyword=="YAXISLINEWIDTH" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YAXISLINEWIDTH" );
    GRA_intCharacteristic *lineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LINEWIDTH"));
    fvalue = static_cast<double>(lineWidth->Get());
  }
  else if( keyword=="POLARAXISLINEWIDTH" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARAXISLINEWIDTH" );
    GRA_intCharacteristic *lineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("LINEWIDTH"));
    fvalue = static_cast<double>(lineWidth->Get());
  }
  else if( keyword=="XAXISCOLOR" || keyword=="XAXISCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XAXISCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="YAXISCOLOR" || keyword=="YAXISCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YAXISCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="POLARAXISCOLOR" || keyword=="POLARAXISCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARAXISCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="HISTOGRAMTYPE" || keyword=="HISTYP" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with HISTOGRAMTYPE" );
    GRA_intCharacteristic *histType =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("HISTOGRAMTYPE"));
    fvalue = static_cast<double>(histType->Get());
  }
  else if( keyword=="POLARORIGINX" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get("XORIGIN"))->Get(percent);
  }
  else if( keyword=="POLARORIGINY" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get("YORIGIN"))->Get(percent);
  }
  else if( keyword=="POLARNAXES" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNAXES" );
    fvalue = static_cast<double>(
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NAXES"))->Get() );
  }
  else if( keyword=="POLARAXISLENGTH" )
  {
    fvalue =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISLENGTH"))->Get(percent);
  }
  else if( keyword=="POLARAXISANGLE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARAXISANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="POLARCLOCKWISE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARCLOCKWISE" );
    GRA_boolCharacteristic *clockwise =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("CLOCKWISE"));
    fvalue = static_cast<double>(static_cast<int>(clockwise->Get()));
  }
  else if( keyword=="POLARCOMPASSLABELS" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARCOMPASSLABELS" );
    GRA_boolCharacteristic *compass =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("COMPASSLABELS"));
    fvalue = static_cast<double>(static_cast<int>(compass->Get()));
  }
  else if( keyword=="XLOWERAXIS" || keyword=="XLAXIS" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOWERAXIS"))->Get(percent);
  }
  else if( keyword=="XUPPERAXIS" || keyword=="XUAXIS" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get("UPPERAXIS"))->Get(percent);
  }
  else if( keyword=="YLOWERAXIS" || keyword=="YLAXIS" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOWERAXIS"))->Get(percent);
  }
  else if( keyword=="YUPPERAXIS" || keyword=="YUAXIS" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get("UPPERAXIS"))->Get(percent);
  }
  else if( keyword=="XLOWERWINDOW" || keyword=="XLWIND" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("XLOWERWINDOW"))->Get(percent);
  }
  else if( keyword=="XUPPERWINDOW" || keyword=="XUWIND" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("XUPPERWINDOW"))->Get(percent);
  }
  else if( keyword=="YLOWERWINDOW" || keyword=="YLWIND" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("YLOWERWINDOW"))->Get(percent);
  }
  else if( keyword=="YUPPERWINDOW" || keyword=="YUWIND" )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get("YUPPERWINDOW"))->Get(percent);
  }
  else if( keyword=="GRAPHBOX" || keyword=="BOX" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with GRAPHBOX" );
    GRA_boolCharacteristic *graphbox =
      static_cast<GRA_boolCharacteristic*>(gw->GetGeneralCharacteristics()->Get("GRAPHBOX"));
    fvalue = static_cast<double>(static_cast<int>(graphbox->Get()));
  }
  else if( keyword=="XNUMBERSFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNUMBERSFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSFONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword=="YNUMBERSFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNUMBERSFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSFONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword=="POLARNUMBERSFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNUMBERSFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSFONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword == "XNUMBERSON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNUMBERSON" );
    GRA_boolCharacteristic *numberson =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSON"));
    fvalue = static_cast<double>(static_cast<int>(numberson->Get()));
  }
  else if( keyword == "YNUMBERSON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNUMBERSON" );
    GRA_boolCharacteristic *numberson =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSON"));
    fvalue = static_cast<double>(static_cast<int>(numberson->Get()));
  }
  else if( keyword == "POLARNUMBERSON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNUMBERSON" );
    GRA_boolCharacteristic *numberson =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSON"));
    fvalue = static_cast<double>(static_cast<int>(numberson->Get()));
  }
  else if( keyword=="XNUMBEROFDIGITS" || keyword=="NXDIG" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNUMBEROFDIGITS" );
    GRA_intCharacteristic *ndig =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBEROFDIGITS"));
    fvalue = static_cast<double>(ndig->Get());
  }
  else if( keyword=="YNUMBEROFDIGITS" || keyword=="NYDIG" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNUMBEROFDIGITS" );
    GRA_intCharacteristic *ndig =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBEROFDIGITS"));
    fvalue = static_cast<double>(ndig->Get());
  }
  else if( keyword=="POLARNUMBEROFDIGITS" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNUMBEROFDIGITS" );
    GRA_intCharacteristic *ndig =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBEROFDIGITS"));
    fvalue = static_cast<double>(ndig->Get());
  }
  else if( keyword=="XNUMBEROFDECIMALS" || keyword=="NXDEC" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNUMBEROFDECIMALS" );
    GRA_intCharacteristic *ndec =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBEROFDECIMALS"));
    fvalue = static_cast<double>(ndec->Get());
  }
  else if( keyword=="YNUMBEROFDECIMALS" || keyword=="NYDEC" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNUMBEROFDECIMALS" );
    GRA_intCharacteristic *ndec =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBEROFDECIMALS"));
    fvalue = static_cast<double>(ndec->Get());
  }
  else if( keyword=="POLARNUMBEROFDECIMALS" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNUMBEROFDECIMALS" );
    GRA_intCharacteristic *ndec =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBEROFDECIMALS"));
    fvalue = static_cast<double>(ndec->Get());
  }
  else if( keyword=="XNUMBERSHEIGHT" || keyword=="XNUMSZ" )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSHEIGHT"));
    fvalue = height->Get( percent );
  }
  else if( keyword=="YNUMBERSHEIGHT" || keyword=="YNUMSZ" )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSHEIGHT"));
    fvalue = height->Get( percent );
  }
  else if( keyword=="POLARNUMBERSHEIGHT" || keyword=="POLARNUMSZ" )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSHEIGHT"));
    fvalue = height->Get( percent );
  }
  else if( keyword=="XIMAGTICLENGTH" || keyword=="XITICL" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("IMAGTICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="YIMAGTICLENGTH" || keyword=="YITICL" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("IMAGTICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="POLARIMAGTICLENGTH" || keyword=="POLARITICL" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("IMAGTICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="XIMAGTICANGLE" || keyword=="XITICA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XTIMAGTICANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("IMAGTICANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="YIMAGTICANGLE" || keyword=="YITICA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YIMAGTICANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("IMAGTICANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="POLARIMAGTICANGLE" || keyword=="POLARITICA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARTIMAGTICANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("IMAGTICANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="XNUMBERSANGLE" || keyword=="XNUMA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNUMBERSANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="YNUMBERSANGLE" || keyword=="YNUMA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNUMBERSANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NUMBERSANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="POLARNUMBERSANGLE" || keyword=="POLARNUMA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNUMBERSANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="XPOWER" || keyword=="XPOW" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XPOWER" );
    GRA_doubleCharacteristic *power =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("POWER"));
    fvalue = power->Get();
  }
  else if( keyword=="YPOWER" || keyword=="YPOW" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YPOWER" );
    GRA_doubleCharacteristic *power =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("POWER"));
    fvalue = power->Get();
  }
  else if( keyword=="POLARPOWER" || keyword=="POLARPOW" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARPOWER" );
    GRA_doubleCharacteristic *power =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("POWER"));
    fvalue = power->Get();
  }
  else if( keyword=="XPOWERAUTO" || keyword=="XPAUTO" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XPOWERAUTO" );
    GRA_intCharacteristic *pauto =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("POWERAUTO"));
    fvalue = static_cast<double>(pauto->Get());
  }
  else if( keyword=="YPOWERAUTO" || keyword=="YPAUTO" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YPOWERAUTO" );
    GRA_intCharacteristic *pauto =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("POWERAUTO"));
    fvalue = static_cast<double>(pauto->Get());
  }
  else if( keyword=="POLARPOWERAUTO" || keyword=="POLARPAUTO" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARPOWERAUTO" );
    GRA_intCharacteristic *pauto =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("POWERAUTO"));
    fvalue = static_cast<double>(pauto->Get());
  }
  else if( keyword == "XLABELON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XLABELON" );
    GRA_boolCharacteristic *labelon =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELON"));
    fvalue = static_cast<double>(static_cast<int>(labelon->Get()));
  }
  else if( keyword == "YLABELON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YLABELON" );
    GRA_boolCharacteristic *labelon =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELON"));
    fvalue = static_cast<double>(static_cast<int>(labelon->Get()));
  }
  else if( keyword == "POLARLABELON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARLABELON" );
    GRA_boolCharacteristic *labelon =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELON"));
    fvalue = static_cast<double>(static_cast<int>(labelon->Get()));
  }
  else if( keyword=="XLABELFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XLABELFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELFONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword=="YLABELFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YLABELFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELFONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword=="POLARLABELFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARLABELFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELFONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword == "XLABEL" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XLABEL" );
    GRA_stringCharacteristic *label =
      static_cast<GRA_stringCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABEL"));
    svalue = label->Get();
    isaString = true;
  }
  else if( keyword == "YLABEL" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YLABEL" );
    GRA_stringCharacteristic *label =
      static_cast<GRA_stringCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABEL"));
    svalue = label->Get();
    isaString = true;
  }
  else if( keyword == "POLARLABEL" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARLABEL" );
    GRA_stringCharacteristic *label =
      static_cast<GRA_stringCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABEL"));
    svalue = label->Get();
    isaString = true;
  }
  else if( keyword=="XLABELCOLOR" || keyword=="XLABELCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XLABELCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="YLABELCOLOR" || keyword=="YLABELCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YLABELCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="POLARLABELCOLOR" || keyword=="POLARLABELCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARLABELCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get("LABELCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="XNUMBERSCOLOR" || keyword=="XNUMBERSCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNUMBERSCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="YNUMBERSCOLOR" || keyword=="YNUMBERSCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNUMBERSCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NUMBERSCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="POLARNUMBERSCOLOR" || keyword=="POLARNUMBERSCOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNUMBERSCOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get("NUMBERSCOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="XLABELHEIGHT" || keyword=="XLABSZ" )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LABELHEIGHT"));
    fvalue = height->Get( percent );
  }
  else if( keyword=="YLABELHEIGHT" || keyword=="YLABSZ" )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LABELHEIGHT"));
    fvalue = height->Get( percent );
  }
  else if( keyword=="XLARGETICLENGTH" || keyword=="XTICL" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LARGETICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="YLARGETICLENGTH" || keyword=="YTICL" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LARGETICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="POLARLARGETICLENGTH" || keyword=="POLARTICL" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("LARGETICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="XSMALLTICLENGTH" || keyword=="XTICS" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get("SMALLTICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="YSMALLTICLENGTH" || keyword=="YTICS" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get("SMALLTICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="POLARSMALLTICLENGTH" || keyword=="POLARTICS" )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get("SMALLTICLENGTH"));
    fvalue = len->Get( percent );
  }
  else if( keyword=="XTICANGLE" || keyword=="XTICA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XTICANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("TICANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="YTICANGLE" || keyword=="YTICA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YTICANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("TICANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="POLARTICANGLE" || keyword=="POLARTICA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARTICANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get("TICANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="XFORCECROSS" || keyword=="XCROSS" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XFORCECROSS" );
    GRA_boolCharacteristic *force =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("FORCECROSS"));
    fvalue = static_cast<double>(static_cast<int>(force->Get()));
  }
  else if( keyword=="YFORCECROSS" || keyword=="YCROSS" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YFORCECROSS" );
    GRA_boolCharacteristic *force =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("FORCECROSS"));
    fvalue = static_cast<double>(static_cast<int>(force->Get()));
  }
  else if( keyword == "XMIN" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XMIN" );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("MIN"));
    fvalue = min->Get();
  }
  else if( keyword == "YMIN" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YMIN" );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("MIN"));
    fvalue = min->Get();
  }
  else if( keyword == "POLARMIN" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARMIN" );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("MIN"));
    fvalue = min->Get();
  }
  else if( keyword == "XMAX" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XMAX" );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("MAX"));
    fvalue = max->Get();
  }
  else if( keyword == "YMAX" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YMAX" );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("MAX"));
    fvalue = max->Get();
  }
  else if( keyword == "POLARMAX" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARMAX" );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("MAX"));
    fvalue = max->Get();
  }
  else if( keyword=="XVIRTUALMIN" || keyword=="XVMIN" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XVIRTUALMIN" );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("VIRTUALMIN"));
    fvalue = min->Get();
  }
  else if( keyword=="YVIRTUALMIN" || keyword=="YVMIN" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YVIRTUALMIN" );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("VIRTUALMIN"));
    fvalue = min->Get();
  }
  else if( keyword=="POLARVIRTUALMIN" || keyword=="POLARVMIN" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARVIRTUALMIN" );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("VIRTUALMIN"));
    fvalue = min->Get();
  }
  else if( keyword=="XVIRTUALMAX" || keyword=="XVMAX" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XVIRTUALMAX" );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("VIRTUALMAX"));
    fvalue = max->Get();
  }
  else if( keyword=="YVIRTUALMAX" || keyword=="YVMAX" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YVIRTUALMAX" );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("VIRTUALMAX"));
    fvalue = max->Get();
  }
  else if( keyword=="POLARVIRTUALMAX" || keyword=="POLARVMAX" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARVIRTUALMAX" );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("VIRTUALMAX"));
    fvalue = max->Get();
  }
  else if( keyword=="XNLINCS" || keyword=="NLXINC" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNLINCS" );
    GRA_intCharacteristic *nlincs =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NLINCS"));
    fvalue = static_cast<double>(nlincs->Get());
  }
  else if( keyword=="YNLINCS" || keyword=="NLYINC" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNLINCS" );
    GRA_intCharacteristic *nlincs =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NLINCS"));
    fvalue = static_cast<double>(nlincs->Get());
  }
  else if( keyword=="POLARNLINCS" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNLINCS" );
    GRA_intCharacteristic *nlincs =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NLINCS"));
    fvalue = static_cast<double>(nlincs->Get());
  }
  else if( keyword=="XNSINCS" || keyword=="NSXINC" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XNSINCS" );
    GRA_intCharacteristic *nsincs =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("NSINCS"));
    fvalue = static_cast<double>(nsincs->Get());
  }
  else if( keyword=="YNSINCS" || keyword=="NSYINC" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YNSINCS" );
    GRA_intCharacteristic *nsincs =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("NSINCS"));
    fvalue = static_cast<double>(nsincs->Get());
  }
  else if( keyword=="POLARNSINCS" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARNSINCS" );
    GRA_intCharacteristic *nsincs =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("NSINCS"));
    fvalue = static_cast<double>(nsincs->Get());
  }
  else if( keyword == "XTICSON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XTICSON" );
    GRA_boolCharacteristic *ticson =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("TICSON"));
    fvalue = static_cast<double>(static_cast<int>(ticson->Get()));
  }
  else if( keyword == "YTICSON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YTICSON" );
    GRA_boolCharacteristic *ticson =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("TICSON"));
    fvalue = static_cast<double>(static_cast<int>(ticson->Get()));
  }
  else if( keyword == "POLARTICSON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARTICSON" );
    GRA_boolCharacteristic *ticson =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("TICSON"));
    fvalue = static_cast<double>(static_cast<int>(ticson->Get()));
  }
  else if( keyword == "XTICSBOTHSIDES" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XTICSBOTHSIDES" );
    GRA_boolCharacteristic *ticsbs =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("TICSBOTHSIDES"));
    fvalue = static_cast<double>(static_cast<int>(ticsbs->Get()));
  }
  else if( keyword == "YTICSBOTHSIDES" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YTICSBOTHSIDES" );
    GRA_boolCharacteristic *ticsbs =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("TICSBOTHSIDES"));
    fvalue = static_cast<double>(static_cast<int>(ticsbs->Get()));
  }
  else if( keyword == "POLARTICSBOTHSIDES" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARTICSBOTHSIDES" );
    GRA_boolCharacteristic *ticsbs =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("TICSBOTHSIDES"));
    fvalue = static_cast<double>(static_cast<int>(ticsbs->Get()));
  }
  else if( keyword == "XAXISON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XAXISON" );
    GRA_boolCharacteristic *axison =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISON"));
    fvalue = static_cast<double>(static_cast<int>(axison->Get()));
  }
  else if( keyword == "YAXISON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YAXISON" );
    GRA_boolCharacteristic *axison =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISON"));
    fvalue = static_cast<double>(static_cast<int>(axison->Get()));
  }
  else if( keyword == "POLARAXISON" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARAXISON" );
    GRA_boolCharacteristic *axison =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("AXISON"));
    fvalue = static_cast<double>(static_cast<int>(axison->Get()));
  }
  else if( keyword=="XGRID" || keyword=="NXGRID" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XGRID" );
    GRA_intCharacteristic *n =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get("GRID"));
    fvalue = static_cast<double>(n->Get());
  }
  else if( keyword=="YGRID" || keyword=="NYGRID" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YGRID" );
    GRA_intCharacteristic *n =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get("GRID"));
    fvalue = static_cast<double>(n->Get());
  }
  else if( keyword=="POLARGRID" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARGRID" );
    GRA_intCharacteristic *n =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get("GRID"));
    fvalue = static_cast<double>(n->Get());
  }
  else if( keyword=="XAXISANGLE" || keyword=="XAXISA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XAXISANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="YAXISANGLE" || keyword=="YAXISA" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YAXISANGLE" );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISANGLE"));
    fvalue = angle->Get();
  }
  else if( keyword=="XLOGBASE" || keyword=="XLOG" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XLOGBASE" );
    GRA_doubleCharacteristic *logbase =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOGBASE"));
    fvalue = logbase->Get();
  }
  else if( keyword == "XLOGSTYLE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XLOGSTYLE" );
    GRA_boolCharacteristic *logstyle =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOGSTYLE"));
    fvalue = static_cast<double>(static_cast<int>(logstyle->Get()));
  }
  else if( keyword=="YLOGBASE" || keyword=="YLOG" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YLOGBASE" );
    GRA_doubleCharacteristic *logbase =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOGBASE"));
    fvalue = logbase->Get();
  }
  else if( keyword == "YLOGSTYLE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YLOGSTYLE" );
    GRA_boolCharacteristic *logstyle =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOGSTYLE"));
    fvalue = static_cast<double>(static_cast<int>(logstyle->Get()));
  }
  else if( keyword == "XZERO" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XZERO" );
    GRA_boolCharacteristic *zero =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("ZERO"));
    fvalue = static_cast<double>(static_cast<int>(zero->Get()));
  }
  else if( keyword == "YZERO" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YZERO" );
    GRA_boolCharacteristic *zero =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("ZERO"));
    fvalue = static_cast<double>(static_cast<int>(zero->Get()));
  }
  else if( keyword == "XMOD" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XMOD" );
    GRA_doubleCharacteristic *mod =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("MOD"));
    fvalue = mod->Get();
  }
  else if( keyword == "YMOD" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YMOD" );
    GRA_doubleCharacteristic *mod =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("MOD"));
    fvalue = mod->Get();
  }
  else if( keyword == "POLARMOD" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARMOD" );
    GRA_doubleCharacteristic *mod =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("MOD"));
    fvalue = mod->Get();
  }
  else if( keyword=="XLEADINGZEROS" || keyword=="XLEADZ" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XLEADINGZEROS" );
    GRA_boolCharacteristic *lzeros =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LEADINGZEROS"));
    fvalue = static_cast<double>(static_cast<int>(lzeros->Get()));
  }
  else if( keyword=="YLEADINGZEROS" || keyword=="YLEADZ" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YLEADINGZEROS" );
    GRA_boolCharacteristic *lzeros =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LEADINGZEROS"));
    fvalue = static_cast<double>(static_cast<int>(lzeros->Get()));
  }
  else if( keyword=="POLARLEADINGZEROS" || keyword=="POLARLEADZ" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARLEADINGZEROS" );
    GRA_boolCharacteristic *lzeros =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("LEADINGZEROS"));
    fvalue = static_cast<double>(static_cast<int>(lzeros->Get()));
  }
  else if( keyword=="XOFFSET" || keyword=="XOFF" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XOFFSET" );
    GRA_doubleCharacteristic *offset =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("OFFSET"));
    fvalue = offset->Get();
  }
  else if( keyword=="YOFFSET" || keyword=="YOFF" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YOFFSET" );
    GRA_doubleCharacteristic *offset =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("OFFSET"));
    fvalue = offset->Get();
  }
  else if( keyword=="POLAROFFSET" || keyword=="POLAROFF" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLAROFFSET" );
    GRA_doubleCharacteristic *offset =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get("OFFSET"));
    fvalue = offset->Get();
  }
  else if( keyword == "XDROPFIRSTNUMBER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XDROPFIRSTNUMBER" );
    GRA_boolCharacteristic *dropf =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("DROPFIRSTNUMBER"));
    fvalue = static_cast<double>(static_cast<int>(dropf->Get()));
  }
  else if( keyword == "YDROPFIRSTNUMBER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YDROPFIRSTNUMBER" );
    GRA_boolCharacteristic *dropf =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("DROPFIRSTNUMBER"));
    fvalue = static_cast<double>(static_cast<int>(dropf->Get()));
  }
  else if( keyword == "POLARDROPFIRSTNUMBER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARDROPFIRSTNUMBER" );
    GRA_boolCharacteristic *dropf =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("DROPFIRSTNUMBER"));
    fvalue = static_cast<double>(static_cast<int>(dropf->Get()));
  }
  else if( keyword == "XDROPLASTNUMBER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with XDROPLASTNUMBER" );
    GRA_boolCharacteristic *dropl =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get("DROPLASTNUMBER"));
    fvalue = static_cast<double>(static_cast<int>(dropl->Get()));
  }
  else if( keyword == "YDROPLASTNUMBER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with YDROPLASTNUMBER" );
    GRA_boolCharacteristic *dropl =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get("DROPLASTNUMBER"));
    fvalue = static_cast<double>(static_cast<int>(dropl->Get()));
  }
  else if( keyword == "POLARDROPLASTNUMBER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with POLARDROPLASTNUMBER" );
    GRA_boolCharacteristic *dropl =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get("DROPLASTNUMBER"));
    fvalue = static_cast<double>(static_cast<int>(dropl->Get()));
  }
  else if( keyword == "AUTOSCALE" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with AUTOSCALE" );
    GRA_stringCharacteristic *autoscale =
      static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get("AUTOSCALE"));
    svalue = autoscale->Get();
    isaString = true;
  }
  else if( keyword == "TENSION" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with TENSION" );
    fvalue = ExGlobals::GetTension();
  }
  else if( keyword == "LEGENDSIZE" )
  {
    GRA_sizeCharacteristic *lsize =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get("LEGENDSIZE"));
    fvalue = lsize->Get(percent);
  }
  else if( keyword == "LEGENDON" )
  {
    if( percent )throw ECommandError( "GET", "% is meaningless with LEGEND" );
    GRA_boolCharacteristic *legendon =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("ON"));
    fvalue = static_cast<double>(static_cast<int>(legendon->Get()));
  }
  else if( keyword == "LEGENDENTRYLINE" )
  {
    if( percent )throw ECommandError( "GET", "% is meaningless with LEGENDENTRYLINE" );
    GRA_boolCharacteristic *entryline =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("ENTRYLINEON"));
    fvalue = static_cast<double>(static_cast<int>(entryline->Get()));
  }
  else if( keyword == "LEGENDFRAMEON" )
  {
    if( percent )throw ECommandError( "GET", "% is meaningless with LEGENDFRAMEON" );
    GRA_boolCharacteristic *frameon =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEON"));
    fvalue = static_cast<double>(static_cast<int>(frameon->Get()));
  }
  else if( keyword == "LEGENDFRAME" )
  {
    GRA_distanceCharacteristic *framexlo =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEXLO"));
    GRA_distanceCharacteristic *frameylo =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEYLO"));
    GRA_distanceCharacteristic *framexhi =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEXHI"));
    GRA_distanceCharacteristic *frameyhi =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMEYHI"));
    dblVec.push_back( framexlo->Get(percent) );
    dblVec.push_back( frameylo->Get(percent) );
    dblVec.push_back( framexhi->Get(percent) );
    dblVec.push_back( frameyhi->Get(percent) );
    isaVector = true;
  }
  else if( keyword=="LEGENDFRAMECOLOR" || keyword=="LEGENDFRAMECOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with LEGENDFRAMECOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("FRAMECOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword == "LEGENDTRANSPARENCY" )
  {
    if( percent )throw ECommandError( "GET", "% is meaningless with LEGENDTRANSPARENCY" );
    GRA_boolCharacteristic *transparency =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TRANSPARENCY"));
    fvalue = static_cast<double>(static_cast<int>(transparency->Get()));
  }
  else if( keyword == "LEGENDSYMBOLS" )
  {
    if( percent )throw ECommandError( "GET", "% is meaningless with LEGENDSYMBOLS" );
    GRA_intCharacteristic *symbols =
      static_cast<GRA_intCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("SYMBOLS"));
    fvalue = static_cast<double>(symbols->Get());
  }
  else if( keyword == "LEGENDTITLEON" )
  {
    if( percent )throw ECommandError( "GET", "% is meaningless with LEGENDTITLEON" );
    GRA_boolCharacteristic *titleon =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLEON"));
    fvalue = static_cast<double>(static_cast<int>(titleon->Get()));
  }
  else if( keyword == "LEGENDTITLE" )
  {
    if( percent )throw ECommandError( "GET", "% is meaningless with LEGENDTITLE" );
    GRA_stringCharacteristic *title =
      static_cast<GRA_stringCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLE"));
    svalue = title->Get();
    isaString = true;
  }
  else if( keyword == "LEGENDTITLEHEIGHT" )
  {
    GRA_sizeCharacteristic *titleheight =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLEHEIGHT"));
    fvalue = titleheight->Get( percent );
  }
  else if( keyword=="LEGENDTITLECOLOR" || keyword=="LEGENDTITLECOLOUR" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with LEGENDTITLECOLOR" );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLECOLOR"));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword=="LEGENDTITLEFONT" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with LEGENDTITLEFONT" );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get("TITLEFONT"))->Get()->GetName();
    isaString = true;
  }
  else if( keyword == "WINDOWNUMBER" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with WINDOWNUMBER" );
    fvalue = static_cast<double>(ExGlobals::GetWindowNumber());
  }
  else if( keyword == "LEGENDSTATUS" )
  {
    int nw = ExGlobals::GetWindowNumber();
    ExString asnw = ExString("For window # ") + ExString(nw);
    ExGlobals::WriteOutput( asnw );
    GRA_setOfCharacteristics *gl = gw->GetGraphLegendCharacteristics();
    GRA_boolCharacteristic *on =
      static_cast<GRA_boolCharacteristic*>(gl->Get("ON"));
    GRA_boolCharacteristic *transparency =
      static_cast<GRA_boolCharacteristic*>(gl->Get("TRANSPARENCY"));
    GRA_intCharacteristic *symbols =
      static_cast<GRA_intCharacteristic*>(gl->Get("SYMBOLS"));
    GRA_boolCharacteristic *entryline =
      static_cast<GRA_boolCharacteristic*>(gl->Get("ENTRYLINEON"));
    GRA_stringCharacteristic *title =
      static_cast<GRA_stringCharacteristic*>(gl->Get("TITLE"));
    GRA_sizeCharacteristic *titleheight =
      static_cast<GRA_sizeCharacteristic*>(gl->Get("TITLEHEIGHT"));
    GRA_boolCharacteristic *titleon =
      static_cast<GRA_boolCharacteristic*>(gl->Get("TITLEON"));
    GRA_colorCharacteristic *titlecolor =
      static_cast<GRA_colorCharacteristic*>(gl->Get("TITLECOLOR"));
    GRA_fontCharacteristic *titlefont =
      static_cast<GRA_fontCharacteristic*>(gl->Get("TITLEFONT"));
    GRA_distanceCharacteristic *framexlo =
      static_cast<GRA_distanceCharacteristic*>(gl->Get("FRAMEXLO"));
    GRA_distanceCharacteristic *frameylo =
      static_cast<GRA_distanceCharacteristic*>(gl->Get("FRAMEYLO"));
    GRA_distanceCharacteristic *framexhi =
      static_cast<GRA_distanceCharacteristic*>(gl->Get("FRAMEXHI"));
    GRA_distanceCharacteristic *frameyhi =
      static_cast<GRA_distanceCharacteristic*>(gl->Get("FRAMEYHI"));
    GRA_boolCharacteristic *frameon =
      static_cast<GRA_boolCharacteristic*>(gl->Get("FRAMEON"));
    if( on->Get() )ExGlobals::WriteOutput(" legend is on");
    else           ExGlobals::WriteOutput(" legend is off");
    if( transparency->Get() )ExGlobals::WriteOutput(" transparency is on");
    else                     ExGlobals::WriteOutput(" transparency is off");
    ExString asns(" number of legend symbols = ");
    asns += ExString(symbols->Get());
    ExGlobals::WriteOutput( asns );
    if( entryline->Get() )ExGlobals::WriteOutput(" legend entry line is on");
    else                  ExGlobals::WriteOutput(" legend entry line is off");
    ExString titleString( title->Get() );
    if(  titleString.empty() )ExGlobals::WriteOutput(" no title");
    else                      ExGlobals::WriteOutput( ExString(" title = ")+titleString );
    ExString asth(" legend title height = ");
    asth += ExString(titleheight->GetAsPercent());
    ExGlobals::WriteOutput( asth+"%" );
    if( titleon->Get() )ExGlobals::WriteOutput(" legend title is on");
    else                ExGlobals::WriteOutput(" legend title is off");
    ExString astc = ExString(" legend title color code = ") +
        ExString(GRA_colorControl::GetColorCode(titlecolor->Get()));
    ExGlobals::WriteOutput( astc );
    ExString asfn(" legend title fontname = ");
    asfn += titlefont->Get()->GetName();
    ExGlobals::WriteOutput( asfn );
    ExString asxlo( framexlo->GetAsPercent() );
    ExString asylo( frameylo->GetAsPercent() );
    ExString asxup( framexhi->GetAsPercent() );
    ExString asyup( frameyhi->GetAsPercent() );
    ExString asf( " frame lower left corner = (" );
    asf += asxlo + "%," + asylo + "%);";
    ExGlobals::WriteOutput( asf );
    asf = " frame upper right corner = (";
    asf += asxup + "%," + asyup + "%);";
    ExGlobals::WriteOutput( asf );
    frameon->Get() ? ExGlobals::WriteOutput(" frame is on") :
                     ExGlobals::WriteOutput(" frame is off");
    return;
  }
  else if( keyword=="NHISTORY" || keyword=="SHOWHISTORY" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with NHISTORY" );
    fvalue = static_cast<double>(ExGlobals::GetNHistory());
  }
  else if( keyword=="MAXHISTORY" )
  {
    if( percent )throw ECommandError( "GET","% is meaningless with MAXHISTORY" );
    fvalue = static_cast<double>(ExGlobals::GetMaxHistory());
  }
  else if( keyword == "CONTOURLABELHEIGHT" )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get("CONTOURLABELHEIGHT"));
    fvalue = height->Get( percent );
  }
  else if( keyword == "CONTOURLABELSEPARATION" )
  {
    GRA_sizeCharacteristic *sep =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get("CONTOURLABELSEPARATION"));
    fvalue = sep->Get( percent );
  }
  else if( keyword == "ASPECTRATIO" )
  {
    fvalue = ExGlobals::GetAspectRatio();
  }
  else
  {
    ExString tmp( "unknown keyword: " );
    tmp += keyword;
    throw ECommandError( "GET", tmp );
  }
  if( isaString )
  {
    if( p->GetNumberOfTokens() == 2 )
    {
      ExString s( keyword );
      s += ExString(" = ")+svalue;
      ExGlobals::WriteOutput( s );
    }
    else
    {
      try
      {
        TextVariable::PutVariable( name, svalue, p->GetInputLine() );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "GET", e.what() );
      }
    }
  }
  else
  {
    if( isaVector )
    {
      if( p->GetNumberOfTokens() == 2 )
      {
        ExString s(keyword);
        s += ExString(" =");
        for( std::size_t i=0; i<dblVec.size(); ++i )s += " "+ExString(dblVec[i]);
        ExGlobals::WriteOutput( s );
      }
      else
      {
        try
        {
          NumericVariable::PutVariable( name, dblVec, 0, p->GetInputLine() );
        }
        catch ( EVariableError &e )
        {
          throw ECommandError( "GET", e.what() );
        }
      }
    }
    else
    {
      if( p->GetNumberOfTokens() == 2 )
      {
        ExString s( keyword );
        s += " = " + ExString(fvalue);
        ExGlobals::WriteOutput( s );
      }
      else
      {
        try
        {
          NumericVariable::PutVariable( name, fvalue, p->GetInputLine() );
        }
        catch ( EVariableError &e )
        {
          throw ECommandError( "GET", e.what() );
        }
      }
    }
  }
  if( multipleEntries )goto TOP;
}

// end of code
