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
#ifndef GRA_LEGEND
#define GRA_LEGEND

#include <ostream>
#include <vector>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_window;
class GRA_color;
class GRA_drawableText;
class GRA_rectangle;
class GRA_plotSymbol;
class GRA_wxWidgets;

class GRA_legend : public GRA_drawableObject
{
public:
  GRA_legend() : GRA_drawableObject(wxT("GRAPHLEGEND")), frame_(0), title_(0), firstTime_(true)
  {}

  ~GRA_legend()
  { DeleteStuff(); }

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

  void AddEntry( wxString &, bool, GRA_color *, int, int,
                 std::vector<GRA_plotSymbol*>, GRA_wxWidgets *, wxDC & );

  void Draw( GRA_wxWidgets *, wxDC & );

  GRA_rectangle *GetFrame() const
  { return frame_; }

  GRA_drawableText *GetTitle() const
  { return title_; }

  std::vector<GRA_drawableText*> *GetTextVec()
  { return &textVec_; }

  std::vector<bool> *GetEntryLines()
  { return &entryLines_; }

  std::vector<int> *GetLineTypes()
  { return &lineTypes_; }
  
  std::vector<int> *GetLineWidths()
  { return &lineWidths_; }

  std::vector<GRA_color*> *GetColors()
  { return &colors_; }
  
  void GetStartEnd( double &start, double &end )
  {
    start = xLineStart_;
    end = xLineEnd_;
  }

  std::vector<GRA_plotSymbol*> *GetSymbols( std::size_t i )
  { return &symbols_[i]; }
  
  friend std::ostream &operator<<( std::ostream &, GRA_legend const & );

private:
  void DeleteStuff();
  void CopyStuff( GRA_legend const & );
  void DrawFrameAndTitle( GRA_wxWidgets *, wxDC & );
  //
  bool firstTime_;
  double xlo_, ylo_, xhi_, yhi_, xLineStart_, xLineEnd_, xText_;
  GRA_rectangle *frame_;
  GRA_drawableText *title_;
  std::vector<wxString> labels_;
  std::vector<GRA_drawableText*> textVec_;
  std::vector< std::vector<GRA_plotSymbol*> > symbols_;
  std::vector<bool> entryLines_; // true=draw line through entry symbols
  std::vector<int> lineTypes_, lineWidths_;
  std::vector<GRA_color*> colors_;
};

#endif
