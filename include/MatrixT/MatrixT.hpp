#ifndef GENERIC_MATRIX_TEMPLATE_H
#define GENERIC_MATRIX_TEMPLATE_H

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

template<typename T>
class Matrix {

    private:

        // our main array
        std::vector<T> container;

        // our permutation vector
        std::vector<T> permutation;

        // rows and collumns
        unsigned int row, col, total_size;

        // matrix status
        bool empty;

        // cache determinant
        T determinant;

        // flag to indicate that the determinant was calculated and is up to date
        bool updated_det;

        // get the container reference
        std::vector<T> getContainerRef(void) const {
            return container;
        }

    public:

        // the default constructor
        Matrix() : container(0), row(0), col(0), total_size(0), empty(true), determinant(0), updated_det(false) {}

        // basic constructor
        Matrix(unsigned int size) : container(size*size), row(size), col(size), total_size(size*size), empty(true), determinant(0), updated_det(false) {}

        // another constructor
        Matrix(unsigned int r, unsigned int c) : container(r*c), row(r), col(c), total_size(r*c), empty(true), determinant(0), updated_det(false) {}

        // another constructor
        Matrix(unsigned int r, unsigned int c, T value) : container(r*c, value), row(r), col(c), total_size(r*c), empty(true), determinant(0), updated_det(false) {}

        // copy constructor
        Matrix(const Matrix<T> &m) :
            container(m.getContainerRef()),
            row(m.row), col(m.col),
            total_size(m.total_size),
            empty(m.empty),
            determinant(m.determinant),
            updated_det(m.updated_det) {}

        // is it a square matrix?
        bool isSquareMatrix() {

            // return the comparison
            return row == col && row*col == total_size;

        }

        // is an empty matrix?
        bool isEmpty() {

            if (empty) {

                // its really empty
                return true;

            } else {

                // a helper flag
                bool flag = true;
                // verify the entire matrix
                for (int i = 0; i < total_size; i++) {
                    if ((T) 0 != container[i]) {
                        flag = false;
                    }
                }

                // and updates the empty flag
                // simple caching
                empty = flag;

                return empty;
            }
        }

        // same dimensions?
        bool sameDimensions(const Matrix<T> &m) {

            // return the comparisons
            return row == m.row && col == m.col && total_size == m.total_size;

        }

        // get an element at a given position
        // int index
        T at(unsigned int i, unsigned int j) {

            // verify bound rimits
            if (i >= row || j >= col) {
                throw std::out_of_range("Invalid indexes");
            }

            // get the element
            return container[i*row + j];

        }

        // set an element at a given position
        void set(unsigned int i, unsigned j, T value) {

            // verify bound rimits
            if (i >= row || j >= col) {
                throw std::out_of_range("Invalid indexes");
            }

            // assign the value
            container[i*row + j] = value;

            // set the determinant update flag to false
            updated_det = false;

        }

        // + operator overloading
        Matrix operator+(const Matrix<T> &m) {

            // verify matrix sizes
            if (total_size != m.total_size) {
                throw std::exception("Incompatible sizes!");
            }

            // creates a new matrix
            Matrix<T> nm(m);

            // sum the matrices
            for (int i = 0; i < total_size; i++) {

                // update the new matrix
                nm.container[i] += container[i];

            }

            return nm;

        }

        // * operator overloading
        Matrix operator*(const Matrix<T> &m) {

            // verify matrix sizes
            if (col == m.row) {

                // creates a new Matrix
                Matrix<T> nm(row, m.col);

                // temp index
                unsigned int index;

                // multiply both matrices
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < m.col; j++) {

                        // casting zero to T type
                        nm.container[i*row + j] = (T) 0;

                        // temporary helper
                        index = i*row + j;

                        for (int k = 0; k < m.col; k++) {

                            // ijk algorithm, Strassen?
                            nm.container[index] += container[i*row + k]*m.container[k*row + j];

                        }
                    }
                }

