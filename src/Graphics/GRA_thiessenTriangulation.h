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
#ifndef GRA_THIESSENTRIANGULATION
#define GRA_THIESSENTRIANGULATION

#include <vector>

class GRA_thiessenTriangulation
{
public:
  GRA_thiessenTriangulation( std::vector<double>, std::vector<double>, std::vector<double> );
  ~GRA_thiessenTriangulation();
  GRA_thiessenTriangulation( GRA_thiessenTriangulation const & );
  GRA_thiessenTriangulation & operator=( GRA_thiessenTriangulation const & );
  void CreateMesh();
  void Gradients( double, std::size_t & );
  void Interpolate( std::size_t, std::size_t, std::vector<double> &, std::vector<double> &, std::vector<double> & );
  double Volume();

private:
  void QuickSort( std::vector<int> & );
  void Permute( std::vector<int> &, std::vector<double> & );
  void AddNode( std::size_t );
  void AddBoundaryNode( std::size_t, std::size_t, std::size_t );
  void AddInteriorNode( std::size_t, std::size_t, std::size_t, std::size_t );
  std::size_t Index( std::size_t, std::size_t );
  void Shift( std::size_t, std::size_t, int, std::vector<std::size_t> & );
  void Swap( std::size_t, std::size_t, std::size_t, std::size_t );
  bool SwapTest( std::size_t, std::size_t, std::size_t, std::size_t );
  void Find( std::size_t, double, double, std::size_t &, std::size_t &, std::size_t & );
  void InterpolateSub( double, double, double &, std::size_t & );
  double TriangleVolume( double, double, double, double, double, double, double, double, double );
  double tval( double, double, double, double, double, double, double, double,
               double, double, double, double, double, double, double, double, double );
  //
  std::vector<double> theX, theY, theZ;
  std::size_t theNumberOfNodes;
  //
  std::vector<std::size_t> iadj; // length = 6*theNumberOfNodes - 9
  // Adjacency lists of neighbors in counterclockwise order.
  // The list for node i+1 follows that for node i where theX and theY define the order.
  // The value 0 denotes the boundary (or a pseudo-node at infinity) and is always the
  // last neighbor of a boundary node.
  //
  std::vector<std::size_t> iend; // length = theNumberOfNodes
  // Pointers to the ends of adjacency lists (sets of neighbors) in iadj.
  // The neighbors of node 1 begin in iadj[0]. For k > 1, the neighbors of node k begin
  // in iadj[iend[k-2]] and k has iend[k-1] - iend[k-2] neighbors including
  // (possibly) the boundary.  iadj[iend[k-1]-1] == 0 iff node k is on the boundary.
  // If nodes are collinear, iend contains the indices of a sequence of nodes ordered
  // from left to right where left and right are defined by assuming node 1 is to the left
  // of node 2.
  //
  std::vector<double> xPartials, yPartials;
  // arrays which contain estimates of the partial derivatives
};
#endif

