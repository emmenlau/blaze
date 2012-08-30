//=================================================================================================
/*!
//  \file src/mtl/DMatSMatMult.cpp
//  \brief Source file for the MTL dense matrix/sparse matrix multiplication kernel
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
#include <blazemark/mtl/DMatSMatMult.h>
#include <blazemark/mtl/init/Compressed2D.h>
#include <blazemark/mtl/init/Dense2D.h>
#include <blazemark/system/Config.h>


namespace blazemark {

namespace mtl {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief MTL dense matrix/sparse matrix multiplication kernel.
//
// \param N The number of rows and columns of the matrices.
// \param F The number of non-zero elements in each row of the sparse matrix.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements the dense matrix/sparse matrix multiplication by means of
// the MTL functionality.
*/
double dmatsmatmult( size_t N, size_t F, size_t steps )
{
   using ::blazemark::element_t;

   typedef ::mtl::tag::row_major  row_major;
   typedef ::mtl::matrix::parameters<row_major>  parameters;
   typedef ::mtl::dense2D<element_t,parameters>  dense2D;
   typedef ::mtl::compressed2D<element_t,parameters>  compressed2D;
   typedef ::mtl::matrix::inserter<compressed2D>  inserter;

   ::blaze::setSeed( seed );

   dense2D A( N, N ), C( N, N );
   compressed2D B( N, N );
   ::blaze::timing::WcTimer timer;

   init( A );
   init( B, F );

   C = A * B;

   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         C = A * B;
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
      std::cerr << " MTL kernel 'dmatsmatmult': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace mtl

} // namespace blazemark
