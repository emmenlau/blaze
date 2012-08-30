//=================================================================================================
/*!
//  \file src/mtl/Custom.cpp
//  \brief Source file for the MTL kernel for custom expressions
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
#include <blazemark/mtl/Custom.h>
#include <blazemark/mtl/Init.h>
#include <blazemark/system/Config.h>


namespace blazemark {

namespace mtl {

//=================================================================================================
//
//  KERNEL FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief MTL kernel for custom expressions.
//
// \param N The number of rows and columns of the matrices and the size of the vectors.
// \param F The number of non-zero elements for the sparse operands.
// \param steps The number of iteration steps to perform.
// \return Minimum runtime of the kernel function.
//
// This kernel function implements a user-defined, custom expression by means of the MTL
// functionality.
*/
double custom( size_t /*N*/, size_t /*F*/, size_t steps )
{
   using ::blazemark::element_t;

   typedef ::mtl::tag::row_major  row_major;
   typedef ::mtl::tag::col_major  col_major;
   typedef ::mtl::matrix::parameters<row_major>  row_parameters;
   typedef ::mtl::matrix::parameters<col_major>  col_parameters;

   ::blaze::setSeed( seed );

   ::blaze::timing::WcTimer timer;

   //** INITIALIZATIONS **


   //** INITIAL KERNEL CALL **


   for( size_t rep=0UL; rep<reps; ++rep )
   {
      timer.start();
      for( size_t step=0UL; step<steps; ++step ) {
         // ** MEASURED KERNEL CALL(S) **

      }
      timer.end();

      if( false )  //** ERROR CONDITION **
         std::cerr << " Line " << __LINE__ << ": ERROR detected!!!\n";

      if( timer.last() > maxtime )
         break;
   }

   const double minTime( timer.min()     );
   const double avgTime( timer.average() );

   if( minTime * ( 1.0 + deviation*0.01 ) < avgTime )
      std::cerr << " MTL kernel 'custom': Time deviation too large!!!\n";

   return minTime;
}
//*************************************************************************************************

} // namespace mtl

} // namespace blazemark
