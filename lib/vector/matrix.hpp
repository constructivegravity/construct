#pragma once

#include <common/error.hpp>
#include <vector/vector.hpp>

#include <sstream>

namespace Construction {
    namespace Vector {

        class DimensionsDoNotMatchException : public Exception {
        public:
            DimensionsDoNotMatchException() : Exception("The dimensions of the matrices do not match") { }
        };

        class CannotMultiplyMatricesException : public Exception {
        public:
            CannotMultiplyMatricesException() : Exception("Cannot multiply these matrices") { }
        };

        class Matrix {
        public:
            /**
                Constructor for a matrix

                \param n    Number of rows
                \param m    Number of columns
             */
            Matrix(unsigned n, unsigned m) : n(n), m(m) {
                data = new float[n*m];
            }

            /**
                Destructor of a matrix
             */
            ~Matrix() {
                if (data != nullptr) {
                    delete[] data;
                }
            }

            Matrix(std::initializer_list<Vector> list) {
                m = list.size();
                n = (*list.begin()).GetDimension();
                data = new float[n*m];

                unsigned i=0;
                for (auto& r : list) {
                    assert(r.GetDimension() == n);
                    for (int j=0; j<r.GetDimension(); j++) {
                        At(j,i) = r[j];
                    }
                    i++;
                }
            }

            Matrix(const std::vector<Vector>& list) {
                m = list.size();
                if (m == 0) return;

                n = list[0].GetDimension();
                data = new float[n*m];

                for (unsigned i=0; i<m; i++) {
                    assert(list[i].GetDimension() == n);
                    for (int j=0; j<n; j++) {
                        At(j,i) = list[i][j];
                    }
                }
            }

            Matrix(const Matrix& other) {
                n = other.n;
                m = other.m;

                data = new float[n*m];
                for (int i=0; i<n*m; i++) {
                    data[i] = other.data[i];
                }
            }

            Matrix(Matrix&& other) {
                n = other.n;
                m = other.m;

                data = other.data;
                other.data = nullptr;
            }
        private:
            void FreeData() {
                if (data != nullptr) {
                    delete[] data;
                }
            }
        public:
            Matrix& operator=(const Matrix& other) {
                FreeData();

                n = other.n;
                m = other.m;

                data = new float[n*m];
                for (int i=0; i<n*m; i++) {
                    data[i] = other.data[i];
                }
                return *this;
            }

            Matrix& operator=(Matrix&& other) {
                FreeData();

                n = other.n;
                m = other.m;

                data = other.data;
                other.data = nullptr;

                return *this;
            }
        public:
            inline unsigned GetNumberOfRows() const {
                return n;
            }

            inline unsigned GetNumberOfColumns() const {
                return m;
            }
        public:
            inline float& At(int i, int j) {
                if (i >= n || j >= m) throw OutOfBoundariesException();
                return data[m*i + j];
            }

            inline float At(int i, int j) const {
                if (i >= n || j >= m) throw OutOfBoundariesException();
                return data[m*i + j];
            }

            inline float& operator()(int i, int j) {
                if (i >= n || j >= m) throw OutOfBoundariesException();
                return data[m*i + j];
            }

            inline float operator()(int i, int j) const {
                if (i >= n || j >= m) throw OutOfBoundariesException();
                return data[m*i + j];
            }
        public:
            Vector GetRowVector(int i) const {
                if (i >= n) throw OutOfBoundariesException();

                Vector result(m);
                for (int j=0; j<m; j++) {
                    result[i] = At(i,j);
                }
                return result;
            }

            Vector GetColumnVector(int i) const {
                if (i >= m) throw OutOfBoundariesException();

                Vector result(n);
                for (int j=0; j<n; j++) {
                    result[j] = At(j,i);
                }
                return result;
            }
        public:
            Matrix Transposed() const {
                Matrix result(m,n);
                for (int i=0; i<m; i++) {
                    for (int j=0; j<n; j++) {
                        result(i,j) = At(j,i);
                    }
                }
                return result;
            }

