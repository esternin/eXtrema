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

#include "EGraphicsError.h"
#include "GRA_thiessenTriangulation.h"

 GRA_thiessenTriangulation::GRA_thiessenTriangulation(
  std::vector<double> x, std::vector<double> y, std::vector<double> z )
     : theX(x), theY(y), theZ(z)
  {
    theNumberOfNodes = theX.size();
    if( theNumberOfNodes < 3 )
      throw EGraphicsError( "ThiessenTriangulation: number of nodes < 3" );
    //
    iadj.resize( 6*theNumberOfNodes-9, 0 );
    iend.resize( theNumberOfNodes, 0 );
    xPartials.resize( theNumberOfNodes, 0.0 );
    yPartials.resize( theNumberOfNodes, 0.0 );
    //
    // Use an order n*log(n) quick sort to reorder the array theX into increasing order
    // A record of the permutations applied to theX is stored in ind, and these
    // permutations are applied to two additional vectors
    //
    std::vector<int> ind( theNumberOfNodes );
    QuickSort( ind );
    Permute( ind, theX );
    Permute( ind, theY );
    Permute( ind, theZ );
  }

 GRA_thiessenTriangulation::~GRA_thiessenTriangulation()
  {}

 GRA_thiessenTriangulation::GRA_thiessenTriangulation( GRA_thiessenTriangulation const & )
  {}

 GRA_thiessenTriangulation &GRA_thiessenTriangulation::operator=( GRA_thiessenTriangulation const &rhs )
  { return *this; }

 void GRA_thiessenTriangulation::QuickSort( std::vector<int> &ind )
  {
    // This routine uses an order n*log(n) quick sort to sort the array theX into
    // increasing order.  The algorithm follows:
    //
    // ind is initialized to the ordered sequence of indices 1,...,n, and all interchanges
    // are applied to ind.  theX is divided into two portions by picking a central element
    // t.  The first and last elements are compared with t, and interchanges are applied
    // as necessary so that the three values are in ascending order.  Interchanges are then
    // applied so that all elements greater than t are in the upper portion of the array
    // and all elements less than t are in the lower portion.  The upper and lower indices
    // of one of the portions are saved in local arrays, and the process is repeated
    // iteratively on the other portion.  When a portion is completely sorted, the process
    // begins again by retrieving the indices bounding another unsorted portion.
    //
    // iu and il are temporary storage for the upper and lower indices of portions of theX
    // iu and il must be dimensioned >= log(theNumberOfNodes) where log has base 2.
    //
    double dn = static_cast<double>(theNumberOfNodes);
    size_t n = static_cast<size_t>(log(dn)/log(2.)+0.5);
    std::vector<size_t> iu( n );
    std::vector<size_t> il( n );
    //
    // initialize ind, m, i, j, and r
    //
    for( size_t i=1; i<=theNumberOfNodes; ++i )ind[i-1] = i;
    size_t m = 1;                // index for iu and il
    size_t i = 1;                // lower index of a portion of theX
    size_t j = theNumberOfNodes; // upper index for a portion of theX
    double r = 0.375;            // pseudo random number for generating index between i and j
    //
    // top of loop
    //
    size_t k, ij, it, itt, indx, l;
    double t;
  L2:
    if( i >= j )goto L10;
    r > 0.5898437 ? r -= 0.21875 : r += 0.0390625;
  L4:
    k = i; // initialize k
    //
    // select a central element of x and save it in t
    //
    ij = i + static_cast<size_t>(r*(j-i));
    it = ind[ij-1];
    t = theX[it-1];  // central element of theX
    //
    // if the first element of the array is greater than t, interchange it with t
    //
    indx = ind[i-1];
    if( theX[indx-1] <= t )goto L5;
    ind[ij-1] = indx;
    ind[i-1] = it;
    it = indx;
    t = theX[it-1];
  L5:
    l = j; // initialize l
    //
    // if the last element of the array is less than t, interchange it with t
    //
    indx = ind[j-1];
    if( theX[indx-1] >= t )goto L7;
    ind[ij-1] = indx;
    ind[j-1] = it;
    it = indx;
    t = theX[it-1];
    //
    // if the first element of the array is greater than t, interchange it with t
    //
    indx = ind[i-1];
    if( theX[indx-1] <= t )goto L7;
    ind[ij-1] = indx;
    ind[i-1] = it;
    it = indx;
    t = theX[it-1];
    goto L7;
  L6:
    //
    // interchange elements k and l
    //
    itt = ind[l-1];
    ind[l-1] = ind[k-1];
    ind[k-1] = itt;
  L7:
    //
    // find an element in the upper part of the array which is not larger than t
    //
    --l;
    indx = ind[l-1];
    if( theX[indx-1] > t )goto L7;
  L8:
    //
    // find an element in the lower part of the array which is not smaller than t
    //
    ++k;
    indx = ind[k-1];
    if( theX[indx-1] < t )goto L8;
    if( k <= l )goto L6;
    //
    // save the upper and lower subscripts of the portion of the array yet to be sorted
    //
    if( l-i <= j-k )
    {
      il[m-1] = k;
      iu[m-1] = j;
      j = l;
    }
    else
    {
      il[m-1] = i;
      iu[m-1] = l;
      i = k;
    }
    ++m;
    goto L11;
  L10:
    //
    // begin again on another unsorted portion of the array
    //
    if( --m == 0 )return;
    i = il[m-1];
    j = iu[m-1];
  L11:
    if( j-i >= 11 )goto L4;
    if( i == 1 )goto L2;
    --i;
  L12:
    ++i;
    if( i == j )goto L10;
    indx = ind[i];
    t = theX[indx-1];
    it = indx;
    indx = ind[i-1];
    if( theX[indx-1] <= t )goto L12;
    k = i;
  L13:
    ind[k] = ind[k-1];
    --k;
    indx = ind[k-1];
    if( t < theX[indx-1] )goto L13;
    ind[k] = it;
    goto L12;
  }

 void GRA_thiessenTriangulation::Permute( std::vector<int> &ip, std::vector<double> &a )
  {
    // This routine applies a set of permutations to a vector.
    //
    size_t const n = a.size();
    size_t k = 1; // index for ind and for the first element of a in a permutation
    size_t j, ipj;
    double temp;
  L1:
    j = k;
    temp = a[k-1];
  L2:
    //
    // Apply permutation to a.  ip(j) is marked (made negative)
    // as being included in the permutation
    //
    ipj = ip[j-1];
    ip[j-1] = -1*ipj;
    if( ipj == k )goto L3;
    a[j-1] = a[ipj-1];
    j = ipj;
    goto L2;
  L3:
    a[j-1] = temp;
  L4:
    //
    // search for an unmarked element of ip
    //
    if( ++k > n )goto L5;
    if( ip[k-1] > 0 )goto L1;
    goto L4;
  L5:
    for( size_t k=1; k<=n; ++k )ip[k-1] = -ip[k-1];
    return; // all permutations have been applied
  }

 void GRA_thiessenTriangulation::CreateMesh()
  {
    // Algorithm 624, Collected Algorithms from ACM.
    // ACM-Trans. Math. Software, vol.10, no. 4, Dec. 1984, p. 437
    //
    // This routine is called from outside.
    //
    // This routine creates a Thiessen triangulation of n arbitrarily spaced
    // points in the plane referred to as nodes.  The triangulation is optimal
    // in the sense that it is as nearly equiangular as possible.  This routine
    // is part of an interpolation package which also provides routines to
    // reorder the nodes, add a new node, delete an arc, plot the mesh, and
    // print the data structure. Unless the nodes are already ordered in some
    // reasonable fashion, they should be reordered by routine reordr for
    // increased efficiency before calling this routine.
    //
    // initialize
    //
    iend[0] = 1;
    iend[1] = 2;
    double xl = theX[0];
    double yl = theY[0];
    double xr = theX[1];
    double yr = theY[1];
    size_t k = 2;
    //
    // begin loop on nodes 3,4,...
    //
    double dxr = xr-xl;
    double dyr = yr-yl;
    //
    // next loop begins here if nl and nr are unchanged
    //
    // nl,nr   leftmost and rightmost nodes in iend as viewed from the right
    //         of 1-2 when iend contains the initial ordered set of nodal indices
    // cprod   vector cross product of nl-nr and nl-k
    //         used to determine the position of node k with respect to the
    //         line defined by the nodes in iend
    // sprod   scalar product used to determine the interval containing node k
    //         when k is on the line defined by the nodes in iend
    double sprod;
    size_t nl, nr, indx, n0, itemp, km1, km1d2, kmi;
    double xk, yk, dxk, dyk, cprod;
    size_t kmin;
  L2:
    if( k == theNumberOfNodes )throw EGraphicsError( "all nodes are collinear" );
    km1 = k++;
    xk = theX[k-1];
    yk = theY[k-1];
    dxk = xk-xl;
    dyk = yk-yl;
    cprod = dxr*dyk - dxk*dyr;
    if( cprod > 0. )goto L6;
    if( cprod < 0. )goto L8;
    //
    // node k lies on the line containing nodes 1,2,...,k-1.
    //   set sprod to (nl-nr,nl-k).
    //
    sprod = dxr*dxk + dyr*dyk;
    if( sprod > 0. )
    {
      //
      // node k is to the right of nl.  find the leftmost node
      //   n0 which lies to the right of k.
      //   set sprod to (n0-nl,n0-k).
      //
      for( size_t ind=2; ind<=km1; ++ind )
      {
        n0 = iend[ind-1];
        sprod = (xl-theX[n0-1])*(xk-theX[n0-1]) + (yl-theY[n0-1])*(yk-theY[n0-1]);
        if( sprod >= 0. ) // node k lies between iend(ind-1) and iend[ind-1]
        {                 // insert k in iend
          Shift( ind, km1, 1, iend );
          iend[ind-1] = k;
          goto L2;
        }
      }
      //
      // node k is to the right of nr
      // insert k as the last (rightmost) node in iend and set nr to k
      //
      iend[k-1] = k;
      xr = xk;
      yr = yk;
    }
    else
    {
      //
      // node k is to the left of nl
      // insert k as the first (leftmost) node in iend and set nl to k
      //
      Shift( 1, km1, 1, iend );
      iend[0] = k;
      xl = xk;
      yl = yk;
    }
    dxr = xr-xl;
    dyr = yr-yl;
    goto L2;
    //
    // node k is to the left of nl-nr.  reorder iend so that nl
    //   is the leftmost node as viewed from k.
    //
  L6:
    km1d2 = km1/2;
    for( size_t i=1; i<=km1d2; ++i )
    {
      kmi = k-i;
      itemp = iend[i-1];
      iend[i-1] = iend[kmi-1];
      iend[kmi-1] = itemp;
    }
    //
    // node k is to the right of nl-nr.  create a triangulation
    //   consisting of nodes 1,2,...,k.
    //
  L8:
    nl = iend[0];
    nr = iend[km1-1];
    //
    // create the adjacency lists for the first k-1 nodes.
    //   insert neighbors in reverse order.  each node has four
    //   neighbors except nl and nr which have three.
    //
    for( size_t ind=1; ind<=km1; ++ind )
    {
      n0 = iend[ind-1];
      indx = 4*n0;
      if( n0 >= nl )--indx;
      if( n0 >= nr )--indx;
      iadj[indx-1] = 0;
      --indx;
      if( ind < km1 )iadj[indx-1] = iend[ind];
      if( ind < km1 )--indx;
      iadj[indx-1] = k;
      if( ind > 1 )iadj[indx-2] = iend[ind-2];
    }
    //
    // create the adjacency list for node k
    //
    indx = 5*km1 - 1;
    iadj[indx-1] = 0;
    for( size_t ind=1; ind<=km1; ++ind )
    {
      --indx;
      iadj[indx-1] = iend[ind-1];
    }
    //
    // replace iend elements with pointers to iadj
    //
    indx = 0;
    for( size_t ind=1; ind<=km1; ++ind )
    {
      indx += 4;
      if( ind==nl || ind==nr )--indx;
      iend[ind-1] = indx;
    }
    indx += k;
    iend[k-1] = indx;
    //
    // add the remaining nodes to the triangulation
    //
    if( k == theNumberOfNodes )return;
    kmin = k+1;
    try
    {
      for( size_t k=kmin; k<=theNumberOfNodes; ++k )AddNode( k );
    }
    catch (EGraphicsError &e)
    {
      throw;
    }
  }
 
 void GRA_thiessenTriangulation::Gradients( double eps, size_t &numberOfIterations )
  {
    // This routine is called from outside.
    //
    // Given a triangulation of nodes in the plane with associated data values,
    // this routine uses a global method to compute estimated gradients at the nodes.
    // The method consists of minimizing a quadratic functional Q(g) over the
    // n-vector g of gradients where Q approximates the linearized curvature of an
    // interpolant F over the triangulation.  The restriction of F to an arc of the
    // triangulation is taken to be the Hermite cubic interpolant of the data values
    // and tangential gradient components at the endpoints of the arc, and Q is the
    // sum of the linearized curvatures of F along the arcs -- the integrals over the
    // arcs of d2f(t)^2 where d2f(t) is the second derivative of F with respect to
    // distance t along the arc. This minimization problem corresponds to an order 2n
    // symmetric positive-definite sparse linear system which is solved for the x and y
    // partial derivatives by the block Gauss-Seidel method with 2 by 2 blocks.
    //
    // eps - convergence criterion
    //       The method is terminated when the maximum change in a
    //       gradient component between iterations is at most eps
    //       eps = 1.e-2 is sufficient for effective convergence
    // numberOfIterations - maximum number of Gauss-Seidel iterations to be applied
    //       This maximum will likely be achieved if eps is smaller than
    //       the machine precision.  Optimal efficiency was achieved in
    //       testing with eps = 0 and numberOfIterations = 3 or 4
    //       When finished, it is number of iterations actually used
    //
    size_t maxit = numberOfIterations;
    size_t iter = 0; // number of iterations used
    double dgmax, xk, yk, zk, zxk, zyk, a11, a12, a22, r1, r2;
    size_t indl, indf;
    //
    // top of iteration loop
    //
  L1:
    if( iter == maxit )throw EGraphicsError(
     "Gradients: method failed to converge within given number of iterations" );
    dgmax = 0.;
    indl = 0;
    for( size_t k=1; k<=theNumberOfNodes; ++k )
    {
      xk = theX[k-1];
      yk = theY[k-1];
      zk = theZ[k-1];
      zxk = xPartials[k-1];
      zyk = yPartials[k-1];
      //
      // initialize components of the 2 by 2 system
      //
      a11 = 0.;
      a12 = 0.;
      a22 = 0.;
      r1 = 0.;
      r2 = 0.;
      //
      //   loop on neighbors nb of k
      //
      indf = indl + 1;
      indl = iend[k-1];
      for( size_t indx=indf; indx<=indl; ++indx )
      {
        size_t nb = iadj[indx-1];
        if( nb != 0 )
        {
          //
          //   compute the components of arc nb-k
          //
          double delx = theX[nb-1] - xk;
          double dely = theY[nb-1] - yk;
          double delxs = delx*delx;
          double delys = dely*dely;
          double dsq = delxs + delys;
          double dcub = dsq*sqrt(dsq);
          //
          //   update the system components for node nb
          //
          a11 = a11 + delxs/dcub;
          a12 = a12 + delx*dely/dcub;
          a22 = a22 + delys/dcub;
          double t = ( 1.5*(theZ[nb-1]-zk) -
                       ((xPartials[nb-1]/2.+zxk)*delx + (yPartials[nb-1]/2.+zyk)*dely) )/dcub;
          r1 += t*delx;
          r2 += t*dely;
        }
      }
      //
      //   solve the 2 by 2 system and update dgmax
      //
      double dzy = (a11*r2 - a12*r1)/(a11*a22 - a12*a12);
      double dzx = (r1 - a12*dzy)/a11;
      dgmax = max(dgmax,max(fabs(dzx),fabs(dzy)));
      //
      //   update the partials at node k
      //
      xPartials[k-1] = zxk + dzx;
      yPartials[k-1] = zyk + dzy;
    }
    //
    //   increment iter and test for convergence
    //
    ++iter;
    if( dgmax > eps )goto L1;
    //
    // method converged
    //
    numberOfIterations = iter;
    return;
    /*
    do
    {
      if( iter == maxit )throw EGraphicsError(
       "Gradients: method failed to converge within given number of iterations" );
      //
      dgmax = 0.; // maximum change in a gradient component between iterations
      size_t indf;       // iadj index of the first neighbor of k
      size_t indl = 0;   // iadj index of the last neighbor of k
      for( size_t k=0; k<theNumberOfNodes; ++k )
      {
        double xk = theX[k];
        double yk = theY[k];
        double zk = theZ[k];
        double zxk = xPartials[k];
        double zyk = yPartials[k];
        //
        // initialize components of the 2 by 2 system
        // a11,a12,a22 are the matrix components of the 2 by 2 block a*dg = r
        // where a is symmetric, dg = (dzx,dzy) is the change in the gradient
        // at k, and r is the residual
        //
        double a11 = 0.;
        double a12 = 0.;
        double a22 = 0.;
        //
        // r1,r2 are the components of the residual
        //       derivatives of q with respect to the components of the gradient at node k
        //
        double r1 = 0.;
        double r2 = 0.;
        //
        //   loop on neighbors nb of k
        //
        indf = indl + 1;
        indl = iend[k];
        for( size_t indx=indf; indx<=indl; ++indx )
        {
          size_t nb = iadj[indx-1];
          if( nb == 0 )continue;
          //
          // compute the components of arc nb-k
          //
          double delx = theX[nb-1] - xk;
          double dely = theY[nb-1] - yk;
          double delxs = delx*delx;
          double delys = dely*dely;
          double dsq = delxs + delys;
          double dcub = dsq*sqrt(dsq);
          //
          // update the system components for node nb
          //
          a11 += delxs/dcub;
          a12 += delx*dely/dcub;
          a22 += delys/dcub;
          double t = (1.5*(theZ[nb-1]-zk)-((xPartials[nb-1]/2.+zxk)*delx+
                                    (yPartials[nb-1]/2.+zyk)*dely))/dcub;
          r1 += t*delx;
          r2 += t*dely;
        }
        //
        // solve the 2 by 2 system and update dgmax
        // dzx,dzy are the solution of the 2 by 2 system
        //         change in derivatives at k from the previous iterate
        //
        double dzy = (a11*r2 - a12*r1)/(a11*a22 - a12*a12);
        double dzx = (r1 - a12*dzy)/a11;
        dgmax = max(max(dgmax,fabs(dzx)),fabs(dzy));
        //
        // update the partials at node k
        //
        xPartials[k] = zxk + dzx;
        yPartials[k] = zyk + dzy;
      }
      ++iter;
    }
    while ( dgmax > eps );
    //
    // method converged
    //
    numberOfIterations = iter;
    */
  }
 
 void GRA_thiessenTriangulation::Interpolate( size_t nx, size_t ny,
                                          std::vector<double> &px, std::vector<double> &py,
                                          std::vector<double> &zz )
  {
    // This routine is called from outside.
    //
    // Given a Thiessen triangulation of a set of points in the plane with corresponding
    // data values, this routine interpolates the data values to a set of rectangular
    // grid points for such applications as contouring.  The interpolant is once
    // continuously differentiable.  Extrapolation is performed at grid points exterior
    // to the triangulation.
    //
    // nx,ny - number of rows and columns in the rectangular grid
    // px,py - vectors of length nx and ny containing the
    //         coordinates of the grid lines
    // zz    - nrow by ncol array with nrow >= nx and ncol >= ny
    //         When finished zz will contain interpolated values at the grid
    //         points. zz[j-1+(i-1)*ny] = f(py[j-1],px[i-1]) for i = 1,...,nx
    //         and j = 1,...,ny.
    //
    if( nx<1 )throw EGraphicsError( "Interpolate: nx < 1" );
    if( ny<1 )throw EGraphicsError( "Interpolate: ny < 1" );
    size_t ist = 1;
    try               // compute interpolated values
    {
      for( size_t j=0; j<ny; ++j )
      {
        for( size_t i=0; i<nx; ++i )InterpolateSub( px[i], py[j], zz[j+i*ny], ist );
      }
    }
    catch (EGraphicsError &e)
    {
      throw;
    }
  }
 
 void GRA_thiessenTriangulation::AddNode( size_t k )
  {
    // This routine adds node kk to a triangulation of a set of points in the plane
    // producing a new triangulation.  A sequence of edge swaps is then applied to the mesh,
    // resulting in an optimal triangulation.
    //
    // k >= 4  -  index of the node to be added to the mesh
    //
    // initialization
    //
    size_t km1 = k-1;
    double xk = theX[k-1];
    double yk = theY[k-1];
    //
    // add node k to the mesh
    //
    size_t i1, i2, i3;
    Find( km1, xk, yk, i1, i2, i3 );
    if( i1 == 0 )
      throw EGraphicsError( "all nodes are collinear" );
    if( i3 == 0 )AddBoundaryNode( k, i1, i2 );
    if( i3 != 0 )AddInteriorNode( k, i1, i2, i3 );
    //
    // initialize variables for optimization of the mesh
    //
    size_t indkF = iend[km1-1] + 1;       // iadj index of the first neighbor of k
    size_t const indkL = iend[k-1];       // iadj index of the last neighbor of k
    size_t const nabor1 = iadj[indkF-1];  // first neighbor of k before any swaps occur
    size_t io2 = nabor1;                  //    to be tested for a swap
    size_t indk1 = indkF + 1;             // index of io1 in the adjacency list for k
    size_t io1 = iadj[indk1-1];           // adjacent neighbors of k defining an arc
    //
    // begin loop -- find the vertex opposite k
    //
    size_t in1, ind2f, ind21;
  L1:
    ind2f = 1;
    if( io2 != 1 )ind2f = iend[io2-2] + 1;
    ind21 = Index( io2, io1 );
    if( ind2f == ind21 )goto L2;
    in1 = iadj[ind21-2];
    goto L3;
    //
    // in1 is the last neighbor of io2
    //
  L2:
    ind21 = iend[io2-1];
    in1 = iadj[ind21-1];
    if( in1 == 0 )goto L4;
  L3:
    if( !SwapTest( in1, k, io1, io2 ) )goto L4;
    Swap( in1, k, io1, io2 );
    io1 = in1;
    --indk1;
    --indkF;
    goto L1;
    //
    // no swap occurred.  reset io2 and io1, and test for termination.
    //
  L4:
    if( io1 != nabor1 )
    {
      io2 = io1;
      if( ++indk1 > indkL )indk1 = indkF;
      io1 = iadj[indk1-1];
      if( io1 != 0 )goto L1;
    }
    return;
  }

 void GRA_thiessenTriangulation::AddBoundaryNode( size_t k, size_t right, size_t left )
  {
    // This routine adds a boundary node to a triangulation of a set of k-1
    // points in the plane.  iadj and iend are updated with the insertion of node k
    //
    // k >= 4 - index of an exterior node to be added
    // right  - first (rightmost as viewed from k) boundary node in the mesh
    //          which is visible from k - the line segment k-right intersects no arcs
    // left   - last (leftmost) boundary node which is visible from k
    //
    // iadj and iend are updated with the addition of node k as the last entry
    // node k will be connected to i1, i2, and all boundary nodes between them
    // no optimization of the mesh is performed
    //
    // local parameters -
    //  nF,nL         indices of iadj bounding the portion of the array to be shifted
    //  n1,n2         iadj index of the first(last) neighbor of left(right)
    //  imin,imax     first(last) elements of iend to be incremented
    //  kend          pointer to the last neighbor of k in iadj
    //  next          next boundary node to be connected to kk
    //
    // initialize variables
    //
    size_t nL = iend[k-2];
    size_t n1 = 1;
    if( left != 1 )n1 = iend[left-2] + 1;
    size_t n2 = iend[right-1];
    size_t nF = max(n1,n2);
    //
    // insert k as a neighbor of max(right,left)
    //
    Shift( nF, nL, 2, iadj );
    iadj[nF] = k;
    size_t imin = max(right,left);
    for( size_t i=imin; i<=k-1; ++i )iend[i-1] += 2;
    //
    // initialize kend and insert k as a neighbor of min(right,left)
    //
    size_t kend = nL + 3;
    nL = nF - 1;
    nF = min(n1,n2);
    Shift( nF, nL, 1, iadj );
    iadj[nF-1] = k;
    size_t imax = imin - 1;
    imin = min(right,left);
    for( size_t i=imin; i<=imax; ++i )iend[i-1] += 1;
    //
    // insert right as the first neighbor of k
    //
    iadj[kend-1] = right;
    //
    // initialize indx for loop on boundary nodes between right and left
    //
    size_t indx = iend[right-1] - 2;
    size_t next;
  L3:
    next = iadj[indx-1];
    if( next == left )goto L4;
    //
    // connect next and k
    //
    ++kend;
    iadj[kend-1] = next;
    indx = iend[next-1];
    iadj[indx-1] = k;
    --indx;
    goto L3;
  L4:
    iadj[kend] = left;
    kend += 2;
    iadj[kend-1] = 0;
    iend[k-1] = kend;
    return;
  }

 void GRA_thiessenTriangulation::AddInteriorNode( size_t k, size_t i1, size_t i2, size_t i3 )
  {
    // This routine adds an interior node to a triangulation of a set of k-1 points
    // in the plane.  iadj and iend are updated with the insertion of node k in the
    // triangle whose vertices are i1, i2, and i3.
    //
    // k >= 4   - index of node to be inserted
    // i1,i2,i3 - indices of the vertices of a triangle containing node k
    //            in counterclockwise order
    //
    size_t n[] = {i1, i2, i3};
    size_t nft[3]; // indices of the tops of the 3 sets of iadj elements
    //                to be shifted downward
    for( size_t i=1; i<=3; ++i )
    {
      size_t n1 = n[i-1];
      size_t index = i%3 + 1;
      size_t n2 = n[index-1];
      index = iend[n1-1] + 1;
      //
      // find the index of n2 as a neighbor of n[i]
      //
  L1:
      --index;
      if( iadj[index-1] != n2 )goto L1;
      nft[i] = index + 1;
    }
    //
    // order the vertices by decreasing magnitude.
    //   n(ip(i+1)) precedes n(ip[i-1]) in iend for
    //   i = 1,2.
    //
    size_t ip1 = 1;
    size_t ip2 = 2;
    size_t ip3 = 3;
    size_t temp;
    if( n[1] <= n[0] )goto L3;
    ip1 = 2;
    ip2 = 1;
  L3:
    if( n[2] <= n[ip1-1] )goto L4;
    ip3 = ip1;
    ip1 = 3;
  L4:
    if( n[ip3-1] <= n[ip2-1] )goto L5;
    temp = ip2;
    ip2 = ip3;
    ip3 = temp;
  L5:
    //
    // add node k to the adjacency lists of each vertex and update iend
    // for each vertex, a set of iadj elements is shifted downward and k is inserted
    // shifting starts at the end of the array
    //
    size_t km1 = k - 1;
    size_t nL = iend[km1-1]; // index of last entry in iadj to be shifted down
    size_t nF = nft[ip1-1];  // index of first entry in iadj to be shifted down
    if( nF <= nL )Shift( nF, nL, 3, iadj );
    iadj[nF+1] = k;
    size_t imin = n[ip1-1]; // first element of iend to be incremented
    size_t imax = km1;      // last element of iend to be incremented
    for( size_t i=imin; i<=imax; ++i )iend[i-1] += 3;
    nL = nF - 1;
    nF = nft[ip2-1];
    Shift( nF, nL, 2, iadj );
    iadj[nF] = k;
    imax = imin - 1;
    imin = n[ip2-1];
    for( size_t i=imin; i<=imax; ++i )iend[i-1] += 2;
    nL = nF - 1;
    nF = nft[ip3-1];
    Shift( nF, nL, 1, iadj );
    iadj[nF-1] = k;
    imax = imin - 1;
    imin = n[ip3-1];
    for( size_t i=imin; i<=imax; ++i )iend[i-1] += 1;
    //
    // add node k to iend and its neighbors to iadj
    //
    size_t index = iend[km1-1];
    iend[k-1] = index + 3;
    for( size_t i=1; i<=3; ++i )
    {
      ++index;
      iadj[index-1] = n[i-1];
    }
  }
 
 size_t GRA_thiessenTriangulation::Index( size_t vertex, size_t nabor )
  {
    // This function returns the index of nabor in the adjacency list for nvertx
    //
    // vertex - node whose adjacency list is to be searched
    // nabor  - node whose index is to be returned
    //          nabor must be connected to nvertx
    //
    size_t index = iend[vertex-1] + 1;
    //
    // search the list of nvertx neighbors for nb
    //
  L1:
    --index;
    if( iadj[index-1] != nabor )goto L1;
    return index;
  }
 
 void GRA_thiessenTriangulation::Shift( size_t nF, size_t nL, int k, std::vector<size_t> &array )
  {
    // This routine shifts a set of contiguous elements of an integer array k
    // positions downward (upward if k < 0). The loops are unrolled in order to
    // increase efficiency.
    //
    // nF,nL - bounds on the portion of array to be shifted
    //         All elements between and including the bounds are shifted unless
    //         nfrst > nlast, in which case no shift occurs
    // k     - number of positions each element is to be shifted
    //         if k < 0 shift up, if k > 0 shift down
    // array - array of length >= nlast + max(k,0)
    //
    if( nF>nL || k==0 )return;
    size_t nlp1 = nL + 1;
    size_t ns = nlp1 - nF;   // number of shifts
    size_t const inc = 5;
    size_t nsl = inc*(ns/inc); // number of shifts done in unrolled for-loop
    if( k < 0 )goto L4;
    //
    // shift downward starting from the bottom
    //
    if( nsl <= 0 )goto L2;
    size_t ibak, indx;
    for( size_t i=1; i<=nsl; i+=inc )
    {
      ibak = nlp1 - i;
      indx = ibak + k;
      array[indx-1] = array[ibak-1];
      array[indx-2] = array[ibak-2];
      array[indx-3] = array[ibak-3];
      array[indx-4] = array[ibak-4];
      array[indx-5] = array[ibak-5];
    }
    //
    // perform the remaining ns-nsl shifts one at a time
    //
  L2:
    ibak = nlp1 - nsl;
  L3:
    if( ibak <= nF )return;
    --ibak;
    indx = ibak + k;
    array[indx-1] = array[ibak-1];
    goto L3;
    //
    // shift upward starting from the top
    //
  L4:
    if( nsl <= 0 )goto L6;
    for( size_t i=nF; i<=nlp1-inc; i+=inc )
    {
      indx = i + k;
      array[indx-1] = array[i-1];
      array[indx]   = array[i];
      array[indx+1] = array[i+1];
      array[indx+2] = array[i+2];
      array[indx+3] = array[i+3];
    }
    //
    // perform the remaining ns-nsl shifts one at a time
    //
  L6:
    size_t i = nsl + nF;
  L7:
    if( i > nL )return;
    indx = i + k;
    array[indx-1] = array[i-1];
    ++i;
    goto L7;
  }

 void GRA_thiessenTriangulation::Swap( size_t nin1, size_t nin2, size_t nout1, size_t nout2 )
  {
    //
    // This routine swaps the diagonals in a convex quadrilateral
    //
    // nin1,nin2,nout1,nout2 - nodal indices of a pair of adjacent triangles
    //                         which form a convex quadrilateral.
    // nout1 and nout2 are connected by an arc which is to be replaced by the
    //  arc nin1-nin2.
    // (nin1,nout1,nout2) must be triangle vertices in counterclockwise order
    //
    size_t in[] = { nin1, nin2 };   // nin1 and nin2 ordered by increasing magnitude
    //                             (the neighbors of in[0] precede those of in[1] in iadj)
    size_t io[] = { nout1, nout2 }; // nout1 and nout2 in increasing order
    //
    // ip1 and ip2 are permutations of (1,2) such that io[ip1-1]
    // precedes io[ip2-1] as a neighbor of in[0]
    //
    size_t ip1=1, ip2;
    //
    // order the indices so that in[0] < in[1] and io[0] < io(2),
    // and choose ip1 and ip2 such that (in[0],io[ip1-1],io[ip2-1]) forms a triangle
    //
    if( in[0] < in[1] )goto L1;
    in[0] = in[1];
    in[1] = nin1;
    ip1 = 2;
  L1:
    if( io[0] < io[1] )goto L2;
    io[0] = io[1];
    io[1] = nout1;
    ip1 = 3 - ip1;
  L2:
    ip2 = 3 - ip1;
    if( io[1] < in[0] ) // the vertices are ordered (io[0],io[1],in[0],in[1])
    {                   // delete io[1] by shifting up by 1
      size_t nf = 1 + Index( io[0], io[1] );
      size_t nl = -1 + Index( io[1], io[0] );
      if( nf <= nl )Shift( nf, nl, -1, iadj );
      for( size_t i=io[0]; i<=io[1]-1; ++i )iend[i-1] -= 1;
      //
      // delete io[0] by shifting up by 2 and insert in[1]
      //
      nf = nl + 2;
      nl = -1 + Index( in[0], io[ip2-1] );
      if( nf <= nl )Shift( nf, nl, -2, iadj );
      iadj[nl-2] = in[1];
      for( size_t i=io[1]; i<=in[0]-1; ++i )iend[i-1] -= 2;
      //
      // shift up by 1 and insert in[0]
      //
      nf = nl + 1;
      nl = -1 + Index( in[1], io[ip1-1] );
      Shift( nf, nl, -1, iadj );
      iadj[nl-1] = in[0];
      for( size_t i=in[0]; i<=in[1]-1; ++i )iend[i-1] -= 1;
    }
    else if( in[1] < io[0] ) // the vertices are ordered (in[0],in[1],io[0],io[1])
    {                        // delete io[0] by shifting down by 1
      size_t nf = 1 + Index( io[0], io[1] );
      size_t nl = -1 + Index( io[1], io[0] );
      if( nf <= nl )Shift( nf, nl, 1, iadj );
      for( size_t i=io[0]; i<=io[1]-1; ++i )iend[i-1] += 1;
      //
      // delete io[1] by shifting down by 2 and insert in[0]
      //
      nl = nf - 2;
      nf = 1 + Index( in[1], io[ip2-1] );
      if( nf <= nl )Shift( nf, nl, 2, iadj );
      iadj[nf] = in[0];
      for( size_t i=in[1]; i<=io[0]-1; ++i )iend[i-1] += 2;
      //
      // shift down by 1 and insert in[1]
      //
      nl = nf - 1;
      nf = 1 + Index( in[0], io[ip1-1] );
      Shift( nf, nl, 1, iadj );
      iadj[nf-1] = in[1];
      for( size_t i=in[0]; i<=in[1]-1; ++i )iend[i-1] += 1;
    }
    else // in[0] and io[0] precede in[1] and io[1]
    {    // for (j,k) = (0,1) and (1,0), delete io[k-1] as a neighbor of io[j-1]
      //    by shifting a portion of iadj either up or down and
      //    and insert in[k-1] as a neighbor of in[j-1]
      //
      for( size_t j=1; j<=2; ++j )
      {
        size_t k = 3 - j;
        if( in[j-1] <= io[j-1] ) // the neighbors of in[j-1] precede those of io[j-1]
        {                    // shift down by 1
          size_t nf = 1 + Index( in[j-1], io[ip1-1] );
          size_t nl = -1 + Index( io[j-1], io[k-1] );
          if( nf <= nl )Shift( nf, nl, 1, iadj );
          iadj[nf-1] = in[k-1];
          for( size_t i=in[j-1]; i<=io[j-1]-1; ++i )iend[i-1] += 1;
        }
        else                 // the neighbors of io[j-1] precede those of in[j-1]
        {                    // shift up by 1
          size_t nf = 1 + Index( io[j-1], io[k-1] );
          size_t nl = -1 + Index( in[j-1], io[ip2-1] );
          if( nf <= nl )Shift( nf, nl, -1, iadj );
          iadj[nl-1] = in[k-1];
          for( size_t i=io[j-1]; i<=in[j-1]-1; ++i )iend[i-1] -= 1;
        }
        //
        // reverse (ip1,ip2) for (j,k) = (1,0)
        //
        ip1 = ip2;
        ip2 = 3 - ip1;
      }
    }
  }
 
 bool GRA_thiessenTriangulation::SwapTest( size_t in1, size_t in2, size_t io1, size_t io2 )
  {
    //
    // This function decides whether or not to replace a diagonal arc in a
    // quadrilateral with the other diagonal.  The determination is based on
    // the sizes of the angles contained in the 2 triangles defined by the diagonal.
    // The diagonal is chosen to maximize the smallest of the six angles over the
    // two pairs of triangles.
    //
    // in1,in2,io1,io2 - node indices of the four points defining the quadrilateral
    //                   io1 and io2 are currently connected by a diagonal arc
    //                   This arc should be replaced by an arc connecting in1, in2 if
    //                   the decision is made to swap.
    //                   in1,io1,io2 must be in counterclockwise order
    // SwapTest returns true iff the arc connecting io1 and io2 is to be replaced
    //
    // compute the vectors containing the angles t1, t2
    //
    double dx11 = theX[io1-1] - theX[in1-1];
    double dx12 = theX[io2-1] - theX[in1-1];
    double dx22 = theX[io2-1] - theX[in2-1];
    double dx21 = theX[io1-1] - theX[in2-1];
    //
    double dy11 = theY[io1-1] - theY[in1-1];
    double dy12 = theY[io2-1] - theY[in1-1];
    double dy22 = theY[io2-1] - theY[in2-1];
    double dy21 = theY[io1-1] - theY[in2-1];
    //
    // compute inner products
    //
    double cos1 = dx11*dx12 + dy11*dy12;
    double cos2 = dx22*dx21 + dy22*dy21;
    //
    // the diagonals should be swapped iff (t1+t2) > 180 degrees
    // the following two tests insure numerical stability
    //
    if( cos1>=0. && cos2>=0. )return false;
    //
    if( cos1<0. && cos2<0. )return true;
    //
    // compute vector cross products
    //
    double sin1 = dx11*dy12 - dx12*dy11;
    double sin2 = dx22*dy21 - dx21*dy22;
    if( sin1*cos2+cos1*sin2 >= 0. )return false;
    return true;
  }
 
 void GRA_thiessenTriangulation::Find( size_t nst, double xp, double yp,
                                   size_t &i1, size_t &i2, size_t &i3 )
  {
    // This routine locates a point p in a Thiessen triangulation,
    // returning the vertex indices of a triangle which contains p
    //
    // nst      - index of node at which Find begins search
    //            search time depends on the proximity of nst to p
    // px,py    - x and y-coordinates of the point to be located
    // i1,i2,i3 - vertex indices in counter-clockwise order
    //            vertices of a triangle containing p if p is an interior node
    //            if p is outside of the boundary of the mesh, i1 and i2 are the
    //            first (rightmost) and last (leftmost) nodes which are visible from p,
    //            and i3 = 0 if p and all of the nodes lie on a line then i1=i2=i3=0
    //
    // initialize variables and find a cone containing p
    //
    size_t n0 = max(nst,static_cast<size_t>(1));
    size_t indx, next, nL, nF, n1, n2, n3, n4, ind;
    //
    // initialize variables and find a cone containing p
    //
  L1:
    indx = iend[n0-1];
    nL = iadj[indx-1];
    indx = 1;
    if( n0 != 1 )indx = iend[n0-2] + 1;
    nF = iadj[indx-1];
    n1 = nF;
    if( nL != 0 )goto L3;
    //
    // n0 is a boundary node.  set nL to the last nonzero
    //   neighbor of n0.
    //
    ind = iend[n0-1] - 1;
    nL = iadj[ind-1];
    if( (theX[nF-1]-theX[n0-1])*(yp-theY[n0-1])>=(xp-theX[n0-1])*(theY[nF-1]-theY[n0-1]) )goto L2;
    //
    // p is outside the boundary
    //
    nL = n0;
    goto L16;
  L2:
    if( (theX[n0-1]-theX[nL-1])*(yp-theY[nL-1])>=(xp-theX[nL-1])*(theY[n0-1]-theY[nL-1]) )goto L4;
    //
    // p is outside the boundary and n0 is the rightmost
    //   visible boundary node
    //
    i1 = n0;
    goto L18;
    //
    // n0 is an interior node.  find n1.
    //
  L3:
    if( (theX[n1-1]-theX[n0-1])*(yp-theY[n0-1])>=(xp-theX[n0-1])*(theY[n1-1]-theY[n0-1]) )goto L4;
    ++indx;
    n1 = iadj[indx-1];
    if( n1 == nL )goto L7;
    goto L3;
    //
    // p is to the left of arc n0-n1.  initialize n2 to the next
    //   neighbor of n0.
    //
  L4:
    ++indx;
    n2 = iadj[indx-1];
    if( !((theX[n2-1]-theX[n0-1])*(yp-theY[n0-1])>=(xp-theX[n0-1])*(theY[n2-1]-theY[n0-1])) )goto L8;
    n1 = n2;
    if( n1 != nL )goto L4;
    if( !((theX[nF-1]-theX[n0-1])*(yp-theY[n0-1])>=(xp-theX[n0-1])*(theY[nF-1]-theY[n0-1])) )goto L7;
    if( xp == theX[n0-1] && yp == theY[n0-1] )goto L6;
    //
    // p is left of or on arcs n0-nb for all neighbors nb
    //   of n0.
    // all points are collinear iff p is left of nb-n0 for
    //   all neighbors nb of n0.  search the neighbors of n0
    //   in reverse order.  note -- n1 = nL and indx points to
    //   nL.
    //
  L5:
    if( !((theX[n0-1]-theX[n1-1])*(yp-theY[n1-1])>=(xp-theX[n1-1])*(theY[n0-1]-theY[n1-1])))goto L6;
    if( n1 == nF )goto L20;
    --indx;
    n1 = iadj[indx-1];
    goto L5;
    //
    // p is to the right of n1-n0, or p=n0.  set n0 to n1 and
    //   start over.
    //
  L6:
    n0 = n1;
    goto L1;
    //
    // p is between arcs n0-n1 and n0-nF
    //
  L7:
    n2 = nF;
    //
    // p is contained in a cone defined by line segments n0-n1
    //   and n0-n2 where n1 is adjacent to n2
    //
  L8:
    n3 = n0;
  L9:
    if( (theX[n2-1]-theX[n1-1])*(yp-theY[n1-1])>=(xp-theX[n1-1])*(theY[n2-1]-theY[n1-1]) )goto L13;
    //
    // set n4 to the first neighbor of n2 following n1
    //
    indx = iend[n2-1];
    if( iadj[indx-1] != n1 )goto L10;
    //
    // n1 is the last neighbor of n2.
    // set n4 to the first neighbor.
    //
    indx = 1;
    if( n2 != 1 )indx = iend[n2-2] + 1;
    n4 = iadj[indx-1];
    goto L11;
    //
    // n1 is not the last neighbor of n2
    //
  L10:
    --indx;
    if( iadj[indx-1] != n1 )goto L10;
    n4 = iadj[indx];
    if( n4 != 0 )goto L11;
    //
    // p is outside the boundary
    //
    nF = n2;
    nL = n1;
    goto L16;
    //
    // define a new arc n1-n2 which intersects the line
    //   segment n0-p
    //
  L11:
    if( (theX[n4-1]-theX[n0-1])*(yp-theY[n0-1])>=(xp-theX[n0-1])*(theY[n4-1]-theY[n0-1]) )goto L12;
    n3 = n2;
    n2 = n4;
    goto L9;
  L12:
    n3 = n1;
    n1 = n4;
    goto L9;
    //
    // p is in the triangle (n1,n2,n3) and not on n2-n3.  if
    //   n3-n1 or n1-n2 is a boundary arc containing p, treat p
    //   as exterior.
    //
  L13:
    indx = iend[n1-1];
    if( iadj[indx-1] != 0 )goto L15;
    //
    // n1 is a boundary node.  n3-n1 is a boundary arc iff n3
    //   is the last nonzero neighbor of n1.
    //
    if( n3 != iadj[indx-2] )goto L14;
    //
    // n3-n1 is a boundary arc
    //
    if( !((theX[n3-1]-theX[n1-1])*(yp-theY[n1-1])>=(xp-theX[n1-1])*(theY[n3-1]-theY[n1-1])) )goto L14;
    //
    // p lies on n1-n3
    //
    i1 = n1;
    i2 = n3;
    i3 = 0;
    return;
    //
    // n3-n1 is not a boundary arc containing p.  n1-n2 is a
    //   boundary arc iff n2 is the first neighbor of n1.
    //
  L14:
    indx = 1;
    if( n1 != 1 )indx = iend[n1-2] + 1;
    if( n2 != iadj[indx-1] )goto L15;
    //
    // n1-n2 is a boundary arc
    //
    if( !((theX[n1-1]-theX[n2-1])*(yp-theY[n2-1])>=(xp-theX[n2-1])*(theY[n1-1]-theY[n2-1])) )goto L15;
    //
    // p lies on n1-n2
    //
    i1 = n2;
    i2 = n1;
    i3 = 0;
    return;
    //
    // p does not lie on a boundary arc.
    //
  L15:
    i1 = n1;
    i2 = n2;
    i3 = n3;
    return;
    //
    // nF and nL are adjacent boundary nodes which are visible
    //   from p.  find the first visible boundary node.
    // set next to the first neighbor of nF.
    //
  L16:
    indx = 1;
    if( nF != 1 )indx = iend[nF-2] + 1;
    next = iadj[indx-1];
    if( (theX[next-1]-theX[nF-1])*(yp-theY[nF-1])>=(xp-theX[nF-1])*(theY[next-1]-theY[nF-1]) )goto L17;
    nF = next;
    goto L16;
    //
    // nF is the first (rightmost) visible boundary node
    //
  L17:
    i1 = nF;
    //
    // find the last visible boundary node.  nL is the first
    //   candidate for i2.
    // set next to the last neighbor of nL.
    //
  L18:
    indx = iend[nL-1] - 1;
    next = iadj[indx-1];
    if( (theX[nL-1]-theX[next-1])*(yp-theY[next-1])>=(xp-theX[next-1])*(theY[nL-1]-theY[next-1]) )goto L19;
    nL = next;
    goto L18;
    //
    // nL is the last (leftmost) visible boundary node
    //
  L19:
    i2 = nL;
    i3 = 0;
    return;
    //
    // all points are collinear
    //
  L20:
    i1 = 0;
    i2 = 0;
    i3 = 0;
    return;
    /*
  L1:
    indx = iend[n0-1];
    nL = iadj[indx-1];
    indx = 1;
    if( n0 != 1 )indx = iend[n0-2] + 1;
    nF = iadj[indx-1];
    n1 = nF;
    if( nL != 0 ) // n0 is an interior node
    {             // find n1
      for( ;; )
      {
        if( (theX[n1-1]-theX[n0-1])*(yp-theY[n0-1]) >=
            (xp-theX[n0-1])*(theY[n1-1]-theY[n0-1]) )goto L4;
        ++indx;
        n1 = iadj[indx-1];
        if( n1 == nL ) // p is between arcs n0-n1 and n0-nF
        {
          n2 = nF;
          //
          // p is contained in a cone defined by line segments n0-n1
          // and n0-n2 where n1 is adjacent to n2
          //
          n3 = n0;
          goto L9;
        }
      }
    }
    //
    // n0 is a boundary node
    // set nL to the last nonzero neighbor of n0
    //
    ind = iend[n0-1] - 1;
    nL = iadj[ind-1];
    if( (theX[nF-1]-theX[n0-1])*(yp-theY[n0-1]) < (xp-theX[n0-1])*(theY[nF-1]-theY[n0-1]) )
    {
      // p is outside the boundary
      nL = n0;
      goto L16;
    }
    if( (theX[n0-1]-theX[nL-1])*(yp-theY[nL-1]) >=
        (xp-theX[nL-1])*(theY[n0-1]-theY[nL-1]) )goto L4;
    //
    // p is outside the boundary and n0 is the rightmost
    //   visible boundary node
    //
    i1 = n0;
    //
    // find the last visible boundary node
    // nL is the first candidate for i2
    // set next to the last neighbor of nL
    //
    for( ;; )
    {
      indx = iend[nL-1] - 1;
      next = iadj[indx-1];
      if( (theX[nL-1]-theX[next-1])*(yp-theY[next-1]) >=
          (xp-theX[next-1])*(theY[nL-1]-theY[next-1]) )
      {
        // nL is the last (leftmost) visible boundary node
        //
        i2 = nL;
        i3 = 0;
        return;
      }
      nL = next;
    }
    //
    // p is to the left of arc n0-n1
    // initialize n2 to the next neighbor of n0
    //
  L4:
    ++indx;
    n2 = iadj[indx-1];
    if( (theX[n2-1]-theX[n0-1])*(yp-theY[n0-1]) <
        (xp-theX[n0-1])*(theY[n2-1]-theY[n0-1]) )
    {
      //
      // p is contained in a cone defined by line segments n0-n1
      // and n0-n2 where n1 is adjacent to n2
      //
      n3 = n0;
      goto L9;
    }
    n1 = n2;
    if( n1 != nL )goto L4;
    if( (theX[nF-1]-theX[n0-1])*(yp-theY[n0-1]) <
        (xp-theX[n0-1])*(theY[nF-1]-theY[n0-1]) ) // p is between arcs n0-n1 and n0-nF
    {
      n2 = nF;
      //
      // p is contained in a cone defined by line segments n0-n1
      // and n0-n2 where n1 is adjacent to n2
      //
      n3 = n0;
      goto L9;
    }
    if( xp==theX[n0-1] && yp==theY[n0-1] )
    {
      //
      // p is to the right of n1-n0, or p=n0
      // set n0 to n1 and start over
      //
      n0 = n1;
      goto L1;
    }
    //
    // p is left of or on arcs n0-nb for all neighbors nb of n0
    // all points are collinear iff p is left of nb-n0 for all neighbors nb of n0
    // search the neighbors of n0 in reverse order
    // note:  n1 = nL and indx points to nL
    //
    for( ;; )
    {
      if( (theX[n0-1]-theX[n1-1])*(yp-theY[n1-1]) <
          (xp-theX[n1-1])*(theY[n0-1]-theY[n1-1]) )
      {
        //
        // p is to the right of n1-n0, or p=n0
        // set n0 to n1 and start over
        //
        n0 = n1;
        goto L1;
      }
      if( n1 == nF ) // all points are collinear
      {
        i1 = 0;
        i2 = 0;
        i3 = 0;
        return;
      }
      --indx;
      n1 = iadj[indx-1];
    }
  L9:
    if( (theX[n2-1]-theX[n1-1])*(yp-theY[n1-1]) >=
        (xp-theX[n1-1])*(theY[n2-1]-theY[n1-1]) )goto L13;
    //
    // set n4 to the first neighbor of n2 following n1
    //
    indx = iend[n2-1];
    if( iadj[indx-1] != n1 ) // n1 is not the last neighbor of n2
    {
      do
      {
        --indx;
      }
      while( iadj[indx-1] != n1 );
      n4 = iadj[indx];
      if( n4 != 0 ) // define a new arc n1-n2 which intersects the line segment n0-p
      {
        if( (theX[n4-1]-theX[n0-1])*(yp-theY[n0-1]) < (xp-theX[n0-1])*(theY[n4-1]-theY[n0-1]) )
        {
          n3 = n2;
          n2 = n4;
        }
        else
        {
          n3 = n1;
          n1 = n4;
        }
        goto L9;
      }
      //
      // p is outside the boundary
      //
      nF = n2;
      nL = n1;
      goto L16;
    }
    //
    // n1 is the last neighbor of n2.
    // set n4 to the first neighbor.
    //
    indx = 1;
    if( n2 != 1 )indx = iend[n2-2] + 1;
    n4 = iadj[indx-1];
    //
    // define a new arc n1-n2 which intersects the line segment n0-p
    //
    if( (theX[n4-1]-theX[n0-1])*(yp-theY[n0-1]) < (xp-theX[n0-1])*(theY[n4-1]-theY[n0-1]) )
    {
      n3 = n1;
      n1 = n4;
    }
    else
    {
      n3 = n2;
      n2 = n4;
    }
    goto L9;
    //
    // p is in the triangle (n1,n2,n3) and not on n2-n3.  if
    //   n3-n1 or n1-n2 is a boundary arc containing p, treat p
    //   as exterior.
    //
  L13:
    indx = iend[n1-1];
    if( iadj[indx-1] != 0 )
    {
      //
      // p does not lie on a boundary arc
      //
      i1 = n1;
      i2 = n2;
      i3 = n3;
      return;
    }
    //
    // n1 is a boundary node.  n3-n1 is a boundary arc iff n3
    //   is the last nonzero neighbor of n1.
    //
    if( n3 != iadj[indx-2] ) // n3-n1 is not a boundary arc containing p
    {                        // n1-n2 is a boundary arc iff n2 is the first neighbor of n1
      indx = 1;
      if( n1 != 1 )indx = iend[n1-2] + 1;
      if( n2 != iadj[indx-1] ) // p does not lie on a boundary arc
      {
        i1 = n1;
        i2 = n2;
        i3 = n3;
      }
      else // n1-n2 is a boundary arc
      {
        if( (theX[n1-1]-theX[n2-1])*(yp-theY[n2-1]) <
            (xp-theX[n2-1])*(theY[n1-1]-theY[n2-1]) ) // p does not lie on a boundary arc
        {
          i1 = n1;
          i2 = n2;
          i3 = n3;
        }
        else // p lies on n1-n2
        {
          i1 = n2;
          i2 = n1;
          i3 = 0;
        }
      }
      return;
    }
    //
    // n3-n1 is a boundary arc
    //
    if( (theX[n3-1]-theX[n1-1])*(yp-theY[n1-1]) >= (xp-theX[n1-1])*(theY[n3-1]-theY[n1-1]) )
    {
      // p lies on n1-n3
      //
      i1 = n1;
      i2 = n3;
      i3 = 0;
    }
    else
    {
      // n3-n1 is not a boundary arc containing p.  n1-n2 is a
      //   boundary arc iff n2 is the first neighbor of n1.
      //
      indx = 1;
      if( n1 != 1 )indx = iend[n1-2] + 1;
      if( n2 != iadj[indx-1] ) // p does not lie on a boundary arc
      {
        i1 = n1;
        i2 = n2;
        i3 = n3;
      }
      else // n1-n2 is a boundary arc
      {
        if( (theX[n1-1]-theX[n2-1])*(yp-theY[n2-1]) <
            (xp-theX[n2-1])*(theY[n1-1]-theY[n2-1]) ) // p does not lie on a boundary arc
        {
          i1 = n1;
          i2 = n2;
          i3 = n3;
        }
        else // p lies on n1-n2
        {
          i1 = n2;
          i2 = n1;
          i3 = 0;
        }
      }
    }
    return;
    //
    // nF and nL are adjacent boundary nodes which are visible from p
    // find the first visible boundary node
    // set next to the first neighbor of nF
    //
  L16:
    for( ;; )
    {
      indx = 1;
      if( nF != 1 )indx = iend[nF-2] + 1;
      next = iadj[indx-1];
      if( (theX[next-1]-theX[nF-1])*(yp-theY[nF-1]) >=
          (xp-theX[nF-1])*(theY[next-1]-theY[nF-1]) )
      {
        i1 = nF; // nF is the first (rightmost) visible boundary node
        //
        // find the last visible boundary node
        // nL is the first candidate for i2
        // set next to the last neighbor of nL
        //
        for( ;; )
        {
          indx = iend[nL-1] - 1;
          next = iadj[indx-1];
          if( (theX[nL-1]-theX[next-1])*(yp-theY[next-1]) >=
              (xp-theX[next-1])*(theY[nL-1]-theY[next-1]) )
          {
            // nL is the last (leftmost) visible boundary node
            //
            i2 = nL;
            i3 = 0;
            return;
          }
          nL = next;
        }
      }
      nF = next;
    }
    */
  }
 
 void GRA_thiessenTriangulation::InterpolateSub( double xp, double yp, double &pz, size_t &ist )
  {
    // Given a triangulation of a set of points in the plane, this routine determines
    // a piecewise cubic function f(x,y) which interpolates a set of data values and
    // partial derivatives at the vertices.  f has continuous first derivatives over
    // the mesh and extends beyond the mesh boundary allowing extrapolation.
    // Interpolation is exact for quadratic data.  The value of f at (px,py) is returned.
    //
    // px,py - coordinates of a point at which f is to be evaluated
    // ist   - index of the starting node in the search for a triangle containing (px,py)
    //         1 <= ist <= theNumberOfNodes
    //         The output value of ist from a previous call may be a good choice
    //         When finished, ist is the index of one of the vertices of the triangle
    //         containing (px,py)
    // pz    - when finished, pz is the value of f at (px,py)
    //
    size_t i1, i2, i3, n1, n2, indx;
    double zx1, zy1, zx2, zy2, zx3, zy3, x1, y1;
    double x2, y2, x3, y3, z1, z2, z3, dp, u, v, xq;
    double yq, r1, r2, a1, a2, b1, b2, c1, c2, f1, f2;
    /*
      i1,i2,i3  vertices determined by Find
      n1,n2     endpoints of the closest boundary edge to p
                when p is outside of the mesh boundary
      indx      iadj index of n1 as a neighbor of n2
      zx1,zy1,zx2,zy2,zx3,zy3  x and y derivatives at the vertices of a triangle t which
                               contains p or at n1 and n2
      x1,y1,x2,y2,x3,y3        x,y coordinates of the vertices of t or of n1 and n2
      z1,z2,z3                 data values at the vertices of t
      dp        inner product of n1-n2 and p-n2
      u,v       x,y coordinates of the vector n2-n1
      xq,yq     x,y coordinates of the closest boundary point to p
                when p is outside of the mesh boundary
      r1,r2     barycentric coordinates of q with respect to the line
                segment n2-n1 containing q
      a1,a2,b1,b2,c1,c2        cardinal functions for evaluating the interpolatory
                               surface at q
      f1,f2     cubic factors used to compute the cardinal functions
    */
    pz = 0.0;
    //
    // find a triangle containing p if p is within the mesh boundary
    //
    Find( ist, xp, yp, i1, i2, i3 );
    if( i1 == 0 )throw EGraphicsError( "InterpolateSub: nodes are collinear" );
    ist = i1;
    if( i3 == 0 )goto L3;
    //
    // derivatives are user provided
    //
    zx1 = xPartials[i1-1];
    zx2 = xPartials[i2-1];
    zx3 = xPartials[i3-1];
    zy1 = yPartials[i1-1];
    zy2 = yPartials[i2-1];
    zy3 = yPartials[i3-1];
    //
    // set local parameters for call to tval
    //
    x1 = theX[i1-1];
    y1 = theY[i1-1];
    x2 = theX[i2-1];
    y2 = theY[i2-1];
    x3 = theX[i3-1];
    y3 = theY[i3-1];
    z1 = theZ[i1-1];
    z2 = theZ[i2-1];
    z3 = theZ[i3-1];
    try
    {
      pz = tval( xp, yp, x1, x2, x3, y1, y2, y3, z1, z2, z3,
                 zx1, zx2, zx3, zy1, zy2, zy3 );
    }
    catch (EGraphicsError &e)
    {
      throw;
    }
    return;
  L3:
    //
    // p is outside of the mesh boundary
    // extrapolate to p by passing a linear function of one variable through the
    // value and directional derivative (in the direction p-q) of the interpolatory
    // surface at q where q is the closest boundary point to p
    //
    // determine q by traversing the boundary starting from the rightmost visible node i1
    //
    n2 = i1;
    //
    // set n1 to the last nonzero neighbor of n2 and compute dp
    //
  L4:
    indx = iend[n2-1] - 1;
    n1 = iadj[indx-1];
    x1 = theX[n1-1];
    y1 = theY[n1-1];
    x2 = theX[n2-1];
    y2 = theY[n2-1];
    dp = (x1-x2)*(xp-x2) + (y1-y2)*(yp-y2);
    if( dp <= 0. )goto L5;
    if( (xp-x1)*(x2-x1)+(yp-y1)*(y2-y1)>0. )goto L8;
    n2 = n1;
    goto L4;
  L5:
    //
    // n2 is the closest boundary point to p
    // compute partial derivatives at n2
    //
    zx2 = xPartials[n2-1];
    zy2 = yPartials[n2-1];
    //
    // compute extrapolated value at p
    //
    pz = theZ[n2-1] + zx2*(xp-x2) + zy2*(yp-y2);
    return;
  L8:
    //
    // the closest boundary point q lies on n2-n1
    // compute partials at n1 and n2
    //
    zx1 = xPartials[n1-1];
    zy1 = yPartials[n1-1];
    zx2 = xPartials[n2-1];
    zy2 = yPartials[n2-1];
    //
    // compute q, its barycentric coordinates,
    // and the cardinal functions for extrapolation
    //
    u = x2-x1;
    v = y2-y1;
    r1 = dp/(u*u + v*v);
    r2 = 1. - r1;
    xq = r1*x1 + r2*x2;
    yq = r1*y1 + r2*y2;
    f1 = r1*r1*r2;
    f2 = r1*r2*r2;
    a1 = r1 + (f1-f2);
    a2 = r2 - (f1-f2);
    b1 = u*f1;
    b2 = -u*f2;
    c1 = v*f1;
    c2 = -v*f2;
    //
    // compute the value of the interpolatory surface at q
    //
    pz = a1*theZ[n1-1] + a2*theZ[n2-1] + b1*zx1 + b2*zx2 + c1*zy1 + c2*zy2;
    //
    // compute the extrapolated value at p
    //
    pz += (r1*zx1 + r2*zx2)*(xp-xq) + (r1*zy1 + r2*zy2)*(yp-yq);
    return;
  }
 
 double
  GRA_thiessenTriangulation::TriangleVolume( double x1, double x2, double x3,
                                         double y1, double y2, double y3,
                                         double z1, double z2, double z3 )
  {
    // This function computes the integral over a triangle of the planar surface which
    // interpolates data values at the vertices
    //
    // x1,x2,x3 - x coordinates of the vertices of the triangle in counterclockwise order
    // y1,y2,y3 - y coordinates of the vertices of the triangle
    // z1,z2,z3 - data values at the vertices
    //
    // output - volume under the interpolatory surface above the triangle or zero if the
    //          coordinates are incorrectly ordered or collinear
    //
    double area = x2*y3-x3*y2 + x3*y1-x1*y3 + x1*y2-x2*y1;
    if( area < 0. )area = 0.;
    //
    // area is twice the area of the triangle
    // return the mean of the data values times the area of the triangle
    //
    return (z1+z2+z3)*area/6.0;
  }
 
 double GRA_thiessenTriangulation::Volume()
  {
    // This routine can be called from outside.
    //
    // Given a set of n data points (x[i],y[i]) and function values
    // z[i]=f(x[i],y[i]) and a triangulation covering the convex hull h of the
    // data points, this function approximates the integral of f over h by
    // integrating the piecewise linear interpolant of the data values.
    //
    // output - sum of the volumes of the linear interpolants on the triangles
    //
    size_t indF = 1;
    double sum = 0.0;
    //
    // loop on triangles (n1,n2,n3) such that n2 and n3 are
    // adjacent neighbors of n1 which are both larger than n1
    //
    for( size_t n1=1; n1<=theNumberOfNodes-2; ++n1 )
    {
      double xn1 = theX[n1-1];
      double yn1 = theY[n1-1];
      double zn1 = theZ[n1-1];
      size_t indL = iend[n1-1];
      for( size_t indx=indF; indx<=indL; ++indx )
      {
        size_t n2 = iadj[indx-1];
        size_t n3 = iadj[indx];
        if( indx == indL )n3 = iadj[indF-1];
        if( n2>=n1 && n3>=n1 )
          sum += TriangleVolume( xn1, theX[n2-1], theX[n3-1],
                                 yn1, theY[n2-1], theY[n3-1],
                                 zn1, theZ[n2-1], theZ[n3-1] );
      }
      indF = indL + 1;
    }
    return sum;
  }
 
 double GRA_thiessenTriangulation::tval( double x, double y,
                                     double x1, double x2, double x3,
                                     double y1, double y2, double y3,
                                     double z1, double z2, double z3,
                                     double zx1, double zx2, double zx3,
                                     double zy1, double zy2, double zy3 )
  {
    // Given function values and first partial derivatives at the three vertices of a
    // triangle, this routine determines a function w which agrees with the given data,
    // returning the value.  The interpolation method is exact for quadratic polynomial
    // data.  The triangle is partitioned into three subtriangles with equal areas.
    // w is cubic in each subtriangle and along the edges, but has only one continuous
    // derivative across edges.  The normal derivative of w varies linearly along
    // each outer edge.  The values and partial derivatives of w along a triangle edge
    // depend only on the data values at the endpoints of the edge.  Thus the method
    // yields c-1 continuity when used to interpolate over a triangular grid.
    // This algorithm is due to C. L. Lawson.
    //
    // x,y - coordinates of a point at which w is to be evaluated
    // x1,x2,x3,y1,y2,y3 - coordinates of the vertices of a triangle containing (x,y)
    // z1,z2,z3 - function values at the vertices to be interpolated
    // zx1,zx2,zx3 - x-derivative values at the vertices
    // zy1,zy2,zy3 - y-derivative values at the vertices
    //
    // tval returns the estimated value of the interpolatory function at (x,y)
    //
    double u[3]; // x-component of the vector representing the side opposite vertex
    u[0] = x3 - x2;
    u[1] = x1 - x3;
    u[2] = x2 - x1;
    //
    double v[3]; // y-component of the vector representing the side opposite vertex
    v[0] = y3 - y2;
    v[1] = y1 - y3;
    v[2] = y2 - y1;
    //
    double sl[3]; // square of the length of the side opposite vertex
    for( size_t i=0; i<3; ++i )sl[i] = u[i]*u[i] + v[i]*v[i];
    //
    // area = 3-1 x 3-2
    //
    double area = u[0]*v[1] - u[1]*v[0]; // twice the area of the triangle
    if( area == 0. )return 0.0;  // vertices are collinear
    //
    // r[0] = (2-3 x 2-(x,y))/area, r[1] = (1-(x,y) x 1-3)/area,
    //   r[2] = (1-2 x 1-(x,y))/area
    //
    double r[3]; // barycentric coordinates
    r[0] = (u[0]*(y-y2) - v[0]*(x-x2))/area;
    r[1] = (u[1]*(y-y1) - v[1]*(x-x1))/area;
    r[2] = (u[2]*(y-y1) - v[2]*(x-x1))/area;
    //
    double phi[3];
    phi[0] = r[1]*r[2];
    phi[1] = r[2]*r[0];
    phi[2] = r[0]*r[1];
    //
    double rmin = min(min(r[0],r[1]),r[2]);
    size_t ip1, ip2, ip3;
    if( rmin != r[0] )goto L3;
    ip1 = 1;
    ip2 = 2;
    ip3 = 3;
    goto L5;
  L3:
    if( rmin != r[1] )goto L4;
    ip1 = 2;
    ip2 = 3;
    ip3 = 1;
    goto L5;
  L4:
    ip1 = 3;
    ip2 = 1;
    ip3 = 2;
  L5:
    double ro[3]; // cubic correction terms for computing g
    double c1 = rmin*rmin/2.;
    double c2 = rmin/3.;
    ro[ip1-1] = (phi[ip1-1] + 5.*c1/3.)*r[ip1-1] - c1;
    ro[ip2-1] = c1*(r[ip3-1] - c2);
    ro[ip3-1] = c1*(r[ip2-1] - c2);
    //
    double f[3];
    f[0] = 3.*(sl[1]-sl[2])/sl[0];
    f[1] = 3.*(sl[2]-sl[0])/sl[1];
    f[2] = 3.*(sl[0]-sl[1])/sl[2];
    //
    double g[3];
    g[0] = (r[1]-r[2])*phi[0] + f[0]*ro[0] - ro[1] + ro[2];
    g[1] = (r[2]-r[0])*phi[1] + f[1]*ro[1] - ro[2] + ro[0];
    g[2] = (r[0]-r[1])*phi[2] + f[2]*ro[2] - ro[0] + ro[1];
    //
    double p[3];
    p[0] = g[0] + phi[0];
    p[1] = g[1] + phi[1];
    p[2] = g[2] + phi[2];
    //
    double q[3];
    q[0] = g[0] - phi[0];
    q[1] = g[1] - phi[1];
    q[2] = g[2] - phi[2];
    //
    double a[3];
    a[0] = r[0] + g[2] - g[1];
    a[1] = r[1] + g[0] - g[2];
    a[2] = r[2] + g[1] - g[0];
    //
    double b[3];
    b[0] = u[2]*p[2] + u[1]*q[1];
    b[1] = u[0]*p[0] + u[2]*q[2];
    b[2] = u[1]*p[1] + u[0]*q[0];
    //
    double c[3];
    c[0] = v[2]*p[2] + v[1]*q[1];
    c[1] = v[0]*p[0] + v[2]*q[2];
    c[2] = v[1]*p[1] + v[0]*q[0];
    //
    // w is a linear combination of the cardinal functions
    //
    return a[0]*z1 + a[1]*z2 + a[2]*z3 +
        (b[0]*zx1 + b[1]*zx2 + b[2]*zx3 + c[0]*zy1 + c[1]*zy2 + c[2]*zy3)/2.;
  }
 
 // end of file
