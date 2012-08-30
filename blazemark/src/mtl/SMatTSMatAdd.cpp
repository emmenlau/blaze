//=================================================================================================
/*!
//  \file src/mtl/SMatTSMatAdd.cpp
//  \brief Source file for the MTL sparse matrix/transpose sparse matrix addition kernel
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iostream>
#include <boost/numeric/mtl/mtl.hpp>
#include <blaze/util/Timing.h>
#include <blazemark/mtl/init/Compressed2D.h>
#include <blazemark/mtl/SMatTSMatAdd.h>
#include <blazemark/system/Config.h>


namespace blazemark {

namespace mtl {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief MTL sparse matrix/transpose sparse matrix addition kernel.
//
// \param N The number of rows and columns of the matrices.
// \param F The number of non-zero elements in each row/column of the sparse matrices.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the sparse matrix/transpose sparse matrix addition by means
// of the MTL functionality.
*/
double smattsmatadd( size_t N, size_t F, size_t steps )
{
   using ::blazemark::element_t;

   typedef ::mtl::tag::row_major  row_major;
   typedef ::mtl::tag::col_major  col_major;
   typedef ::mtl::matrix::parameters<row_major>  row_parameters;
   typedef ::mtl::matrix::parameters<col_major>  col_parameters;
   typedef ::mtl::compressed2D<element_t,row_parameters>  row_compressed2D;
   typedef ::mtl::compressed2D<element_t,col_parameters>  col_compressed2D;
   typedef ::mtl::matrix::inserter<row_compressed2D>  row_inserter;
   typedef ::mtl::matrix::inserter<col_compressed2D>  col_inserter;

   ::blaze::setSeed( seed );

   row_compressed2D A( N, N ), C( N, N );
   col_compressed2D B( N, N );
   ::blaze::timing::WcTimer timer;

   init( A, F );
   init( B, F );

   C = A + B;

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         C = A + B;
      }
      timer.end();

      if( num_rows(C) != N )
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " MTL kernel 'smattsmatadd': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace mtl

} // namespace blazemark
