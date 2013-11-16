//=================================================================================================
/*!
//  \file blaze/math/views/DenseSubmatrix.h
//  \brief Header file for the DenseSubmatrix class template
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

#ifndef _BLAZE_MATH_VIEWS_DENSESUBMATRIX_H_
#define _BLAZE_MATH_VIEWS_DENSESUBMATRIX_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <iterator>
#include <stdexcept>
#include <blaze/math/constraints/Computation.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/Matrix.h>
#include <blaze/math/constraints/RequiresEvaluation.h>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/constraints/TransExpr.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/expressions/Expression.h>
#include <blaze/math/Forward.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/traits/AddTrait.h>
#include <blaze/math/traits/ColumnTrait.h>
#include <blaze/math/traits/DivTrait.h>
#include <blaze/math/traits/MultTrait.h>
#include <blaze/math/traits/RowTrait.h>
#include <blaze/math/traits/SubmatrixExprTrait.h>
#include <blaze/math/traits/SubmatrixTrait.h>
#include <blaze/math/traits/SubTrait.h>
#include <blaze/math/typetraits/IsColumnMajorMatrix.h>
#include <blaze/math/typetraits/IsComputation.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsMatAbsExpr.h>
#include <blaze/math/typetraits/IsMatEvalExpr.h>
#include <blaze/math/typetraits/IsMatMatAddExpr.h>
#include <blaze/math/typetraits/IsMatMatMultExpr.h>
#include <blaze/math/typetraits/IsMatMatSubExpr.h>
#include <blaze/math/typetraits/IsMatScalarDivExpr.h>
#include <blaze/math/typetraits/IsMatScalarMultExpr.h>
#include <blaze/math/typetraits/IsMatTransExpr.h>
#include <blaze/math/typetraits/IsSparseMatrix.h>
#include <blaze/math/typetraits/IsTransExpr.h>
#include <blaze/math/typetraits/IsVecTVecMultExpr.h>
#include <blaze/util/Assert.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/logging/FunctionTrace.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/mpl/Or.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/Template.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/IsConst.h>
#include <blaze/util/typetraits/IsFloatingPoint.h>
#include <blaze/util/typetraits/IsNumeric.h>
#include <blaze/util/typetraits/IsSame.h>


namespace blaze {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\defgroup dense_submatrix Dense Submatrix
// \ingroup views
*/
/*!\brief View on a specific submatrix of a dense matrix.
// \ingroup dense_submatrix
//
// The DenseSubmatrix template represents a view on a specific submatrix of a dense matrix
// primitive. The type of the dense matrix is specified via the first template parameter:

   \code
   template< typename MT, bool SO >
   class DenseSubmatrix;
   \endcode

//  - MT: specifies the type of the dense matrix primitive. DenseSubmatrix can be used with every
//        dense matrix primitive, but does not work with any matrix expression type.
//  - SO: specifies the storage order (blaze::rowMajor, blaze::columnMajor) of the dense matrix.
//        This template parameter doesn't have to be explicitly defined, but is automatically
//        derived from the first template parameter.
//
//
// \n \section dense_submatrix_setup Setup of Dense Submatrices
//
// A view on a dense submatrix can be created very conveniently via the \c submatrix() function.
// This view can be treated as any other dense matrix, i.e. it can be assigned to, it can be
// copied from, and it can be used in arithmetic operations. The view can also be used on both
// sides of an assignment: The submatrix can either be used as an alias to grant write access to
// a specific submatrix of a dense matrix primitive on the left-hand side of an assignment or
// to grant read-access to a specific submatrix of a dense matrix primitive or expression on
// the right-hand side of an assignment. The following example demonstrates this in detail:

   \code
   typedef blaze::DynamicMatrix<double,blaze::columnMajor>  DenseMatrixType;
   typedef blaze::CompressedMatrix<double,blaze::rowMajor>  SparseMatrixType;

   DenseMatrixType A, B;
   SparseMatrixType C;
   // ... Resizing and initialization

   // Creating a dense submatrix of size 8x4, starting in row 0 and column 2
   blaze::DenseSubmatrix<DenseMatrixType> sm = submatrix( A, 0UL, 2UL, 8UL, 4UL );

   // Setting the submatrix of A to a 8x4 submatrix of B
   sm = submatrix( B, 0UL, 0UL, 8UL, 4UL );

   // Copying the sparse matrix C into another 8x4 submatrix of A
   submatrix( A, 8UL, 2UL, 8UL, 4UL ) = C;

   // Assigning part of the result of a matrix addition to the first submatrix
   sm = submatrix( B + C, 0UL, 0UL, 8UL, 4UL );
   \endcode

// \n \section dense_submatrix_element_access Element access
//
// A dense submatrix can be used like any other dense matrix. For instance, the elements of the
// dense submatrix can be directly accessed with the function call operator:

   \code
   typedef blaze::DynamicMatrix<double,blaze::rowMajor>  MatrixType;
   MatrixType A;
   // ... Resizing and initialization

   // Creating a 8x8 submatrix, starting from position (4,4)
   blaze::DenseSubmatrix<MatrixType> sm = submatrix( A, 4UL, 4UL, 8UL, 8UL );

   // Setting the element (0,0) of the submatrix, which corresponds to
   // the element at position (4,4) in matrix A
   sm(0,0) = 2.0;
   \endcode

// Alternatively, the elements of a submatrix can be traversed via (const) iterators. Just as
// with matrices, in case of non-const submatrices, \c begin() and \c end() return an Iterator,
// which allows a manipulation of the non-zero values, in case of constant submatrices a
// ConstIterator is returned:

   \code
   typedef blaze::DynamicMatrix<int,blaze::rowMajor>  MatrixType;
   typedef blaze::DenseSubmatrix<MatrixType>          SubmatrixType;

   MatrixType A( 256UL, 512UL );
   // ... Resizing and initialization

   // Creating a reference to a specific submatrix of matrix A
   SubmatrixType sm = submatrix( A, 16UL, 16UL, 64UL, 128UL );

   // Traversing the elements of the 0th row via iterators to non-const elements
   for( SubmatrixType::Iterator it=sm.begin(0); it!=sm.end(0); ++it ) {
      *it = ...;  // OK: Write access to the dense submatrix value.
      ... = *it;  // OK: Read access to the dense submatrix value.
   }

   // Traversing the elements of the 1st row via iterators to const elements
   for( SubmatrixType::ConstIterator it=sm.begin(1); it!=sm.end(1); ++it ) {
      *it = ...;  // Compilation error: Assignment to the value via a ConstIterator is invalid.
      ... = *it;  // OK: Read access to the dense submatrix value.
   }
   \endcode

// \n \section dense_submatrix_common_operations Common Operations
//
// The current size of the matrix, i.e. the number of rows or columns can be obtained via the
// \c rows() and \c columns() functions, the current total capacity via the \c capacity() function,
// and the number of non-zero elements via the \c nonZeros() function. However, since submatrices
// are views on a specific submatrix of a matrix, several operations are not possible on views,
// such as resizing and swapping:

   \code
   typedef blaze::DynamicMatrix<int,blaze::rowMajor>  MatrixType;
   typedef blaze::DenseSubmatrix<MatrixType>          SubmatrixType;

   MatrixType A;
   // ... Resizing and initialization

   // Creating a view on the a 8x12 submatrix of matrix A
   SubmatrixType sm = submatrix( A, 0UL, 0UL, 8UL, 12UL );

   sm.rows();      // Returns the number of rows of the submatrix
   sm.columns();   // Returns the number of columns of the submatrix
   sm.capacity();  // Returns the capacity of the submatrix
   sm.nonZeros();  // Returns the number of non-zero elements contained in the submatrix

   sm.resize( 10UL, 8UL );  // Compilation error: Cannot resize a submatrix of a matrix

   SubmatrixType sm2 = submatrix( A, 8UL, 0UL, 12UL, 8UL );
   swap( sm, sm2 );  // Compilation error: Swap operation not allowed
   \endcode

// \n \section dense_submatrix_arithmetic_operations Arithmetic Operations
//
// The following example gives an impression of the use of DenseSubmatrix within arithmetic
// operations. All operations (addition, subtraction, multiplication, scaling, ...) can be
// performed on all possible combinations of dense and sparse matrices with fitting element
// types:

   \code
   typedef blaze::DynamicMatrix<double,blaze::rowMajor>     DenseMatrixType;
   typedef blaze::CompressedMatrix<double,blaze::rowMajor>  SparseMatrixType;
   DenseMatrixType D1, D2, D3;
   SparseMatrixType S1, S2;

   typedef blaze::CompressedVector<double,blaze::columnVector>  SparseVectorType;
   SparseVectorType a, b;

   // ... Resizing and initialization

   typedef DenseSubmatrix<DenseMatrixType>  SubmatrixType;
   SubmatrixType sm = submatrix( D1, 0UL, 0UL, 8UL, 8UL );  // View on the 8x8 submatrix of matrix D1
                                                            // starting from row 0 and column 0

   submatrix( D1, 0UL, 8UL, 8UL, 8UL ) = D2;  // Dense matrix initialization of the 8x8 submatrix
                                              // starting in row 0 and column 8
   sm = S1;                                   // Sparse matrix initialization of the second 8x8 submatrix

   D3 = sm + D2;                                    // Dense matrix/dense matrix addition
   S2 = S1  - submatrix( D1, 8UL, 0UL, 8UL, 8UL );  // Sparse matrix/dense matrix subtraction
   D2 = sm * submatrix( D1, 8UL, 8UL, 8UL, 8UL );   // Dense matrix/dense matrix multiplication

   submatrix( D1, 8UL, 0UL, 8UL, 8UL ) *= 2.0;      // In-place scaling of a submatrix of D1
   D2 = submatrix( D1, 8UL, 8UL, 8UL, 8UL ) * 2.0;  // Scaling of the a submatrix of D1
   D2 = 2.0 * sm;                                   // Scaling of the a submatrix of D1

   submatrix( D1, 0UL, 8UL, 8UL, 8UL ) += D2;  // Addition assignment
   submatrix( D1, 8UL, 0UL, 8UL, 8UL ) -= S1;  // Subtraction assignment
   submatrix( D1, 8UL, 8UL, 8UL, 8UL ) *= sm;  // Multiplication assignment

   a = submatrix( D1, 4UL, 4UL, 8UL, 8UL ) * b;  // Dense matrix/sparse vector multiplication
   \endcode

// \n \section dense_submatrix_on_dense_submatrix Submatrix on Submatrix
//
// It is also possible to create a submatrix view on another submatrix. In this context it is
// important to remember that the type returned by the \c submatrix() function is the same type
// as the type of the given submatrix, since the view on a submatrix is just another view on the
// underlying dense matrix:

   \code
   typedef blaze::DynamicMatrix<double,blaze::rowMajor>  MatrixType;
   typedef blaze::DenseSubmatrix<MatrixType>             SubmatrixType;

   MatrixType D1;

   // ... Resizing and initialization

   // Creating a submatrix view on the dense matrix D1
   SubmatrixType sm1 = submatrix( D1, 4UL, 4UL, 8UL, 16UL );

   // Creating a submatrix view on the dense submatrix sm1
   SubmatrixType sm2 = submatrix( sm1, 1UL, 1UL, 4UL, 8UL );
   \endcode
*/
template< typename MT                                 // Type of the dense matrix
        , bool SO = IsColumnMajorMatrix<MT>::value >  // Storage order
