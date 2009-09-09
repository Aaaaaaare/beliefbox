/* -*- Mode: c++ -*- */
/* VER: $Id: MathFunctions.h,v 1.2 2006/11/06 15:48:53 cdimitrakakis Exp cdimitrakakis $ */
// copyright (c) 2006 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include "real.h"
#include "Vector.h"
#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>

/// \brief An n-by-m dimensional matrix.
class Matrix
{
public:
    /// Check bounds?
    enum BoundsCheckingStatus {NO_CHECK_BOUNDS=0, CHECK_BOUNDS=1};
    static Matrix Null(int rows, int columns, enum BoundsCheckingStatus check = NO_CHECK_BOUNDS);
    static Matrix Unity(int rows, int columns, enum BoundsCheckingStatus check = NO_CHECK_BOUNDS);

    Matrix (int rows = 1, int columns = 1,  enum BoundsCheckingStatus check = NO_CHECK_BOUNDS);
    Matrix (int rows, int columns, real* y, enum BoundsCheckingStatus check = NO_CHECK_BOUNDS);
    Matrix (const Vector& v, enum BoundsCheckingStatus check_ = NO_CHECK_BOUNDS);
    Matrix (const Matrix& rhs, bool clone = true);
    ~Matrix();
    void Resize(int rows_, int columns_);
    Matrix& operator= (const Matrix& rhs);
    bool operator== (const Matrix& rhs) const;
    bool operator!= (const Matrix& rhs) const;
    Matrix operator+ (const Matrix& rhs);
    Matrix& operator+= (const Matrix& rhs);
    Matrix operator- (const Matrix& rhs);
    Matrix& operator-= (const Matrix& rhs);
    Matrix& operator*= (const real& rhs);
    Matrix operator* (const Matrix& rhs);
    Matrix operator* (const real& rhs);
    bool isTriangular();
    bool isUpperTriangular();
    bool isLowerTriangular();
    real det();
    real ColumnSum(int c);
    real RowSum(int r);
    real compute_det_triangular();
    real gaussian_elimination_forward(real epsilon = 0.000001);
    std::vector<Matrix> LUDecomposition(real& determinant, real epsilon = 0.000001);
    void Transpose();
    Vector getColumn(int c);
    Vector getRow(int r);
    int Rows() const;
    int Columns() const;
    real& operator() (int i, int j);
    const real& operator() (int i, int j) const;
    void print(FILE* f);
    friend Matrix operator* (const real& lhs, const Matrix& rhs);
    friend Matrix operator* (const Vector& lhs, const Matrix& rhs);
protected:
    int rows; ///< number of rows in the matrix
    int columns; ///< number of columns in the matrix
    real* x; ///< data
    const enum BoundsCheckingStatus checking_bounds;
    bool transposed;
    bool clear_data;
    const real& qGet(int i, int j);
    void qSet(int i, int j, real v);
};

Matrix operator* (const real& lhs, const Matrix& rhs);
Matrix operator* (const Vector& lhs, const Matrix& rhs);
Matrix Transpose (const Matrix& rhs);

inline
void Matrix::Transpose()
{
    transposed = !transposed;
}

inline
int Matrix::Rows() const
{
    if (transposed) {
        return columns;
    }
    return rows;
}

inline
int Matrix::Columns() const
{
    if (transposed) {
        return rows;
    } 
    return columns;
}

inline
real& Matrix::operator() (int i, int j)
{
    if (transposed) {
        int tmp = i;
        i = j;
        j = tmp;
    }
                 
    if (checking_bounds) {
        if (i<0 || j<0 || i>=rows || j>=columns) {
	    fprintf(stderr, "bad access: (%d, %d) on a %d x %d matrix\n",
		    i, j, rows, columns);
	    fflush (stderr);
        throw std::out_of_range("matrix index out of range");
        }
    }
    return x[i*columns + j];
}

inline
const real& Matrix::operator() (int i, int j) const
{
    if (transposed) {
        int tmp = i;
        i = j;
        j = tmp;
    }
                 
    if (checking_bounds) {
        if (i<0 || j<0 || i>=rows || j>=columns) {
	    fprintf(stderr, "bad access: (%d, %d) on a %d x %d matrix\n",
		    i, j, rows, columns);
	    fflush (stderr);
            throw std::out_of_range("matrix index out of range");
        }
    }
    return x[i*columns + j];
}

inline
const real& Matrix::qGet(int i, int j)
{
    return x[i*columns + j];
}

inline
void Matrix::qSet(int i, int j, real v)
{
    x[i*columns + j] = v;
}

#endif
