//=================================================================================================
/*!
//  \file blaze/math/dense/DynamicVector.h
//  \brief Header file for the implementation of an arbitrarily sized vector
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_DENSE_DYNAMICVECTOR_H_
#define _BLAZE_MATH_DENSE_DYNAMICVECTOR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <algorithm>
#include <stdexcept>
#include <blaze/math/dense/DenseIterator.h>
#include <blaze/math/expressions/DenseVector.h>
#include <blaze/math/Forward.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/traits/AddTrait.h>
#include <blaze/math/traits/CrossTrait.h>
#include <blaze/math/traits/DivTrait.h>
#include <blaze/math/traits/MathTrait.h>
#include <blaze/math/traits/MultTrait.h>
#include <blaze/math/traits/SubTrait.h>
#include <blaze/math/traits/SubvectorTrait.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsSparseVector.h>
#include <blaze/system/CacheSize.h>
#include <blaze/system/Restrict.h>
#include <blaze/system/TransposeFlag.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Const.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/Pointer.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/constraints/Vectorizable.h>
#include <blaze/util/constraints/Volatile.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/Memory.h>
#include <blaze/util/Null.h>
#include <blaze/util/Template.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsNumeric.h>
#include <blaze/util/typetraits/IsSame.h>
#include <blaze/util/typetraits/IsVectorizable.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup dynamic_vector DynamicVector
// \ingroup dense_vector
*/
/*!\brief Efficient implementation of an arbitrary sized vector.
// \ingroup dynamic_vector
//
// The DynamicVector class template is the representation of an arbitrary sized vector with
// dynamically allocated elements of arbitrary type. The type of the elements and the transpose
// flag of the vector can be specified via the two template parameters:

   \code
   template< typename Type, bool TF >
   class DynamicVector;
   \endcode

//  - Type: specifies the type of the vector elements. DynamicVector can be used with any
//          non-cv-qualified, non-reference, non-pointer element type.
//  - TF  : specifies whether the vector is a row vector (\a blaze::rowVector) or a column
//          vector (\a blaze::columnVector). The default value is \a blaze::columnVector.
//
// These contiguously stored elements can be directly accessed with the subscript operator. The
// numbering of the vector elements is

                             \f[\left(\begin{array}{*{5}{c}}
                             0 & 1 & 2 & \cdots & N-1 \\
                             \end{array}\right)\f]

// The use of DynamicVector is very natural and intuitive. All operations (addition, subtraction,
// multiplication, scaling, ...) can be performed on all possible combinations of dense and sparse
// vectors with fitting element types. The following example gives an impression of the use of
// DynamicVector:

   \code
   using blaze::DynamicVector;
   using blaze::CompressedVector;
   using blaze::DynamicMatrix;

   DynamicVector<double> a( 2 );  // Non-initialized 2D vector of size 2
   a[0] = 1.0;                    // Initialization of the first element
   a[1] = 2.0;                    // Initialization of the second element

   DynamicVector<double>   b( 2, 2.0  );  // Directly, homogeneously initialized 2D vector
   CompressedVector<float> c( 2 );        // Empty sparse single precision vector
   DynamicVector<double>   d;             // Default constructed dynamic vector
   DynamicMatrix<double>   A;             // Default constructed row-major matrix

   d = a + b;  // Vector addition between vectors of equal element type
   d = a - c;  // Vector subtraction between a dense and sparse vector with different element types
   d = a * b;  // Component-wise vector multiplication

   a *= 2.0;      // In-place scaling of vector
   d  = a * 2.0;  // Scaling of vector a
   d  = 2.0 * a;  // Scaling of vector a

   d += a - b;  // Addition assignment
   d -= a + c;  // Subtraction assignment
   d *= a * b;  // Multiplication assignment

   double scalar = trans( a ) * b;  // Scalar/dot/inner product between two vectors

   A = a * trans( b );  // Outer product between two vectors
   \endcode
*/
template< typename Type                     // Data type of the vector
        , bool TF = defaultTransposeFlag >  // Transpose flag
class DynamicVector : public DenseVector< DynamicVector<Type,TF>, TF >
{
 private:
   //**Type definitions****************************************************************************
   typedef IntrinsicTrait<Type>  IT;  //!< Intrinsic trait for the vector element type.
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DynamicVector<Type,TF>     This;            //!< Type of this DynamicVector instance.
   typedef This                       ResultType;      //!< Result type for expression template evaluations.
   typedef DynamicVector<Type,!TF>    TransposeType;   //!< Transpose type for expression template evaluations.
   typedef Type                       ElementType;     //!< Type of the vector elements.
   typedef typename IT::Type          IntrinsicType;   //!< Intrinsic type of the vector elements.
   typedef const Type&                ReturnType;      //!< Return type for expression template evaluations
   typedef const DynamicVector&       CompositeType;   //!< Data type for composite expression templates.
   typedef Type&                      Reference;       //!< Reference to a non-constant vector value.
   typedef const Type&                ConstReference;  //!< Reference to a constant vector value.
   typedef DenseIterator<Type>        Iterator;        //!< Iterator over non-constant elements.
   typedef DenseIterator<const Type>  ConstIterator;   //!< Iterator over constant elements.
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation flag for intrinsic optimization.
   /*! The \a vectorizable compilation flag indicates whether expressions the vector is involved
       in can be optimized via intrinsics. In case the element type of the vector is an intrinsic
       data type, the \a vectorizable compilation flag is set to \a true, otherwise it is set to
       \a false. */
   enum { vectorizable = IsVectorizable<Type>::value };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
                           explicit inline DynamicVector();
                           explicit inline DynamicVector( size_t n );
                           explicit inline DynamicVector( size_t n, const Type& init );
                                    inline DynamicVector( const DynamicVector& v );
   template< typename VT >          inline DynamicVector( const Vector<VT,TF>& v );

