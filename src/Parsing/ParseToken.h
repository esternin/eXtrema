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
#ifndef EX_PARSETOKEN
#define EX_PARSETOKEN

#include <vector>

#include "wx/wx.h"

class ParseToken
{
private:
  typedef std::vector<wxString*> stringVec;

public:
  enum Types { NULLFIELD, NUMERICFIELD, STRINGFIELD, EQUALSIGN };

  ParseToken() : tokenType_(NULLFIELD), stringToken_(wxT("")), numericToken_(0.0)
  {}

  ~ParseToken()
  { DeleteQualifiers(); }

  ParseToken( ParseToken const &rhs )
  { CopyStuff( rhs ); }

  ParseToken &operator=( ParseToken const & );
  bool operator==( ParseToken const & ) const;
  bool operator<( ParseToken const & ) const;

  void SetString( wxString const &s )
  {
    stringToken_ = s;
    tokenType_ = STRINGFIELD;
  }
  
  void SetNumeric( double v )
  {
    numericToken_ = v;
    tokenType_ = NUMERICFIELD;
  }

  void MakeNewQualifier()
  {
    wxString *tmp = new wxString();
    qualifiers_.push_back( tmp );
  }
  
  void SetType( Types t )
  { tokenType_ = t; }

  void SetNull()
  { tokenType_ = NULLFIELD; }
  
  void SetNumeric()
  { tokenType_ = NUMERICFIELD; }
  
  void SetString()
  { tokenType_ = STRINGFIELD; }
  
  void SetEqualSign()
  { tokenType_ = EQUALSIGN; }
  
  bool IsaNull() const
  { return (tokenType_ == NULLFIELD); }
  
  bool IsaNumeric() const
  { return (tokenType_ == NUMERICFIELD); }
  
  bool IsaString() const
  { return (tokenType_ == STRINGFIELD); }
  
  bool IsaEqualSign() const
  { return (tokenType_ == EQUALSIGN); }
  
  Types GetType() const
  { return tokenType_; }
  
  wxString &GetString()
  { return stringToken_; }
  
  double GetNumeric() const
  { return numericToken_; }

  void SetQualifier( wxString *q )
  { qualifiers_.back() = q; }
  
  stringVec const &GetQualifiers() const
  { return qualifiers_; }
  
  wxString *GetQualifier( int i ) const
  { return qualifiers_[i]; }
  
  wxString *GetLastQualifier() const
  { return qualifiers_.back(); }
  
  int GetNumberOfQualifiers() const
  { return qualifiers_.size(); }

private:
  wxString stringToken_;
  double numericToken_;
  stringVec qualifiers_;
  Types tokenType_;
  //
  void DeleteQualifiers();
  void CopyStuff( ParseToken const & );
};

#endif

