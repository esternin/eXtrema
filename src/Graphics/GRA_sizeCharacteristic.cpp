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
#include <cmath>

#include "GRA_sizeCharacteristic.h"

double GRA_sizeCharacteristic::GetAsPercent() const
{
  double result;
  result = setAsPercent_ ? value_ : value_/(max_-min_)*100.0;
  return std::min(100.0,std::max(0.0,result));
}

std::vector<double> GRA_sizeCharacteristic::GetAsPercents() const
{
  double result;
  std::vector<double> results;
  std::vector<double>::const_iterator end = values_.end();
  for( std::vector<double>::const_iterator i=values_.begin(); i!=end; ++i )
  {
    result = setAsPercent_ ? (*i) : (*i)/(max_-min_)*100.0;
    results.push_back(std::min(100.0,std::max(0.0,result)));
  }
  return results;
}

double GRA_sizeCharacteristic::GetAsWorld() const
{
  double result;
  result = setAsPercent_ ? value_*(max_-min_)/100.0 : value_;
  return std::min(fabs(max_-min_),std::max(0.0,result));
}

std::vector<double> GRA_sizeCharacteristic::GetAsWorlds() const
{
  double result;
  std::vector<double> results;
  std::vector<double>::const_iterator end = values_.end();
  for( std::vector<double>::const_iterator i=values_.begin(); i!=end; ++i )
  {
    result = setAsPercent_ ? (*i)*(max_-min_)/100.0 : (*i);
    results.push_back(std::min(fabs(max_-min_),std::max(0.0,result)));
  }
  return results;
}

std::ostream &operator<<( std::ostream &out, GRA_sizeCharacteristic const &c )
{
  if( c.isAVector_ )
  {
    std::vector<double> values( c.GetAsPercents() );
    std::size_t size = values.size();
    out << "<characteristic name=\"" << c.name_.mb_str(wxConvUTF8) << "\""
        << " type=\"SIZE\" shape=\"VECTOR\">\n"
        << "<data size=\"" << size << "\">";
    for( std::size_t i=0; i<size-1; ++i )out << values[i] << " ";
    out << values[size-1] << "</data>\n</characteristic>\n";
  }
  else
  {
    if( c.value_ != c.default_ )
      out << "<characteristic name=\"" << c.name_.mb_str(wxConvUTF8) << "\""
          << " type=\"SIZE\" shape=\"SCALAR\" value=\"" << c.GetAsPercent() << "\"/>\n";
  }
  return out;
}

void GRA_sizeCharacteristic::CopyStuff( GRA_sizeCharacteristic const &rhs )
{
  name_ = rhs.name_;
  type_ = rhs.type_;
  isAVector_ = rhs.isAVector_;
  min_ = rhs.min_;
  max_ = rhs.max_;
  value_ = rhs.value_;
  default_ = rhs.default_;
  values_.assign( rhs.values_.begin(), rhs.values_.end() );
  setAsPercent_ = rhs.setAsPercent_;
}

// end of file
