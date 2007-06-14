/*
Copyright (C) 2006 Joseph L. Chuma, TRIUMF

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

#include "wxPLplotstream.h"

#include "GRA_color.h"
#include "GRA_colorMap.h"
#include "GRA_colorControl.h"
#include "EGraphicsError.h"

wxPLplotstream::wxPLplotstream( wxDC *dc, int width, int height )
    : plstream(), dc_(dc), width_(width), height_(height)
{
  //::plstream();
  sdev( "wxwidgets" );
  spage( 0.0, 0.0, width_, height_, 0, 0 );
  SetOpt( "text", "1" );    // use freetype
  SetOpt( "smooth", "1" );  // antialiased text
  //
  // set the background colour for the dc
  wxColour colour( dc_->GetBackground().GetColour() );
  plscolbg( (int)colour.Red(), (int)colour.Green(), (int)colour.Blue() );
  plscol0( 1, 0,0,0 ); // black
  //
  GRA_colorMap *cmap = GRA_colorControl::GetColorMap();
  int const cmapSize = cmap->GetSize();
  int *rgb;
  if( (rgb=(int*)malloc(3*cmapSize*sizeof(int))) == 0 )
    throw EGraphicsError( wxT("unable to allocate memory for rgb colours in wxPLplotstream constructor") );
  for( int i=0; i<cmapSize; ++i )cmap->GetColor(i+1)->GetRGB( rgb[i], rgb[i+cmapSize], rgb[i+2*cmapSize] );
  plscmap1( rgb, rgb+cmapSize, rgb+2*cmapSize, cmapSize );
  free(rgb);
  init();
  plP_esc( PLESC_DEVINIT, (void*)dc_ );
}

void wxPLplotstream::set_stream()
{ plstream::set_stream(); }

void wxPLplotstream::SetSize( int width, int height )
{
  width_ = width;
  height_ = height;
  plP_esc( PLESC_CLEAR, NULL );
  wxSize size( width_, height_ );
  plP_esc( PLESC_RESIZE, (void*)&size );
}

void wxPLplotstream::RenewPlot()
{
  plP_esc( PLESC_CLEAR, NULL );
  replot();
}

/*
 In the wxWidgets application a wxMemoryDC must be created, e.g., in the constructor of a wxWindow,
 and made known to the driver, e.g.,
 
  wxMemoryDC *MemPlotDC = new wxMemoryDC;
  MemPlotDCBitmap = new wxBitmap( 640, 400, -1 );
  MemPlotDC->SelectObject( *MemPlotDCBitmap );
  my_stream = new wxPLplotstream( (wxDC*)MemPlotDC, MemPlotDC_width, MemPlotDC_height );
  
  
 The OnPaint() event handler looks like this (double buffering is used here)

 void plotwindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
 {
   int width, height;
   GetSize( &width, &height );
   //
   // Check if we window was resized (or dc is invalid)
   //
   if( (my_stream == NULL) || (MemPlotDC_width!=width) || (MemPlotDC_height!=height) )
   {
     MemPlotDC->SelectObject( wxNullBitmap );
     if( MemPlotDCBitmap )delete MemPlotDCBitmap;
     MemPlotDCBitmap = new wxBitmap( width, height, -1 );
     MemPlotDC->SelectObject( *MemPlotDCBitmap );
     my_stream->SetSize( width, height );
     my_stream->replot();
     MemPlotDC_width = width;
     MemPlotDC_height = height;
   }
   wxPaintDC dc( this );
   dc.SetClippingRegion( GetUpdateRegion() );
   dc.BeginDrawing();
   dc.Blit( 0, 0, width, height, MemPlotDC, 0, 0 );
   dc.EndDrawing();
 }
*/

// end of file