   template< typename Other, size_t N >
   explicit inline DynamicVector( const Other (&rhs)[N] );
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   /*!\name Destructor */
   //@{
   inline ~DynamicVector();
   //@}
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator[]( size_t index );
   inline ConstReference operator[]( size_t index ) const;
   inline Type*          data  ();
   inline const Type*    data  () const;
   inline Iterator       begin ();
   inline ConstIterator  begin () const;
   inline ConstIterator  cbegin() const;
   inline Iterator       end   ();
   inline ConstIterator  end   () const;
   inline ConstIterator  cend  () const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
   template< typename Other, size_t N >
   inline DynamicVector& operator=( const Other (&rhs)[N] );

                           inline DynamicVector& operator= ( const Type& rhs );
                           inline DynamicVector& operator= ( const DynamicVector& rhs );
   template< typename VT > inline DynamicVector& operator= ( const Vector<VT,TF>& rhs );
   template< typename VT > inline DynamicVector& operator+=( const Vector<VT,TF>& rhs );
   template< typename VT > inline DynamicVector& operator-=( const Vector<VT,TF>& rhs );
   template< typename VT > inline DynamicVector& operator*=( const Vector<VT,TF>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DynamicVector >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DynamicVector >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t         size() const;
                              inline size_t         capacity() const;
                              inline size_t         nonZeros() const;
                              inline void           reset();
                              inline void           clear();
                              inline void           resize( size_t n, bool preserve=true );
                              inline void           extend( size_t n, bool preserve=true );
                              inline void           reserve( size_t n );
   template< typename Other > inline DynamicVector& scale( Other scalar );
                              inline void           swap( DynamicVector& v ) /* throw() */;
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedAddAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value &&
                     IntrinsicTrait<Type>::addition };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedSubAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value &&
                     IntrinsicTrait<Type>::subtraction };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename VT >
   struct VectorizedMultAssign {
      enum { value = vectorizable && VT::vectorizable &&
                     IsSame<Type,typename VT::ElementType>::value &&
                     IntrinsicTrait<Type>::multiplication };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool canAlias ( const Other* alias ) const;
   template< typename Other > inline bool isAliased( const Other* alias ) const;

   inline IntrinsicType load  ( size_t index ) const;
   inline IntrinsicType loadu ( size_t index ) const;
   inline void          store ( size_t index, const IntrinsicType& value );
   inline void          storeu( size_t index, const IntrinsicType& value );
   inline void          stream( size_t index, const IntrinsicType& value );

   template< typename VT >
   inline typename DisableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAssign<VT> >::Type
      assign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void assign( const SparseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedAddAssign<VT> >::Type
      addAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void addAssign( const SparseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedSubAssign<VT> >::Type
      subAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void subAssign( const SparseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename DisableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT >
   inline typename EnableIf< VectorizedMultAssign<VT> >::Type
      multAssign( const DenseVector<VT,TF>& rhs );

   template< typename VT > inline void multAssign( const SparseVector<VT,TF>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   inline size_t adjustCapacity( size_t minCapacity ) const;
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   size_t size_;             //!< The current size/dimension of the vector.
   size_t capacity_;         //!< The maximum capacity of the vector.
   Type* BLAZE_RESTRICT v_;  //!< The dynamically allocated vector elements.
                             /*!< Access to the vector elements is gained via the subscript operator.
                                  The order of the elements is
                                  \f[\left(\begin{array}{*{5}{c}}
                                  0 & 1 & 2 & \cdots & N-1 \\
                                  \end{array}\right)\f] */
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_NOT_BE_POINTER_TYPE  ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_REFERENCE_TYPE( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_CONST         ( Type );
   BLAZE_CONSTRAINT_MUST_NOT_BE_VOLATILE      ( Type );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The default constructor for DynamicVector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector()
   : size_    ( 0UL )   // The current size/dimension of the vector
   , capacity_( 0UL )   // The maximum capacity of the vector
   , v_       ( NULL )  // The vector elements
{}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a vector of size \a n. No element initialization is performed!
//
// \param n The size of the vector.
//
// \b Note: This constructor is only responsible to allocate the required dynamic memory. No
//          element initialization is performed!
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector( size_t n )
   : size_    ( n )                            // The current size/dimension of the vector
   , capacity_( adjustCapacity( n ) )          // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   if( IsNumeric<Type>::value ) {
      for( size_t i=size_; i<capacity_; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Constructor for a homogeneous initialization of all \a n vector elements.
//
// \param n The size of the vector.
// \param init The initial value of the vector elements.
//
// All vector elements are initialized with the specified value.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector( size_t n, const Type& init )
   : size_    ( n )                            // The current size/dimension of the vector
   , capacity_( adjustCapacity( n ) )          // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   for( size_t i=0UL; i<size_; ++i )
      v_[i] = init;

   if( IsNumeric<Type>::value ) {
      for( size_t i=size_; i<capacity_; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief The copy constructor for DynamicVector.
//
// \param v Vector to be copied.
//
// The copy constructor is explicitly defined due to the required dynamic memory management
// and in order to enable/facilitate NRV optimization.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::DynamicVector( const DynamicVector& v )
   : size_    ( v.size_ )                      // The current size/dimension of the vector
   , capacity_( adjustCapacity( v.size_ ) )    // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   BLAZE_INTERNAL_ASSERT( capacity_ <= v.capacity_, "Invalid capacity estimation" );

   for( size_t i=0UL; i<capacity_; ++i )
      v_[i] = v.v_[i];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Conversion constructor from different vectors.
//
// \param v Vector to be copied.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the foreign vector
inline DynamicVector<Type,TF>::DynamicVector( const Vector<VT,TF>& v )
   : size_    ( (~v).size() )                  // The current size/dimension of the vector
   , capacity_( adjustCapacity( size_ ) )      // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   using blaze::assign;

   if( IsNumeric<Type>::value ) {
      if( IsSparseVector<VT>::value )
         reset();
      for( size_t i=size_; i<capacity_; ++i )
         v_[i] = Type();
   }

   assign( *this, ~v );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Array initialization of all vector elements.
//
// \param rhs N-dimensional array for the initialization.
//
// This assignment operator offers the option to directly initialize the elements of the vector:

   \code
   const real init[4] = { 1, 2, 3 };
   blaze::DynamicVector<real> v( init );
   \endcode

// The vector is sized accoring to the size of the array and initialized with the given values.
// Missing values are initialized with zero (as e.g. the fourth element in the example).
*/
template< typename Type   // Data type of the vector
        , bool TF >       // Transpose flag
template< typename Other  // Data type of the initialization array
        , size_t N >      // Dimension of the initialization array
inline DynamicVector<Type,TF>::DynamicVector( const Other (&rhs)[N] )
   : size_    ( N )                            // The current size/dimension of the vector
   , capacity_( adjustCapacity( N ) )          // The maximum capacity of the vector
   , v_       ( allocate<Type>( capacity_ ) )  // The vector elements
{
   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs[i];

   if( IsNumeric<Type>::value ) {
      for( size_t i=size_; i<capacity_; ++i )
         v_[i] = Type();
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  DESTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The destructor for DynamicVector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>::~DynamicVector()
{
   deallocate( v_ );
}
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Subscript operator for the direct access to the vector elements.
//
// \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::Reference
   DynamicVector<Type,TF>::operator[]( size_t index )
{
   BLAZE_USER_ASSERT( index < size_, "Invalid vector access index" );
   return v_[index];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subscript operator for the direct access to the vector elements.
//
// \param index Access index. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::ConstReference
   DynamicVector<Type,TF>::operator[]( size_t index ) const
{
   BLAZE_USER_ASSERT( index < size_, "Invalid vector access index" );
   return v_[index];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the vector elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline Type* DynamicVector<Type,TF>::data()
{
   return v_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the vector elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline const Type* DynamicVector<Type,TF>::data() const
{
   return v_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of the dynamic vector.
//
// \return Iterator to the first element of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::Iterator DynamicVector<Type,TF>::begin()
{
   return Iterator( v_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of the dynamic vector.
//
// \return Iterator to the first element of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::ConstIterator DynamicVector<Type,TF>::begin() const
{
   return ConstIterator( v_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first element of the dynamic vector.
//
// \return Iterator to the first element of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::ConstIterator DynamicVector<Type,TF>::cbegin() const
{
   return ConstIterator( v_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of the dynamic vector.
//
// \return Iterator just past the last element of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::Iterator DynamicVector<Type,TF>::end()
{
   return Iterator( v_ + size_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of the dynamic vector.
//
// \return Iterator just past the last element of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::ConstIterator DynamicVector<Type,TF>::end() const
{
   return ConstIterator( v_ + size_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last element of the dynamic vector.
//
// \return Iterator just past the last element of the dynamic vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::ConstIterator DynamicVector<Type,TF>::cend() const
{
   return ConstIterator( v_ + size_ );
}
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Array assignment to all vector elements.
//
// \param rhs N-dimensional array for the assignment.
// \return Reference to the assigned vector.
//
// This assignment operator offers the option to directly set all elements of the vector:

   \code
   const real init[4] = { 1, 2, 3 };
   blaze::DynamicVector<real> v;
   v = init;
   \endcode

// The vector is resized accoring to the size of the array and initialized with the given values.
// Missing values are initialized with zero (as e.g. the fourth element in the example).
*/
template< typename Type   // Data type of the vector
        , bool TF >       // Transpose flag
template< typename Other  // Data type of the initialization array
        , size_t N >      // Dimension of the initialization array
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( const Other (&rhs)[N] )
{
   resize( N, false );

   for( size_t i=0UL; i<N; ++i )
      v_[i] = rhs[i];

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Homogenous assignment to all vector elements.
//
// \param rhs Scalar value to be assigned to all vector elements.
// \return Reference to the assigned vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( const Type& rhs )
{
   for( size_t i=0UL; i<size_; ++i )
      v_[i] = rhs;
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Copy assignment operator for DynamicVector.
//
// \param rhs Vector to be copied.
// \return Reference to the assigned vector.
//
// The vector is resized according to the given N-dimensional vector and initialized as a
// copy of this vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( const DynamicVector& rhs )
{
   if( &rhs == this ) return *this;

   resize( rhs.size_, false );

   for( size_t i=0UL; i<size_; ++i )
      v_[i] = rhs.v_[i];

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different vectors.
//
// \param rhs Vector to be copied.
// \return Reference to the assigned vector.
//
// The vector is resized according to the given vector and initialized as a copy of this vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator=( const Vector<VT,TF>& rhs )
{
   using blaze::assign;

   if( (~rhs).canAlias( this ) ) {
      DynamicVector tmp( ~rhs );
      swap( tmp );
   }
   else {
      resize( (~rhs).size(), false );
      if( IsSparseVector<VT>::value )
         reset();
      assign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Addition assignment operator for the addition of a vector (\f$ \vec{a}+=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be added to the vector.
// \return Reference to the vector.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator+=( const Vector<VT,TF>& rhs )
{
   using blaze::addAssign;

   if( (~rhs).size() != size_ )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( (~rhs).canAlias( this ) ) {
      typename VT::ResultType tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subtraction assignment operator for the subtraction of a vector
//        (\f$ \vec{a}-=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be subtracted from the vector.
// \return Reference to the vector.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator-=( const Vector<VT,TF>& rhs )
{
   using blaze::subAssign;

   if( (~rhs).size() != size_ )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( (~rhs).canAlias( this ) ) {
      typename VT::ResultType tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication of a vector
//        (\f$ \vec{a}*=\vec{b} \f$).
//
// \param rhs The right-hand side vector to be multiplied with the vector.
// \return Reference to the vector.
// \exception std::invalid_argument Vector sizes do not match.
//
// In case the current sizes of the two vectors don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side vector
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::operator*=( const Vector<VT,TF>& rhs )
{
   using blaze::assign;

   if( (~rhs).size() != size_ )
      throw std::invalid_argument( "Vector sizes do not match" );

   if( (~rhs).canAlias( this ) || IsSparseVector<VT>::value ) {
      DynamicVector<Type,TF> tmp( *this * (~rhs) );
      swap( tmp );
   }
   else {
      assign( *this, *this * (~rhs) );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication between a vector and
//        a scalar value (\f$ \vec{a}*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the vector.
*/
template< typename Type     // Data type of the vector
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DynamicVector<Type,TF> >::Type&
   DynamicVector<Type,TF>::operator*=( Other rhs )
{
   return operator=( (*this) * rhs );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a vector by a scalar value
//        (\f$ \vec{a}/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the vector.
//
// \b Note: A division by zero is only checked by an user assert.
*/
template< typename Type     // Data type of the vector
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DynamicVector<Type,TF> >::Type&
   DynamicVector<Type,TF>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   return operator=( (*this) / rhs );
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns the current size/dimension of the vector.
//
// \return The size of the vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::size() const
{
   return size_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the maximum capacity of the vector.
//
// \return The capacity of the vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::capacity() const
{
   return capacity_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the vector.
//
// \return The number of non-zero elements in the vector.
//
// Note that the number of non-zero elements is always less than or equal to the current size
// of the vector.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::nonZeros() const
{
   size_t nonzeros( 0 );

   for( size_t i=0UL; i<size_; ++i ) {
      if( !isDefault( v_[i] ) )
         ++nonzeros;
   }

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::reset()
{
   using blaze::reset;
   for( size_t i=0UL; i<size_; ++i )
      reset( v_[i] );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the vector.
//
// \return void
//
// After the clear() function, the size of the vector is 0.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::clear()
{
   resize( 0UL, false );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Changing the size of the vector.
//
// \param n The new size of the vector.
// \param preserve \a true if the old values of the vector should be preserved, \a false if not.
// \return void
//
// This function resizes the vector using the given size to \a n. During this operation, new
// dynamic memory may be allocated in case the capacity of the vector is too small. Therefore
// this function potentially changes all vector elements. In order to preserve the old vector
// values, the \a preserve flag can be set to \a true. However, new vector elements are not
// initialized!\n
// The following example illustrates the resize operation of a vector of size 2 to a vector of
// size 4. The new, uninitialized elements are marked with \a x:

                              \f[
                              \left(\begin{array}{*{2}{c}}
                              1 & 2 \\
                              \end{array}\right)

                              \Longrightarrow

                              \left(\begin{array}{*{4}{c}}
                              1 & 2 & x & x \\
                              \end{array}\right)
                              \f]
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::resize( size_t n, bool preserve )
{
   if( n > capacity_ )
   {
      // Allocating a new array
      const size_t newCapacity( adjustCapacity( n ) );
      Type* BLAZE_RESTRICT tmp = allocate<Type>( newCapacity );

      // Initializing the new array
      if( preserve ) {
         std::copy( v_, v_+size_, tmp );
      }

      if( IsNumeric<Type>::value ) {
         for( size_t i=size_; i<newCapacity; ++i )
            tmp[i] = Type();
      }

      // Replacing the old array
      std::swap( v_, tmp );
      deallocate( tmp );
      capacity_ = newCapacity;
   }
   else if( IsNumeric<Type>::value && n < size_ )
   {
      for( size_t i=n; i<size_; ++i )
         v_[i] = Type();
   }

   size_ = n;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Extending the size of the vector.
//
// \param n Number of additional vector elements.
// \param preserve \a true if the old values of the vector should be preserved, \a false if not.
// \return void
//
// This function increases the vector size by \a n elements. During this operation, new dynamic
// memory may be allocated in case the capacity of the vector is too small. Therefore this
// function potentially changes all vector elements. In order to preserve the old vector values,
// the \a preserve flag can be set to \a true. However, new vector elements are not initialized!
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::extend( size_t n, bool preserve )
{
   resize( size_+n, preserve );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Setting the minimum capacity of the vector.
//
// \param n The new minimum capacity of the vector.
// \return void
//
// This function increases the capacity of the vector to at least \a n elements. The current
// values of the vector elements are preserved.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::reserve( size_t n )
{
   if( n > capacity_ )
   {
      // Allocating a new array
      const size_t newCapacity( adjustCapacity( n ) );
      Type* BLAZE_RESTRICT tmp = allocate<Type>( newCapacity );

      // Initializing the new array
      std::copy( v_, v_+size_, tmp );

      if( IsNumeric<Type>::value ) {
         for( size_t i=size_; i<newCapacity; ++i )
            tmp[i] = Type();
      }

      // Replacing the old array
      std::swap( tmp, v_ );
      deallocate( tmp );
      capacity_ = newCapacity;
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the vector by the scalar value \a scalar (\f$ \vec{a}=\vec{b}*s \f$).
//
// \param scalar The scalar value for the vector scaling.
// \return Reference to the vector.
*/
template< typename Type     // Data type of the vector
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the scalar value
inline DynamicVector<Type,TF>& DynamicVector<Type,TF>::scale( Other scalar )
{
   for( size_t i=0UL; i<size_; ++i )
      v_[i] *= scalar;
   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two vectors.
//
// \param v The vector to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::swap( DynamicVector& v ) /* throw() */
{
   std::swap( size_, v.size_ );
   std::swap( capacity_, v.capacity_ );
   std::swap( v_, v.v_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Adjusting the new capacity of the vector according to its data type \a Type.
//
// \param minCapacity The minimum necessary capacity.
// \return The new capacity.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline size_t DynamicVector<Type,TF>::adjustCapacity( size_t minCapacity ) const
{
   if( IsNumeric<Type>::value )
      return minCapacity + ( IT::size - ( minCapacity % IT::size ) ) % IT::size;
   else return minCapacity;
}
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether the vector can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this vector, \a false if not.
//
// This function returns whether the given address can alias with the vector. In contrast
// to the isAliased() function this function is allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename Type     // Data type of the vector
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the foreign expression
inline bool DynamicVector<Type,TF>::canAlias( const Other* alias ) const
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the vector is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this vector, \a false if not.
//
// This function returns whether the given address is aliased with the vector. In contrast
// to the conAlias() function this function is not allowed to use compile time expressions
// to optimize the evaluation.
*/
template< typename Type     // Data type of the vector
        , bool TF >         // Transpose flag
template< typename Other >  // Data type of the foreign expression
inline bool DynamicVector<Type,TF>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( this ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned load of an intrinsic element of the vector.
//
// \param index Access index. The index must be smaller than the number of vector elements.
// \return The loaded intrinsic element.
//
// This function performs an aligned load of a specific intrinsic element of the dense vector.
// The index must be smaller than the number of vector elements and it must be a multiple of
// the number of values inside the intrinsic element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::IntrinsicType
   DynamicVector<Type,TF>::load( size_t index ) const
{
   using blaze::load;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( index            <  size_    , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + IT::size <= capacity_, "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index % IT::size == 0UL      , "Invalid vector access index" );

   return load( v_+index );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned load of an intrinsic element of the vector.
//
// \param index Access index. The index must be smaller than the number of vector elements.
// \return The loaded intrinsic element.
//
// This function performs an unaligned load of a specific intrinsic element of the dense vector.
// The index must be smaller than the number of vector elements and it must be a multiple of
// the number of values inside the intrinsic element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline typename DynamicVector<Type,TF>::IntrinsicType
   DynamicVector<Type,TF>::loadu( size_t index ) const
{
   using blaze::loadu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( index            <  size_    , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + IT::size <= capacity_, "Invalid vector access index" );

   return loadu( v_+index );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned store of an intrinsic element of the vector.
//
// \param index Access index. The index must be smaller than the number of vector elements.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned store of a specific intrinsic element of the dense vector.
// The index must be smaller than the number of vector elements and it must be a multiple of
// the number of values inside the intrinsic element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::store( size_t index, const IntrinsicType& value )
{
   using blaze::store;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( index            <  size_    , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + IT::size <= capacity_, "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index % IT::size == 0UL      , "Invalid vector access index" );

   store( v_+index, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned store of an intrinsic element of the dense vector.
//
// \param index Access index. The index must be smaller than the number of vector elements.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an unaligned store of a specific intrinsic element of the dense vector.
// The index must be smaller than the number of vector elements and it must be a multiple of the
// number of values inside the intrinsic element. This function must \b NOT be called explicitly!
// It is used internally for the performance optimized evaluation of expression templates.
// Calling this function explicitly might result in erroneous results and/or in compilation
// errors.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::storeu( size_t index, const IntrinsicType& value )
{
   using blaze::storeu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( index            <  size_    , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + IT::size <= capacity_, "Invalid vector access index" );

   storeu( v_+index, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of an intrinsic element of the dense vector.
//
// \param index Access index. The index must be smaller than the number of vector elements.
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned, non-temporal store of a specific intrinsic element of
// the dense vector. The index must be smaller than the number of vector elements and it must
// be a multiple of the number of values inside the intrinsic element. This function must
// \b NOT be called explicitly! It is used internally for the performance optimized evaluation
// of expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void DynamicVector<Type,TF>::stream( size_t index, const IntrinsicType& value )
{
   using blaze::stream;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   BLAZE_INTERNAL_ASSERT( index            <  size_    , "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index + IT::size <= capacity_, "Invalid vector access index" );
   BLAZE_INTERNAL_ASSERT( index % IT::size == 0UL      , "Invalid vector access index" );

   stream( v_+index, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DynamicVector<Type,TF>::assign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t iend( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<iend; i+=2UL ) {
      v_[i    ] = (~rhs)[i    ];
      v_[i+1UL] = (~rhs)[i+1UL];
   }
   if( iend < (~rhs).size() )
      v_[iend] = (~rhs)[iend];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAssign<VT> >::Type
   DynamicVector<Type,TF>::assign( const DenseVector<VT,TF>& rhs )
{
   using blaze::store;
   using blaze::stream;

   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   if( size_ > ( cacheSize/( sizeof(Type) * 3UL ) ) && !(~rhs).isAliased( this ) )
   {
      for( size_t i=0UL; i<size_; i+=IT::size ) {
         stream( v_+i, (~rhs).load(i) );
      }
   }
   else
   {
      const size_t iend( size_ & size_t(-IT::size*4) );
      BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == iend, "Invalid end calculation" );

      for( size_t i=0UL; i<iend; i+=IT::size*4UL ) {
         store( v_+i             , (~rhs).load(i             ) );
         store( v_+i+IT::size    , (~rhs).load(i+IT::size    ) );
         store( v_+i+IT::size*2UL, (~rhs).load(i+IT::size*2UL) );
         store( v_+i+IT::size*3UL, (~rhs).load(i+IT::size*3UL) );
      }
      for( size_t i=iend; i<size_; i+=IT::size ) {
         store( v_+i, (~rhs).load(i) );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::assign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      v_[element->index()] = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DynamicVector<Type,TF>::addAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t iend( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<iend; i+=2UL ) {
      v_[i    ] += (~rhs)[i    ];
      v_[i+1UL] += (~rhs)[i+1UL];
   }
   if( iend < (~rhs).size() )
      v_[iend] += (~rhs)[iend];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the addition assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedAddAssign<VT> >::Type
   DynamicVector<Type,TF>::addAssign( const DenseVector<VT,TF>& rhs )
{
   using blaze::load;
   using blaze::store;

   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   const size_t iend( size_ & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == iend, "Invalid end calculation" );

   for( size_t i=0UL; i<iend; i+=IT::size*4UL ) {
      store( v_+i             , load(v_+i             ) + (~rhs).load(i             ) );
      store( v_+i+IT::size    , load(v_+i+IT::size    ) + (~rhs).load(i+IT::size    ) );
      store( v_+i+IT::size*2UL, load(v_+i+IT::size*2UL) + (~rhs).load(i+IT::size*2UL) );
      store( v_+i+IT::size*3UL, load(v_+i+IT::size*3UL) + (~rhs).load(i+IT::size*3UL) );
   }
   for( size_t i=iend; i<size_; i+=IT::size ) {
      store( v_+i, load(v_+i) + (~rhs).load(i) );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::addAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      v_[element->index()] += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DynamicVector<Type,TF>::subAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t iend( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<iend; i+=2UL ) {
      v_[i    ] -= (~rhs)[i    ];
      v_[i+1UL] -= (~rhs)[i+1UL];
   }
   if( iend < (~rhs).size() )
      v_[iend] -= (~rhs)[iend];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedSubAssign<VT> >::Type
   DynamicVector<Type,TF>::subAssign( const DenseVector<VT,TF>& rhs )
{
   using blaze::load;
   using blaze::store;

   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   const size_t iend( size_ & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == iend, "Invalid end calculation" );

   for( size_t i=0UL; i<iend; i+=IT::size*4UL ) {
      store( v_+i             , load(v_+i             ) - (~rhs).load(i             ) );
      store( v_+i+IT::size    , load(v_+i+IT::size    ) - (~rhs).load(i+IT::size    ) );
      store( v_+i+IT::size*2UL, load(v_+i+IT::size*2UL) - (~rhs).load(i+IT::size*2UL) );
      store( v_+i+IT::size*3UL, load(v_+i+IT::size*3UL) - (~rhs).load(i+IT::size*3UL) );
   }
   for( size_t i=iend; i<size_; i+=IT::size ) {
      store( v_+i, load(v_+i) - (~rhs).load(i) );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::subAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      v_[element->index()] -= element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename DisableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DynamicVector<Type,TF>::multAssign( const DenseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const size_t iend( (~rhs).size() & size_t(-2) );
   for( size_t i=0UL; i<iend; i+=2UL ) {
      v_[i    ] *= (~rhs)[i    ];
      v_[i+1UL] *= (~rhs)[i+1UL];
   }
   if( iend < (~rhs).size() )
      v_[iend] *= (~rhs)[iend];
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the multiplication assignment of a dense vector.
//
// \param rhs The right-hand side dense vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side dense vector
inline typename EnableIf< typename DynamicVector<Type,TF>::BLAZE_TEMPLATE VectorizedMultAssign<VT> >::Type
   DynamicVector<Type,TF>::multAssign( const DenseVector<VT,TF>& rhs )
{
   using blaze::load;
   using blaze::store;

   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( Type );

   const size_t iend( size_ & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( size_ - ( size_ % (IT::size*4UL) ) ) == iend, "Invalid end calculation" );

   for( size_t i=0UL; i<iend; i+=IT::size*4UL ) {
      store( v_+i             , load(v_+i             ) * (~rhs).load(i             ) );
      store( v_+i+IT::size    , load(v_+i+IT::size    ) * (~rhs).load(i+IT::size    ) );
      store( v_+i+IT::size*2UL, load(v_+i+IT::size*2UL) * (~rhs).load(i+IT::size*2UL) );
      store( v_+i+IT::size*3UL, load(v_+i+IT::size*3UL) * (~rhs).load(i+IT::size*3UL) );
   }
   for( size_t i=iend; i<size_; i+=IT::size ) {
      store( v_+i, load(v_+i) * (~rhs).load(i) );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the multiplication assignment of a sparse vector.
//
// \param rhs The right-hand side sparse vector to be multiplied.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
template< typename VT >  // Type of the right-hand side sparse vector
inline void DynamicVector<Type,TF>::multAssign( const SparseVector<VT,TF>& rhs )
{
   BLAZE_INTERNAL_ASSERT( size_ == (~rhs).size(), "Invalid vector sizes" );

   const DynamicVector tmp( *this );

   reset();

   for( typename VT::ConstIterator element=(~rhs).begin(); element!=(~rhs).end(); ++element )
      v_[element->index()] = tmp[element->index()] * element->value();
}
//*************************************************************************************************




//=================================================================================================
//
//  DYNAMICVECTOR OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name DynamicVector operators */
//@{
template< typename Type, bool TF >
inline void reset( DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline void clear( DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline bool isDefault( const DynamicVector<Type,TF>& v );

template< typename Type, bool TF >
inline void swap( DynamicVector<Type,TF>& a, DynamicVector<Type,TF>& b ) /* throw() */;
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given dynamic vector.
// \ingroup dense_vector_N
//
// \param v The dynamic vector to be resetted.
// \return void
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void reset( DynamicVector<Type,TF>& v )
{
   v.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given dynamic vector.
// \ingroup dense_vector_N
//
// \param v The dynamic vector to be cleared.
// \return void
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void clear( DynamicVector<Type,TF>& v )
{
   v.clear();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given dynamic vector is in default state.
// \ingroup dense_vector_N
//
// \param v The dynamic vector to be tested for its default state.
// \return \a true in case the given vector is component-wise zero, \a false otherwise.
//
// This function checks whether the N-dimensional vector is in default state. For instance,
// in case the vector is instantiated for a built-in integral or floating point data type,
// the function returns \a true in case all vector elements are 0 and \a false in case any
// vector element is not 0. The following example demonstrates the use of the \a isDefault
// function:

   \code
   blaze::DynamicVector<int> a;
   // ... Resizing and initialization
   if( isDefault( a ) ) { ... }
   \endcode
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline bool isDefault( const DynamicVector<Type,TF>& v )
{
   for( size_t i=0UL; i<v.size(); ++i )
      if( !isDefault( v[i] ) ) return false;
   return true;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Swapping the contents of two vectors.
// \ingroup dense_vector_N
//
// \param a The first vector to be swapped.
// \param b The second vector to be swapped.
// \return void
// \exception no-throw guarantee.
*/
template< typename Type  // Data type of the vector
        , bool TF >      // Transpose flag
inline void swap( DynamicVector<Type,TF>& a, DynamicVector<Type,TF>& b ) /* throw() */
{
   a.swap( b );
}
//*************************************************************************************************




//=================================================================================================
//
//  ISRESIZABLE SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T, bool TF >
struct IsResizable< DynamicVector<T,TF> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool TF >
struct IsResizable< const DynamicVector<T,TF> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool TF >
struct IsResizable< volatile DynamicVector<T,TF> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};

template< typename T, bool TF >
struct IsResizable< const volatile DynamicVector<T,TF> > : public TrueType
{
   enum { value = 1 };
   typedef TrueType  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ADDTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, bool TF, typename T2, size_t N >
struct AddTrait< DynamicVector<T1,TF>, StaticVector<T2,N,TF> >
{
   typedef StaticVector< typename AddTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, size_t N, bool TF, typename T2 >
struct AddTrait< StaticVector<T1,N,TF>, DynamicVector<T2,TF> >
{
   typedef StaticVector< typename AddTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, bool TF, typename T2, size_t N >
struct AddTrait< DynamicVector<T1,TF>, HybridVector<T2,N,TF> >
{
   typedef HybridVector< typename AddTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, size_t N, bool TF, typename T2 >
struct AddTrait< HybridVector<T1,N,TF>, DynamicVector<T2,TF> >
{
   typedef HybridVector< typename AddTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, bool TF, typename T2 >
struct AddTrait< DynamicVector<T1,TF>, DynamicVector<T2,TF> >
{
   typedef DynamicVector< typename AddTrait<T1,T2>::Type, TF >  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SUBTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, bool TF, typename T2, size_t N >
struct SubTrait< DynamicVector<T1,TF>, StaticVector<T2,N,TF> >
{
   typedef StaticVector< typename SubTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, size_t N, bool TF, typename T2 >
struct SubTrait< StaticVector<T1,N,TF>, DynamicVector<T2,TF> >
{
   typedef StaticVector< typename SubTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, bool TF, typename T2, size_t N >
struct SubTrait< DynamicVector<T1,TF>, HybridVector<T2,N,TF> >
{
   typedef HybridVector< typename SubTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, size_t N, bool TF, typename T2 >
struct SubTrait< HybridVector<T1,N,TF>, DynamicVector<T2,TF> >
{
   typedef HybridVector< typename SubTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, bool TF, typename T2 >
struct SubTrait< DynamicVector<T1,TF>, DynamicVector<T2,TF> >
{
   typedef DynamicVector< typename SubTrait<T1,T2>::Type, TF >  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  MULTTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, bool TF, typename T2 >
struct MultTrait< DynamicVector<T1,TF>, T2 >
{
   typedef DynamicVector< typename MultTrait<T1,T2>::Type, TF >  Type;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T2 );
};

template< typename T1, typename T2, bool TF >
struct MultTrait< T1, DynamicVector<T2,TF> >
{
   typedef DynamicVector< typename MultTrait<T1,T2>::Type, TF >  Type;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T1 );
};

template< typename T1, bool TF, typename T2, size_t N >
struct MultTrait< DynamicVector<T1,TF>, StaticVector<T2,N,TF> >
{
   typedef StaticVector< typename MultTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, typename T2, size_t N >
struct MultTrait< DynamicVector<T1,false>, StaticVector<T2,N,true> >
{
   typedef DynamicMatrix< typename MultTrait<T1,T2>::Type, false >  Type;
};

template< typename T1, typename T2, size_t N >
struct MultTrait< DynamicVector<T1,true>, StaticVector<T2,N,false> >
{
   typedef typename MultTrait<T1,T2>::Type  Type;
};

template< typename T1, size_t N, bool TF, typename T2 >
struct MultTrait< StaticVector<T1,N,TF>, DynamicVector<T2,TF> >
{
   typedef StaticVector< typename MultTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, size_t N, typename T2 >
struct MultTrait< StaticVector<T1,N,false>, DynamicVector<T2,true> >
{
   typedef DynamicMatrix< typename MultTrait<T1,T2>::Type, false >  Type;
};

template< typename T1, size_t N, typename T2 >
struct MultTrait< StaticVector<T1,N,true>, DynamicVector<T2,false> >
{
   typedef typename MultTrait<T1,T2>::Type  Type;
};

template< typename T1, bool TF, typename T2, size_t N >
struct MultTrait< DynamicVector<T1,TF>, HybridVector<T2,N,TF> >
{
   typedef HybridVector< typename MultTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, typename T2, size_t N >
struct MultTrait< DynamicVector<T1,false>, HybridVector<T2,N,true> >
{
   typedef DynamicMatrix< typename MultTrait<T1,T2>::Type, false >  Type;
};

template< typename T1, typename T2, size_t N >
struct MultTrait< DynamicVector<T1,true>, HybridVector<T2,N,false> >
{
   typedef typename MultTrait<T1,T2>::Type  Type;
};

template< typename T1, size_t N, bool TF, typename T2 >
struct MultTrait< HybridVector<T1,N,TF>, DynamicVector<T2,TF> >
{
   typedef HybridVector< typename MultTrait<T1,T2>::Type, N, TF >  Type;
};

template< typename T1, size_t N, typename T2 >
struct MultTrait< HybridVector<T1,N,false>, DynamicVector<T2,true> >
{
   typedef DynamicMatrix< typename MultTrait<T1,T2>::Type, false >  Type;
};

template< typename T1, size_t N, typename T2 >
struct MultTrait< HybridVector<T1,N,true>, DynamicVector<T2,false> >
{
   typedef typename MultTrait<T1,T2>::Type  Type;
};

template< typename T1, bool TF, typename T2 >
struct MultTrait< DynamicVector<T1,TF>, DynamicVector<T2,TF> >
{
   typedef DynamicVector< typename MultTrait<T1,T2>::Type, TF >  Type;
};

template< typename T1, typename T2 >
struct MultTrait< DynamicVector<T1,false>, DynamicVector<T2,true> >
{
   typedef DynamicMatrix< typename MultTrait<T1,T2>::Type, false >  Type;
};

template< typename T1, typename T2 >
struct MultTrait< DynamicVector<T1,true>, DynamicVector<T2,false> >
{
   typedef typename MultTrait<T1,T2>::Type  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  CROSSTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, typename T2 >
struct CrossTrait< DynamicVector<T1,false>, StaticVector<T2,3UL,false> >
{
 private:
   typedef typename MultTrait<T1,T2>::Type  T;

 public:
   typedef StaticVector< typename SubTrait<T,T>::Type, 3UL, false >  Type;
};

template< typename T1, typename T2 >
struct CrossTrait< StaticVector<T1,3UL,false>, DynamicVector<T2,false> >
{
 private:
   typedef typename MultTrait<T1,T2>::Type  T;

 public:
   typedef StaticVector< typename SubTrait<T,T>::Type, 3UL, false >  Type;
};

template< typename T1, typename T2, size_t N >
struct CrossTrait< DynamicVector<T1,false>, HybridVector<T2,N,false> >
{
 private:
   typedef typename MultTrait<T1,T2>::Type  T;

 public:
   typedef StaticVector< typename SubTrait<T,T>::Type, 3UL, false >  Type;
};

template< typename T1, size_t N, typename T2 >
struct CrossTrait< HybridVector<T1,N,false>, DynamicVector<T2,false> >
{
 private:
   typedef typename MultTrait<T1,T2>::Type  T;

 public:
   typedef StaticVector< typename SubTrait<T,T>::Type, 3UL, false >  Type;
};

template< typename T1, typename T2 >
struct CrossTrait< DynamicVector<T1,false>, DynamicVector<T2,false> >
{
 private:
   typedef typename MultTrait<T1,T2>::Type  T;

 public:
   typedef StaticVector< typename SubTrait<T,T>::Type, 3UL, false >  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DIVTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, bool TF, typename T2 >
struct DivTrait< DynamicVector<T1,TF>, T2 >
{
   typedef DynamicVector< typename DivTrait<T1,T2>::Type, TF >  Type;
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T2 );
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  MATHTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, bool TF, typename T2 >
struct MathTrait< DynamicVector<T1,TF>, DynamicVector<T2,TF> >
{
   typedef DynamicVector< typename MathTrait<T1,T2>::HighType, TF >  HighType;
   typedef DynamicVector< typename MathTrait<T1,T2>::LowType , TF >  LowType;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SUBVECTORTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename T1, bool TF >
struct SubvectorTrait< DynamicVector<T1,TF> >
{
   typedef DynamicVector<T1,TF>  Type;
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
