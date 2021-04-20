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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <iostream>
#include <ios>

#include <ctime>

#include <libxml/parser.h>

#include "ExXML.h"

void ExXML::OpenFileForRead( ExString file )
{
  reader_ = xmlNewTextReaderFilename( file.c_str() );
  if( !reader_ )
  {
    ExString tmp("Unable to open ");
    tmp += file;
    throw runtime_error( tmp.c_str() );
  }
  if( xmlTextReaderSetParserProp(reader_,XML_PARSER_VALIDATE,1) )
  {
    ExString tmp("Unable to validate the xml in file ");
    tmp += file;
    throw runtime_error( tmp.c_str() );
  }
}

int ExXML::NextElementNode()
{
  int result = xmlTextReaderRead(reader_);
  while( (result==1) && (xmlTextReaderNodeType(reader_)!=1) )
    result = xmlTextReaderRead(reader_);
  return result;
}

ExString ExXML::GetName()
{
  ExString name;
  xmlChar const *val = const_cast<xmlChar const*>(xmlTextReaderName(reader_));
  if( val )
  {
    name = ExString( my_cast<char const*>(val) );
    xmlFree( (void*)val );
  }
  return name;
}

ExString ExXML::GetTextValue()
{
  ExString value;
  if( xmlTextReaderRead(reader_) == 1 )
  {
    xmlChar const *val = const_cast<xmlChar const*>(xmlTextReaderValue(reader_));
    if( val )
    {
      if( xmlTextReaderNodeType(reader_)==3 || xmlTextReaderNodeType(reader_)==4 )
        value = ExString( my_cast<char const*>(val) );
      xmlFree( (void*)val );
    }
  }
  return value;
}

ExString ExXML::GetAttributeValue( ExString const &name )
{
  xmlChar *val = xmlTextReaderGetAttribute(reader_,my_cast<xmlChar const*>(name.c_str()));
  ExString value;
  if( val )
  {
    value = ExString( my_cast<char const*>(const_cast<xmlChar const*>(val)) );
    xmlFree( (void*)val );
  }
  return value;
}

void ExXML::OpenFileForWrite( ExString file )
{
  if( file.ExtractFileExt().empty() )file += ".xml";
  if( outputFile_.is_open() )outputFile_.close();
  std::ios_base::openmode mode = std::ios_base::out|std::ios_base::trunc;
  outputFile_.open( file.c_str(), mode );
  if( !outputFile_ )
  {
    ExString tmp("Unable to open file ");
    tmp += file;
    throw runtime_error( tmp.c_str() );
  }
  // write XML header, including the DTD
  // + means 1 or more
  // * means 0 or more
  // ? means 0 or 1
  //
  time_t now;
  time( &now );
  ExString temp( ctime(&now) );
  temp.erase( temp.size()-1 );
  outputFile_ << "<?xml version=\"" << version_ << "\" encoding=\"" << encoding_ << "\"?>\n"
    << "<!-- created by extrema on " << temp << " -->\n"
    << "<!DOCTYPE extrema [\n"
    << "<!ELEMENT extrema (analysiswindow,visualizationwindow,numericvariables?,textvariables?,colormap,graphicspage*)>\n"
    << "<!ELEMENT analysiswindow (messages*,commands*)>\n"
    << "<!ELEMENT visualizationwindow EMPTY>\n"
    << "<!ELEMENT numericvariables (scalar|vector|matrix|tensor)*>\n"
    << "<!ELEMENT textvariables (textscalar|textvector)*>\n"
    << "<!ELEMENT graphicspage (graphwindow*)>\n"
    << "<!ELEMENT colormap (color*)>\n"
    << "<!ELEMENT messages (string*)>\n"
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
    << "<!ELEMENT ticcordinates (data*)>\n"
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
    << "<!ELEMENT graphlegend (entry*)>\n"
    << "<!ELEMENT entry (string,plotsymbol*)>\n"
    << "<!ELEMENT contour (data,drawabletexts)>\n"
    << "<!ELEMENT boxplot (data,data,data)>\n"
    << "<!ELEMENT diffusionplot (data,data,data)>\n"
    << "<!ELEMENT ditheringplot (data,data,data,data,data)>\n\n"
    << "<!ELEMENT gradientplot (data,data,data)>\n"
    << "<!ATTLIST analysiswindow top CDATA #REQUIRED left CDATA #REQUIRED height CDATA #REQUIRED width CDATA #REQUIRED novicemode CDATA #REQUIRED varshow CDATA #REQUIRED readvec CDATA #REQUIRED readscal CDATA #REQUIRED readmat CDATA #REQUIRED readtxt CDATA #REQUIRED exec CDATA #REQUIRED writevec CDATA #REQUIRED writescal CDATA #REQUIRED writemat CDATA #REQUIRED writetxt CDATA #REQUIRED fit CDATA #REQUIRED generate CDATA #REQUIRED>\n"
    << "<!ATTLIST areafillcolors size CDATA #REQUIRED>\n"
    << "<!ATTLIST messages size CDATA #REQUIRED>\n"
    << "<!ATTLIST commands size CDATA #REQUIRED index CDATA #REQUIRED>\n"
    << "<!ATTLIST visualizationwindow top CDATA #REQUIRED left CDATA #REQUIRED width CDATA #REQUIRED height CDATA #REQUIRED aspectratio CDATA #REQUIRED currentpage CDATA #REQUIRED figure CDATA #REQUIRED peak CDATA #REQUIRED graph CDATA #REQUIRED text CDATA #REQUIRED>\n"
    << "<!ATTLIST graphicspage currentwindow CDATA #REQUIRED>\n"
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
    << "<!ATTLIST cartesiancurve histyp CDATA #REQUIRED linetype CDATA #REQUIRED linewidth CDATA #REQUIRED smooth CDATA #REQUIRED linecolor CDATA #REQUIRED areafillcolor CDATA #REQUIRED xlaxis CDATA #REQUIRED ylaxis CDATA #REQUIRED xuaxis CDATA #REQUIRED yuaxis CDATA #REQUIRED xmin CDATA #REQUIRED xmax CDATA #REQUIRED ymin CDATA #REQUIRED ymax CDATA #REQUIRED>\n"
    << "<!ATTLIST graphlegend size CDATA #REQUIRED>\n"
    << "<!ATTLIST entry line CDATA #REQUIRED linetype CDATA #REQUIRED linewidth CDATA #REQUIRED linecolor CDATA #REQUIRED size CDATA #REQUIRED>\n"
    << "<!ATTLIST contour color CDATA #REQUIRED>\n"
    << "<!ATTLIST boxplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED bscale CDATA #REQUIRED>\n"
    << "<!ATTLIST diffusionplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED>\n"
    << "<!ATTLIST ditheringplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED legend CDATA #REQUIRED equalspaced CDATA #REQUIRED areas CDATA #REQUIRED volumes CDATA #REQUIRED lines CDATA #REQUIRED>\n"
    << "<!ATTLIST gradientplot nrow CDATA #REQUIRED fmin CDATA #REQUIRED fmax CDATA #REQUIRED gmin CDATA #REQUIRED gmax CDATA #REQUIRED xprofile CDATA #REQUIRED yprofile CDATA #REQUIRED border CDATA #REQUIRED zoom CDATA #REQUIRED axes CDATA #REQUIRED reset CDATA #REQUIRED legend CDATA #REQUIRED linear CDATA #REQUIRED>\n"
    << "]>\n";
}

// end of file
