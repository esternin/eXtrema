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
#ifndef GRA_VISUALIZATIONWINDOW
#define GRA_VISUALIZATIONWINDOW

class GRA_visualizationwindow
{
public:
  GRA_visualizationWindow()
  {}
  
  virtual ~GRA_visualizationWindow()
  { DeleteStuff(); }

  GRA_visualizationWindow( GRA_visualizationWindow const &rhs )
  { CopyStuff( rhs ); }
  
  GRA_visualizationWindow &operator=( GRA_visualizationWindow const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      CopyStuff( rhs );
    }
    return *this;
  }

  friend std::ostream &operator<<( std::ostream &, GRA_visualizationWindow const & );

private:
  void DeleteStuff();
  void CopyStuff( GRA_visualizationWindow const & );
};

#endif