class DenseSubmatrix : public DenseMatrix< DenseSubmatrix<MT,SO>, SO >
                     , private Expression
{
 private:
   //**Type definitions****************************************************************************
   //! Composite data type of the dense matrix expression.
   typedef typename SelectType< IsExpression<MT>::value, MT, MT& >::Type  Operand;

   //! Intrinsic trait for the matrix element type.
   typedef IntrinsicTrait<typename MT::ElementType>  IT;
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the non-const reference and iterator types.
   /*! The \a useConst compile time constant expression represents a compilation switch for
       the non-const reference and iterator types. In case the given dense matrix of type
       \a MT is const qualified, \a useConst will be set to 1 and the dense submatrix will
       return references and iterators to const. Otherwise \a useConst will be set to 0 and
       the dense submatrix will offer write access to the dense matrix elements both via
       the function call operator and iterators. */
   enum { useConst = IsConst<MT>::value };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DenseSubmatrix<MT,SO>               This;           //!< Type of this DenseSubmatrix instance.
   typedef typename SubmatrixTrait<MT>::Type   ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::OppositeType   OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename ResultType::TransposeType  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType            ElementType;    //!< Type of the submatrix elements.
   typedef typename IT::Type                   IntrinsicType;  //!< Intrinsic type of the subvector elements.
   typedef typename MT::ReturnType             ReturnType;     //!< Return type for expression template evaluations
   typedef const DenseSubmatrix&               CompositeType;  //!< Data type for composite expression templates.

   //! Reference to a constant submatrix value.
   typedef typename MT::ConstReference  ConstReference;

   //! Reference to a non-constant submatrix value.
   typedef typename SelectType< useConst, ConstReference, typename MT::Reference >::Type  Reference;

   //! Pointer to a constant row value.
   typedef const ElementType*  ConstPointer;

   //! Pointer to a constant row value.
   typedef typename SelectType< useConst, ConstPointer, ElementType* >::Type  Pointer;
   //**********************************************************************************************

   //**SubmatrixIterator class definition**********************************************************
   /*!\brief Iterator over the elements of the sparse submatrix.
   */
   template< typename IteratorType >  // Type of the dense matrix iterator
   class SubmatrixIterator
   {
    public:
      //**Type definitions*************************************************************************
      //! The iterator category.
      typedef typename std::iterator_traits<IteratorType>::iterator_category  IteratorCategory;

      //! Type of the underlying elements.
      typedef typename std::iterator_traits<IteratorType>::value_type  ValueType;

      //! Pointer return type.
      typedef typename std::iterator_traits<IteratorType>::pointer  PointerType;

      //! Reference return type.
      typedef typename std::iterator_traits<IteratorType>::reference  ReferenceType;

      //! Difference between two iterators.
      typedef typename std::iterator_traits<IteratorType>::difference_type  DifferenceType;

      // STL iterator requirements
      typedef IteratorCategory  iterator_category;  //!< The iterator category.
      typedef ValueType         value_type;         //!< Type of the underlying elements.
      typedef PointerType       pointer;            //!< Pointer return type.
      typedef ReferenceType     reference;          //!< Reference return type.
      typedef DifferenceType    difference_type;    //!< Difference between two iterators.
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Constructor for the SubmatrixIterator class.
      //
      // \param iterator Iterator to the initial element.
      */
      explicit inline SubmatrixIterator( IteratorType iterator )
         : iterator_( iterator )  // Iterator to the current submatrix element
      {}
      //*******************************************************************************************

      //**Addition assignment operator*************************************************************
      /*!\brief Addition assignment operator.
      //
      // \param inc The increment of the iterator.
      // \return The incremented iterator.
      */
      inline SubmatrixIterator& operator+=( size_t inc ) {
         iterator_ += inc;
         return *this;
      }
      //*******************************************************************************************

      //**Subtraction assignment operator**********************************************************
      /*!\brief Subtraction assignment operator.
      //
      // \param dec The decrement of the iterator.
      // \return The decremented iterator.
      */
      inline SubmatrixIterator& operator-=( size_t dec ) {
         iterator_ -= dec;
         return *this;
      }
      //*******************************************************************************************

      //**Prefix increment operator****************************************************************
      /*!\brief Pre-increment operator.
      //
      // \return Reference to the incremented iterator.
      */
      inline SubmatrixIterator& operator++() {
         ++iterator_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix increment operator***************************************************************
      /*!\brief Post-increment operator.
      //
      // \return The previous position of the iterator.
      */
      inline const SubmatrixIterator operator++( int ) {
         return SubmatrixIterator( iterator_++ );
      }
      //*******************************************************************************************

      //**Prefix decrement operator****************************************************************
      /*!\brief Pre-decrement operator.
      //
      // \return Reference to the decremented iterator.
      */
      inline SubmatrixIterator& operator--() {
         --iterator_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix decrement operator***************************************************************
      /*!\brief Post-decrement operator.
      //
      // \return The previous position of the iterator.
      */
      inline const SubmatrixIterator operator--( int ) {
         return SubmatrixIterator( iterator_-- );
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the element at the current iterator position.
      //
      // \return The resulting value.
      */
      inline ReferenceType operator*() const {
         return *iterator_;
      }
      //*******************************************************************************************

      //**Load function****************************************************************************
      /*!\brief Aligned load of an intrinsic element of the dense submatrix.
      //
      // \return The loaded intrinsic element.
      //
      // This function performs an aligned load of the current intrinsic element of the submatrix
      // iterator. This function must \b NOT be called explicitly! It is used internally for the
      // performance optimized evaluation of expression templates. Calling this function explicitly
      // might result in erroneous results and/or in compilation errors.
      */
      inline IntrinsicType load() const {
         return iterator_.loadu();
      }
      //*******************************************************************************************

      //**Loadu function***************************************************************************
      /*!\brief Unaligned load of an intrinsic element of the dense submatrix.
      //
      // \return The loaded intrinsic element.
      //
      // This function performs an unaligned load of the current intrinsic element of the submatrix
      // iterator. This function must \b NOT be called explicitly! It is used internally for the
      // performance optimized evaluation of expression templates. Calling this function explicitly
      // might result in erroneous results and/or in compilation errors.
      */
      inline IntrinsicType loadu() const {
         return iterator_.loadu();
      }
      //*******************************************************************************************

      //**Equality operator************************************************************************
      /*!\brief Equality comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the iterators refer to the same element, \a false if not.
      */
      inline bool operator==( const SubmatrixIterator& rhs ) const {
         return iterator_ == rhs.iterator_;
      }
      //*******************************************************************************************

      //**Inequality operator**********************************************************************
      /*!\brief Inequality comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the iterators don't refer to the same element, \a false if they do.
      */
      inline bool operator!=( const SubmatrixIterator& rhs ) const {
         return iterator_ != rhs.iterator_;
      }
      //*******************************************************************************************

      //**Less-than operator***********************************************************************
      /*!\brief Less-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is smaller, \a false if not.
      */
      inline bool operator<( const SubmatrixIterator& rhs ) const {
         return iterator_ < rhs.iterator_;
      }
      //*******************************************************************************************

      //**Greater-than operator********************************************************************
      /*!\brief Greater-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is greater, \a false if not.
      */
      inline bool operator>( const SubmatrixIterator& rhs ) const {
         return iterator_ > rhs.iterator_;
      }
      //*******************************************************************************************

      //**Less-or-equal-than operator**************************************************************
      /*!\brief Less-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is smaller or equal, \a false if not.
      */
      inline bool operator<=( const SubmatrixIterator& rhs ) const {
         return iterator_ <= rhs.iterator_;
      }
      //*******************************************************************************************

      //**Greater-or-equal-than operator***********************************************************
      /*!\brief Greater-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is greater or equal, \a false if not.
      */
      inline bool operator>=( const SubmatrixIterator& rhs ) const {
         return iterator_ >= rhs.iterator_;
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Calculating the number of elements between two iterators.
      //
      // \param rhs The right-hand side iterator.
      // \return The number of elements between the two iterators.
      */
      inline DifferenceType operator-( const SubmatrixIterator& rhs ) const {
         return iterator_ - rhs.iterator_;
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between a SubmatrixIterator and an integral value.
      //
      // \param it The iterator to be incremented.
      // \param inc The number of elements the iterator is incremented.
      // \return The incremented iterator.
      */
      friend inline const SubmatrixIterator operator+( const SubmatrixIterator& it, size_t inc ) {
         return SubmatrixIterator( it.iterator_ + inc );
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between an integral value and a SubmatrixIterator.
      //
      // \param inc The number of elements the iterator is incremented.
      // \param it The iterator to be incremented.
      // \return The incremented iterator.
      */
      friend inline const SubmatrixIterator operator+( size_t inc, const SubmatrixIterator& it ) {
         return SubmatrixIterator( it.iterator_ + inc );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Subtraction between a SubmatrixIterator and an integral value.
      //
      // \param it The iterator to be decremented.
      // \param inc The number of elements the iterator is decremented.
      // \return The decremented iterator.
      */
      friend inline const SubmatrixIterator operator-( const SubmatrixIterator& it, size_t dec ) {
         return SubmatrixIterator( it.iterator_ - dec );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Subtraction between an integral value and a SubmatrixIterator.
      //
      // \param inc The number of elements the iterator is decremented.
      // \param it The iterator to be decremented.
      // \return The decremented iterator.
      */
      friend inline const SubmatrixIterator operator-( size_t dec, const SubmatrixIterator& it ) {
         return SubmatrixIterator( it.iterator_ - dec );
      }
      //*******************************************************************************************

    private:
      //**Member variables*************************************************************************
      IteratorType iterator_;  //!< Iterator to the current submatrix element.
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   //! Iterator over constant elements.
   typedef SubmatrixIterator<typename MT::ConstIterator>  ConstIterator;

   //! Iterator over non-constant elements.
   typedef typename SelectType< useConst, ConstIterator, SubmatrixIterator<typename MT::Iterator> >::Type  Iterator;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = MT::vectorizable };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline DenseSubmatrix( MT& matrix, size_t row, size_t column, size_t m, size_t n );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator()( size_t i, size_t j );
   inline ConstReference operator()( size_t i, size_t j ) const;
   inline Pointer        data  ();
   inline ConstPointer   data  () const;
   inline Iterator       begin ( size_t i );
   inline ConstIterator  begin ( size_t i ) const;
   inline ConstIterator  cbegin( size_t i ) const;
   inline Iterator       end   ( size_t i );
   inline ConstIterator  end   ( size_t i ) const;
   inline ConstIterator  cend  ( size_t i ) const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
                                      inline DenseSubmatrix& operator= ( const ElementType& rhs );
                                      inline DenseSubmatrix& operator= ( const DenseSubmatrix& rhs );
   template< typename MT2, bool SO2 > inline DenseSubmatrix& operator= ( const Matrix<MT2,SO2>& rhs );
   template< typename MT2, bool SO2 > inline DenseSubmatrix& operator+=( const Matrix<MT2,SO2>& rhs );
   template< typename MT2, bool SO2 > inline DenseSubmatrix& operator-=( const Matrix<MT2,SO2>& rhs );
   template< typename MT2, bool SO2 > inline DenseSubmatrix& operator*=( const Matrix<MT2,SO2>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t          rows() const;
                              inline size_t          columns() const;
                              inline size_t          spacing() const;
                              inline size_t          capacity() const;
                              inline size_t          capacity( size_t i ) const;
                              inline size_t          nonZeros() const;
                              inline size_t          nonZeros( size_t i ) const;
                              inline void            reset();
                              inline void            reset( size_t i );
   template< typename Other > inline DenseSubmatrix& scale( Other scalar );
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT2 >
   struct VectorizedAssign {
      enum { value = vectorizable && MT2::vectorizable &&
                     IsSame<ElementType,typename MT2::ElementType>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT2 >
   struct VectorizedAddAssign {
      enum { value = vectorizable && MT2::vectorizable &&
                     IsSame<ElementType,typename MT2::ElementType>::value &&
                     IntrinsicTrait<ElementType>::addition };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT2 >
   struct VectorizedSubAssign {
      enum { value = vectorizable && MT2::vectorizable &&
                     IsSame<ElementType,typename MT2::ElementType>::value &&
                     IntrinsicTrait<ElementType>::subtraction };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool canAlias ( const Other* alias ) const;
   template< typename Other > inline bool isAliased( const Other* alias ) const;

   inline IntrinsicType load  ( size_t i, size_t j ) const;
   inline IntrinsicType loadu ( size_t i, size_t j ) const;
   inline void          store ( size_t i, size_t j, const IntrinsicType& value );
   inline void          storeu( size_t i, size_t j, const IntrinsicType& value );
   inline void          stream( size_t i, size_t j, const IntrinsicType& value );

   template< typename MT2 >
   inline typename DisableIf< VectorizedAssign<MT2> >::Type
      assign( const DenseMatrix<MT2,SO>& rhs );

   template< typename MT2 >
   inline typename EnableIf< VectorizedAssign<MT2> >::Type
      assign( const DenseMatrix<MT2,SO>& rhs );

   template< typename MT2 > inline void assign( const DenseMatrix<MT2,!SO>&  rhs );
   template< typename MT2 > inline void assign( const SparseMatrix<MT2,SO>&  rhs );
   template< typename MT2 > inline void assign( const SparseMatrix<MT2,!SO>& rhs );

   template< typename MT2 >
   inline typename DisableIf< VectorizedAddAssign<MT2> >::Type
      addAssign( const DenseMatrix<MT2,SO>& rhs );

   template< typename MT2 >
   inline typename EnableIf< VectorizedAddAssign<MT2> >::Type
      addAssign( const DenseMatrix<MT2,SO>& rhs );

   template< typename MT2 > inline void addAssign( const DenseMatrix<MT2,!SO>&  rhs );
   template< typename MT2 > inline void addAssign( const SparseMatrix<MT2,SO>&  rhs );
   template< typename MT2 > inline void addAssign( const SparseMatrix<MT2,!SO>& rhs );

   template< typename MT2 >
   inline typename DisableIf< VectorizedSubAssign<MT2> >::Type
      subAssign( const DenseMatrix<MT2,SO>& rhs );

   template< typename MT2 >
   inline typename EnableIf< VectorizedSubAssign<MT2> >::Type
      subAssign( const DenseMatrix<MT2,SO>& rhs );

   template< typename MT2 > inline void subAssign( const DenseMatrix<MT2,!SO>&  rhs );
   template< typename MT2 > inline void subAssign( const SparseMatrix<MT2,SO>&  rhs );
   template< typename MT2 > inline void subAssign( const SparseMatrix<MT2,!SO>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Operand      matrix_;   //!< The dense matrix containing the submatrix.
   const size_t row_;      //!< The first row of the submatrix.
   const size_t column_;   //!< The first column of the submatrix.
   const size_t m_;        //!< The number of rows of the submatrix.
   const size_t n_;        //!< The number of columns of the submatrix.
   const size_t rest_;     //!< The number of remaining elements in an unaligned intrinsic operation.
   const size_t final_;    //!< The final index for unaligned intrinsic operations.
                           /*!< In case the submatrix is not fully aligned and the submatrix is
                                involved in a vectorized operation, the final index indicates at
                                which index a special treatment for the remaining elements is
                                required. */
   const bool   aligned_;  //!< Memory alignment flag.
                           /*!< The alignment flag indicates whether the submatrix is fully aligned.
                                In case the submatrix is fully aligned, no special handling has to
                                be used for the last elements of the submatrix in a vectorized
                                operation. In order to be aligned, the following conditions must
                                hold for the submatrix:
                                 - The first element of each row/column must be aligned
                                 - The submatrix must be at the end of the given matrix or
                                 - The number of rows/columns of the submatrix must be a multiple
                                   of the number of values per intrinsic element. */
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename MT2, bool SO2 >
   friend DenseSubmatrix<MT2,SO2>
      submatrix( const DenseSubmatrix<MT2,SO2>& dm, size_t row, size_t column, size_t m, size_t n );
   /*! \endcond */
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE    ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_TRANSEXPR_TYPE   ( MT );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief The constructor for DenseSubmatrix.
//
// \param matrix The dense matrix containing the submatrix.
// \param row The index of the first row of the submatrix in the given dense matrix.
// \param column The index of the first column of the submatrix in the given dense matrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \exception std::invalid_argument Invalid submatrix specification.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline DenseSubmatrix<MT,SO>::DenseSubmatrix( MT& matrix, size_t row, size_t column, size_t m, size_t n )
   : matrix_ ( matrix       )  // The dense matrix containing the submatrix
   , row_    ( row          )  // The first row of the submatrix
   , column_ ( column       )  // The first column of the submatrix
   , m_      ( m            )  // The number of rows of the submatrix
   , n_      ( n            )  // The number of columns of the submatrix
   , rest_   ( n % IT::size )  // The number of remaining elements in an unaligned intrinsic operation
   , final_  ( n - rest_    )  // The final index for unaligned intrinsic operations
   , aligned_( ( column % IT::size == 0UL ) &&
               ( column + n == matrix.columns() || n % IT::size == 0UL ) )
{
   if( ( row + m > matrix.rows() ) || ( column + n > matrix.columns() ) )
      throw std::invalid_argument( "Invalid submatrix specification" );
}
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief 2D-access to the dense submatrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::Reference
   DenseSubmatrix<MT,SO>::operator()( size_t i, size_t j )
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   return matrix_(row_+i,column_+j);
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief 2D-access to the dense submatrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::ConstReference
   DenseSubmatrix<MT,SO>::operator()( size_t i, size_t j ) const
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   return const_cast<const MT&>( matrix_ )(row_+i,column_+j);
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the submatrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense submatrix. Note that
// you can NOT assume that all matrix elements lie adjacent to each other! The dense submatrix
// may use techniques such as padding to improve the alignment of the data.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::Pointer DenseSubmatrix<MT,SO>::data()
{
   return matrix_.data() + row_*spacing() + column_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Low-level data access to the submatrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense submatrix. Note that
// you can NOT assume that all matrix elements lie adjacent to each other! The dense submatrix
// may use techniques such as padding to improve the alignment of the data.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::ConstPointer DenseSubmatrix<MT,SO>::data() const
{
   return matrix_.data() + row_*spacing() + column_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first non-zero element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first non-zero element of row/column \a i.
//
// This function returns a row/column iterator to the first non-zero element of row/column \a i.
// In case the storage order is set to \a rowMajor the function returns an iterator to the first
// non-zero element of row \a i, in case the storage flag is set to \a columnMajor the function
// returns an iterator to the first non-zero element of column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::Iterator DenseSubmatrix<MT,SO>::begin( size_t i )
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid dense submatrix row access index" );
   return Iterator( matrix_.begin( row_ + i ) + column_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first non-zero element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first non-zero element of row/column \a i.
//
// This function returns a row/column iterator to the first non-zero element of row/column \a i.
// In case the storage order is set to \a rowMajor the function returns an iterator to the first
// non-zero element of row \a i, in case the storage flag is set to \a columnMajor the function
// returns an iterator to the first non-zero element of column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::ConstIterator DenseSubmatrix<MT,SO>::begin( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid dense submatrix row access index" );
   return ConstIterator( matrix_.cbegin( row_ + i ) + column_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator to the first non-zero element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator to the first non-zero element of row/column \a i.
//
// This function returns a row/column iterator to the first non-zero element of row/column \a i.
// In case the storage order is set to \a rowMajor the function returns an iterator to the first
// non-zero element of row \a i, in case the storage flag is set to \a columnMajor the function
// returns an iterator to the first non-zero element of column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::ConstIterator DenseSubmatrix<MT,SO>::cbegin( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid dense submatrix row access index" );
   return ConstIterator( matrix_.cbegin( row_ + i ) + column_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last non-zero element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last non-zero element of row/column \a i.
//
// This function returns an row/column iterator just past the last non-zero element of row/column
// \a i. In case the storage order is set to \a rowMajor the function returns an iterator just
// past the last non-zero element of row \a i, in case the storage flag is set to \a columnMajor
// the function returns an iterator just past the last non-zero element of column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::Iterator DenseSubmatrix<MT,SO>::end( size_t i )
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid dense submatrix row access index" );
   return Iterator( matrix_.begin( row_ + i ) + column_ + n_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last non-zero element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last non-zero element of row/column \a i.
//
// This function returns an row/column iterator just past the last non-zero element of row/column
// \a i. In case the storage order is set to \a rowMajor the function returns an iterator just
// past the last non-zero element of row \a i, in case the storage flag is set to \a columnMajor
// the function returns an iterator just past the last non-zero element of column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::ConstIterator DenseSubmatrix<MT,SO>::end( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid dense submatrix row access index" );
   return ConstIterator( matrix_.cbegin( row_ + i ) + column_ + n_ );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns an iterator just past the last non-zero element of row/column \a i.
//
// \param i The row/column index.
// \return Iterator just past the last non-zero element of row/column \a i.
//
// This function returns an row/column iterator just past the last non-zero element of row/column
// \a i. In case the storage order is set to \a rowMajor the function returns an iterator just
// past the last non-zero element of row \a i, in case the storage flag is set to \a columnMajor
// the function returns an iterator just past the last non-zero element of column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::ConstIterator DenseSubmatrix<MT,SO>::cend( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid dense submatrix row access index" );
   return ConstIterator( matrix_.cbegin( row_ + i ) + column_ + n_ );
}
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Homogenous assignment to all submatrix elements.
//
// \param rhs Scalar value to be assigned to all submatrix elements.
// \return Reference to the assigned submatrix.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline DenseSubmatrix<MT,SO>& DenseSubmatrix<MT,SO>::operator=( const ElementType& rhs )
{
   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );

   for( size_t i=row_; i<iend; ++i )
      for( size_t j=column_; j<jend; ++j )
         matrix_(i,j) = rhs;

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Copy assignment operator for DenseSubmatrix.
//
// \param rhs Sparse submatrix to be copied.
// \return Reference to the assigned submatrix.
// \exception std::invalid_argument Submatrix sizes do not match.
//
// The dense submatrix is initialized as a copy of the given dense submatrix. In case the
// current sizes of the two submatrices don't match, a \a std::invalid_argument exception is
// thrown.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline DenseSubmatrix<MT,SO>& DenseSubmatrix<MT,SO>::operator=( const DenseSubmatrix& rhs )
{
   using blaze::assign;

   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE  ( ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

   if( this == &rhs || ( &matrix_ == &rhs.matrix_ && row_ == rhs.row_ && column_ == rhs.column_ ) )
      return *this;

   if( rows() != rhs.rows() || columns() != rhs.columns() )
      throw std::invalid_argument( "Submatrix sizes do not match" );

   if( rhs.canAlias( &matrix_ ) ) {
      const ResultType tmp( rhs );
      assign( *this, tmp );
   }
   else {
      if( IsSparseMatrix<MT>::value )
         reset();
      assign( *this, rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Assignment operator for different matrices.
//
// \param rhs Matrix to be assigned.
// \return Reference to the assigned submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// The dense submatrix is initialized as a copy of the given matrix. In case the current
// sizes of the two matrices don't match, a \a std::invalid_argument exception is thrown.
*/
template< typename MT   // Type of the dense matrix
        , bool SO >     // Storage order
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,SO>& DenseSubmatrix<MT,SO>::operator=( const Matrix<MT2,SO2>& rhs )
{
   using blaze::assign;

   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename MT2::ResultType );

   if( rows() != (~rhs).rows() || columns() != (~rhs).columns() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( IsSparseMatrix<MT2>::value )
      reset();

   if( (~rhs).canAlias( &matrix_ ) ) {
      const typename MT2::ResultType tmp( ~rhs );
      assign( *this, tmp );
   }
   else {
      assign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Addition assignment operator for the addition of a matrix (\f$ A+=B \f$).
//
// \param rhs The right-hand side matrix to be added to the submatrix.
// \return Reference to the dense submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT   // Type of the dense matrix
        , bool SO >     // Storage order
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,SO>&
   DenseSubmatrix<MT,SO>::operator+=( const Matrix<MT2,SO2>& rhs )
{
   using blaze::addAssign;

   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename MT2::ResultType );

   if( rows() != (~rhs).rows() || columns() != (~rhs).columns() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( (~rhs).canAlias( &matrix_ ) ) {
      const typename MT2::ResultType tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Subtraction assignment operator for the subtraction of a matrix (\f$ A-=B \f$).
//
// \param rhs The right-hand side matrix to be subtracted from the submatrix.
// \return Reference to the dense submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT   // Type of the dense matrix
        , bool SO >     // Storage order
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,SO>& DenseSubmatrix<MT,SO>::operator-=( const Matrix<MT2,SO2>& rhs )
{
   using blaze::subAssign;

   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename MT2::ResultType );

   if( rows() != (~rhs).rows() || columns() != (~rhs).columns() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( (~rhs).canAlias( &matrix_ ) ) {
      const typename MT2::ResultType tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication of a matrix (\f$ A*=B \f$).
//
// \param rhs The right-hand side matrix for the multiplication.
// \return Reference to the dense submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two given matrices don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename MT   // Type of the dense matrix
        , bool SO >     // Storage order
template< typename MT2  // Type of the right-hand side matrix
        , bool SO2 >    // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,SO>& DenseSubmatrix<MT,SO>::operator*=( const Matrix<MT2,SO2>& rhs )
{
   if( columns() != (~rhs).rows() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   typedef typename MultTrait<ResultType,typename MT2::ResultType>::Type  MultType;

   BLAZE_CONSTRAINT_MUST_BE_MATRIX_TYPE( MultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( MultType );

   const MultType tmp( *this * (~rhs) );
   if( IsSparseMatrix<MultType>::value )
      reset();
   assign( tmp );

   return *this;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Multiplication assignment operator for the multiplication between a dense submatrix
//        and a scalar value (\f$ A*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the dense submatrix.
*/
template< typename MT       // Type of the dense matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix<MT,SO> >::Type&
   DenseSubmatrix<MT,SO>::operator*=( Other rhs )
{
   return operator=( (*this) * rhs );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Division assignment operator for the division of a dense submatrix by a scalar value
//        (\f$ A/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the dense submatrix.
*/
template< typename MT       // Type of the dense matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix<MT,SO> >::Type&
   DenseSubmatrix<MT,SO>::operator/=( Other rhs )
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
/*!\brief Returns the number of rows of the dense submatrix.
//
// \return The number of rows of the dense submatrix.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline size_t DenseSubmatrix<MT,SO>::rows() const
{
   return m_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of columns of the dense submatrix.
//
// \return The number of columns of the dense submatrix.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline size_t DenseSubmatrix<MT,SO>::columns() const
{
   return n_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the spacing between the beginning of two rows/columns.
//
// \return The spacing between the beginning of two rows/columns.
//
// This function returns the spacing between the beginning of two rows/columns, i.e. the
// total number of elements of a row/column. In case the storage order is set to \a rowMajor
// the function returns the spacing between two rows, in case the storage flag is set to
// \a columnMajor the function returns the spacing between two columns.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline size_t DenseSubmatrix<MT,SO>::spacing() const
{
   return matrix_.spacing();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the maximum capacity of the dense submatrix.
//
// \return The capacity of the dense submatrix.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline size_t DenseSubmatrix<MT,SO>::capacity() const
{
   return rows() * columns();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the current capacity of the specified row/column.
//
// \param i The index of the row/column.
// \return The current capacity of row/column \a i.
//
// This function returns the current capacity of the specified row/column. In case the
// storage order is set to \a rowMajor the function returns the capacity of row \a i,
// in case the storage flag is set to \a columnMajor the function returns the capacity
// of column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline size_t DenseSubmatrix<MT,SO>::capacity( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );
   return columns();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the dense submatrix
//
// \return The number of non-zero elements in the dense submatrix.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline size_t DenseSubmatrix<MT,SO>::nonZeros() const
{
   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );
   size_t nonzeros( 0UL );

   for( size_t i=row_; i<iend; ++i )
      for( size_t j=column_; j<jend; ++j )
         if( !isDefault( matrix_(i,j) ) )
            ++nonzeros;

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns the number of non-zero elements in the specified row/column.
//
// \param i The index of the row/column.
// \return The number of non-zero elements of row/column \a i.
//
// This function returns the current number of non-zero elements in the specified row/column.
// In case the storage order is set to \a rowMajor the function returns the number of non-zero
// elements in row \a i, in case the storage flag is set to \a columnMajor the function returns
// the number of non-zero elements in column \a i.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline size_t DenseSubmatrix<MT,SO>::nonZeros( size_t i ) const
{
   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );

   const size_t jend( column_ + n_ );
   size_t nonzeros( 0UL );

   for( size_t j=column_; j<jend; ++j )
      if( !isDefault( matrix_(row_+i,j) ) )
         ++nonzeros;

   return nonzeros;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void DenseSubmatrix<MT,SO>::reset()
{
   using blaze::reset;

   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );

   for( size_t i=row_; i<iend; ++i )
      for( size_t j=column_; j<jend; ++j )
         reset( matrix_(i,j) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Reset the specified row/column to the default initial values.
//
// \param i The index of the row/column.
// \return void
//
// This function resets the values in the specified row/column to their default value. In case
// the storage order is set to \a rowMajor the function resets the values in row \a i, in case
// the storage order is set to \a columnMajor the function resets the values in column \a i.
// Note that the capacity of the row/column remains unchanged.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void DenseSubmatrix<MT,SO>::reset( size_t i )
{
   using blaze::reset;

   BLAZE_USER_ASSERT( i < rows(), "Invalid row access index" );

   const size_t jend( column_ + n_ );
   for( size_t j=column_; j<jend; ++j )
      reset( matrix_(row_+i,j) );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Scaling of the dense submatrix by the scalar value \a scalar (\f$ A=B*s \f$).
//
// \param scalar The scalar value for the submatrix scaling.
// \return Reference to the dense submatrix.
*/
template< typename MT       // Type of the dense matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the scalar value
inline DenseSubmatrix<MT,SO>& DenseSubmatrix<MT,SO>::scale( Other scalar )
{
   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );

   for( size_t i=row_; i<iend; ++i )
      for( size_t j=column_; j<jend; ++j )
         matrix_(i,j) *= scalar;

   return *this;
}
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Returns whether the submatrix can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this submatrix, \a false if not.
//
// This function returns whether the given address can alias with the submatrix. In contrast
// to the isAliased() function this function is allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT       // Type of the dense matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the foreign expression
inline bool DenseSubmatrix<MT,SO>::canAlias( const Other* alias ) const
{
   return static_cast<const void*>( &matrix_ ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the submatrix is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this submatrix, \a false if not.
//
// This function returns whether the given address is aliased with the submatrix. In contrast
// to the conAlias() function this function is not allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT       // Type of the dense matrix
        , bool SO >         // Storage order
template< typename Other >  // Data type of the foreign expression
inline bool DenseSubmatrix<MT,SO>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( &matrix_ ) == static_cast<const void*>( alias );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned load of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded intrinsic element.
//
// This function performs an aligned load of a specific intrinsic element of the dense
// submatrix. The row index must be smaller than the number of rows and the column index
// must be smaller than the number of columns. Additionally, the column index (in case of
// a row-major matrix) or the row index (in case of a column-major matrix) must be a multiple
// of the number of values inside the intrinsic element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::IntrinsicType
   DenseSubmatrix<MT,SO>::load( size_t i, size_t j ) const
{
   return loadu( i, j );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned load of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded intrinsic element.
//
// This function performs an unaligned load of a specific intrinsic element of the dense
// submatrix. The row index must be smaller than the number of rows and the column index
// must be smaller than the number of columns. Additionally, the column index (in case of
// a row-major matrix) or the row index (in case of a column-major matrix) must be a multiple
// of the number of values inside the intrinsic element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DenseSubmatrix<MT,SO>::IntrinsicType
   DenseSubmatrix<MT,SO>::loadu( size_t i, size_t j ) const
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( i < rows()         , "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( j < columns()      , "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j % IT::size == 0UL, "Invalid column access index" );

   if( aligned_ || j != final_ ) {
      return matrix_.loadu( row_+i, column_+j );
   }
   else {
      IntrinsicType value;
      for( size_t k=0UL; k<rest_; ++k )
         value[k] = matrix_(row_+i,column_+j+k);
      return value;
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned store of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned store of a specific intrinsic element of the dense submatrix.
// The row index must be smaller than the number of rows and the column index must be smaller than
// the number of columns. Additionally, the column index (in case of a row-major matrix) or the
// row index (in case of a column-major matrix) must be a multiple of the number of values inside
// the intrinsic element. This function must \b NOT be called explicitly! It is used internally
// for the performance optimized evaluation of expression templates. Calling this function
// explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void DenseSubmatrix<MT,SO>::store( size_t i, size_t j, const IntrinsicType& value )
{
   storeu( i, j, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Unaligned store of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an unaligned store of a specific intrinsic element of the dense
// submatrix. The row index must be smaller than the number of rows and the column index must
// be smaller than the number of columns. Additionally, the column index (in case of a row-major
// matrix) or the row index (in case of a column-major matrix) must be a multiple of the number
// of values inside the intrinsic element. This function must \b NOT be called explicitly! It
// is used internally for the performance optimized evaluation of expression templates. Calling
// this function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void DenseSubmatrix<MT,SO>::storeu( size_t i, size_t j, const IntrinsicType& value )
{
   using blaze::storeu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( i < rows()         , "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( j < columns()      , "Invalid column access index" );
   BLAZE_INTERNAL_ASSERT( j % IT::size == 0UL, "Invalid column access index" );

   if( aligned_ || j != final_ ) {
      matrix_.storeu( row_+i, column_+j, value );
   }
   else {
      for( size_t k=0UL; k<rest_; ++k )
         matrix_(row_+i,column_+j+k) = value[k];
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Aligned, non-temporal store of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned, non-temporal store of a specific intrinsic element of
// the dense submatrix. The row index must be smaller than the number of rows and the column
// index must be smaller than the number of columns. Additionally, the column index (in case
// of a row-major matrix) or the row index (in case of a column-major matrix) must be a multiple
// of the number of values inside the intrinsic element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void DenseSubmatrix<MT,SO>::stream( size_t i, size_t j, const IntrinsicType& value )
{
   storeu( i, j, value );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename DisableIf< typename DenseSubmatrix<MT,SO>::BLAZE_TEMPLATE VectorizedAssign<MT2> >::Type
   DenseSubmatrix<MT,SO>::assign( const DenseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t jend( n_ & size_t(-2) );
   BLAZE_INTERNAL_ASSERT( ( n_ - ( n_ % 2UL ) ) == jend, "Invalid end calculation" );

   for( size_t i=0UL; i<m_; ++i ) {
      for( size_t j=0UL; j<jend; j+=2UL ) {
         matrix_(row_+i,column_+j    ) = (~rhs)(i,j    );
         matrix_(row_+i,column_+j+1UL) = (~rhs)(i,j+1UL);
      }
      if( jend < n_ ) {
         matrix_(row_+i,column_+jend) = (~rhs)(i,jend);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename EnableIf< typename DenseSubmatrix<MT,SO>::BLAZE_TEMPLATE VectorizedAssign<MT2> >::Type
   DenseSubmatrix<MT,SO>::assign( const DenseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   if( aligned_ && m_*n_ > ( cacheSize / ( sizeof(ElementType) * 3UL ) ) && !(~rhs).isAliased( &matrix_ ) )
   {
      for( size_t i=0UL; i<m_; ++i )
         for( size_t j=0UL; j<n_; j+=IT::size )
            matrix_.stream( row_+i, column_+j, (~rhs).load(i,j) );
   }
   else
   {
      const size_t jend( n_ & size_t(-IT::size*4) );
      BLAZE_INTERNAL_ASSERT( ( n_ - ( n_ % (IT::size*4UL) ) ) == jend, "Invalid end calculation" );

      for( size_t i=0UL; i<m_; ++i ) {
         for( size_t j=0UL; j<jend; j+=IT::size*4UL ) {
            matrix_.storeu( row_+i, column_+j             , (~rhs).load(i,j             ) );
            matrix_.storeu( row_+i, column_+j+IT::size    , (~rhs).load(i,j+IT::size    ) );
            matrix_.storeu( row_+i, column_+j+IT::size*2UL, (~rhs).load(i,j+IT::size*2UL) );
            matrix_.storeu( row_+i, column_+j+IT::size*3UL, (~rhs).load(i,j+IT::size*3UL) );
         }
         for( size_t j=jend; j<n_; j+=IT::size ) {
            storeu( i, j, (~rhs).load(i,j) );
         }
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline void DenseSubmatrix<MT,SO>::assign( const DenseMatrix<MT2,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t block( 16UL );

   for( size_t ii=0UL; ii<m_; ii+=block ) {
      const size_t iend( ( m_<(ii+block) )?( m_ ):( ii+block ) );
      for( size_t jj=0UL; jj<n_; jj+=block ) {
         const size_t jend( ( n_<(jj+block) )?( n_ ):( jj+block ) );
         for( size_t i=ii; i<iend; ++i ) {
            for( size_t j=jj; j<jend; ++j ) {
               matrix_(row_+i,column_+j) = (~rhs)(i,j);
            }
         }
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,SO>::assign( const SparseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t i=0UL; i<m_; ++i )
      for( typename MT2::ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         matrix_(row_+i,column_+element->index()) = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,SO>::assign( const SparseMatrix<MT2,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t j=0UL; j<n_; ++j )
      for( typename MT2::ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         matrix_(row_+element->index(),column_+j) = element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename DisableIf< typename DenseSubmatrix<MT,SO>::BLAZE_TEMPLATE VectorizedAddAssign<MT2> >::Type
   DenseSubmatrix<MT,SO>::addAssign( const DenseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t jend( n_ & size_t(-2) );
   BLAZE_INTERNAL_ASSERT( ( n_ - ( n_ % 2UL ) ) == jend, "Invalid end calculation" );

   for( size_t i=0UL; i<m_; ++i ) {
      for( size_t j=0UL; j<jend; j+=2UL ) {
         matrix_(row_+i,column_+j    ) += (~rhs)(i,j    );
         matrix_(row_+i,column_+j+1UL) += (~rhs)(i,j+1UL);
      }
      if( jend < n_ ) {
         matrix_(row_+i,column_+jend) += (~rhs)(i,jend);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the addition assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename EnableIf< typename DenseSubmatrix<MT,SO>::BLAZE_TEMPLATE VectorizedAddAssign<MT2> >::Type
   DenseSubmatrix<MT,SO>::addAssign( const DenseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   const size_t jend( n_ & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( n_ - ( n_ % (IT::size*4UL) ) ) == jend, "Invalid end calculation" );

   for( size_t i=0UL; i<m_; ++i ) {
      for( size_t j=0UL; j<jend; j+=IT::size*4UL ) {
         matrix_.storeu( row_+i, column_+j             , load(i,j             ) + (~rhs).load(i,j             ) );
         matrix_.storeu( row_+i, column_+j+IT::size    , load(i,j+IT::size    ) + (~rhs).load(i,j+IT::size    ) );
         matrix_.storeu( row_+i, column_+j+IT::size*2UL, load(i,j+IT::size*2UL) + (~rhs).load(i,j+IT::size*2UL) );
         matrix_.storeu( row_+i, column_+j+IT::size*3UL, load(i,j+IT::size*3UL) + (~rhs).load(i,j+IT::size*3UL) );
      }
      for( size_t j=jend; j<n_; j+=IT::size ) {
         storeu( i, j, load(i,j) + (~rhs).load(i,j) );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline void DenseSubmatrix<MT,SO>::addAssign( const DenseMatrix<MT2,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t block( 16UL );

   for( size_t ii=0UL; ii<m_; ii+=block ) {
      const size_t iend( ( m_<(ii+block) )?( m_ ):( ii+block ) );
      for( size_t jj=0UL; jj<n_; jj+=block ) {
         const size_t jend( ( n_<(jj+block) )?( n_ ):( jj+block ) );
         for( size_t i=ii; i<iend; ++i ) {
            for( size_t j=jj; j<jend; ++j ) {
               matrix_(row_+i,column_+j) += (~rhs)(i,j);
            }
         }
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the sparse matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,SO>::addAssign( const SparseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t i=0UL; i<m_; ++i )
      for( typename MT2::ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         matrix_(row_+i,column_+element->index()) += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the addition assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the sparse matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,SO>::addAssign( const SparseMatrix<MT2,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t j=0UL; j<n_; ++j )
      for( typename MT2::ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         matrix_(row_+element->index(),column_+j) += element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename DisableIf< typename DenseSubmatrix<MT,SO>::BLAZE_TEMPLATE VectorizedSubAssign<MT2> >::Type
   DenseSubmatrix<MT,SO>::subAssign( const DenseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t jend( n_ & size_t(-2) );
   BLAZE_INTERNAL_ASSERT( ( n_ - ( n_ % 2UL ) ) == jend, "Invalid end calculation" );

   for( size_t i=0UL; i<m_; ++i ) {
      for( size_t j=0UL; j<jend; j+=2UL ) {
         matrix_(row_+i,column_+j    ) -= (~rhs)(i,j    );
         matrix_(row_+i,column_+j+1UL) -= (~rhs)(i,j+1UL);
      }
      if( jend < n_ ) {
         matrix_(row_+i,column_+jend) -= (~rhs)(i,jend);
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename EnableIf< typename DenseSubmatrix<MT,SO>::BLAZE_TEMPLATE VectorizedSubAssign<MT2> >::Type
   DenseSubmatrix<MT,SO>::subAssign( const DenseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   const size_t jend( n_ & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( n_ - ( n_ % (IT::size*4UL) ) ) == jend, "Invalid end calculation" );

   for( size_t i=0UL; i<m_; ++i ) {
      for( size_t j=0UL; j<jend; j+=IT::size*4UL ) {
         matrix_.storeu( row_+i, column_+j             , load(i,j             ) - (~rhs).load(i,j             ) );
         matrix_.storeu( row_+i, column_+j+IT::size    , load(i,j+IT::size    ) - (~rhs).load(i,j+IT::size    ) );
         matrix_.storeu( row_+i, column_+j+IT::size*2UL, load(i,j+IT::size*2UL) - (~rhs).load(i,j+IT::size*2UL) );
         matrix_.storeu( row_+i, column_+j+IT::size*3UL, load(i,j+IT::size*3UL) - (~rhs).load(i,j+IT::size*3UL) );
      }
      for( size_t j=jend; j<n_; j+=IT::size ) {
         storeu( i, j, load(i,j) - (~rhs).load(i,j) );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side dense matrix
inline void DenseSubmatrix<MT,SO>::subAssign( const DenseMatrix<MT2,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t block( 16UL );

   for( size_t ii=0UL; ii<m_; ii+=block ) {
      const size_t iend( ( m_<(ii+block) )?( m_ ):( ii+block ) );
      for( size_t jj=0UL; jj<n_; jj+=block ) {
         const size_t jend( ( n_<(jj+block) )?( n_ ):( jj+block ) );
         for( size_t i=ii; i<iend; ++i ) {
            for( size_t j=jj; j<jend; ++j ) {
               matrix_(row_+i,column_+j) -= (~rhs)(i,j);
            }
         }
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,SO>::subAssign( const SparseMatrix<MT2,SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t i=0UL; i<m_; ++i )
      for( typename MT2::ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         matrix_(row_+i,column_+element->index()) -= element->value();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Default implementation of the subtraction assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT     // Type of the dense matrix
        , bool SO >       // Storage order
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,SO>::subAssign( const SparseMatrix<MT2,!SO>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t j=0UL; j<n_; ++j )
      for( typename MT2::ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         matrix_(row_+element->index(),column_+j) -= element->value();
}
//*************************************************************************************************








//=================================================================================================
//
//  CLASS TEMPLATE SPECIALIZATION FOR COLUMN-MAJOR MATRICES
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Specialization of DenseSubmatrix for column-major matrices.
// \ingroup dense_submatrix
//
// This specialization of DenseSubmatrix adapts the class template to the requirements of
// column-major matrices.
*/
template< typename MT >  // Type of the dense matrix
class DenseSubmatrix<MT,true> : public DenseMatrix< DenseSubmatrix<MT,true>, true >
                              , private Expression
{
 private:
   //**Type definitions****************************************************************************
   //! Composite data type of the dense matrix expression.
   typedef typename SelectType< IsExpression<MT>::value, MT, MT& >::Type  Operand;

   //! Intrinsic trait for the matrix element type.
   typedef IntrinsicTrait<typename MT::ElementType>  IT;
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the non-const reference and iterator types.
   /*! The \a useConst compile time constant expression represents a compilation switch for
       the non-const reference and iterator types. In case the given dense matrix of type
       \a MT is const qualified, \a useConst will be set to 1 and the dense submatrix will
       return references and iterators to const. Otherwise \a useConst will be set to 0 and
       the dense submatrix will offer write access to the dense matrix elements both via
       the function call operator and iterators. */
   enum { useConst = IsConst<MT>::value };
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef DenseSubmatrix<MT,true>             This;           //!< Type of this DenseSubmatrix instance.
   typedef typename SubmatrixTrait<MT>::Type   ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::OppositeType   OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename ResultType::TransposeType  TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename MT::ElementType            ElementType;    //!< Type of the submatrix elements.
   typedef typename IT::Type                   IntrinsicType;  //!< Intrinsic type of the subvector elements.
   typedef typename MT::ReturnType             ReturnType;     //!< Return type for expression template evaluations
   typedef const DenseSubmatrix&               CompositeType;  //!< Data type for composite expression templates.

   //! Reference to a constant submatrix value.
   typedef typename MT::ConstReference  ConstReference;

   //! Reference to a non-constant submatrix value.
   typedef typename SelectType< useConst, ConstReference, typename MT::Reference >::Type  Reference;

   //! Pointer to a constant row value.
   typedef const ElementType*  ConstPointer;

   //! Pointer to a constant row value.
   typedef typename SelectType< useConst, ConstPointer, ElementType* >::Type  Pointer;
   //**********************************************************************************************

   //**SubmatrixIterator class definition**********************************************************
   /*!\brief Iterator over the elements of the sparse submatrix.
   */
   template< typename IteratorType >  // Type of the dense matrix iterator
   class SubmatrixIterator
   {
    public:
      //**Type definitions*************************************************************************
      //! The iterator category.
      typedef typename std::iterator_traits<IteratorType>::iterator_category  IteratorCategory;

      //! Type of the underlying elements.
      typedef typename std::iterator_traits<IteratorType>::value_type  ValueType;

      //! Pointer return type.
      typedef typename std::iterator_traits<IteratorType>::pointer  PointerType;

      //! Reference return type.
      typedef typename std::iterator_traits<IteratorType>::reference  ReferenceType;

      //! Difference between two iterators.
      typedef typename std::iterator_traits<IteratorType>::difference_type  DifferenceType;

      // STL iterator requirements
      typedef IteratorCategory  iterator_category;  //!< The iterator category.
      typedef ValueType         value_type;         //!< Type of the underlying elements.
      typedef PointerType       pointer;            //!< Pointer return type.
      typedef ReferenceType     reference;          //!< Reference return type.
      typedef DifferenceType    difference_type;    //!< Difference between two iterators.
      //*******************************************************************************************

      //**Constructor******************************************************************************
      /*!\brief Constructor for the SubmatrixIterator class.
      //
      // \param iterator Iterator to the initial element.
      */
      explicit inline SubmatrixIterator( IteratorType iterator )
         : iterator_( iterator )  // Iterator to the current submatrix element
      {}
      //*******************************************************************************************

      //**Addition assignment operator*************************************************************
      /*!\brief Addition assignment operator.
      //
      // \param inc The increment of the iterator.
      // \return The incremented iterator.
      */
      inline SubmatrixIterator& operator+=( size_t inc ) {
         iterator_ += inc;
         return *this;
      }
      //*******************************************************************************************

      //**Subtraction assignment operator**********************************************************
      /*!\brief Subtraction assignment operator.
      //
      // \param dec The decrement of the iterator.
      // \return The decremented iterator.
      */
      inline SubmatrixIterator& operator-=( size_t dec ) {
         iterator_ -= dec;
         return *this;
      }
      //*******************************************************************************************

      //**Prefix increment operator****************************************************************
      /*!\brief Pre-increment operator.
      //
      // \return Reference to the incremented iterator.
      */
      inline SubmatrixIterator& operator++() {
         ++iterator_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix increment operator***************************************************************
      /*!\brief Post-increment operator.
      //
      // \return The previous position of the iterator.
      */
      inline const SubmatrixIterator operator++( int ) {
         return SubmatrixIterator( iterator_++ );
      }
      //*******************************************************************************************

      //**Prefix decrement operator****************************************************************
      /*!\brief Pre-decrement operator.
      //
      // \return Reference to the decremented iterator.
      */
      inline SubmatrixIterator& operator--() {
         --iterator_;
         return *this;
      }
      //*******************************************************************************************

      //**Postfix decrement operator***************************************************************
      /*!\brief Post-decrement operator.
      //
      // \return The previous position of the iterator.
      */
      inline const SubmatrixIterator operator--( int ) {
         return SubmatrixIterator( iterator_-- );
      }
      //*******************************************************************************************

      //**Element access operator******************************************************************
      /*!\brief Direct access to the element at the current iterator position.
      //
      // \return The resulting value.
      */
      inline ReferenceType operator*() const {
         return *iterator_;
      }
      //*******************************************************************************************

      //**Load function****************************************************************************
      /*!\brief Aligned load of an intrinsic element of the dense submatrix.
      //
      // \return The loaded intrinsic element.
      //
      // This function performs an aligned load of the current intrinsic element of the submatrix
      // iterator. This function must \b NOT be called explicitly! It is used internally for the
      // performance optimized evaluation of expression templates. Calling this function explicitly
      // might result in erroneous results and/or in compilation errors.
      */
      inline IntrinsicType load() const {
         return iterator_.loadu();
      }
      //*******************************************************************************************

      //**Loadu function***************************************************************************
      /*!\brief Unaligned load of an intrinsic element of the dense submatrix.
      //
      // \return The loaded intrinsic element.
      //
      // This function performs an unaligned load of the current intrinsic element of the submatrix
      // iterator. This function must \b NOT be called explicitly! It is used internally for the
      // performance optimized evaluation of expression templates. Calling this function explicitly
      // might result in erroneous results and/or in compilation errors.
      */
      inline IntrinsicType loadu() const {
         return iterator_.loadu();
      }
      //*******************************************************************************************

      //**Equality operator************************************************************************
      /*!\brief Equality comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the iterators refer to the same element, \a false if not.
      */
      inline bool operator==( const SubmatrixIterator& rhs ) const {
         return iterator_ == rhs.iterator_;
      }
      //*******************************************************************************************

      //**Inequality operator**********************************************************************
      /*!\brief Inequality comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the iterators don't refer to the same element, \a false if they do.
      */
      inline bool operator!=( const SubmatrixIterator& rhs ) const {
         return iterator_ != rhs.iterator_;
      }
      //*******************************************************************************************

      //**Less-than operator***********************************************************************
      /*!\brief Less-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is smaller, \a false if not.
      */
      inline bool operator<( const SubmatrixIterator& rhs ) const {
         return iterator_ < rhs.iterator_;
      }
      //*******************************************************************************************

      //**Greater-than operator********************************************************************
      /*!\brief Greater-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is greater, \a false if not.
      */
      inline bool operator>( const SubmatrixIterator& rhs ) const {
         return iterator_ > rhs.iterator_;
      }
      //*******************************************************************************************

      //**Less-or-equal-than operator**************************************************************
      /*!\brief Less-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is smaller or equal, \a false if not.
      */
      inline bool operator<=( const SubmatrixIterator& rhs ) const {
         return iterator_ <= rhs.iterator_;
      }
      //*******************************************************************************************

      //**Greater-or-equal-than operator***********************************************************
      /*!\brief Greater-than comparison between two SubmatrixIterator objects.
      //
      // \param rhs The right-hand side iterator.
      // \return \a true if the left-hand side iterator is greater or equal, \a false if not.
      */
      inline bool operator>=( const SubmatrixIterator& rhs ) const {
         return iterator_ >= rhs.iterator_;
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Calculating the number of elements between two iterators.
      //
      // \param rhs The right-hand side iterator.
      // \return The number of elements between the two iterators.
      */
      inline DifferenceType operator-( const SubmatrixIterator& rhs ) const {
         return iterator_ - rhs.iterator_;
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between a SubmatrixIterator and an integral value.
      //
      // \param it The iterator to be incremented.
      // \param inc The number of elements the iterator is incremented.
      // \return The incremented iterator.
      */
      friend inline const SubmatrixIterator operator+( const SubmatrixIterator& it, size_t inc ) {
         return SubmatrixIterator( it.iterator_ + inc );
      }
      //*******************************************************************************************

      //**Addition operator************************************************************************
      /*!\brief Addition between an integral value and a SubmatrixIterator.
      //
      // \param inc The number of elements the iterator is incremented.
      // \param it The iterator to be incremented.
      // \return The incremented iterator.
      */
      friend inline const SubmatrixIterator operator+( size_t inc, const SubmatrixIterator& it ) {
         return SubmatrixIterator( it.iterator_ + inc );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Subtraction between a SubmatrixIterator and an integral value.
      //
      // \param it The iterator to be decremented.
      // \param inc The number of elements the iterator is decremented.
      // \return The decremented iterator.
      */
      friend inline const SubmatrixIterator operator-( const SubmatrixIterator& it, size_t dec ) {
         return SubmatrixIterator( it.iterator_ - dec );
      }
      //*******************************************************************************************

      //**Subtraction operator*********************************************************************
      /*!\brief Subtraction between an integral value and a SubmatrixIterator.
      //
      // \param inc The number of elements the iterator is decremented.
      // \param it The iterator to be decremented.
      // \return The decremented iterator.
      */
      friend inline const SubmatrixIterator operator-( size_t dec, const SubmatrixIterator& it ) {
         return SubmatrixIterator( it.iterator_ - dec );
      }
      //*******************************************************************************************

    private:
      //**Member variables*************************************************************************
      IteratorType iterator_;  //!< Iterator to the current submatrix element.
      //*******************************************************************************************
   };
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   //! Iterator over constant elements.
   typedef SubmatrixIterator<typename MT::ConstIterator>  ConstIterator;

   //! Iterator over non-constant elements.
   typedef typename SelectType< useConst, ConstIterator, SubmatrixIterator<typename MT::Iterator> >::Type  Iterator;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = MT::vectorizable };
   //**********************************************************************************************

   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit inline DenseSubmatrix( MT& matrix, size_t row, size_t column, size_t m, size_t n );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

   //**Data access functions***********************************************************************
   /*!\name Data access functions */
   //@{
   inline Reference      operator()( size_t i, size_t j );
   inline ConstReference operator()( size_t i, size_t j ) const;
   inline Pointer        data  ();
   inline ConstPointer   data  () const;
   inline Iterator       begin ( size_t i );
   inline ConstIterator  begin ( size_t i ) const;
   inline ConstIterator  cbegin( size_t i ) const;
   inline Iterator       end   ( size_t i );
   inline ConstIterator  end   ( size_t i ) const;
   inline ConstIterator  cend  ( size_t i ) const;
   //@}
   //**********************************************************************************************

   //**Assignment operators************************************************************************
   /*!\name Assignment operators */
   //@{
                                     inline DenseSubmatrix& operator= ( const ElementType& rhs );
                                     inline DenseSubmatrix& operator= ( const DenseSubmatrix& rhs );
   template< typename MT2, bool SO > inline DenseSubmatrix& operator= ( const Matrix<MT2,SO>& rhs );
   template< typename MT2, bool SO > inline DenseSubmatrix& operator+=( const Matrix<MT2,SO>& rhs );
   template< typename MT2, bool SO > inline DenseSubmatrix& operator-=( const Matrix<MT2,SO>& rhs );
   template< typename MT2, bool SO > inline DenseSubmatrix& operator*=( const Matrix<MT2,SO>& rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix >::Type&
      operator*=( Other rhs );

   template< typename Other >
   inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix >::Type&
      operator/=( Other rhs );
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
                              inline size_t          rows() const;
                              inline size_t          columns() const;
                              inline size_t          spacing() const;
                              inline size_t          capacity() const;
                              inline size_t          capacity( size_t i ) const;
                              inline size_t          nonZeros() const;
                              inline size_t          nonZeros( size_t i ) const;
                              inline void            reset();
                              inline void            reset( size_t i );
   template< typename Other > inline DenseSubmatrix& scale( Other scalar );
   //@}
   //**********************************************************************************************

 private:
   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT2 >
   struct VectorizedAssign {
      enum { value = vectorizable && MT2::vectorizable &&
                     IsSame<ElementType,typename MT2::ElementType>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT2 >
   struct VectorizedAddAssign {
      enum { value = vectorizable && MT2::vectorizable &&
                     IsSame<ElementType,typename MT2::ElementType>::value &&
                     IntrinsicTrait<ElementType>::addition };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   template< typename MT2 >
   struct VectorizedSubAssign {
      enum { value = vectorizable && MT2::vectorizable &&
                     IsSame<ElementType,typename MT2::ElementType>::value &&
                     IntrinsicTrait<ElementType>::subtraction };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Expression template evaluation functions****************************************************
   /*!\name Expression template evaluation functions */
   //@{
   template< typename Other > inline bool canAlias ( const Other* alias ) const;
   template< typename Other > inline bool isAliased( const Other* alias ) const;

   inline IntrinsicType load  ( size_t i, size_t j ) const;
   inline IntrinsicType loadu ( size_t i, size_t j ) const;
   inline void          store ( size_t i, size_t j, const IntrinsicType& value );
   inline void          storeu( size_t i, size_t j, const IntrinsicType& value );
   inline void          stream( size_t i, size_t j, const IntrinsicType& value );

   template< typename MT2 >
   inline typename DisableIf< VectorizedAssign<MT2> >::Type
      assign( const DenseMatrix<MT2,true>& rhs );

   template< typename MT2 >
   inline typename EnableIf< VectorizedAssign<MT2> >::Type
      assign( const DenseMatrix<MT2,true>& rhs );

   template< typename MT2 > inline void assign( const DenseMatrix<MT2,false>&  rhs );
   template< typename MT2 > inline void assign( const SparseMatrix<MT2,true>&  rhs );
   template< typename MT2 > inline void assign( const SparseMatrix<MT2,false>& rhs );

   template< typename MT2 >
   inline typename DisableIf< VectorizedAddAssign<MT2> >::Type
      addAssign( const DenseMatrix<MT2,true>& rhs );

   template< typename MT2 >
   inline typename EnableIf< VectorizedAddAssign<MT2> >::Type
      addAssign( const DenseMatrix<MT2,true>& rhs );

   template< typename MT2 > inline void addAssign( const DenseMatrix<MT2,false>&  rhs );
   template< typename MT2 > inline void addAssign( const SparseMatrix<MT2,true>&  rhs );
   template< typename MT2 > inline void addAssign( const SparseMatrix<MT2,false>& rhs );

   template< typename MT2 >
   inline typename DisableIf< VectorizedSubAssign<MT2> >::Type
      subAssign( const DenseMatrix<MT2,true>& rhs );

   template< typename MT2 >
   inline typename EnableIf< VectorizedSubAssign<MT2> >::Type
      subAssign( const DenseMatrix<MT2,true>& rhs );

   template< typename MT2 > inline void subAssign( const DenseMatrix<MT2,false>&  rhs );
   template< typename MT2 > inline void subAssign( const SparseMatrix<MT2,true>&  rhs );
   template< typename MT2 > inline void subAssign( const SparseMatrix<MT2,false>& rhs );
   //@}
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   Operand      matrix_;   //!< The dense matrix containing the submatrix.
   const size_t row_;      //!< The first row of the submatrix.
   const size_t column_;   //!< The first column of the submatrix.
   const size_t m_;        //!< The number of rows of the submatrix.
   const size_t n_;        //!< The number of columns of the submatrix.
   const size_t rest_;     //!< The number of remaining elements in an unaligned intrinsic operation.
   const size_t final_;    //!< The final index for unaligned intrinsic operations.
                           /*!< In case the submatrix is not fully aligned and the submatrix is
                                involved in a vectorized operation, the final index indicates at
                                which index a special treatment for the remaining elements is
                                required. */
   const bool   aligned_;  //!< Memory alignment flag.
                           /*!< The alignment flag indicates whether the submatrix is fully aligned.
                                In case the submatrix is fully aligned, no special handling has to
                                be used for the last elements of the submatrix in a vectorized
                                operation. In order to be aligned, the following conditions must
                                hold for the submatrix:
                                 - The first element of each row/column must be aligned
                                 - The submatrix must be at the end of the given matrix or
                                 - The number of rows/columns of the submatrix must be a multiple
                                   of the number of values per intrinsic element. */
   //@}
   //**********************************************************************************************

   //**Friend declarations*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   template< typename MT2, bool SO2 >
   friend DenseSubmatrix<MT2,SO2>
      submatrix( const DenseSubmatrix<MT2,SO2>& dm, size_t row, size_t column, size_t m, size_t n );
   /*! \endcond */
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE       ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_COMPUTATION_TYPE    ( MT );
   BLAZE_CONSTRAINT_MUST_NOT_BE_TRANSEXPR_TYPE      ( MT );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT );
   /*! \endcond */
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTOR
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief The constructor for DenseSubmatrix.
//
// \param matrix The dense matrix containing the submatrix.
// \param row The index of the first row of the submatrix in the given dense matrix.
// \param column The index of the first column of the submatrix in the given dense matrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \exception std::invalid_argument Invalid submatrix specification.
*/
template< typename MT >  // Type of the dense matrix
inline DenseSubmatrix<MT,true>::DenseSubmatrix( MT& matrix, size_t row, size_t column, size_t m, size_t n )
   : matrix_ ( matrix       )  // The dense matrix containing the submatrix
   , row_    ( row          )  // The first row of the submatrix
   , column_ ( column       )  // The first column of the submatrix
   , m_      ( m            )  // The number of rows of the submatrix
   , n_      ( n            )  // The number of columns of the submatrix
   , rest_   ( m % IT::size )  // The number of remaining elements in an unaligned intrinsic operation
   , final_  ( m - rest_    )  // The final index for unaligned intrinsic operations
   , aligned_( ( row % IT::size == 0UL ) &&
               ( row + m == matrix.rows() || m % IT::size == 0UL ) )
{
   if( ( row + m > matrix.rows() ) || ( column + n > matrix.columns() ) )
      throw std::invalid_argument( "Invalid submatrix specification" );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  DATA ACCESS FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the dense submatrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::Reference
   DenseSubmatrix<MT,true>::operator()( size_t i, size_t j )
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   return matrix_(row_+i,column_+j);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief 2D-access to the dense submatrix elements.
//
// \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
// \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
// \return Reference to the accessed value.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::ConstReference
   DenseSubmatrix<MT,true>::operator()( size_t i, size_t j ) const
{
   BLAZE_USER_ASSERT( i < rows()   , "Invalid row access index"    );
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   return const_cast<const MT&>( matrix_ )(row_+i,column_+j);
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the submatrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense submatrix. Note that
// you can NOT assume that all matrix elements lie adjacent to each other! The dense submatrix
// may use techniques such as padding to improve the alignment of the data.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::Pointer DenseSubmatrix<MT,true>::data()
{
   return matrix_.data() + row_ + column_*spacing();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Low-level data access to the submatrix elements.
//
// \return Pointer to the internal element storage.
//
// This function returns a pointer to the internal storage of the dense submatrix. Note that
// you can NOT assume that all matrix elements lie adjacent to each other! The dense submatrix
// may use techniques such as padding to improve the alignment of the data.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::ConstPointer DenseSubmatrix<MT,true>::data() const
{
   return matrix_.data() + row_ + column_*spacing();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator to the first non-zero element of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::Iterator DenseSubmatrix<MT,true>::begin( size_t j )
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid dense submatrix column access index" );
   return Iterator( matrix_.begin( column_ + j ) + row_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator to the first non-zero element of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::ConstIterator DenseSubmatrix<MT,true>::begin( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid dense submatrix column access index" );
   return ConstIterator( matrix_.cbegin( column_ + j ) + row_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator to the first non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator to the first non-zero element of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::ConstIterator DenseSubmatrix<MT,true>::cbegin( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid dense submatrix column access index" );
   return ConstIterator( matrix_.cbegin( column_ + j ) + row_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last non-zero element of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::Iterator DenseSubmatrix<MT,true>::end( size_t j )
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid dense submatrix column access index" );
   return Iterator( matrix_.begin( column_ + j ) + row_ + m_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last non-zero element of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::ConstIterator DenseSubmatrix<MT,true>::end( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid dense submatrix column access index" );
   return ConstIterator( matrix_.cbegin( column_ + j ) + row_ + m_ );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns an iterator just past the last non-zero element of column \a j.
//
// \param j The column index.
// \return Iterator just past the last non-zero element of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::ConstIterator DenseSubmatrix<MT,true>::cend( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid dense submatrix column access index" );
   return ConstIterator( matrix_.cbegin( column_ + j ) + row_ + m_ );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ASSIGNMENT OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Homogenous assignment to all submatrix elements.
//
// \param rhs Scalar value to be assigned to all submatrix elements.
// \return Reference to the assigned submatrix.
*/
template< typename MT >  // Type of the dense matrix
inline DenseSubmatrix<MT,true>& DenseSubmatrix<MT,true>::operator=( const ElementType& rhs )
{
   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );

   for( size_t j=column_; j<jend; ++j )
      for( size_t i=row_; i<iend; ++i )
         matrix_(i,j) = rhs;

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Copy assignment operator for DenseSubmatrix.
//
// \param rhs Sparse submatrix to be copied.
// \return Reference to the assigned submatrix.
// \exception std::invalid_argument Submatrix sizes do not match.
//
// The dense submatrix is initialized as a copy of the given dense submatrix. In case the
// current sizes of the two submatrices don't match, a \a std::invalid_argument exception is
// thrown.
*/
template< typename MT >  // Type of the dense matrix
inline DenseSubmatrix<MT,true>& DenseSubmatrix<MT,true>::operator=( const DenseSubmatrix& rhs )
{
   using blaze::assign;

   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE  ( ResultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( ResultType );

   if( this == &rhs || ( &matrix_ == &rhs.matrix_ && row_ == rhs.row_ && column_ == rhs.column_ ) )
      return *this;

   if( rows() != rhs.rows() || columns() != rhs.columns() )
      throw std::invalid_argument( "Submatrix sizes do not match" );

   if( rhs.canAlias( &matrix_ ) ) {
      const ResultType tmp( rhs );
      assign( *this, tmp );
   }
   else {
      if( IsSparseMatrix<MT>::value )
         reset();
      assign( *this, rhs );
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Assignment operator for different matrices.
//
// \param rhs Matrix to be assigned.
// \return Reference to the assigned submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// The dense submatrix is initialized as a copy of the given matrix. In case the current
// sizes of the two matrices don't match, a \a std::invalid_argument exception is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,true>& DenseSubmatrix<MT,true>::operator=( const Matrix<MT2,SO>& rhs )
{
   using blaze::assign;

   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename MT2::ResultType );

   if( rows() != (~rhs).rows() || columns() != (~rhs).columns() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( IsSparseMatrix<MT2>::value )
      reset();

   if( (~rhs).canAlias( &matrix_ ) ) {
      const typename MT2::ResultType tmp( ~rhs );
      assign( *this, tmp );
   }
   else {
      assign( *this, ~rhs );
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Addition assignment operator for the addition of a matrix (\f$ A+=B \f$).
//
// \param rhs The right-hand side matrix to be added to the submatrix.
// \return Reference to the dense submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the right-hand side matrix
        , bool SO  >     // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,true>& DenseSubmatrix<MT,true>::operator+=( const Matrix<MT2,SO>& rhs )
{
   using blaze::addAssign;

   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename MT2::ResultType );

   if( rows() != (~rhs).rows() || columns() != (~rhs).columns() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( (~rhs).canAlias( &matrix_ ) ) {
      const typename MT2::ResultType tmp( ~rhs );
      addAssign( *this, tmp );
   }
   else {
      addAssign( *this, ~rhs );
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Subtraction assignment operator for the subtraction of a matrix (\f$ A-=B \f$).
//
// \param rhs The right-hand side matrix to be subtracted from the submatrix.
// \return Reference to the dense submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two matrices don't match, a \a std::invalid_argument exception
// is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,true>& DenseSubmatrix<MT,true>::operator-=( const Matrix<MT2,SO>& rhs )
{
   using blaze::subAssign;

   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( typename MT2::ResultType );

   if( rows() != (~rhs).rows() || columns() != (~rhs).columns() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   if( (~rhs).canAlias( &matrix_ ) ) {
      const typename MT2::ResultType tmp( ~rhs );
      subAssign( *this, tmp );
   }
   else {
      subAssign( *this, ~rhs );
   }

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication of a matrix (\f$ A*=B \f$).
//
// \param rhs The right-hand side matrix for the multiplication.
// \return Reference to the dense submatrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// In case the current sizes of the two given matrices don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename MT >  // Type of the dense matrix
template< typename MT2   // Type of the right-hand side matrix
        , bool SO >      // Storage order of the right-hand side matrix
inline DenseSubmatrix<MT,true>& DenseSubmatrix<MT,true>::operator*=( const Matrix<MT2,SO>& rhs )
{
   if( columns() != (~rhs).rows() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   typedef typename MultTrait<ResultType,typename MT2::ResultType>::Type  MultType;

   BLAZE_CONSTRAINT_MUST_BE_MATRIX_TYPE( MultType );
   BLAZE_CONSTRAINT_MUST_NOT_REQUIRE_EVALUATION( MultType );

   const MultType tmp( *this * (~rhs) );
   if( IsSparseMatrix<MultType>::value )
      reset();
   assign( tmp );

   return *this;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Multiplication assignment operator for the multiplication between a dense submatrix
//        and a scalar value (\f$ A*=s \f$).
//
// \param rhs The right-hand side scalar value for the multiplication.
// \return Reference to the dense submatrix.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix<MT,true> >::Type&
   DenseSubmatrix<MT,true>::operator*=( Other rhs )
{
   return operator=( (*this) * rhs );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Division assignment operator for the division of a dense submatrix by a scalar value
//        (\f$ A/=s \f$).
//
// \param rhs The right-hand side scalar value for the division.
// \return Reference to the dense submatrix.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the right-hand side scalar
inline typename EnableIf< IsNumeric<Other>, DenseSubmatrix<MT,true> >::Type&
   DenseSubmatrix<MT,true>::operator/=( Other rhs )
{
   BLAZE_USER_ASSERT( rhs != Other(0), "Division by zero detected" );

   return operator=( (*this) / rhs );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of rows of the dense submatrix.
//
// \return The number of rows of the dense submatrix.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseSubmatrix<MT,true>::rows() const
{
   return m_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of columns of the dense submatrix.
//
// \return The number of columns of the dense submatrix.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseSubmatrix<MT,true>::columns() const
{
   return n_;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the spacing between the beginning of two columns.
//
// \return The spacing between the beginning of two columns.
//
// This function returns the spacing between the beginning of two columns, i.e. the total
// number of elements of a column.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseSubmatrix<MT,true>::spacing() const
{
   return matrix_.spacing();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the maximum capacity of the dense submatrix.
//
// \return The capacity of the dense submatrix.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseSubmatrix<MT,true>::capacity() const
{
   return rows() * columns();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the current capacity of the specified column.
//
// \param j The index of the column.
// \return The current capacity of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseSubmatrix<MT,true>::capacity( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );
   return rows();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the dense submatrix
//
// \return The number of non-zero elements in the dense submatrix.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseSubmatrix<MT,true>::nonZeros() const
{
   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );
   size_t nonzeros( 0UL );

   for( size_t j=column_; j<jend; ++j )
      for( size_t i=row_; i<iend; ++i )
         if( !isDefault( matrix_(i,j) ) )
            ++nonzeros;

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns the number of non-zero elements in the specified column.
//
// \param j The index of the column.
// \return The number of non-zero elements of column \a j.
*/
template< typename MT >  // Type of the dense matrix
inline size_t DenseSubmatrix<MT,true>::nonZeros( size_t j ) const
{
   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const size_t iend( row_ + m_ );
   size_t nonzeros( 0UL );

   for( size_t i=row_; i<iend; ++i )
      if( !isDefault( matrix_(i,column_+j) ) )
         ++nonzeros;

   return nonzeros;
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset to the default initial values.
//
// \return void
*/
template< typename MT >  // Type of the dense matrix
inline void DenseSubmatrix<MT,true>::reset()
{
   using blaze::reset;

   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );

   for( size_t j=column_; j<jend; ++j )
      for( size_t i=row_; i<iend; ++i )
         reset( matrix_(i,j) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Reset the specified column to the default initial values.
//
// \param j The index of the column.
// \return void
*/
template< typename MT >  // Type of the dense matrix
inline void DenseSubmatrix<MT,true>::reset( size_t j )
{
   using blaze::reset;

   BLAZE_USER_ASSERT( j < columns(), "Invalid column access index" );

   const size_t iend( row_ + m_ );
   for( size_t i=row_; i<iend; ++i )
      reset( matrix_(i,column_+j) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Scaling of the dense submatrix by the scalar value \a scalar (\f$ A=B*s \f$).
//
// \param scalar The scalar value for the submatrix scaling.
// \return Reference to the dense submatrix.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the scalar value
inline DenseSubmatrix<MT,true>& DenseSubmatrix<MT,true>::scale( Other scalar )
{
   const size_t iend( row_ + m_ );
   const size_t jend( column_ + n_ );

   for( size_t j=column_; j<jend; ++j )
      for( size_t i=row_; i<iend; ++i )
         matrix_(i,j) *= scalar;

   return *this;
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TEMPLATE EVALUATION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the submatrix can alias with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this submatrix, \a false if not.
//
// This function returns whether the given address can alias with the submatrix. In contrast
// to the isAliased() function this function is allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the foreign expression
inline bool DenseSubmatrix<MT,true>::canAlias( const Other* alias ) const
{
   return static_cast<const void*>( &matrix_ ) == static_cast<const void*>( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Returns whether the submatrix is aliased with the given address \a alias.
//
// \param alias The alias to be checked.
// \return \a true in case the alias corresponds to this submatrix, \a false if not.
//
// This function returns whether the given address is aliased with the submatrix. In contrast
// to the conAlias() function this function is not allowed to use compile time expressions to
// optimize the evaluation.
*/
template< typename MT >     // Type of the dense matrix
template< typename Other >  // Data type of the foreign expression
inline bool DenseSubmatrix<MT,true>::isAliased( const Other* alias ) const
{
   return static_cast<const void*>( &matrix_ ) == static_cast<const void*>( alias );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned load of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded intrinsic element.
//
// This function performs an aligned load of a specific intrinsic element of the dense
// submatrix. The row index must be smaller than the number of rows and the column index
// must be smaller than the number of columns. Additionally, the row index must be a
// multiple of the number of values inside the intrinsic element. This function must
// \b NOT be called explicitly! It is used internally for the performance optimized
// evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::IntrinsicType
   DenseSubmatrix<MT,true>::load( size_t i, size_t j ) const
{
   return loadu( i, j );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned load of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \return The loaded intrinsic element.
//
// This function performs an aligned load of a specific intrinsic element of the dense
// submatrix. The row index must be smaller than the number of rows and the column index
// must be smaller than the number of columns. Additionally, the row index must be a
// multiple of the number of values inside the intrinsic element. This function must
// \b NOT be called explicitly! It is used internally for the performance optimized
// evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors.
*/
template< typename MT >  // Type of the dense matrix
inline typename DenseSubmatrix<MT,true>::IntrinsicType
   DenseSubmatrix<MT,true>::loadu( size_t i, size_t j ) const
{
   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( i < rows()         , "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( i % IT::size == 0UL, "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( j < columns()      , "Invalid column access index" );

   if( aligned_ || i != final_ ) {
      return matrix_.loadu( row_+i, column_+j );
   }
   else {
      IntrinsicType value;
      for( size_t k=0UL; k<rest_; ++k )
         value[k] = matrix_(row_+i+k,column_+j);
      return value;
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned store of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned store of a specific intrinsic element of the dense submatrix.
// The row index must be smaller than the number of rows and the column index must be smaller than
// the number of columns. Additionally, the row index must be a multiple of the number of values
// inside the intrinsic element. This function must \b NOT be called explicitly! It is used
// internally for the performance optimized evaluation of expression templates. Calling this
// function explicitly might result in erroneous results and/or in compilation errors.
*/
template< typename MT >  // Type of the dense matrix
inline void DenseSubmatrix<MT,true>::store( size_t i, size_t j, const IntrinsicType& value )
{
   storeu( i, j, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Unaligned store of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an unaligned store of a specific intrinsic element of the dense
// submatrix. The row index must be smaller than the number of rows and the column index must
// be smaller than the number of columns. Additionally, the row index must be a multiple of
// the number of values inside the intrinsic element. This function must \b NOT be called
// explicitly! It is used internally for the performance optimized evaluation of expression
// templates. Calling this function explicitly might result in erroneous results and/or in
// compilation errors.
*/
template< typename MT >  // Type of the dense matrix
inline void DenseSubmatrix<MT,true>::storeu( size_t i, size_t j, const IntrinsicType& value )
{
   using blaze::storeu;

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   BLAZE_INTERNAL_ASSERT( i < rows()         , "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( i % IT::size == 0UL, "Invalid row access index"    );
   BLAZE_INTERNAL_ASSERT( j < columns()      , "Invalid column access index" );

   if( aligned_ || i != final_ ) {
      matrix_.storeu( row_+i, column_+j, value );
   }
   else {
      for( size_t k=0UL; k<rest_; ++k )
         matrix_(row_+i+k,column_+j) = value[k];
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Aligned, non-temporal store of an intrinsic element of the submatrix.
//
// \param i Access index for the row. The index has to be in the range [0..M-1].
// \param j Access index for the column. The index has to be in the range [0..N-1].
// \param value The intrinsic element to be stored.
// \return void
//
// This function performs an aligned, non-temporal store of a specific intrinsic element of
// the dense submatrix. The row index must be smaller than the number of rows and the column
// index must be smaller than the number of columns. Additionally, the row index must be a
// multiple of the number of values inside the intrinsic element. This function must \b NOT
// be called explicitly! It is used internally for the performance optimized evaluation of
// expression templates. Calling this function explicitly might result in erroneous results
// and/or in compilation errors.
*/
template< typename MT >  // Type of the dense matrix
inline void DenseSubmatrix<MT,true>::stream( size_t i, size_t j, const IntrinsicType& value )
{
   storeu( i, j, value );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename DisableIf< typename DenseSubmatrix<MT,true>::BLAZE_TEMPLATE VectorizedAssign<MT2> >::Type
   DenseSubmatrix<MT,true>::assign( const DenseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t iend( m_ & size_t(-2) );
   BLAZE_INTERNAL_ASSERT( ( m_ - ( m_ % 2UL ) ) == iend, "Invalid end calculation" );

   for( size_t j=0UL; j<n_; ++j ) {
      for( size_t i=0UL; i<iend; i+=2UL ) {
         matrix_(row_+i    ,column_+j) = (~rhs)(i    ,j);
         matrix_(row_+i+1UL,column_+j) = (~rhs)(i+1UL,j);
      }
      if( iend < m_ ) {
         matrix_(row_+iend,column_+j) = (~rhs)(iend,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename EnableIf< typename DenseSubmatrix<MT,true>::BLAZE_TEMPLATE VectorizedAssign<MT2> >::Type
   DenseSubmatrix<MT,true>::assign( const DenseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   if( aligned_ && m_*n_ > ( cacheSize / ( sizeof(ElementType) * 3UL ) ) && !(~rhs).isAliased( &matrix_ ) )
   {
      for( size_t j=0UL; j<n_; ++j )
         for( size_t i=0UL; i<m_; i+=IT::size )
            matrix_.stream( row_+i, column_+j, (~rhs).load(i,j) );
   }
   else
   {
      const size_t iend( m_ & size_t(-IT::size*4) );
      BLAZE_INTERNAL_ASSERT( ( m_ - ( m_ % (IT::size*4UL) ) ) == iend, "Invalid end calculation" );

      for( size_t j=0UL; j<n_; ++j ) {
         for( size_t i=0UL; i<iend; i+=IT::size*4UL ) {
            matrix_.storeu( row_+i             , column_+j, (~rhs).load(i             ,j) );
            matrix_.storeu( row_+i+IT::size    , column_+j, (~rhs).load(i+IT::size    ,j) );
            matrix_.storeu( row_+i+IT::size*2UL, column_+j, (~rhs).load(i+IT::size*2UL,j) );
            matrix_.storeu( row_+i+IT::size*3UL, column_+j, (~rhs).load(i+IT::size*3UL,j) );
         }
         for( size_t i=iend; i<m_; i+=IT::size ) {
            storeu( i, j, (~rhs).load(i,j) );
         }
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline void DenseSubmatrix<MT,true>::assign( const DenseMatrix<MT2,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t block( 16UL );

   for( size_t jj=0UL; jj<n_; jj+=block ) {
      const size_t jend( ( n_<(jj+block) )?( n_ ):( jj+block ) );
      for( size_t ii=0UL; ii<m_; ii+=block ) {
         const size_t iend( ( m_<(ii+block) )?( m_ ):( ii+block ) );
         for( size_t j=jj; j<jend; ++j ) {
            for( size_t i=ii; i<iend; ++i ) {
               matrix_(row_+i,column_+j) = (~rhs)(i,j);
            }
         }
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,true>::assign( const SparseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t j=0UL; j<n_; ++j )
      for( typename MT2::ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         matrix_(row_+element->index(),column_+j) = element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be assigned.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,true>::assign( const SparseMatrix<MT2,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t i=0UL; i<m_; ++i )
      for( typename MT2::ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         matrix_(row_+i,column_+element->index()) = element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename DisableIf< typename DenseSubmatrix<MT,true>::BLAZE_TEMPLATE VectorizedAddAssign<MT2> >::Type
   DenseSubmatrix<MT,true>::addAssign( const DenseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t iend( m_ & size_t(-2) );
   BLAZE_INTERNAL_ASSERT( ( m_ - ( m_ % 2UL ) ) == iend, "Invalid end calculation" );

   for( size_t j=0UL; j<n_; ++j ) {
      for( size_t i=0UL; i<iend; i+=2UL ) {
         matrix_(row_+i    ,column_+j) += (~rhs)(i    ,j);
         matrix_(row_+i+1UL,column_+j) += (~rhs)(i+1UL,j);
      }
      if( iend < m_ ) {
         matrix_(row_+iend,column_+j) += (~rhs)(iend,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the addition assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename EnableIf< typename DenseSubmatrix<MT,true>::BLAZE_TEMPLATE VectorizedAddAssign<MT2> >::Type
   DenseSubmatrix<MT,true>::addAssign( const DenseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   const size_t iend( m_ & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( m_ - ( m_ % (IT::size*4UL) ) ) == iend, "Invalid end calculation" );

   for( size_t j=0UL; j<n_; ++j ) {
      for( size_t i=0UL; i<iend; i+=IT::size*4UL ) {
         matrix_.storeu( row_+i             , column_+j, load(i             ,j) + (~rhs).load(i             ,j) );
         matrix_.storeu( row_+i+IT::size    , column_+j, load(i+IT::size    ,j) + (~rhs).load(i+IT::size    ,j) );
         matrix_.storeu( row_+i+IT::size*2UL, column_+j, load(i+IT::size*2UL,j) + (~rhs).load(i+IT::size*2UL,j) );
         matrix_.storeu( row_+i+IT::size*3UL, column_+j, load(i+IT::size*3UL,j) + (~rhs).load(i+IT::size*3UL,j) );
      }
      for( size_t i=iend; i<m_; i+=IT::size ) {
         storeu( i, j, load(i,j) + (~rhs).load(i,j) );
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline void DenseSubmatrix<MT,true>::addAssign( const DenseMatrix<MT2,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t block( 16UL );

   for( size_t jj=0UL; jj<n_; jj+=block ) {
      const size_t jend( ( n_<(jj+block) )?( n_ ):( jj+block ) );
      for( size_t ii=0UL; ii<m_; ii+=block ) {
         const size_t iend( ( m_<(ii+block) )?( m_ ):( ii+block ) );
         for( size_t j=jj; j<jend; ++j ) {
            for( size_t i=ii; i<iend; ++i ) {
               matrix_(row_+i,column_+j) += (~rhs)(i,j);
            }
         }
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the sparse matrix
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,true>::addAssign( const SparseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t j=0UL; j<n_; ++j )
      for( typename MT2::ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         matrix_(row_+element->index(),column_+j) += element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the addition assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be added.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the sparse matrix
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,true>::addAssign( const SparseMatrix<MT2,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t i=0UL; i<m_; ++i )
      for( typename MT2::ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         matrix_(row_+i,column_+element->index()) += element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename DisableIf< typename DenseSubmatrix<MT,true>::BLAZE_TEMPLATE VectorizedSubAssign<MT2> >::Type
   DenseSubmatrix<MT,true>::subAssign( const DenseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t iend( m_ & size_t(-2) );
   BLAZE_INTERNAL_ASSERT( ( m_ - ( m_ % 2UL ) ) == iend, "Invalid end calculation" );

   for( size_t j=0UL; j<n_; ++j ) {
      for( size_t i=0UL; i<iend; i+=2UL ) {
         matrix_(row_+i    ,column_+j) -= (~rhs)(i    ,j);
         matrix_(row_+i+1UL,column_+j) -= (~rhs)(i+1UL,j);
      }
      if( iend < m_ ) {
         matrix_(row_+iend,column_+j) -= (~rhs)(iend,j);
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Intrinsic optimized implementation of the subtraction assignment of a column-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline typename EnableIf< typename DenseSubmatrix<MT,true>::BLAZE_TEMPLATE VectorizedSubAssign<MT2> >::Type
   DenseSubmatrix<MT,true>::subAssign( const DenseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   BLAZE_CONSTRAINT_MUST_BE_VECTORIZABLE_TYPE( ElementType );

   const size_t iend( m_ & size_t(-IT::size*4) );
   BLAZE_INTERNAL_ASSERT( ( m_ - ( m_ % (IT::size*4UL) ) ) == iend, "Invalid end calculation" );

   for( size_t j=0UL; j<n_; ++j ) {
      for( size_t i=0UL; i<iend; i+=IT::size*4UL ) {
         matrix_.storeu( row_+i             , column_+j, load(i             ,j) - (~rhs).load(i             ,j) );
         matrix_.storeu( row_+i+IT::size    , column_+j, load(i+IT::size    ,j) - (~rhs).load(i+IT::size    ,j) );
         matrix_.storeu( row_+i+IT::size*2UL, column_+j, load(i+IT::size*2UL,j) - (~rhs).load(i+IT::size*2UL,j) );
         matrix_.storeu( row_+i+IT::size*3UL, column_+j, load(i+IT::size*3UL,j) - (~rhs).load(i+IT::size*3UL,j) );
      }
      for( size_t i=iend; i<m_; i+=IT::size ) {
         storeu( i, j, load(i,j) - (~rhs).load(i,j) );
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a row-major dense matrix.
//
// \param rhs The right-hand side dense matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side dense matrix
inline void DenseSubmatrix<MT,true>::subAssign( const DenseMatrix<MT2,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   const size_t block( 16UL );

   for( size_t jj=0UL; jj<n_; jj+=block ) {
      const size_t jend( ( n_<(jj+block) )?( n_ ):( jj+block ) );
      for( size_t ii=0UL; ii<m_; ii+=block ) {
         const size_t iend( ( m_<(ii+block) )?( m_ ):( ii+block ) );
         for( size_t j=jj; j<jend; ++j ) {
            for( size_t i=ii; i<iend; ++i ) {
               matrix_(row_+i,column_+j) -= (~rhs)(i,j);
            }
         }
      }
   }
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a column-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,true>::subAssign( const SparseMatrix<MT2,true>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t j=0UL; j<n_; ++j )
      for( typename MT2::ConstIterator element=(~rhs).begin(j); element!=(~rhs).end(j); ++element )
         matrix_(row_+element->index(),column_+j) -= element->value();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Default implementation of the subtraction assignment of a row-major sparse matrix.
//
// \param rhs The right-hand side sparse matrix to be subtracted.
// \return void
//
// This function must \b NOT be called explicitly! It is used internally for the performance
// optimized evaluation of expression templates. Calling this function explicitly might result
// in erroneous results and/or in compilation errors. Instead of using this function use the
// assignment operator.
*/
template< typename MT >   // Type of the dense matrix
template< typename MT2 >  // Type of the right-hand side sparse matrix
inline void DenseSubmatrix<MT,true>::subAssign( const SparseMatrix<MT2,false>& rhs )
{
   BLAZE_INTERNAL_ASSERT( m_ == (~rhs).rows()   , "Invalid number of rows"    );
   BLAZE_INTERNAL_ASSERT( n_ == (~rhs).columns(), "Invalid number of columns" );

   for( size_t i=0UL; i<m_; ++i )
      for( typename MT2::ConstIterator element=(~rhs).begin(i); element!=(~rhs).end(i); ++element )
         matrix_(row_+i,column_+element->index()) -= element->value();
}
/*! \endcond */
//*************************************************************************************************








//=================================================================================================
//
//  DENSESUBMATRIX OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\name DenseSubmatrix operators */
//@{
template< typename MT, bool SO >
inline void reset( DenseSubmatrix<MT,SO>& dm );

template< typename MT, bool SO >
inline void clear( DenseSubmatrix<MT,SO>& dm );

template< typename MT, bool SO >
inline bool isDefault( const DenseSubmatrix<MT,SO>& dm );
//@}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Resetting the given dense submatrix.
// \ingroup dense_submatrix
//
// \param dm The dense submatrix to be resetted.
// \return void
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void reset( DenseSubmatrix<MT,SO>& dm )
{
   dm.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Clearing the given dense matrix.
// \ingroup dense_submatrix
//
// \param dm The dense matrix to be cleared.
// \return void
//
// Clearing a dense submatrix is equivalent to resetting it via the reset() function.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline void clear( DenseSubmatrix<MT,SO>& dm )
{
   dm.reset();
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Returns whether the given dense submatrix is in default state.
// \ingroup dense_submatrix
//
// \param dm The dense submatrix to be tested for its default state.
// \return \a true in case the given submatrix is component-wise zero, \a false otherwise.
//
// This function checks whether the submatrix is in default state. For instance, in
// case the submatrix is instantiated for a built-in integral or floating point data type, the
// function returns \a true in case all submatrix elements are 0 and \a false in case any submatrix
// element is not 0. The following example demonstrates the use of the \a isDefault function:

   \code
   blaze::DynamicMatrix<double,rowMajor> A;
   // ... Resizing and initialization
   if( isDefault( submatrix( A, 12UL, 13UL, 22UL, 33UL ) ) ) { ... }
   \endcode
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline bool isDefault( const DenseSubmatrix<MT,SO>& dm )
{
   using blaze::isDefault;

   if( SO == rowMajor ) {
      for( size_t i=0UL; i<(~dm).rows(); ++i )
         for( size_t j=0UL; j<(~dm).columns(); ++j )
            if( !isDefault( (~dm)(i,j) ) )
               return false;
   }
   else {
      for( size_t j=0UL; j<(~dm).columns(); ++j )
         for( size_t i=0UL; i<(~dm).rows(); ++i )
            if( !isDefault( (~dm)(i,j) ) )
               return false;
   }

   return true;
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL FUNCTION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Creating a view on a specific submatrix of the given dense matrix.
// \ingroup views
//
// \param dm The dense matrix containing the submatrix.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specific submatrix of the dense matrix.
// \exception std::invalid_argument Invalid submatrix specification.
//
// This function returns an expression representing the specified submatrix of the given dense
// matrix. The following example demonstrates the creation of a submatrix of size 4 by 4 starting
// from position (3,2):

   \code
   using blaze::rowMatrix;

   typedef blaze::DynamicMatrix<double,rowMatrix>  Matrix;

   Matrix A;
   // ... Resizing and initialization
   blaze::DenseSubmatrix<Matrix> = submatrix( A, 3UL, 2UL, 4UL, 4UL );
   \endcode
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DisableIf< Or< IsComputation<MT>, IsTransExpr<MT> >, DenseSubmatrix<MT> >::Type
   submatrix( DenseMatrix<MT,SO>& dm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return DenseSubmatrix<MT>( ~dm, row, column, m, n );
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Creating a view on a specific submatrix of the given dense matrix.
// \ingroup views
//
// \param dm The dense matrix containing the submatrix.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specific submatrix of the dense matrix.
// \exception std::invalid_argument Invalid submatrix specification.
//
// This function returns an expression representing the specified submatrix of the given dense
// matrix. The following example demonstrates the creation of a submatrix of size 4 by 4 starting
// from position (3,2):

   \code
   using blaze::rowMatrix;

   typedef blaze::DynamicMatrix<double,rowMatrix>  Matrix;

   Matrix A;
   // ... Resizing and initialization
   blaze::DenseSubmatrix<Matrix> = submatrix( A, 3UL, 2UL, 4UL, 4UL );
   \endcode
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename DisableIf< Or< IsComputation<MT>, IsTransExpr<MT> >, DenseSubmatrix<const MT> >::Type
   submatrix( const DenseMatrix<MT,SO>& dm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return DenseSubmatrix<const MT>( ~dm, row, column, m, n );
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL RESTRUCTURING OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of another dense submatrix.
// \ingroup views
//
// \param dm The constant dense submatrix
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the other dense submatrix.
//
// This function returns an expression representing the specified submatrix of the given
// dense submatrix.
*/
template< typename MT  // Type of the dense submatrix
        , bool SO >    // Storage order
inline DenseSubmatrix<MT,SO>
   submatrix( const DenseSubmatrix<MT,SO>& dm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return DenseSubmatrix<MT,SO>( dm.matrix_, dm.row_ + row, dm.column_ + column, m, n );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix/matrix addition.
// \ingroup views
//
// \param sm The constant matrix/matrix addition.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the addition.
//
// This function returns an expression representing the specified submatrix of the given
// matrix/matrix addition.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatMatAddExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return submatrix( (~sm).leftOperand() , row, column, m, n ) +
          submatrix( (~sm).rightOperand(), row, column, m, n );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix/matrix subtraction.
// \ingroup views
//
// \param sm The constant matrix/matrix subtraction.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the subtraction.
//
// This function returns an expression representing the specified submatrix of the given
// matrix/matrix subtraction.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatMatSubExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return submatrix( (~sm).leftOperand() , row, column, m, n ) -
          submatrix( (~sm).rightOperand(), row, column, m, n );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix/matrix multiplication.
// \ingroup views
//
// \param sm The constant matrix/matrix multiplication.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the multiplication.
//
// This function returns an expression representing the specified submatrix of the given
// matrix/matrix multiplication.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatMatMultExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   typename MT::LeftOperand  left ( (~sm).leftOperand()  );
   typename MT::RightOperand right( (~sm).rightOperand() );

   return submatrix( left, row, 0UL, m, left.columns() ) * submatrix( right, 0UL, column, right.rows(), n );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given outer product.
// \ingroup views
//
// \param sm The constant outer product.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the outer product.
//
// This function returns an expression representing the specified submatrix of the given
// outer product.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsVecTVecMultExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return subvector( (~sm).leftOperand(), row, m ) * subvector( (~sm).rightOperand(), column, n );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix/scalar multiplication.
// \ingroup views
//
// \param sm The constant matrix/scalar multiplication.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the multiplication.
//
// This function returns an expression representing the specified submatrix of the given
// matrix/scalar multiplication.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatScalarMultExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return submatrix( (~sm).leftOperand(), row, column, m, n ) * (~sm).rightOperand();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix/scalar division.
// \ingroup views
//
// \param sm The constant matrix/scalar division.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the division.
//
// This function returns an expression representing the specified submatrix of the given
// matrix/scalar division.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatScalarDivExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return submatrix( (~sm).leftOperand(), row, column, m, n ) / (~sm).rightOperand();
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix abs operation.
// \ingroup views
//
// \param sv The constant matrix abs operation.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the abs operation.
//
// This function returns an expression representing the specified submatrix of the given matrix
// abs operation.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatAbsExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return abs( submatrix( (~sm).operand(), row, column, m, n ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix evaluation operation.
// \ingroup views
//
// \param sv The constant matrix evaluation operation.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the evaluation operation.
//
// This function returns an expression representing the specified submatrix of the given matrix
// evaluation operation.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatEvalExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return eval( submatrix( (~sm).operand(), row, column, m, n ) );
}
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Creating a view on a specific submatrix of the given matrix transpose operation.
// \ingroup views
//
// \param sv The constant matrix transpose operation.
// \param row The index of the first row of the submatrix.
// \param column The index of the first column of the submatrix.
// \param m The number of rows of the submatrix.
// \param n The number of columns of the submatrix.
// \return View on the specified submatrix of the transpose operation.
//
// This function returns an expression representing the specified submatrix of the given matrix
// transpose operation.
*/
template< typename MT  // Type of the dense matrix
        , bool SO >    // Storage order
inline typename EnableIf< IsMatTransExpr<MT>, typename SubmatrixExprTrait<MT>::Type >::Type
   submatrix( const DenseMatrix<MT,SO>& sm, size_t row, size_t column, size_t m, size_t n )
{
   BLAZE_FUNCTION_TRACE;

   return trans( submatrix( (~sm).operand(), column, row, n, m ) );
}
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  SUBMATRIXTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool SO >
struct SubmatrixTrait< DenseSubmatrix<MT,SO> >
{
   typedef typename SubmatrixTrait< typename DenseSubmatrix<MT,SO>::ResultType >::Type  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ROWTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool SO >
struct RowTrait< DenseSubmatrix<MT,SO> >
{
   typedef typename RowTrait< typename DenseSubmatrix<MT,SO>::ResultType >::Type  Type;
};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  COLUMNTRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT, bool SO >
struct ColumnTrait< DenseSubmatrix<MT,SO> >
{
   typedef typename ColumnTrait< typename DenseSubmatrix<MT,SO>::ResultType >::Type  Type;
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
