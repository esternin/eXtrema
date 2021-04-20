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

#include "GRA_distanceCharacteristic.h"

double GRA_distanceCharacteristic::GetAsPercent() const
{
  double result;
  setAsPercent_ ? result=value_ : result=(value_-min_)/(max_-min_)*100.0;
  return std::min(100.0,std::max(0.0,result));
}

std::vector<double> GRA_distanceCharacteristic::GetAsPercents() const
{
  std::vector<double> results;
  std::vector<double>::const_iterator end = values_.end();
  for( std::vector<double>::const_iterator i=values_.begin(); i!=end; ++i )
  {
    double result;
    setAsPercent_ ? result=(*i) : result=((*i)-min_)/(max_-min_)*100.0;
    results.push_back(std::min(100.0,std::max(0.0,result)));
  }
  return results;
}

double GRA_distanceCharacteristic::GetAsWorld() const
{
  double result;
  setAsPercent_ ? result=min_+value_*(max_-min_)/100.0 : result=value_;
  return std::min(max_,std::max(min_,result));
}

std::vector<double> GRA_distanceCharacteristic::GetAsWorlds() const
{
  std::vector<double> results;
  std::vector<double>::const_iterator end = values_.end();
  for( std::vector<double>::const_iterator i=values_.begin(); i!=end; ++i )
  {
    double result;
    setAsPercent_ ? result=min_+(*i)*(max_-min_)/100.0 : result=(*i);
    results.push_back(std::min(max_,std::max(min_,result)));
  }
  return results;
}

std::ostream &operator<<( std::ostream &out, GRA_distanceCharacteristic const &c )
{
  if( c.isAVector_ )
  {
    std::vector<double> values( c.GetAsPercents() );
    std::size_t size = values.size();
    out << "<characteristic name=\"" << c.name_ << "\""
        << " type=\"DISTANCE\" shape=\"VECTOR\">\n"
        << "<data size=\"" << size << "\">";
    for( std::size_t i=0; i<size-1; ++i )out << values[i] << " ";
    out << values[size-1] << "</data>\n</characteristic>\n";
  }
  else
  {
    if( c.value_ != c.default_ )
    {
      out << "<characteristic name=\"" << c.name_ << "\""
          << " type=\"DISTANCE\" shape=\"SCALAR\" value=\"" << c.GetAsPercent() << "\"/>\n";
    }
  }
  return out;
}

void GRA_distanceCharacteristic::CopyStuff( GRA_distanceCharacteristic const &rhs )
{
  min_ = rhs.min_;
  max_ = rhs.max_;
  value_ = rhs.value_;
  default_ = rhs.default_;
  values_.assign( rhs.values_.begin(), rhs.values_.end() );
  setAsPercent_ = rhs.setAsPercent_;
}

// end of file