                // return the new matrix
                return nm;

            } else {

                // throw a generic exception
                throw "Incompatible sizes: A.col != B.row!";

            }

        }

        // = operator overloading
        void operator=(const Matrix<T> &m) {

            // copy the commom values
            row = m.row;
            col = m.col;
            total_size = m.total_size;
            empty = m.empty;
            determinant = m.determinant;
            updated_det = m.updated_det;

            // copy the values
            container = m.container;

        }

        // add a given matrix to this one
        void add(const Matrix<T> &m) {

            // verify matrix sizes
            if (sameDimensions(m)) {

                // sum the matrices
                for (int i = 0; i < total_size; i++) {

                    // update the new matrix
                    container[i] += m.container[i];

                }

                // set the determinant update flag to false
                updated_det = false;

            } else {

                // throw a generic exception
                throw "Incompatible sizes: A.row != B.row || A.col != B.col!";
            }

        }

        // multiply a given matrix to this one
        // only defined for some valid dimensions
        void mult(const Matrix<T> &m) {

            // verify if both matrices have the appropriated dimensions - particular case
            if (col == m.row && col == m.col) {

                // temp index
                unsigned int index;

                // get the correct multiplication
                for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col; j++) {

                        // temporary index
                        index = i*row + j;

                        for (int k = 0; k < col; k++) {

                            // ijk algorithm, Strassen?
                            container[index] += container[i*row + k]*m.container[k*row + j];
                        }
                    }
                }

                // set the determinant update flag to false
                updated_det = false;

            } else {

                // return a generic exception
                throw "Incompatible dimensions. A.col != B.row || A.col != B.col!";
            }
        }

        // matrix transpose
        Matrix transpose() {

            // creates a new matrix with swapped row and col
            Matrix<T> nm(col, row);

            // copy the elements in an appropriated order
            for (int i = 0; i < nm.row; i++) {
                for (int j = 0; j < nm.col; j++) {

                    // copy the element
                    nm.container[i*nm.row + j] = container[j*row + i];

                }
            }

            // set the determinant flag to false
            nm.updated_det = false;

            // return the new matrix
            return nm;
        }

        // matrix self transpose - caution
        void selfTranspose() {

            // transpose
            *this = transpose();

        }

        // determinant
        T det() {

            if (isSquareMatrix()) {

                // verify if we have a valid determinant
                if (updated_det) {

                    // return the valid determinant
                    return determinant;

                } else {

                    // let's update
                    Matrix<T> lu(*this);

                    // update the determinant value
                    determinant = lu.luDec();

                    // determinant update flag
                    updated_det = true;

                    return determinant;
                }

            } else {

                // throw an generic exception
                throw std::exception("The current matrix is not a square one. So no determinants!");

            }
        }

        // inverse
        Matrix inverse() {

            // creates a new matrix
            Matrix<T> nm(row, col);

            if (isSquareMatrix() && (T) 0 != det()) {


            }

            return nm;

        }

        // a simple method just to fill the matrix with an sequence
        void seq(int v, int step) {

            // fill the entire array with the sequence PA
            for (int i = v; i < total_size; i += step) {

                // assign the sequence number
                container[i] = (T) i;

            }

        }

        // LUP decomposition
        std::vector<T> dec() {

            // vector[0] is the LU matrix
            // vector [1] is the P matrix
            std::vector<T> lup(0);

            // verify if we have a square matrix
            if (isSquareMatrix()) {

                // create a new LU matrix
                Matrix<T> lu(*this);

                // creates a vector to indicate the permutation
                Matrix<T> p(row, 1);

                // set the row sequence
                p.seq(0, 1);

                // LUP decomposition
                determinant = lu->luDec(p);

                // update the determinant flag
                updated_det = true;

                // add the lu and p matrices to our vector
                lup.push_back(lu);
                lup.push_back(p);

            }

            // return the LU matrix
            return lup;
        }

        // own decomposition
        // another method, just to avoid unwanted changes
        T luDec(Matrix<unsigned int> &perm) {

            // temporary helpers
            T r;

            // consider the first element as the greater one
            T max, tmp;

            // the pivot rowe index
            unsigned int p, diag;

            // set the determinant to 1
            determinant = (T) 1.0;

            // rowe iterators
            typename std::vector<T>::iterator rit1, rit2;

            // rets do the decomposition
            for (int i = 0; i < row - 1; i++) {

                // get the pivot rowe index
                p = i;

                // the diagonal
                diag = i*row + i;

                // get the max element
                 max = (T) (std::fabs((double) container[i*row + i]));

                 // verify at the current collumn if there's another better pivot element
                for (int m = i + 1; m < row; m++) {

                    // get the new max element
                    tmp = (T) (std::fabs((double) container[m*row + m]));

                    // verify the max element
                    if (tmp > max) {

                        // get the new max element rowe
                        p = m;

                        // update the max element
                        max = tmp;

                    }

                }

                // verify if we need to swap rowes
                if (p != i) {

                    // iterators
                    rit1 = &container[i*row];
                    rit2 = &container[p*row];

                    // swap rowe p and i
                    for (int r = 0; r < col; r++) {

                        // swap element
                        tmp = *rit1;
                        *rit1 = *rit2;
                        *rit2 = tmp;

                        // increase pointers
                        rit1++;
                        rit2++;

                    }

                    // updates the determinant
                    det *= -1;

                    // set the permutation
                    int swapIndex = (int) perm.at(i,0);
                    perm.set(i,0, perm.at(p, 0));
                    perm.set(p, 0, swapIndex);

                }

                // gauss substitution
                for (int j = i + 1; j < row; j++) {

                    // get the gauss element
                    r = container[j*row + i]/container[diag];

                    // save the coef
                    container[j*row + i] = r;

                    // update the entire j rowe
                    rit1 = &container[i*row + (i+1)];
                    rit2 = &container[j*row + (i+1)];

                    // update
                    *rit2 -= (*rit1)*r;

                    // update the entire row
                    for (int k = i + 1; k < col; k++) {

                        // update the element
                        *rit2 -= (*rit1)*r;
                        // increment iterators
                        rit1++;
                        rit2++;

                    }

                }

                // updates the determinant
                determinant *= container[diag];
            }

            // update the determinant update flag
            updated_det = true;

            // return
            return true;

        }

        // own decomposition
        // another method, just to avoid unwanted changes and now without the permutation matrix
        T luDec() {

            // verify if it's a square matrix
            if (!isSquareMatrix()) {

                // it's not a square matrix, so returns false
                return false;

            }

            // temporary helpers
            T r;

            // helpers, max element as pivot and a temp
            T max, tmp;

            // the pivot row index
            unsigned int p, diag;

            // set the determinant to 1
            determinant = (T) 1.0;

            // rowe iterators
            typename std::vector<T>::iterator rit1, rit2;

            // rets do the decomposition
            for (int i = 0; i < row - 1; i++) {

                // get the pivot rowe index
                p = i;

                // the diagonal
                diag = i*row + i;

                // get the max element
                 max = (T) (std::fabs((double) container[i*row + i]));

                 // verify at the current collumn if there's another better pivot element
                for (int m = i + 1; m < row; m++) {

                    // get the new max element
                    tmp = (T) (std::fabs((double) container[m*row + m]));

                    // verify the max element
                    if (tmp > max) {

                        // get the new max element rowe
                        p = m;

                        // update the max element
                        max = tmp;

                    }

                }

                // verify if we need to swap rowes
                if (p != i) {

                    // iterators
                    rit1 = &container[i*row];
                    rit2 = &container[p*row];

                    // swap rowe p and i
                    for (int r = 0; r < col; r++) {

                        // swap element
                        tmp = *rit1;
                        *rit1 = *rit2;
                        *rit2 = tmp;

                        // increase pointers
                        rit1++;
                        rit2++;

                    }

                    // updates the determinant
                    det *= -1;

                }

                // gauss substitution
                for (int j = i + 1; j < row; j++) {

                    // get the gauss element
                    r = container[j*row + i]/container[diag];

                    // save the coef
                    container[j*row + i] = r;

                    // update the entire j rowe
                    rit1 = &container[i*row + (i+1)];
                    rit2 = &container[j*row + (i+1)];

                    // update
                    *rit2 -= (*rit1)*r;

                    // update the entire row
                    for (int k = i + 1; k < col; k++) {

                        // update the element
                        *rit2 -= (*rit1)*r;
                        // increment iterators
                        rit1++;
                        rit2++;

                    }

                }

                // updates the determinant
                determinant *= container[diag];
            }

            // update the determinant update flag
            updated_det = true;

            // return
            return determinant;

        }

        //
        void show() {

            // iterates over the entire matrix and prints each element
            for(int i = 0; i < row; i++) {
                std::cout << std::endl << "| ";
                for (int j = 0; j < col; j++) {
                    std::cout << container[i*row + j] << " ";
                }
                std::cout << "|" << std::endl;
            }

        }

};

#endif
