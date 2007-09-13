/*
Copyright (C) 2006,...,2007 Joseph L. Chuma, TRIUMF

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
#ifndef EXTREMA_XML_H
#define EXTREMA_XML_H

#include <fstream>
#include <string>
#include <vector>

#include "wx/wx.h"
#include "wx/xml/xml.h"

class ExXML
{
public:
  ExXML() : encoding_(wxT("UTF-8")), version_(wxT("1.0"))
  {}
  
  ~ExXML()
  {}
  
  void OpenFileForRead( wxString const & );
  void OpenFileForWrite( wxString );

  void CloseFile()
  { output_.close(); }
  
  std::ofstream *GetStream()
  { return &output_; }
  
  bool GetFirstChild();
  bool GetNextSibling();
  wxString GetName();
  wxString GetTextValue();
  wxString GetPropertyValue( wxString const & );
  void SetBackToParent();
  
protected:
  wxXmlDocument doc_;
  wxString encoding_, version_;
  wxXmlNode *rootNode_, *currentNode_;
  std::ofstream output_;
};

#endif
