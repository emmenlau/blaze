//=================================================================================================
/*!
//  \file blaze/math/intrinsics/Stream.h
//  \brief Header file for the intrinsic stream functionality
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

#ifndef _BLAZE_MATH_INTRINSICS_STREAM_H_
#define _BLAZE_MATH_INTRINSICS_STREAM_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <blaze/math/intrinsics/BasicTypes.h>
#include <blaze/system/Vectorization.h>
#include <blaze/util/AlignmentCheck.h>
#include <blaze/util/Assert.h>
#include <blaze/util/Complex.h>
#include <blaze/util/constraints/Integral.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/StaticAssert.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Auxiliary helper struct for intrinsic stream operations.
// \ingroup intrinsics
//
// This helper structure provides the mapping between the size of an integral data type and the
// according intrinsic stream function. Note that the type \a T must be an integral data type.
// Instantiating the Stream class with a non-integral data type results in a compilation error.
*/
template< typename T  // Type of the integral
        , size_t N >  // Size of the integral
struct Stream;
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SPECIALIZATIONS OF THE STREAM CLASS TEMPLATE
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of the Stream class template for 2-byte integral data types.
// \ingroup intrinsics
*/
template< typename T >  // Type of the integral
struct Stream<T,2UL>
{
 public:
   //**Type definitions****************************************************************************
   typedef sse_int16_t  Type;
   //**********************************************************************************************

   //**Set function********************************************************************************
   static inline void stream( T* address, const Type& value )
   {
      BLAZE_INTERNAL_ASSERT( checkAlignment( address ), "Invalid alignment detected" );

#if BLAZE_AVX2_MODE
      _mm256_stream_si256( reinterpret_cast<__m256i*>( address ), value.value );
#elif BLAZE_SSE2_MODE
      _mm_stream_si128( reinterpret_cast<__m128i*>( address ), value.value );
#else
      *address = value.value;
#endif
   }
   //**********************************************************************************************

 private:
   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_INTEGRAL_TYPE( T );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of the Stream class template for 4-byte integral data types.
// \ingroup intrinsics
*/
template< typename T >  // Type of the integral
struct Stream<T,4UL>
{
 public:
   //**Type definitions****************************************************************************
   typedef sse_int32_t  Type;
   //**********************************************************************************************

   //**Set function********************************************************************************
   static inline void stream( T* address, const Type& value )
   {
      BLAZE_INTERNAL_ASSERT( checkAlignment( address ), "Invalid alignment detected" );

#if BLAZE_MIC_MODE
      _mm512_store_epi32( address, value.value );
#elif BLAZE_AVX2_MODE
      _mm256_stream_si256( reinterpret_cast<__m256i*>( address ), value.value );
#elif BLAZE_SSE2_MODE
      _mm_stream_si128( reinterpret_cast<__m128i*>( address ), value.value );
#else
      *address = value.value;
#endif
   }
   //**********************************************************************************************

 private:
   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_INTEGRAL_TYPE( T );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of the Stream class template for 8-byte integral data types.
// \ingroup intrinsics
*/
template< typename T >  // Type of the integral
struct Stream<T,8UL>
{
 public:
   //**Type definitions****************************************************************************
   typedef sse_int64_t  Type;
   //**********************************************************************************************

   //**Set function********************************************************************************
   static inline void stream( T* address, const Type& value )
   {
      BLAZE_INTERNAL_ASSERT( checkAlignment( address ), "Invalid alignment detected" );

#if BLAZE_MIC_MODE
      _mm512_store_epi64( address, value.value );
#elif BLAZE_AVX2_MODE
      _mm256_stream_si256( reinterpret_cast<__m256i*>( address ), value.value );
#elif BLAZE_SSE2_MODE
      _mm_stream_si128( reinterpret_cast<__m128i*>( address ), value.value );
#else
      *address = value.value;
#endif
   }
   //**********************************************************************************************

 private:
   //**Compile time checks*************************************************************************
   BLAZE_CONSTRAINT_MUST_BE_INTEGRAL_TYPE( T );
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  INTRINSIC STREAM FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Aligned, non-temporal store of a vector of integral values.
// \ingroup intrinsics
//
// \param address The target address.
// \param value The integral vector to be streamed.
// \return void
*/
template< typename T >  // Type of the integral value
inline typename EnableIf< IsIntegral<T> >::Type
   stream( T* address, const typename Stream<T,sizeof(T)>::Type& value )
{
   Stream<T,sizeof(T)>::stream( address, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of a vector of 'float' values.
// \ingroup intrinsics
//
// \param address The target address.
// \param value The 'float' vector to be streamed.
// \return void
*/
inline void stream( float* address, const sse_float_t& value )
{
   BLAZE_INTERNAL_ASSERT( checkAlignment( address ), "Invalid alignment detected" );

#if BLAZE_MIC_MODE
   _mm512_storenr_ps( address, value.value );
#elif BLAZE_AVX_MODE
   _mm256_stream_ps( address, value.value );
#elif BLAZE_SSE_MODE
   _mm_stream_ps( address, value.value );
#else
   *address = value.value;
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of a vector of 'double' values.
// \ingroup intrinsics
//
// \param address The target address.
// \param value The 'double' vector to be streamed.
// \return void
*/
inline void stream( double* address, const sse_double_t& value )
{
   BLAZE_INTERNAL_ASSERT( checkAlignment( address ), "Invalid alignment detected" );

#if BLAZE_MIC_MODE
   _mm512_storenr_pd( address, value.value );
#elif BLAZE_AVX_MODE
   _mm256_stream_pd( address, value.value );
#elif BLAZE_SSE2_MODE
   _mm_stream_pd( address, value.value );
#else
   *address = value.value;
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of a vector of 'complex<float>' values.
// \ingroup intrinsics
//
// \param address The target address.
// \param value The 'complex<float>' vector to be streamed.
// \return void
*/
inline void stream( complex<float>* address, const sse_cfloat_t& value )
{
   BLAZE_STATIC_ASSERT  ( sizeof( complex<float> ) == 2UL*sizeof( float ) );
   BLAZE_INTERNAL_ASSERT( checkAlignment( address ), "Invalid alignment detected" );

#if BLAZE_MIC_MODE
   _mm512_storenr_ps( reinterpret_cast<float*>( address ), value.value );
#elif BLAZE_AVX_MODE
   _mm256_stream_ps( reinterpret_cast<float*>( address ), value.value );
#elif BLAZE_SSE_MODE
   _mm_stream_ps( reinterpret_cast<float*>( address ), value.value );
#else
   *address = value.value;
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of a vector of 'complex<double>' values.
// \ingroup intrinsics
//
// \param address The target address.
// \param value The 'complex<double>' vector to be streamed.
// \return void
*/
inline void stream( complex<double>* address, const sse_cdouble_t& value )
{
   BLAZE_STATIC_ASSERT  ( sizeof( complex<double> ) == 2UL*sizeof( double ) );
   BLAZE_INTERNAL_ASSERT( checkAlignment( address ), "Invalid alignment detected" );

#if BLAZE_MIC_MODE
   _mm512_storenr_pd( reinterpret_cast<double*>( address ), value.value );
#elif BLAZE_AVX_MODE
   _mm256_stream_pd( reinterpret_cast<double*>( address ), value.value );
#elif BLAZE_SSE2_MODE
   _mm_stream_pd( reinterpret_cast<double*>( address ), value.value );
#else
   *address = value.value;
#endif
}
//*************************************************************************************************

} // namespace blaze

#endif