            void Transpose() {
                // TODO: implement
            }
        public:
            bool operator==(const Matrix& other) const {
                if (m != other.m || n != other.n) return false;
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != other(i,j)) return false;
                    }
                }
                return true;
            }

            bool operator!=(const Matrix& other) const {
                if (m != other.m || n != other.n) return true;
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != other(i,j)) return true;
                    }
                }
                return false;
            }
        public:
            Matrix& operator+=(const Matrix& other) {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                for (int i=0; i<n*m; i++) {
                    data[i] += other.data[i];
                }
                return *this;
            }

            Matrix operator+(const Matrix& other) const {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                Matrix result = *this;

                for (int i=0; i<n*m; i++) {
                    result.data[i] += other.data[i];
                }
                return result;
            }

            Matrix& operator-=(const Matrix& other) {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                for (int i=0; i<n*m; i++) {
                    data[i] -= other.data[i];
                }
                return *this;
            }

            Matrix operator-(const Matrix& other) const {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                Matrix result = *this;

                for (int i=0; i<n*m; i++) {
                    result.data[i] -= other.data[i];
                }
                return result;
            }

            Matrix& operator*=(float c) {
                for (int i=0; i<m*n; i++) {
                    data[i] *= c;
                }
                return *this;
            }

            Matrix operator*(float c) const {
                Matrix result = *this;
                for (int i=0; i<m*n; i++) {
                    result.data[i] *= c;
                }
                return result;
            }

            Matrix operator*(const Matrix& other) const {
                if (m != other.n) throw CannotMultiplyMatricesException();
                Matrix result(n, other.m);

                for (int i=0; i<n; i++) {
                    for (int j=0; j<other.m; j++) {
                        for (int k=0; k<m; k++) {
                            result(i,j) = At(i,k)*other(k,j);
                        }
                    }
                }

                return result;
            }

            Vector operator*(const Vector& v) const {
                if (m != v.GetDimension()) throw OutOfBoundariesException();
                Vector result(n);
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        result[i] = At(i,j) * v[j];
                    }
                }
                return result;
            }

            Matrix& operator/=(float c) {
                c = 1.0/c;
                for (int i=0; i<m*n; i++) {
                    data[i] *= c;
                }
                return *this;
            }

            Matrix operator/(float c) const {
                c = 1.0/c;
                Matrix result = *this;
                for (int i=0; i<m*n; i++) {
                    result.data[i] *= c;
                }
                return result;
            }
        public:
            /**
                \brief Returns the row echelon form of the matrix
             */
            Matrix GetRowEchelonForm() const {
                Matrix result = *this;

                unsigned lead=0;
                for (unsigned r=0; r<result.GetNumberOfRows(); r++) {
                    if (lead >= result.GetNumberOfColumns()) return result;

                    unsigned i = r;
                    while (result(i, lead) == 0) {
                        i++;
                        if (result.GetNumberOfRows() <= i) {
                            i = r;
                            lead++;
                            if (lead >= result.GetNumberOfColumns()) return result;
                        }
                    }
                    result.SwapRows(i,r);

                    // Divide row r through M[r,lead]
                    if (result(r, lead) != 0) {
                        float x = result(r, lead);
                        for (int k=0; k<result.GetNumberOfColumns(); k++) result(r, k) /= x;
                    }

                    for (unsigned i=0; i<result.GetNumberOfRows(); i++) {
                        if (i != r) {
                            float x = result(i, lead);
                            for (unsigned k=0; k<result.GetNumberOfColumns(); k++) {
                                float y = result(r,k);
                                result(i,k) -= x*result(r,k);
                            }
                        }
                    }
                    lead++;
                }

                return result;
            }

            /**
                \brief Returns the row echelon form of the matrix
             */
            void ToRowEchelonForm() {
                unsigned lead=0;
                for (unsigned r=0; r<GetNumberOfRows(); r++) {
                    if (lead >= GetNumberOfColumns()) return;

                    unsigned i = r;
                    while (At(i, lead) == 0) {
                        i++;
                        if (GetNumberOfRows() <= i) {
                            i = r;
                            lead++;
                            if (lead >= GetNumberOfColumns()) return;
                        }
                    }
                    SwapRows(i,r);

                    // Divide row r through M[r,lead]
                    if (At(r, lead) != 0) {
                        float x = At(r, lead);
                        for (int k=0; k<GetNumberOfColumns(); k++) At(r, k) /= x;
                    }

                    for (unsigned i=0; i<GetNumberOfRows(); i++) {
                        if (i != r) {
                            float x = At(i, lead);
                            for (unsigned k=0; k<GetNumberOfColumns(); k++) {
                                float y = At(r,k);
                                At(i,k) -= x*At(r,k);
                            }
                        }
                    }
                    lead++;
                }
            }
        public:
            void SwapRows(unsigned i, unsigned j) {
                assert(i < n && j < n);
                if (i == j) return;

                for (int k=0; k<m; k++) {
                    float r = At(i,k);
                    At(i,k) = At(j,k);
                    At(j,k) = r;
                }
            }

            void SwapColumns(unsigned i, unsigned j) {
                assert(i < m && j < m);
                if (i == j) return;

                for (int k=0; k<n; k++) {
                    float r = At(k,i);
                    At(k,i) = At(k,j);
                    At(k,j) = r;
                }
            }
        public:
            friend std::ostream& operator<<(std::ostream& os, const Matrix& v) {
                os << "[";
                for (int i=0; i<v.n; i++) {
                    os << "[";
                    for (int j=0; j<v.m; j++) {
                        os << v(i,j);
                        if (j != v.m-1) os << ", ";
                    }
                    os << "]"; //<< std::endl;
                    if (i != v.n-1) os << ", " << std::endl;
                }
                os << "]";
                return os;
            }
        private:
            unsigned n;
            unsigned m;

            float* data;
        };

    }
}