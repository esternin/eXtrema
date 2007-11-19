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
#include "wx/wx.h"

#include "CMD_get.h"
#include "ECommandError.h"
#include "CMD_read.h"
#include "CMD_set.h"
#include "ESyntaxError.h"
#include "ParseLine.h"
#include "Script.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "GRA_window.h"
#include "GRA_font.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
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
#include "ExGlobals.h"
#include "UsefulFunctions.h"

CMD_get *CMD_get::cmd_get_ = 0;

CMD_get::CMD_get() : Command( wxT("GET") )
{}

void CMD_get::Execute( ParseLine const *p_in )
{
  ParseLine p( *p_in );
  SetUp( &p );
  //
  if( p.GetNumberOfTokens()>=2 && !p.IsString(1) )
    throw ECommandError( wxT("GET: expecting keyword") );
  wxString command( wxT("GET: ") );
  bool multipleEntries = false;
  if( p.GetNumberOfTokens() == 1 )
  {
    if( ExGlobals::NotInaScript() )throw ECommandError( command+wxT("expecting keyword"));
    multipleEntries = true;
  }
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
    if( line.empty() || line==wxString(wxT(" ")) )return;
    if( line.at(0) == Script::GetCommentCharacter() )goto NEXTLINE;
    line = wxString(wxT("GET "))+line;
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
  if( p.GetNumberOfTokens()==3 && !p.IsString(2) )
    throw ECommandError( command+wxT("expecting output variable name") );
  //
  wxString temp( p.GetString(1).Upper() );
  wxString name;
  if( p.GetNumberOfTokens() == 3 )
  {
    name = p.GetString(2);
    AddToStackLine( p.GetString(2) );
  }
  bool percent = (temp.at(0)==wxT('%'));
  //
  double fvalue;
  wxString svalue;
  bool isaString = false;
  bool isaVector = false;
  std::vector<double> dblVec;
  std::vector<double>().swap( dblVec );
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  wxString keyword;
  percent ? keyword.assign(temp,1,temp.length()-1) : keyword.assign(temp,0,temp.length());
  //
  if( keyword==wxT("ERRORFILL") || keyword==wxT("ERRFILL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with ERRORFILL") );
    fvalue = CMD_read::Instance()->GetErrorFill();
  }
  else if( keyword == wxT("ORIENTATION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with ORIENTATION") );
    (ExGlobals::GetAspectRatio() < 1.0) ? svalue=wxT("LANDSCAPE") : svalue=wxT("PORTRAIT");
    isaString = true;
  }
  else if( keyword==wxT("COLORMAP") || keyword==wxT("COLOURMAP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with COLORMAP") );
    svalue = GRA_colorControl::GetColorMapName();
    isaString = true;
  }
  else if( keyword==wxT("COLORMAPSIZE") || keyword==wxT("COLOURMAPSIZE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with COLORMAPSIZE") );
    fvalue = static_cast<double>(GRA_colorControl::GetColorMapSize());
  }
  else if( keyword==wxT("EXTENSION") || keyword==wxT("SCRIPTEXTENSION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with SCRIPTEXTENSION") );
    svalue = ExGlobals::GetScriptExtension();
    isaString = true;
  }
  else if( keyword==wxT("STACKEXTENSION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with STACKEXTENSION") );
    svalue = ExGlobals::GetStackExtension();
    isaString = true;
  }
  else if( keyword == wxT("COMMENT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with COMMENT") );
    svalue = Script::GetCommentCharacter();
    isaString = true;
  }
  else if( keyword == wxT("PARAMETER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with PARAMETER") );
    svalue = Script::GetParameterCharacter();
    isaString = true;
  }
  else if( keyword == wxT("TEXTINTERACTIVE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with TEXTINTERACTIVE") );
    GRA_boolCharacteristic *interactive =
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("INTERACTIVE")));
    fvalue = static_cast<double>(static_cast<int>(interactive->Get()));
  }
  else if( keyword==wxT("TEXTALIGN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with TEXTALIGN") );
    GRA_intCharacteristic *alignment =
      static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("ALIGNMENT")));
    fvalue = static_cast<double>(alignment->Get());
  }
  else if( keyword==wxT("CURSOR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with CURSOR") );
    GRA_boolCharacteristic *interactive =
      static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("INTERACTIVE")));
    interactive->Get() ? fvalue=1.0 : fvalue=-1.0;
    GRA_intCharacteristic *alignment =
      static_cast<GRA_intCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("ALIGNMENT")));
    fvalue *= static_cast<double>(alignment->Get());
  }
  else if( keyword==wxT("TEXTHEIGHT") || keyword==wxT("TXTHIT") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("HEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword==wxT("TEXTANGLE") || keyword==wxT("TXTANG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with TEXTANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("ANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("XTEXTLOCATION") || keyword==wxT("XLOC") )
  {
    GRA_distanceCharacteristic *location =
      static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("XLOCATION")));
    fvalue = location->Get( percent );
  }
  else if( keyword==wxT("YTEXTLOCATION") || keyword==wxT("YLOC") )
  {
    GRA_distanceCharacteristic *location =
      static_cast<GRA_distanceCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("YLOCATION")));
    fvalue = location->Get( percent );
  }
  else if( keyword==wxT("TEXTFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with TEXTFONT") );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("FONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword==wxT("TEXTCOLOR") || keyword==wxT("TEXTCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with TEXTCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("COLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("PLOTSYMBOL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with PLOTSYMBOL") );
    GRA_intCharacteristic *plotsymbol =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOL")));
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
  else if( keyword==wxT("PCHAR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with PCHAR") );
    svalue = CMD_set::Instance()->GetPCHARsymbol() + wxT(" ") +
             CMD_set::Instance()->GetPCHARsize() + wxT(" ") +
             CMD_set::Instance()->GetPCHARcolor() + wxT(" ") +
             CMD_set::Instance()->GetPCHARangle();
    isaString = true;
  }
  else if( keyword==wxT("PLOTSYMBOLSIZE") || keyword==wxT("CHARSZ") )
  {
    GRA_sizeCharacteristic *plotsymbolsize =
      static_cast<GRA_sizeCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLSIZE")));
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
  else if( keyword==wxT("PLOTSYMBOLANGLE") || keyword==wxT("CHARA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with PLOTSYMBOLANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLANGLE")));
    if( angle->IsVector() )
    {
      isaVector = true;
      std::vector<double> tmp( angle->Gets() );
      dblVec.assign( tmp.begin(), tmp.end() );
    }
    else
      fvalue = angle->Get();
  }
  else if( keyword==wxT("PLOTSYMBOLCOLOR") || keyword==wxT("PLOTSYMBOLCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with PLOTSYMBOLCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLCOLOR")));
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
  else if( keyword==wxT("PLOTSYMBOLLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with PLOTSYMBOLLINEWIDTH") );
    GRA_intCharacteristic *psLineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("PLOTSYMBOLLINEWIDTH")));
    fvalue = static_cast<double>(psLineWidth->Get());
  }
  else if( keyword==wxT("CURVECOLOR") || keyword==wxT("CURVECOLOUR") ||
           keyword==wxT("LINECOLOR") || keyword==wxT("LINECOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with CURVECOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVECOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("AREAFILLCOLOR") || keyword==wxT("AREAFILLCOLOUR") ||
           keyword==wxT("FILLCOLOR") || keyword==wxT("FILLCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with AREAFILLCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("AREAFILLCOLOR")));
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
  else if( keyword==wxT("GRIDLINETYPE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with GRIDLINETYPE") );
    GRA_intCharacteristic *gridLineType =
      static_cast<GRA_intCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("GRIDLINETYPE")));
    fvalue = static_cast<double>(gridLineType->Get());
  }
  else if( keyword==wxT("CURVELINETYPE") || keyword==wxT("LINTYP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with CURVELINETYPE") );
    GRA_intCharacteristic *curveLineType =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVELINETYPE")));
    fvalue = static_cast<double>(curveLineType->Get());
  }
  else if( keyword==wxT("CURVELINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with CURVELINEWIDTH") );
    GRA_intCharacteristic *curveLineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVELINEWIDTH")));
    fvalue = static_cast<double>(curveLineWidth->Get());
  }
  else if( keyword==wxT("XAXISLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XAXISLINEWIDTH") );
    GRA_intCharacteristic *axisLineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LINEWIDTH")));
    fvalue = static_cast<double>(axisLineWidth->Get());
  }
  else if( keyword==wxT("YAXISLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YAXISLINEWIDTH") );
    GRA_intCharacteristic *axisLineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LINEWIDTH")));
    fvalue = static_cast<double>(axisLineWidth->Get());
  }
  else if( keyword==wxT("POLARAXISLINEWIDTH") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARAXISLINEWIDTH") );
    GRA_intCharacteristic *axisLineWidth =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LINEWIDTH")));
    fvalue = static_cast<double>(axisLineWidth->Get());
  }
  else if( keyword==wxT("XAXISCOLOR") || keyword==wxT("XAXISCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XAXISCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("AXISCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("YAXISCOLOR") || keyword==wxT("YAXISCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YAXISCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("AXISCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("POLARAXISCOLOR") || keyword==wxT("POLARAXISCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARAXISCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("AXISCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("HISTOGRAMTYPE") || keyword==wxT("HISTYP") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with HISTOGRAMTYPE") );
    GRA_intCharacteristic *histType =
      static_cast<GRA_intCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("HISTOGRAMTYPE")));
    fvalue = static_cast<double>(histType->Get());
  }
  else if( keyword==wxT("POLARORIGINX") )
  {
    fvalue =
        static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("XORIGIN")))->Get(percent);
  }
  else if( keyword==wxT("POLARORIGINY") )
  {
    fvalue =
        static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("YORIGIN")))->Get(percent);
  }
  else if( keyword==wxT("POLARAXISLENGTH") )
  {
    fvalue =
        static_cast<GRA_distanceCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("AXISLENGTH")))->Get(percent);
  }
  else if( keyword==wxT("POLARNAXES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNAXES") );
    GRA_intCharacteristic *naxes =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NAXES")));
    fvalue = static_cast<double>(naxes->Get());
  }
  else if( keyword==wxT("XLOWERAXIS") || keyword==wxT("XLAXIS") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOWERAXIS")))->Get(percent);
  }
  else if( keyword==wxT("XUPPERAXIS") || keyword==wxT("XUAXIS") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("UPPERAXIS")))->Get(percent);
  }
  else if( keyword==wxT("YLOWERAXIS") || keyword==wxT("YLAXIS") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOWERAXIS")))->Get(percent);
  }
  else if( keyword==wxT("YUPPERAXIS") || keyword==wxT("YUAXIS") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("UPPERAXIS")))->Get(percent);
  }
  else if( keyword==wxT("XLOWERWINDOW") || keyword==wxT("XLWIND") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("XLOWERWINDOW")))->Get(percent);
  }
  else if( keyword==wxT("XUPPERWINDOW") || keyword==wxT("XUWIND") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("XUPPERWINDOW")))->Get(percent);
  }
  else if( keyword==wxT("YLOWERWINDOW") || keyword==wxT("YLWIND") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("YLOWERWINDOW")))->Get(percent);
  }
  else if( keyword==wxT("YUPPERWINDOW") || keyword==wxT("YUWIND") )
  {
    fvalue =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("YUPPERWINDOW")))->Get(percent);
  }
  else if( keyword==wxT("GRAPHBOX") || keyword==wxT("BOX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with GRAPHBOX") );
    GRA_boolCharacteristic *graphbox =
      static_cast<GRA_boolCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("GRAPHBOX")));
    fvalue = static_cast<double>(static_cast<int>(graphbox->Get()));
  }
  else if( keyword==wxT("XNUMBERSFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNUMBERSFONT") );
    svalue =
        static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSFONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword==wxT("YNUMBERSFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNUMBERSFONT") );
    svalue =
        static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSFONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword==wxT("POLARNUMBERSFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNUMBERSFONT") );
    svalue =
        static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSFONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword == wxT("XNUMBERSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNUMBERSON") );
    GRA_boolCharacteristic *numberson =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSON")));
    fvalue = static_cast<double>(static_cast<int>(numberson->Get()));
  }
  else if( keyword == wxT("YNUMBERSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNUMBERSON") );
    GRA_boolCharacteristic *numberson =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSON")));
    fvalue = static_cast<double>(static_cast<int>(numberson->Get()));
  }
  else if( keyword == wxT("POLARNUMBERSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNUMBERSON") );
    GRA_boolCharacteristic *numberson =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSON")));
    fvalue = static_cast<double>(static_cast<int>(numberson->Get()));
  }
  else if( keyword==wxT("XNUMBEROFDIGITS") || keyword==wxT("NXDIG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNUMBEROFDIGITS") );
    GRA_intCharacteristic *ndig =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBEROFDIGITS")));
    fvalue = static_cast<double>(ndig->Get());
  }
  else if( keyword==wxT("YNUMBEROFDIGITS") || keyword==wxT("NYDIG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNUMBEROFDIGITS") );
    GRA_intCharacteristic *ndig =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBEROFDIGITS")));
    fvalue = static_cast<double>(ndig->Get());
  }
  else if( keyword==wxT("POLARNUMBEROFDIGITS") || keyword==wxT("NPDIG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNUMBEROFDIGITS") );
    GRA_intCharacteristic *ndig =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBEROFDIGITS")));
    fvalue = static_cast<double>(ndig->Get());
  }
  else if( keyword==wxT("XNUMBEROFDECIMALS") || keyword==wxT("NXDEC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNUMBEROFDECIMALS") );
    GRA_intCharacteristic *ndec =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBEROFDECIMALS")));
    fvalue = static_cast<double>(ndec->Get());
  }
  else if( keyword==wxT("YNUMBEROFDECIMALS") || keyword==wxT("NYDEC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNUMBEROFDECIMALS") );
    GRA_intCharacteristic *ndec =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBEROFDECIMALS")));
    fvalue = static_cast<double>(ndec->Get());
  }
  else if( keyword==wxT("POLARNUMBEROFDECIMALS") || keyword==wxT("NPDEC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNUMBEROFDECIMALS") );
    GRA_intCharacteristic *ndec =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBEROFDECIMALS")));
    fvalue = static_cast<double>(ndec->Get());
  }
  else if( keyword==wxT("XNUMBERSHEIGHT") || keyword==wxT("XNUMSZ") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSHEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword==wxT("YNUMBERSHEIGHT") || keyword==wxT("YNUMSZ") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSHEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword==wxT("POLARNUMBERSHEIGHT") || keyword==wxT("PNUMSZ") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSHEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword==wxT("XIMAGTICLENGTH") || keyword==wxT("XITICL") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("IMAGTICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("YIMAGTICLENGTH") || keyword==wxT("YITICL") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("IMAGTICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("POLARIMAGTICLENGTH") || keyword==wxT("PITICL") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("IMAGTICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("XIMAGTICANGLE") || keyword==wxT("XITICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XTIMAGTICANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("IMAGTICANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("YIMAGTICANGLE") || keyword==wxT("YITICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YIMAGTICANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("IMAGTICANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("POLOARIMAGTICANGLE") || keyword==wxT("PITICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARTIMAGTICANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("IMAGTICANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("XNUMBERSANGLE") || keyword==wxT("XNUMA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNUMBERSANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("YNUMBERSANGLE") || keyword==wxT("YNUMA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNUMBERSANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NUMBERSANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("POLARNUMBERSANGLE") || keyword==wxT("PNUMA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNUMBERSANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("XPOWER") || keyword==wxT("XPOW") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XPOWER") );
    GRA_doubleCharacteristic *power =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("POWER")));
    fvalue = power->Get();
  }
  else if( keyword==wxT("YPOWER") || keyword==wxT("YPOW") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YPOWER") );
    GRA_doubleCharacteristic *power =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("POWER")));
    fvalue = power->Get();
  }
  else if( keyword==wxT("POLARPOWER") || keyword==wxT("PPOW") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARPOWER") );
    GRA_doubleCharacteristic *power =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("POWER")));
    fvalue = power->Get();
  }
  else if( keyword==wxT("XPOWERAUTO") || keyword==wxT("XPAUTO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XPOWERAUTO") );
    GRA_intCharacteristic *pauto =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("POWERAUTO")));
    fvalue = static_cast<double>(pauto->Get());
  }
  else if( keyword==wxT("YPOWERAUTO") || keyword==wxT("YPAUTO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YPOWERAUTO") );
    GRA_intCharacteristic *pauto =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("POWERAUTO")));
    fvalue = static_cast<double>(pauto->Get());
  }
  else if( keyword==wxT("POLARPOWERAUTO") || keyword==wxT("PPAUTO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARPOWERAUTO") );
    GRA_intCharacteristic *pauto =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("POWERAUTO")));
    fvalue = static_cast<double>(pauto->Get());
  }
  else if( keyword==wxT("XLABELFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XLABELFONT") );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELFONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword==wxT("YLABELFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YLABELFONT") );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELFONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword==wxT("POLARLABELFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARLABELFONT") );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELFONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword == wxT("XLABEL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XLABEL") );
    GRA_stringCharacteristic *label =
      static_cast<GRA_stringCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABEL")));
    svalue = label->Get();
    isaString = true;
  }
  else if( keyword == wxT("YLABEL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YLABEL") );
    GRA_stringCharacteristic *label =
      static_cast<GRA_stringCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABEL")));
    svalue = label->Get();
    isaString = true;
  }
  else if( keyword == wxT("POLARLABEL") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARLABEL") );
    GRA_stringCharacteristic *label =
      static_cast<GRA_stringCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABEL")));
    svalue = label->Get();
    isaString = true;
  }
  else if( keyword==wxT("XLABELCOLOR") || keyword==wxT("XLABELCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XLABELCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("YLABELCOLOR") || keyword==wxT("YLABELCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YLABELCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("POLARLABELCOLOR") || keyword==wxT("POLARLABELCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARLABELCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("XNUMBERSCOLOR") || keyword==wxT("XNUMBERSCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNUMBERSCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("YNUMBERSCOLOR") || keyword==wxT("YNUMBERSCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNUMBERSCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NUMBERSCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("POLARNUMBERSCOLOR") || keyword==wxT("POLARNUMBERSCOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNUMBERSCOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NUMBERSCOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("XLABELHEIGHT") || keyword==wxT("XLABSZ") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELHEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword==wxT("YLABELHEIGHT") || keyword==wxT("YLABSZ") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELHEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword==wxT("POLARLABELHEIGHT") || keyword==wxT("PLABSZ") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELHEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword == wxT("XLABELON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XLABELON") );
    GRA_boolCharacteristic *labelon =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LABELON")));
    fvalue = static_cast<double>(static_cast<int>(labelon->Get()));
  }
  else if( keyword == wxT("YLABELON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YLABELON") );
    GRA_boolCharacteristic *labelon =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LABELON")));
    fvalue = static_cast<double>(static_cast<int>(labelon->Get()));
  }
  else if( keyword == wxT("POLARLABELON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARLABELON") );
    GRA_boolCharacteristic *labelon =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LABELON")));
    fvalue = static_cast<double>(static_cast<int>(labelon->Get()));
  }
  else if( keyword==wxT("XLARGETICLENGTH") || keyword==wxT("XTICL") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LARGETICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("YLARGETICLENGTH") || keyword==wxT("YTICL") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LARGETICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("POLARLARGETICLENGTH") || keyword==wxT("PTICL") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LARGETICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("XSMALLTICLENGTH") || keyword==wxT("XTICS") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("SMALLTICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("YSMALLTICLENGTH") || keyword==wxT("YTICS") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("SMALLTICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("POLARSMALLTICLENGTH") || keyword==wxT("PTICS") )
  {
    GRA_sizeCharacteristic *len =
      static_cast<GRA_sizeCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("SMALLTICLENGTH")));
    fvalue = len->Get( percent );
  }
  else if( keyword==wxT("XTICANGLE") || keyword==wxT("XTICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XTICANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("TICANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("YTICANGLE") || keyword==wxT("YTICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YTICANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("TICANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("POLARTICANGLE") || keyword==wxT("PTICA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARTICANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("TICANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("XFORCECROSS") || keyword==wxT("XCROSS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XFORCECROSS") );
    GRA_boolCharacteristic *force =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("FORCECROSS")));
    fvalue = static_cast<double>(static_cast<int>(force->Get()));
  }
  else if( keyword==wxT("YFORCECROSS") || keyword==wxT("YCROSS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YFORCECROSS") );
    GRA_boolCharacteristic *force =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("FORCECROSS")));
    fvalue = static_cast<double>(static_cast<int>(force->Get()));
  }
  else if( keyword == wxT("XMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XMIN") );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("MIN")));
    fvalue = min->Get();
  }
  else if( keyword == wxT("YMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YMIN") );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("MIN")));
    fvalue = min->Get();
  }
  else if( keyword == wxT("XMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XMAX") );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("MAX")));
    fvalue = max->Get();
  }
  else if( keyword == wxT("YMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YMAX") );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("MAX")));
    fvalue = max->Get();
  }
  else if( keyword == wxT("POLARMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARMAX") );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("MAX")));
    fvalue = max->Get();
  }
  else if( keyword==wxT("XVIRTUALMIN") || keyword==wxT("XVMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XVIRTUALMIN") );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("VIRTUALMIN")));
    fvalue = min->Get();
  }
  else if( keyword==wxT("YVIRTUALMIN") || keyword==wxT("YVMIN") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YVIRTUALMIN") );
    GRA_doubleCharacteristic *min =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("VIRTUALMIN")));
    fvalue = min->Get();
  }
  else if( keyword==wxT("XVIRTUALMAX") || keyword==wxT("XVMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XVIRTUALMAX") );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("VIRTUALMAX")));
    fvalue = max->Get();
  }
  else if( keyword==wxT("YVIRTUALMAX") || keyword==wxT("YVMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YVIRTUALMAX") );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("VIRTUALMAX")));
    fvalue = max->Get();
  }
  else if( keyword==wxT("POLARVIRTUALMAX") || keyword==wxT("PVMAX") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARVIRTUALMAX") );
    GRA_doubleCharacteristic *max =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("VIRTUALMAX")));
    fvalue = max->Get();
  }
  else if( keyword==wxT("XNLINCS") || keyword==wxT("NLXINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNLINCS") );
    GRA_intCharacteristic *nlincs =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NLINCS")));
    fvalue = static_cast<double>(nlincs->Get());
  }
  else if( keyword==wxT("YNLINCS") || keyword==wxT("NLYINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNLINCS") );
    GRA_intCharacteristic *nlincs =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NLINCS")));
    fvalue = static_cast<double>(nlincs->Get());
  }
  else if( keyword==wxT("POLARNLINCS") || keyword==wxT("NLPINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNLINCS") );
    GRA_intCharacteristic *nlincs =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NLINCS")));
    fvalue = static_cast<double>(nlincs->Get());
  }
  else if( keyword==wxT("XNSINCS") || keyword==wxT("NSXINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XNSINCS") );
    GRA_intCharacteristic *nsincs =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("NSINCS")));
    fvalue = static_cast<double>(nsincs->Get());
  }
  else if( keyword==wxT("YNSINCS") || keyword==wxT("NSYINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YNSINCS") );
    GRA_intCharacteristic *nsincs =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("NSINCS")));
    fvalue = static_cast<double>(nsincs->Get());
  }
  else if( keyword==wxT("POLARNSINCS") || keyword==wxT("NSPINC") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARNSINCS") );
    GRA_intCharacteristic *nsincs =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("NSINCS")));
    fvalue = static_cast<double>(nsincs->Get());
  }
  else if( keyword == wxT("XTICSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XTICSON") );
    GRA_boolCharacteristic *ticson =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("TICSON")));
    fvalue = static_cast<double>(static_cast<int>(ticson->Get()));
  }
  else if( keyword == wxT("YTICSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YTICSON") );
    GRA_boolCharacteristic *ticson =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("TICSON")));
    fvalue = static_cast<double>(static_cast<int>(ticson->Get()));
  }
  else if( keyword == wxT("POLARTICSON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARTICSON") );
    GRA_boolCharacteristic *ticson =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("TICSON")));
    fvalue = static_cast<double>(static_cast<int>(ticson->Get()));
  }
  else if( keyword == wxT("XTICSBOTHSIDES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XTICSBOTHSIDES") );
    GRA_boolCharacteristic *ticsbs =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("TICSBOTHSIDES")));
    fvalue = static_cast<double>(static_cast<int>(ticsbs->Get()));
  }
  else if( keyword == wxT("YTICSBOTHSIDES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YTICSBOTHSIDES") );
    GRA_boolCharacteristic *ticsbs =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("TICSBOTHSIDES")));
    fvalue = static_cast<double>(static_cast<int>(ticsbs->Get()));
  }
  else if( keyword == wxT("POLARTICSBOTHSIDES") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARTICSBOTHSIDES") );
    GRA_boolCharacteristic *ticsbs =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("TICSBOTHSIDES")));
    fvalue = static_cast<double>(static_cast<int>(ticsbs->Get()));
  }
  else if( keyword == wxT("XAXISON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XAXIS") );
    GRA_boolCharacteristic *axison =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("AXISON")));
    fvalue = static_cast<double>(static_cast<int>(axison->Get()));
  }
  else if( keyword == wxT("YAXISON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YAXIS") );
    GRA_boolCharacteristic *axison =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("AXISON")));
    fvalue = static_cast<double>(static_cast<int>(axison->Get()));
  }
  else if( keyword == wxT("POLARAXISON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARAXIS") );
    GRA_boolCharacteristic *axison =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("AXISON")));
    fvalue = static_cast<double>(static_cast<int>(axison->Get()));
  }
  else if( keyword==wxT("XGRID") || keyword==wxT("NXGRID") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XGRID") );
    GRA_intCharacteristic *n =
      static_cast<GRA_intCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("GRID")));
    fvalue = static_cast<double>(n->Get());
  }
  else if( keyword==wxT("YGRID") || keyword==wxT("NYGRID") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YGRID") );
    GRA_intCharacteristic *n =
      static_cast<GRA_intCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("GRID")));
    fvalue = static_cast<double>(n->Get());
  }
  else if( keyword==wxT("POLARGRID") || keyword==wxT("NPGRID") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARGRID") );
    GRA_intCharacteristic *n =
      static_cast<GRA_intCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("GRID")));
    fvalue = static_cast<double>(n->Get());
  }
  else if( keyword==wxT("XAXISANGLE") || keyword==wxT("XAXISA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XAXISANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("AXISANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("YAXISANGLE") || keyword==wxT("YAXISA") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YAXISANGLE") );
    GRA_angleCharacteristic *angle =
      static_cast<GRA_angleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("AXISANGLE")));
    fvalue = angle->Get();
  }
  else if( keyword==wxT("XLOGBASE") || keyword==wxT("XLOG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XLOGBASE") );
    GRA_doubleCharacteristic *logbase =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOGBASE")));
    fvalue = logbase->Get();
  }
  else if( keyword==wxT("YLOGBASE") || keyword==wxT("YLOG") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YLOGBASE") );
    GRA_doubleCharacteristic *logbase =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOGBASE")));
    fvalue = logbase->Get();
  }
  else if( keyword == wxT("XLOGSTYLE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XLOGSTYLE") );
    GRA_boolCharacteristic *logstyle =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOGSTYLE")));
    fvalue = static_cast<double>(static_cast<int>(logstyle->Get()));
  }
  else if( keyword == wxT("YLOGSTYLE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YLOGSTYLE") );
    GRA_boolCharacteristic *logstyle =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOGSTYLE")));
    fvalue = static_cast<double>(static_cast<int>(logstyle->Get()));
  }
  else if( keyword == wxT("XZERO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XZERO") );
    GRA_boolCharacteristic *zero =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("ZERO")));
    fvalue = static_cast<double>(static_cast<int>(zero->Get()));
  }
  else if( keyword == wxT("YZERO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YZERO") );
    GRA_boolCharacteristic *zero =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("ZERO")));
    fvalue = static_cast<double>(static_cast<int>(zero->Get()));
  }
  else if( keyword == wxT("POLARZERO") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARZERO") );
    GRA_boolCharacteristic *zero =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("ZERO")));
    fvalue = static_cast<double>(static_cast<int>(zero->Get()));
  }
  else if( keyword == wxT("XMOD") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XMOD") );
    GRA_doubleCharacteristic *mod =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("MOD")));
    fvalue = mod->Get();
  }
  else if( keyword == wxT("YMOD") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YMOD") );
    GRA_doubleCharacteristic *mod =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("MOD")));
    fvalue = mod->Get();
  }
  else if( keyword == wxT("POLARMOD") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARMOD") );
    GRA_doubleCharacteristic *mod =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("MOD")));
    fvalue = mod->Get();
  }
  else if( keyword==wxT("XLEADINGZEROS") || keyword==wxT("XLEADZ") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XLEADINGZEROS") );
    GRA_boolCharacteristic *lzeros =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LEADINGZEROS")));
    fvalue = static_cast<double>(static_cast<int>(lzeros->Get()));
  }
  else if( keyword==wxT("YLEADINGZEROS") || keyword==wxT("YLEADZ") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YLEADINGZEROS") );
    GRA_boolCharacteristic *lzeros =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LEADINGZEROS")));
    fvalue = static_cast<double>(static_cast<int>(lzeros->Get()));
  }
  else if( keyword==wxT("POLARLEADINGZEROS") || keyword==wxT("PLEADZ") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARLEADINGZEROS") );
    GRA_boolCharacteristic *lzeros =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("LEADINGZEROS")));
    fvalue = static_cast<double>(static_cast<int>(lzeros->Get()));
  }
  else if( keyword==wxT("XOFFSET") || keyword==wxT("XOFF") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XOFFSET") );
    GRA_doubleCharacteristic *offset =
      static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("OFFSET")));
    fvalue = offset->Get();
  }
  else if( keyword==wxT("YOFFSET") || keyword==wxT("YOFF") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YOFFSET") );
    GRA_doubleCharacteristic *offset =
      static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("OFFSET")));
    fvalue = offset->Get();
  }
  else if( keyword==wxT("POLAROFFSET") || keyword==wxT("POFF") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLAROFFSET") );
    GRA_doubleCharacteristic *offset =
      static_cast<GRA_doubleCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("OFFSET")));
    fvalue = offset->Get();
  }
  else if( keyword == wxT("XDROPFIRSTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XDROPFIRSTNUMBER") );
    GRA_boolCharacteristic *dropf =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("DROPFIRSTNUMBER")));
    fvalue = static_cast<double>(static_cast<int>(dropf->Get()));
  }
  else if( keyword == wxT("YDROPFIRSTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YDROPFIRSTNUMBER") );
    GRA_boolCharacteristic *dropf =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("DROPFIRSTNUMBER")));
    fvalue = static_cast<double>(static_cast<int>(dropf->Get()));
  }
  else if( keyword == wxT("POLARDROPFIRSTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARDROPFIRSTNUMBER") );
    GRA_boolCharacteristic *dropf =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("DROPFIRSTNUMBER")));
    fvalue = static_cast<double>(static_cast<int>(dropf->Get()));
  }
  else if( keyword == wxT("XDROPLASTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with XDROPLASTNUMBER") );
    GRA_boolCharacteristic *dropl =
      static_cast<GRA_boolCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("DROPLASTNUMBER")));
    fvalue = static_cast<double>(static_cast<int>(dropl->Get()));
  }
  else if( keyword == wxT("YDROPLASTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with YDROPLASTNUMBER") );
    GRA_boolCharacteristic *dropl =
      static_cast<GRA_boolCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("DROPLASTNUMBER")));
    fvalue = static_cast<double>(static_cast<int>(dropl->Get()));
  }
  else if( keyword == wxT("POLARDROPLASTNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with POLARDROPLASTNUMBER") );
    GRA_boolCharacteristic *dropl =
      static_cast<GRA_boolCharacteristic*>(gw->GetPolarCharacteristics()->Get(wxT("DROPLASTNUMBER")));
    fvalue = static_cast<double>(static_cast<int>(dropl->Get()));
  }
  else if( keyword == wxT("AUTOSCALE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with AUTOSCALE") );
    GRA_stringCharacteristic *autoscale =
      static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("AUTOSCALE")));
    svalue = autoscale->Get();
    isaString = true;
  }
  else if( keyword == wxT("TENSION") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with TENSION") );
    fvalue = ExGlobals::GetTension();
  }
  else if( keyword == wxT("LEGENDSIZE") )
  {
    GRA_sizeCharacteristic *lsize =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("LEGENDSIZE")));
    fvalue = lsize->Get(percent);
  }
  else if( keyword == wxT("LEGENDON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGEND") );
    GRA_boolCharacteristic *legendon =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("ON")));
    fvalue = static_cast<double>(static_cast<int>(legendon->Get()));
  }
  else if( keyword == wxT("LEGENDENTRYLINE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDENTRYLINE") );
    GRA_boolCharacteristic *entryline =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("ENTRYLINEON")));
    fvalue = static_cast<double>(static_cast<int>(entryline->Get()));
  }
  else if( keyword == wxT("LEGENDFRAMEON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDFRAMEON") );
    GRA_boolCharacteristic *frameon =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEON")));
    fvalue = static_cast<double>(static_cast<int>(frameon->Get()));
  }
  else if( keyword == wxT("LEGENDFRAME") )
  {
    GRA_distanceCharacteristic *framexlo =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEXLO")));
    GRA_distanceCharacteristic *frameylo =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEYLO")));
    GRA_distanceCharacteristic *framexhi =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEXHI")));
    GRA_distanceCharacteristic *frameyhi =
      static_cast<GRA_distanceCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMEYHI")));
    dblVec.push_back( framexlo->Get(percent) );
    dblVec.push_back( frameylo->Get(percent) );
    dblVec.push_back( framexhi->Get(percent) );
    dblVec.push_back( frameyhi->Get(percent) );
    isaVector = true;
  }
  else if( keyword==wxT("LEGENDFRAMECOLOR") || keyword==wxT("LEGENDFRAMECOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDFRAMECOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("FRAMECOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword == wxT("LEGENDTRANSPARENCY") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDTRANSPARENCY") );
    GRA_boolCharacteristic *transparency =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TRANSPARENCY")));
    fvalue = static_cast<double>(static_cast<int>(transparency->Get()));
  }
  else if( keyword == wxT("LEGENDSYMBOLS") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDSYMBOLS") );
    GRA_intCharacteristic *symbols =
      static_cast<GRA_intCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("SYMBOLS")));
    fvalue = static_cast<double>(symbols->Get());
  }
  else if( keyword == wxT("LEGENDTITLEON") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDTITLEON") );
    GRA_boolCharacteristic *titleon =
      static_cast<GRA_boolCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLEON")));
    fvalue = static_cast<double>(static_cast<int>(titleon->Get()));
  }
  else if( keyword == wxT("LEGENDTITLE") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDTITLE") );
    GRA_stringCharacteristic *title =
      static_cast<GRA_stringCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLE")));
    svalue = title->Get();
    isaString = true;
  }
  else if( keyword == wxT("LEGENDTITLEHEIGHT") )
  {
    GRA_sizeCharacteristic *titleheight =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLEHEIGHT")));
    fvalue = titleheight->Get( percent );
  }
  else if( keyword==wxT("LEGENDTITLECOLOR") || keyword==wxT("LEGENDTITLECOLOUR") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDTITLECOLOR") );
    GRA_colorCharacteristic *color =
      static_cast<GRA_colorCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLECOLOR")));
    fvalue = static_cast<double>(GRA_colorControl::GetColorCode(color->Get()));
  }
  else if( keyword==wxT("LEGENDTITLEFONT") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with LEGENDTITLEFONT") );
    svalue = static_cast<GRA_fontCharacteristic*>(gw->GetGraphLegendCharacteristics()->Get(wxT("TITLEFONT")))->Get()->GetFontName();
    isaString = true;
  }
  else if( keyword == wxT("WINDOWNUMBER") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with WINDOWNUMBER") );
    fvalue = static_cast<double>(ExGlobals::GetWindowNumber());
  }
  else if( keyword == wxT("LEGENDSTATUS") )
  {
    int nw = ExGlobals::GetWindowNumber();
    wxString asnw( wxT("For window # ") );
    ExGlobals::WriteOutput( asnw << nw );
    GRA_setOfCharacteristics *gl = gw->GetGraphLegendCharacteristics();
    GRA_boolCharacteristic *on =
      static_cast<GRA_boolCharacteristic*>(gl->Get(wxT("ON")));
    GRA_boolCharacteristic *transparency =
      static_cast<GRA_boolCharacteristic*>(gl->Get(wxT("TRANSPARENCY")));
    GRA_intCharacteristic *symbols =
      static_cast<GRA_intCharacteristic*>(gl->Get(wxT("SYMBOLS")));
    GRA_boolCharacteristic *entryline =
      static_cast<GRA_boolCharacteristic*>(gl->Get(wxT("ENTRYLINEON")));
    GRA_stringCharacteristic *title =
      static_cast<GRA_stringCharacteristic*>(gl->Get(wxT("TITLE")));
    GRA_sizeCharacteristic *titleheight =
      static_cast<GRA_sizeCharacteristic*>(gl->Get(wxT("TITLEHEIGHT")));
    GRA_boolCharacteristic *titleon =
      static_cast<GRA_boolCharacteristic*>(gl->Get(wxT("TITLEON")));
    GRA_colorCharacteristic *titlecolor =
      static_cast<GRA_colorCharacteristic*>(gl->Get(wxT("TITLECOLOR")));
    GRA_fontCharacteristic *titlefont =
      static_cast<GRA_fontCharacteristic*>(gl->Get(wxT("TITLEFONT")));
    GRA_distanceCharacteristic *framexlo =
      static_cast<GRA_distanceCharacteristic*>(gl->Get(wxT("FRAMEXLO")));
    GRA_distanceCharacteristic *frameylo =
      static_cast<GRA_distanceCharacteristic*>(gl->Get(wxT("FRAMEYLO")));
    GRA_distanceCharacteristic *framexhi =
      static_cast<GRA_distanceCharacteristic*>(gl->Get(wxT("FRAMEXHI")));
    GRA_distanceCharacteristic *frameyhi =
      static_cast<GRA_distanceCharacteristic*>(gl->Get(wxT("FRAMEYHI")));
    GRA_boolCharacteristic *frameon =
      static_cast<GRA_boolCharacteristic*>(gl->Get(wxT("FRAMEON")));
    if( on->Get() )ExGlobals::WriteOutput(wxT(" legend is on"));
    else           ExGlobals::WriteOutput(wxT(" legend is off"));
    if( transparency->Get() )ExGlobals::WriteOutput(wxT(" transparency is on"));
    else                     ExGlobals::WriteOutput(wxT(" transparency is off"));
    wxString asns(wxT(" number of legend symbols = "));
    ExGlobals::WriteOutput( asns << symbols->Get() );
    if( entryline->Get() )ExGlobals::WriteOutput(wxT(" legend entry line is on"));
    else                  ExGlobals::WriteOutput(wxT(" legend entry line is off"));
    wxString titleString( title->Get() );
    if(  titleString.empty() )ExGlobals::WriteOutput(wxT(" no title"));
    else                      ExGlobals::WriteOutput( wxString(wxT(" title = "))+titleString );
    wxString asth(wxT(" legend title height = "));
    asth << titleheight->GetAsPercent() << wxT('%');
    if( titleon->Get() )ExGlobals::WriteOutput(wxT(" legend title is on"));
    else                ExGlobals::WriteOutput(wxT(" legend title is off"));
    wxString astc = wxString(wxT(" legend title color code = "));
    ExGlobals::WriteOutput( astc << GRA_colorControl::GetColorCode(titlecolor->Get()) );
    wxString asfn(wxT(" legend title fontname = "));
    asfn += titlefont->Get()->GetFontName();
    ExGlobals::WriteOutput( asfn );
    wxString asxlo;
    asxlo << framexlo->GetAsPercent();
    wxString asylo;
    asylo << frameylo->GetAsPercent();
    wxString asxup;
    asxup << framexhi->GetAsPercent();
    wxString asyup;
    asyup << frameyhi->GetAsPercent();
    wxString asf( wxT(" frame lower left corner = (") );
    asf += asxlo + wxT("%,") + asylo + wxT("%);");
    ExGlobals::WriteOutput( asf );
    asf = wxT(" frame upper right corner = (");
    asf += asxup + wxT("%,") + asyup + wxT("%);");
    ExGlobals::WriteOutput( asf );
    frameon->Get() ? ExGlobals::WriteOutput(wxT(" frame is on")) :
                     ExGlobals::WriteOutput(wxT(" frame is off"));
    return;
  }
  else if( keyword==wxT("MAXHISTORY") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with MAXHISTORY") );
    fvalue = static_cast<double>(ExGlobals::GetMaxHistory());
  }
  else if( keyword==wxT("NHISTORY") || keyword==wxT("SHOWHISTORY") )
  {
    if( percent )throw ECommandError( command+wxT("% is meaningless with NHISTORY") );
    fvalue = static_cast<double>(ExGlobals::GetNHistory());
  }
  else if( keyword == wxT("CONTOURLABELHEIGHT") )
  {
    GRA_sizeCharacteristic *height =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("CONTOURLABELHEIGHT")));
    fvalue = height->Get( percent );
  }
  else if( keyword == wxT("CONTOURLABELSEPARATION") )
  {
    GRA_sizeCharacteristic *sep =
      static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("CONTOURLABELSEPARATION")));
    fvalue = sep->Get( percent );
  }

  else if( keyword == wxT("ASPECTRATIO") )
  {
    fvalue = ExGlobals::GetAspectRatio();
  }
  else
  {
    throw ECommandError( command+wxT("unknown keyword: ")+keyword );
  }
  if( isaString )
  {
    if( p.GetNumberOfTokens() == 2 )
    {
      wxString s( keyword );
      s += wxString(wxT(" = "))+svalue;
      ExGlobals::WriteOutput( s );
    }
    else
    {
      try
      {
        TextVariable::PutVariable( name, svalue, p.GetInputLine() );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
  }
  else
  {
    if( isaVector )
    {
      if( p.GetNumberOfTokens() == 2 )
      {
        wxString s;
        if( percent )s = wxT('%');
        s += keyword + wxT(" = ");
        for( std::size_t i=0; i<dblVec.size(); ++i )s << wxT(" ") << dblVec[i];
        ExGlobals::WriteOutput( s );
      }
      else
      {
        try
        {
          NumericVariable::PutVariable( name, dblVec, 0, p.GetInputLine() );
        }
        catch ( EVariableError &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
      }
    }
    else
    {
      if( p.GetNumberOfTokens() == 2 )
      {
        wxString s;
        if( percent )s = wxT('%');
        s += keyword + wxT(" = ");
        ExGlobals::WriteOutput( s << fvalue );
      }
      else
      {
        try
        {
          NumericVariable::PutVariable( name, fvalue, p.GetInputLine() );
        }
        catch ( EVariableError &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
      }
    }
  }
  if( multipleEntries )goto TOP;
}

// end of code
