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
#ifndef EX_BNDCHK
#define EX_BNDCHK

class Workspace;
class NumericVariable;

class BndChk
{
public:
  BndChk()
  {}
  
  ~BndChk()
  {}

public:
  void NVarType()
  { type_ = NUMERICVARIABLE; }
  
  void WrkspcType()
  { type_ = WORKSPACE; }
  
  bool IsaNVar() const
  { return (type_ == NUMERICVARIABLE); }
  
  bool IsaWrkspc() const
  { return (type_ == WORKSPACE); }
  
  void NVarPtr( NumericVariable const *v )
  {
    type_ = NUMERICVARIABLE;
    numericVariable_ = v;
  }
  
  NumericVariable const *NVarPtr() const
  { return numericVariable_; }
  
  void WorkspacePtr( Workspace const *w )
  {
    type_ = WORKSPACE;
    workspace_ = w;
  }
  
  Workspace const *WorkspacePtr() const
  { return workspace_; }
  
  void Dimension( unsigned int i )
  { dimension_ = i; }
  
  unsigned int Dimension() const
  { return dimension_; }
  
private:
  enum TokenType { NUMERICVARIABLE, WORKSPACE };
  TokenType type_;
  NumericVariable const *numericVariable_;
  Workspace const *workspace_;
  unsigned int dimension_;
};

#endif
   
