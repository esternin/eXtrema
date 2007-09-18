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
#include <stdexcept>
#include <iostream>
#include <ios>
#include <ctime>

#include "ExXML.h"
#include "ExGlobals.h"

void ExXML::OpenFileForRead( wxString const &file )
{
  //if( !doc_.Load(file,encoding_,wxXMLDOC_KEEP_WHITESPACE_NODES) )
  if( !doc_.Load(file,encoding_) )
  {
    wxString tmp(wxT("Unable to open "));
    tmp += file;
    throw std::runtime_error( std::string(tmp.mb_str(wxConvUTF8)) );
  }
  rootNode_ = doc_.GetRoot();

  if( rootNode_->GetName() != wxT("extrema") )
    throw std::runtime_error(
      std::string((wxString()<<wxT("root node extrema not found in file ")<<file).mb_str(wxConvUTF8)) );
  
  currentNode_ = rootNode_;
}

bool ExXML::GetFirstChild()
{
  if( !currentNode_ )return false;

  wxXmlNode *node0 = currentNode_;

  currentNode_ = currentNode_->GetChildren();
  if( !currentNode_ )
  {
    currentNode_ = node0;
    
    //std::cout << "GetFirstChild 0: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
    //          << "| type=" << currentNode_->GetType() << "\n";

    return false;
  }
    
  //std::cout << "GetFirstChild 1: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
  //          << "| type=" << currentNode_->GetType() << "\n";

  node0 = currentNode_;
  if( currentNode_->GetType() == wxXML_TEXT_NODE ||
      currentNode_->GetType() == wxXML_CDATA_SECTION_NODE )
  {
    currentNode_ = currentNode_->GetNext();
    if( !currentNode_ )
    {
      currentNode_ = node0;
    
      //std::cout << "GetFirstChild 2: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
      //          << "| type=" << currentNode_->GetType() << "\n";

      return false;
    }
  }
    
  //std::cout << "GetFirstChild 3: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
  //          << "| type=" << currentNode_->GetType() << "\n";

  return true;
}

bool ExXML::GetNextSibling()
{
  if( !currentNode_ )return false;
  wxXmlNode *node0 = currentNode_;
  currentNode_ = currentNode_->GetNext();
  if( !currentNode_ )
  {
    currentNode_ = node0;
    
    //std::cout << "GetNextSibling 1: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
    //          << "| type=" << currentNode_->GetType() << "\n";
    
    return false;
  }
  if( currentNode_->GetType() == wxXML_TEXT_NODE ||
      currentNode_->GetType() == wxXML_CDATA_SECTION_NODE )
  {
    currentNode_ = currentNode_->GetNext();
    if( !currentNode_ )
    {
      currentNode_ = node0;
      
      //std::cout << "GetNextSibling 2: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
      //          << "| type=" << currentNode_->GetType() << "\n";
      
      return false;
    }
  }
    
  //std::cout << "GetNextSibling 3: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
  //          << "| type=" << currentNode_->GetType() << "\n";
    
  return true;
}

wxString ExXML::GetName()
{
  if( !currentNode_ )return wxString();
  return currentNode_->GetName();
}

wxString ExXML::GetPropertyValue( wxString const &name )
{
  if( !currentNode_ )throw std::runtime_error( "current node is NULL" );
  wxString value;
  if( !currentNode_->GetPropVal(name,&value) )
    throw std::runtime_error( std::string( (wxString()<<wxT("node ")<<currentNode_->GetName()
                                           <<wxT(" should have a property named ")<<name).mb_str(wxConvUTF8) ) );
  return value;
}

wxString ExXML::GetTextValue()
{
  if( !currentNode_ )return wxString();
  wxXmlNode *node = currentNode_->GetChildren();
  while( node )
  {
    if( node->GetType() == wxXML_TEXT_NODE ||
        node->GetType() == wxXML_CDATA_SECTION_NODE )return node->GetContent();
    node = node->GetNext();
  }
  return wxString();
}

void ExXML::SetBackToParent()
{
  if( !currentNode_ )return;
  currentNode_ = currentNode_->GetParent();
  
  //std::cout << "SetBackToParent: name=|"<<currentNode_->GetName().mb_str(wxConvUTF8)
  //          << "| type=" << currentNode_->GetType() << "\n";
}

