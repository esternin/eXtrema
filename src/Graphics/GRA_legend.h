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
#ifndef GRA_LEGEND
#define GRA_LEGEND

#include <ostream>
#include <vector>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_window;
class GRA_color;
class GRA_font;
class GRA_drawableText;
class GRA_rectangle;
class GRA_plotSymbol;
class GRA_wxWidgets;
class GRA_legendEntry;
class GRA_outputType;

class GRA_legend : public GRA_drawableObject
{
public:
  GRA_legend( GRA_window *gw)
    : GRA_drawableObject(wxT("GRAPHLEGEND")), graphWindow_(gw), frame_(0),
      title_(0), popup_(false)
  { Initialize(); }

  ~GRA_legend();

  GRA_legend( GRA_legend const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff(rhs); }

  GRA_legend &operator=( GRA_legend const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      type_ = rhs.type_;
      CopyStuff(rhs);
    }
    return *this;
  }

  void Initialize();
  void MakeFrame();
  void MakeTitle();

  void AddEntry( wxString const & );

  void AddEntry( GRA_legendEntry *entry )
  { entries_.push_back(entry); }
  
  std::vector<GRA_legendEntry*> const &GetEntries() const
  { return entries_; }
      
  void Draw( GRA_wxWidgets *, wxDC & );
  bool Inside( double, double );
  
  GRA_rectangle *GetFrame() const
  { return frame_; }

  GRA_drawableText *GetTitle() const
  { return title_; }

  double GetLineStart() const
  { return xLineStart_; }

  double GetLineEnd() const
  { return xLineEnd_; }
  
  double GetXLabel() const
  { return xLabel_; }
  
  void SetFrame( GRA_rectangle * );
  void SetTitle( GRA_drawableText * );

  GRA_window *GetGraphWindow() const
  { return graphWindow_; }
  
  double GetYHi() const
  { return yhi_; }
  
  int GetNumberOfEntries() const
  { return static_cast<int>(entries_.size()); }
  
  void SetPopup()
  { popup_ = true; }
  
  void Disconnect()
  { popup_ = false; }
  
  friend std::ostream &operator<<( std::ostream &, GRA_legend const & );

private:
  void DeleteStuff();
  void CopyStuff( GRA_legend const & );
  //
  GRA_window *graphWindow_;
  std::vector<GRA_legendEntry*> entries_;
  double xlo_, ylo_, xhi_, yhi_, xLineStart_, xLineEnd_, xLabel_;
  GRA_rectangle *frame_;
  GRA_drawableText *title_;
  bool popup_;
};

class GRA_legendEntry
{
public:
  GRA_legendEntry( GRA_legend *, wxString const &, double, GRA_font *, GRA_color *, int,
                   int, double, double, GRA_color *, int, bool, int, int, GRA_color * );

  ~GRA_legendEntry()
  { DeleteStuff(); }
  
  GRA_legendEntry( GRA_legendEntry const &rhs )
  { CopyStuff(rhs); }
  
  GRA_legendEntry &operator=( GRA_legendEntry const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      CopyStuff( rhs );
    }
    return *this;
  }
  
  GRA_drawableText *GetLabel() const
  { return label_; }
  
  double GetLabelHeight() const
  { return labelHeight_; }

  int GetEntryNumber() const
  { return entryNumber_; }

  bool GetDrawLineSegment() const
  { return drawLineSegment_; }

  int GetLineType() const
  { return lineType_; }
  
  int GetLineWidth() const
  { return lineWidth_; }
  
  GRA_color *GetLineColor() const
  { return lineColor_; }

  int GetNSymbols() const
  { return nSymbols_; }
  
  GRA_plotSymbol *GetPlotSymbol() const
  { return symbol_; }
  
  void Draw( GRA_wxWidgets *, wxDC & );
  
  friend std::ostream &operator<<( std::ostream &, GRA_legendEntry const & );

private:
  void DeleteStuff();
  void CopyStuff( GRA_legendEntry const & );
  
  GRA_legend *legend_;
  GRA_drawableText *label_;
  GRA_plotSymbol *symbol_;
  bool drawLineSegment_;
  double labelHeight_;
  int entryNumber_, nSymbols_, lineType_, lineWidth_;
  GRA_color *lineColor_, *labelColor_;
  GRA_font *labelFont_;
};

#endif
