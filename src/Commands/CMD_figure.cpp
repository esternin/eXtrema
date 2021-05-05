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
#include "wx/wx.h"

#include "CMD_figure.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "GRA_window.h"
#include "GRA_arrow1.h"
#include "GRA_arrow2.h"
#include "GRA_arrow3.h"
#include "GRA_rectangle.h"
#include "GRA_triangle.h"
#include "GRA_ellipse.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "ExGlobals.h"

CMD_figure *CMD_figure::cmd_figure_ = 0;

CMD_figure::CMD_figure() : Command( wxT("FIGURE") )
{
  AddQualifier( wxT("GRAPH"), false );
  AddQualifier( wxT("PERCENT"), false );
  AddQualifier( wxT("WORLD"), true );
}

void CMD_figure::Execute( ParseLine const *p )
{
  // FIGURE ARROW style nheads x1 y1 x2 y2
  // FIGURE RECTANGLE x1 y1 x2 y2
  // FIGURE TRIANGLE x1 y1 x2 y2 x3 y3
  // FIGURE ELLIPSE x1 y1 x2 y2
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_color *cc =
      static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get(wxT("CURVECOLOR")))->Get();
  GRA_setOfCharacteristics *generalChars = gw->GetGeneralCharacteristics();
  GRA_colorCharacteristic *colorChar =
    static_cast<GRA_colorCharacteristic*>(generalChars->Get(wxT("AREAFILLCOLOR")));
  GRA_color *fc = colorChar->IsVector() ? colorChar->Gets().front() : colorChar->Get();
  int lw = static_cast<GRA_intCharacteristic*>(generalChars->Get(wxT("LINEWIDTH")))->Get();
  //
  if( p->GetNumberOfTokens() < 2 )throw ECommandError( wxT("FIGURE: expecting figure type")  );
  if( !p->IsString(1) )throw ECommandError( wxT("FIGURE: expecting figure type")  );
  AddToStackLine( p->GetString(1) );
  wxString figType( p->GetString(1).Upper() );
  //
  if( figType == wxT("ARROW") )
  {
    wxString command( wxT("FIGURE ARROW: ") );
    // FIGURE ARROW style nheads x1 y1 x2 y2
    //
    if( p->GetNumberOfTokens() < 3 )
      throw ECommandError( command+wxT("expecting style code")  );
    double d;
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("style code"), d );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    int style = static_cast<int>(d+0.5);
    if( style!=1 && style!=2 && style!=3 )
      throw ECommandError( command+wxT("style code must be 1, 2, or 3")  );
    if( p->GetNumberOfTokens() < 4 )
      throw ECommandError( command+wxT("expecting number of heads")  );
    try
    {
      NumericVariable::GetScalar( p->GetString(3), wxT("number of heads"), d );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(3) );
    int heads = static_cast<int>(d+0.5);
    if( heads!=1 && heads!=2 )
      throw ECommandError( command+wxT("number of heads must be 1 or 2")  );
    if( p->GetNumberOfTokens() < 8 )
      throw ECommandError( command+wxT("expecting x1, y1, x2, y2")  );
    //
    bool headsBothEnds = (heads==2);
    double x1, y1, x2, y2;
    try
    {
      NumericVariable::GetScalar( p->GetString(4), wxT("x1"), x1 );
      NumericVariable::GetScalar( p->GetString(5), wxT("y1"), y1 );
      NumericVariable::GetScalar( p->GetString(6), wxT("x2"), x2 );
      NumericVariable::GetScalar( p->GetString(7), wxT("y2"), y2 );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(4) );
    AddToStackLine( p->GetString(5) );
    AddToStackLine( p->GetString(6) );
    AddToStackLine( p->GetString(7) );
    //
    double x1w, y1w, x2w, y2w;
    if( qualifiers[wxT("GRAPH")] )
    {
      gw->GraphToWorld( x1, y1, x1w, y1w, true );
      gw->GraphToWorld( x2, y2, x2w, y2w );
    }
    else if( qualifiers[wxT("PERCENT")] )
    {
      gw->PercentToWorld( x1, y1, x1w, y1w );
      gw->PercentToWorld( x2, y2, x2w, y2w );
    }
    else
    {
      x1w = x1;
      y1w = y1;
      x2w = x2;
      y2w = y2;
    }
    double headWidth = static_cast<GRA_doubleCharacteristic*>(generalChars->Get(wxT("ARROWHEADWIDTH")))->Get();
    double headLength = static_cast<GRA_doubleCharacteristic*>(generalChars->Get(wxT("ARROWHEADLENGTH")))->Get();
    switch (style)
    {
      case 1:
      {
        GRA_arrow1 *arrow = new GRA_arrow1(x2w,y2w,x1w,y1w,headsBothEnds,cc,fc,lw,headWidth,headLength);
        gw->AddDrawableObject( arrow );
        break;
      }
      case 2:
      {
        GRA_arrow2 *arrow = new GRA_arrow2(x2w,y2w,x1w,y1w,headsBothEnds,cc,fc,lw,headWidth,headLength);
        gw->AddDrawableObject( arrow );
        break;
      }
      case 3:
      {
        GRA_arrow3 *arrow = new GRA_arrow3(x2w,y2w,x1w,y1w,headsBothEnds,cc,lw,headWidth,headLength);
        gw->AddDrawableObject( arrow );
      }
    }
  }
  else if( figType == wxT("RECTANGLE") )
  {
    // FIGURE RECTANGLE x1 y1 x2 y2
    //
    wxString command( wxT("FIGURE RECTANGLE") );
    if( p->GetNumberOfTokens() < 6 )
      throw ECommandError( command+wxT("expecting x1, y1, x2, y2")  );
    double x1, y1, x2, y2;
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("x1"), x1 );
      NumericVariable::GetScalar( p->GetString(3), wxT("y1"), y1 );
      NumericVariable::GetScalar( p->GetString(4), wxT("x2"), x2 );
      NumericVariable::GetScalar( p->GetString(5), wxT("y2"), y2 );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    AddToStackLine( p->GetString(3) );
    AddToStackLine( p->GetString(4) );
    AddToStackLine( p->GetString(5) );
    //
    double x1w, y1w, x2w, y2w;
    if( qualifiers[wxT("GRAPH")] )
    {
      gw->GraphToWorld( x1, y1, x1w, y1w, true );
      gw->GraphToWorld( x2, y2, x2w, y2w );
    }
    else if( qualifiers[wxT("PERCENT")] )
    {
      gw->PercentToWorld( x1, y1, x1w, y1w );
      gw->PercentToWorld( x2, y2, x2w, y2w );
    }
    else
    {
      x1w = x1;
      y1w = y1;
      x2w = x2;
      y2w = y2;
    }
    GRA_rectangle *rect = new GRA_rectangle(x1w,y1w,x2w,y2w,0.0,false,cc,fc,lw);
    gw->AddDrawableObject( rect );
  }
  else if( figType == wxString(wxT("TRIANGLE")) )
  {
    // FIGURE TRIANGLE x1 y1 x2 y2 x3 y3
    //
    wxString command( wxT("FIGURE TRIANGLE: ") );
    if( p->GetNumberOfTokens() < 8 )
      throw ECommandError( command+wxT("expecting x1,y1,x2,y2,x3,y3")  );
    double x1, y1, x2, y2, x3, y3;
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("x1"), x1 );
      NumericVariable::GetScalar( p->GetString(3), wxT("y1"), y1 );
      NumericVariable::GetScalar( p->GetString(4), wxT("x2"), x2 );
      NumericVariable::GetScalar( p->GetString(5), wxT("y2"), y2 );
      NumericVariable::GetScalar( p->GetString(6), wxT("x3"), x3 );
      NumericVariable::GetScalar( p->GetString(7), wxT("y3"), y3 );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    AddToStackLine( p->GetString(3) );
    AddToStackLine( p->GetString(4) );
    AddToStackLine( p->GetString(5) );
    AddToStackLine( p->GetString(6) );
    AddToStackLine( p->GetString(7) );
    //
    double x1w, y1w, x2w, y2w, x3w, y3w;
    if( qualifiers[wxT("GRAPH")] )
    {
      gw->GraphToWorld( x1, y1, x1w, y1w, true );
      gw->GraphToWorld( x2, y2, x2w, y2w );
      gw->GraphToWorld( x3, y3, x3w, y3w );
    }
    else if( qualifiers[wxT("PERCENT")] )
    {
      gw->PercentToWorld( x1, y1, x1w, y1w );
      gw->PercentToWorld( x2, y2, x2w, y2w );
      gw->PercentToWorld( x3, y3, x3w, y3w );
    }
    else
    {
      x1w = x1;
      y1w = y1;
      x2w = x2;
      y2w = y2;
      x3w = x3;
      y3w = y3;
    }
    GRA_triangle *tri = new GRA_triangle(x1w,y1w,x2w,y2w,x3w,y3w,cc,fc,lw);
    gw->AddDrawableObject( tri );
  }
  else if( figType == wxString(wxT("ELLIPSE")) )
  {
    // FIGURE ELLIPSE x1 y1 x2 y2
    //
    wxString command( wxT("FIGURE ELLIPSE: ") );
    if( p->GetNumberOfTokens() < 6 )throw ECommandError( command+wxT("expecting x1, y1, x2, y2")  );
    double x1, y1, x2, y2;
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("x1"), x1 );
      NumericVariable::GetScalar( p->GetString(3), wxT("y1"), y1 );
      NumericVariable::GetScalar( p->GetString(4), wxT("x2"), x2 );
      NumericVariable::GetScalar( p->GetString(5), wxT("y2"), y2 );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    AddToStackLine( p->GetString(3) );
    AddToStackLine( p->GetString(4) );
    AddToStackLine( p->GetString(5) );
    //
    double x1w, y1w, x2w, y2w;
    if( qualifiers[wxT("GRAPH")] )
    {
      gw->GraphToWorld( x1, y1, x1w, y1w, true );
      gw->GraphToWorld( x2, y2, x2w, y2w );
    }
    else if( qualifiers[wxT("PERCENT")] )
    {
      gw->PercentToWorld( x1, y1, x1w, y1w );
      gw->PercentToWorld( x2, y2, x2w, y2w );
    }
    else
    {
      x1w = x1;
      y1w = y1;
      x2w = x2;
      y2w = y2;
    }
    GRA_ellipse *ellipse = new GRA_ellipse(x1w,y1w,x2w,y2w,false,cc,fc,lw,1);
    gw->AddDrawableObject( ellipse );
  }
  ExGlobals::RefreshGraphics();
}

// end of file