void ExXML::OpenFileForWrite( wxString file )
{
  // get the file extension, if there was one given
  //
  wxString extension( ExGlobals::GetFileExtension(file) );
  if( extension.empty() )file += wxT(".xml");
  if( output_.is_open() )output_.close();
  std::ios_base::openmode mode = std::ios_base::out|std::ios_base::trunc;
  output_.open( file.mb_str(wxConvUTF8), mode );
  if( !output_ )
  {
    wxString tmp(wxT("Unable to open file "));
    tmp += file;
    throw std::runtime_error( std::string(tmp.mb_str(wxConvUTF8)) );
  }
  // write XML header, including the DTD
  // + means 1 or more
  // * means 0 or more
  // ? means 0 or 1
  //
  wxDateTime now = wxDateTime::Now();
  wxString datetime = now.Format(wxT("%c"));
  
  output_ << "<?xml version=\"" << version_.mb_str(wxConvUTF8) << "\" encoding=\""
          << encoding_.mb_str(wxConvUTF8) << "\"?>\n"
          << "<!-- created by extrema on " << datetime.mb_str(wxConvUTF8) << " -->\n"
          << "<!DOCTYPE extrema [\n"
          << "<!ELEMENT extrema (analysiswindow,visualizationwindow,numericvariables,textvariables,colormap,graphicspage*)>\n"
          << "<!ELEMENT analysiswindow (string,commands)>\n"
          << "<!ELEMENT visualizationwindow EMPTY>\n"
          << "<!ELEMENT numericvariables (scalar|vector|matrix|tensor)*>\n"
          << "<!ELEMENT textvariables (textscalar|textvector)*>\n"
          << "<!ELEMENT colormap (color*)>\n"
          << "<!ELEMENT graphicspage (graphwindow*)>\n"
          << "<!ELEMENT commands (string*)>\n"
          << "<!ELEMENT string (#PCDATA)>\n"
          << "<!ELEMENT scalar (data,history,origin)>\n"
          << "<!ELEMENT vector (data,history,origin)>\n"
          << "<!ELEMENT matrix (data,history,origin)>\n"
          << "<!ELEMENT tensor (data,history,origin)>\n"
          << "<!ELEMENT origin (#PCDATA)>\n"
          << "<!ELEMENT textscalar (string,history,origin)>\n"
          << "<!ELEMENT data (#PCDATA)>\n"
          << "<!ELEMENT history (string+)>\n"
          << "<!ELEMENT color EMPTY>\n"
          << "<!ELEMENT textvector (string+,history,origin)>\n"
          << "<!ELEMENT graphwindow (axisc,axisc,generalc,textc,graphlegendc,datacurvec,drawableobjects)>\n"
          << "<!ELEMENT axisc (characteristic*)>\n"
          << "<!ELEMENT generalc (characteristic*)>\n"
          << "<!ELEMENT textc (characteristic*)>\n"
          << "<!ELEMENT graphlegendc (characteristic*)>\n"
          << "<!ELEMENT datacurvec (characteristic*)>\n"
          << "<!ELEMENT characteristic (data?)>\n"
          << "<!ELEMENT font EMPTY>\n"
          << "<!ELEMENT drawableobjects (point|polyline|ellipse|polygon|multilinefigure|plotsymbol|drawabletext|cartesianaxes|cartesiancurve|graphlegend|contour|boxplot|diffusionplot|ditheringplot|gradientplot)*>\n"
          << "<!ELEMENT point EMPTY>\n"
          << "<!ELEMENT polyline (data)>\n"
          << "<!ELEMENT ellipse EMPTY>\n"
          << "<!ELEMENT polygon (data)>\n"
          << "<!ELEMENT multilinefigure (data)>\n"
          << "<!ELEMENT plotsymbol EMPTY>\n"
          << "<!ELEMENT drawabletext (string)>\n"
          << "<!ELEMENT cartesianaxes (axis+)>\n"
          << "<!ELEMENT axis (ticcoordinates,axisc,polylines,drawabletexts)>\n"
          << "<!ELEMENT ticcoordinates (data*)>\n"
          << "<!ELEMENT polylines (polyline*)>\n"
          << "<!ELEMENT drawabletexts (drawabletext*)>\n"
          << "<!ELEMENT cartesiancurve (areafillcolors,data,xe1,xe2,ye1,ye2,xycurve,pen,plotsymbols,errorbars)>\n"
          << "<!ELEMENT xe1 (#PCDATA)>\n"
          << "<!ELEMENT xe2 (#PCDATA)>\n"
          << "<!ELEMENT ye1 (#PCDATA)>\n"
          << "<!ELEMENT ye2 (#PCDATA)>\n"
          << "<!ELEMENT xycurve (#PCDATA)>\n"
          << "<!ELEMENT pen (#PCDATA)>\n"
          << "<!ELEMENT areafillcolors (#PCDATA)>\n"
          << "<!ELEMENT plotsymbols (plotsymbol*)>\n"
          << "<!ELEMENT errorbars (errorbar*)>\n"
          << "<!ELEMENT errorbar EMPTY>\n"
          << "<!ELEMENT graphlegend (legendentry*,polygon,drawabletext)>\n"
          << "<!ELEMENT legendentry (drawabletext,plotsymbol)>\n"
          << "<!ELEMENT contour (data,drawabletexts)>\n"
          << "<!ELEMENT boxplot (data,data,data)>\n"
          << "<!ELEMENT diffusionplot (data,data,data)>\n"
          << "<!ELEMENT ditheringplot (data,data,data,data,data)>\n"
          << "<!ELEMENT gradientplot (data,data,data)>\n"
          << "<!ATTLIST analysiswindow top CDATA #REQUIRED left CDATA #REQUIRED height CDATA #REQUIRED width CDATA #REQUIRED>\n"
          << "<!ATTLIST visualizationwindow top CDATA #REQUIRED left CDATA #REQUIRED height CDATA #REQUIRED width CDATA #REQUIRED aspectratio CDATA #REQUIRED currentpage CDATA #REQUIRED>\n"
          << "<!ATTLIST graphicspage currentwindow CDATA #REQUIRED>\n"
          << "<!ATTLIST axis xorigin CDATA #REQUIRED yorigin CDATA #REQUIRED>\n"
          << "<!ATTLIST areafillcolors size CDATA #REQUIRED>\n"
          << "<!ATTLIST commands size CDATA #REQUIRED>\n"
          << "<!ATTLIST numericvariables size CDATA #REQUIRED>\n"
          << "<!ATTLIST scalar name CDATA #REQUIRED type (FIXED|FIT) #REQUIRED>\n"
          << "<!ATTLIST vector name CDATA #REQUIRED size CDATA #REQUIRED order (UNORDERED|ASCENDING|DESCENDING) #REQUIRED>\n"
          << "<!ATTLIST matrix name CDATA #REQUIRED rows CDATA #REQUIRED columns CDATA #REQUIRED>\n"
          << "<!ATTLIST tensor name CDATA #REQUIRED rows CDATA #REQUIRED columns CDATA #REQUIRED planes CDATA #REQUIRED>\n"
          << "<!ATTLIST textvariables size CDATA #REQUIRED>\n"
          << "<!ATTLIST textscalar name CDATA #REQUIRED>\n"
          << "<!ATTLIST textvector name CDATA #REQUIRED size CDATA #REQUIRED>\n"
          << "<!ATTLIST data size CDATA #REQUIRED>\n"
          << "<!ATTLIST xe1 size CDATA #REQUIRED>\n"
          << "<!ATTLIST xe2 size CDATA #REQUIRED>\n"
          << "<!ATTLIST ye1 size CDATA #REQUIRED>\n"
          << "<!ATTLIST ye2 size CDATA #REQUIRED>\n"
          << "<!ATTLIST xycurve size CDATA #REQUIRED>\n"
          << "<!ATTLIST pen size CDATA #REQUIRED>\n"
          << "<!ATTLIST plotsymbols size CDATA #REQUIRED>\n"
          << "<!ATTLIST errorbars size CDATA #REQUIRED>\n"
          << "<!ATTLIST errorbar x CDATA #REQUIRED y CDATA #REQUIRED linewidth CDATA #REQUIRED linecolor CDATA #REQUIRED bottom CDATA #REQUIRED top CDATA #REQUIRED vertical CDATA #REQUIRED footsize CDATA #REQUIRED>\n"
          << "<!ATTLIST history size CDATA #IMPLIED>\n"
          << "<!ATTLIST colormap name CDATA #REQUIRED size CDATA #IMPLIED>\n"
          << "<!ATTLIST color r CDATA #REQUIRED g CDATA #REQUIRED b CDATA #REQUIRED>\n"
          << "<!ATTLIST graphwindow number CDATA #REQUIRED xPrevious CDATA #REQUIRED yPrevious CDATA #REQUIRED xLoP CDATA #REQUIRED yLoP CDATA #REQUIRED xHiP CDATA #REQUIRED yHiP CDATA #REQUIRED>\n"
          << "<!ATTLIST characteristic name CDATA #REQUIRED type (SIZE|DISTANCE|INT|DOUBLE|BOOL|ANGLE|FONT|COLOR|STRING) #REQUIRED shape (SCALAR|VECTOR) #REQUIRED value CDATA #IMPLIED>\n"
          << "<!ATTLIST font name CDATA #REQUIRED>\n"
          << "<!ATTLIST drawableobjects size CDATA #REQUIRED>\n"
          << "<!ATTLIST drawabletext x CDATA #REQUIRED y CDATA #REQUIRED height CDATA #REQUIRED angle CDATA #REQUIRED alignment CDATA #REQUIRED color CDATA #REQUIRED font CDATA #REQUIRED graphunits CDATA #REQUIRED>\n"
          << "<!ATTLIST point x CDATA #REQUIRED y CDATA #REQUIRED color CDATA #REQUIRED>\n"
          << "<!ATTLIST polyline linewidth CDATA #REQUIRED linetype CDATA #REQUIRED linecolor CDATA #REQUIRED>\n"
          << "<!ATTLIST multilinefigure linewidth CDATA #REQUIRED linetype CDATA #REQUIRED linecolor CDATA #REQUIRED>\n"
          << "<!ATTLIST polygon linewidth CDATA #REQUIRED linetype CDATA #REQUIRED linecolor CDATA #REQUIRED fillcolor CDATA #REQUIRED>\n"
          << "<!ATTLIST ellipse xmin CDATA #REQUIRED ymin CDATA #REQUIRED xmax CDATA #REQUIRED ymax CDATA #REQUIRED circle CDATA #REQUIRED linecolor CDATA #REQUIRED fillcolor CDATA #REQUIRED linewidth CDATA #REQUIRED linetype CDATA #REQUIRED>\n"
          << "<!ATTLIST plotsymbol x CDATA #REQUIRED y CDATA #REQUIRED shape CDATA #REQUIRED angle CDATA #REQUIRED size CDATA #REQUIRED color CDATA #REQUIRED linewidth CDATA #REQUIRED>\n"
          << "<!ATTLIST cartesianaxes size CDATA #REQUIRED>\n"
          << "<!ATTLIST ticcoordinates size CDATA #REQUIRED>\n"
          << "<!ATTLIST polylines size CDATA #REQUIRED>\n"
          << "<!ATTLIST drawabletexts size CDATA #REQUIRED>\n"
          << "<!ATTLIST cartesiancurve histype CDATA #REQUIRED linetype CDATA #REQUIRED linewidth CDATA #REQUIRED smooth CDATA #REQUIRED linecolor CDATA #REQUIRED areafillcolor CDATA #REQUIRED xlaxis CDATA #REQUIRED ylaxis CDATA #REQUIRED xuaxis CDATA #REQUIRED yuaxis CDATA #REQUIRED xmin CDATA #REQUIRED xmax CDATA #REQUIRED ymin CDATA #REQUIRED ymax CDATA #REQUIRED>\n"
          << "<!ATTLIST graphlegend size CDATA #REQUIRED frameon CDATA #REQUIRED titleon CDATA #REQUIRED xlo CDATA #REQUIRED ylo CDATA #REQUIRED xhi CDATA #REQUIRED yhi CDATA #REQUIRED>\n"
          << "<!ATTLIST legendentry nsymbols CDATA #REQUIRED line CDATA #REQUIRED linetype CDATA #REQUIRED linewidth CDATA #REQUIRED linecolor CDATA #REQUIRED>\n"
          << "<!ATTLIST contour color CDATA #REQUIRED>\n"
          << "<!ATTLIST boxplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED bscale CDATA #REQUIRED>\n"
          << "<!ATTLIST diffusionplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED>\n"
          << "<!ATTLIST ditheringplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED legend CDATA #REQUIRED equalspaced CDATA #REQUIRED areas CDATA #REQUIRED volumes CDATA #REQUIRED lines CDATA #REQUIRED>\n"
          << "<!ATTLIST gradientplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED legend CDATA #REQUIRED linear CDATA #REQUIRED>\n"
          << "]>\n";
}

// end of file
