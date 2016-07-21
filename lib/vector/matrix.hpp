#pragma once

#include <common/error.hpp>
#include <vector/vector.hpp>

#include <iomanip>
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

        class MatrixIndex {
        public:
            MatrixIndex(unsigned row, unsigned column) : row(row), column(column) { }

            MatrixIndex(const MatrixIndex& other) : row(other.row), column(other.column) { }
            MatrixIndex(MatrixIndex&& other) : row(other.row), column(other.column) { }
        public:
            MatrixIndex& operator=(const MatrixIndex& other) {
                row = other.row;
                column = other.column;
                return *this;
            }

            MatrixIndex& operator=(MatrixIndex&& other) {
                row = std::move(other.row);
                column = std::move(other.column);
                return *this;
            }
        public:
            bool operator<(const MatrixIndex& other) const {
                if (row < other.row) return true;
                else if (row > other.row) return false;
                return column < other.column;
            }

            bool operator<=(const MatrixIndex& other) const {
                if (row < other.row) return true;
                else if (row > other.row) return false;
                return column <= other.column;
            }

            bool operator>(const MatrixIndex& other) const {
                if (row > other.row) return true;
                else if (row < other.row) return false;
                return column > other.column;
            }

            bool operator>=(const MatrixIndex& other) const {
                if (row > other.row) return true;
                else if (row < other.row) return false;
                return column >= other.column;
            }

            bool operator==(const MatrixIndex& other) const {
                return row == other.row && column == other.column;
            }

            bool operator!=(const MatrixIndex& other) const {
                return (row != other.row) || (column != other.column);
            }
        private:
            unsigned row;
            unsigned column;
        };

        class Matrix {
        public:
            /**
                Constructor for a matrix

                \param n    Number of rows
                \param m    Number of columns
             */
            Matrix(unsigned n, unsigned m) : n(n), m(m) { }

            /**
                Destructor of a matrix
             */
            ~Matrix() { }

            Matrix(std::initializer_list<Vector> list) {
                m = list.size();
                n = (*list.begin()).GetDimension();

                unsigned i=0;
                for (auto& r : list) {
                    assert(r.GetDimension() == n);
                    for (int j=0; j<r.GetDimension(); j++) {
                        if (r[j] != 0) {
                            values.insert({ MatrixIndex(j,i), r[j] });
                        }
                    }
                    i++;
                }
            }

            Matrix(const std::vector<Vector>& list) {
                m = list.size();
                if (m == 0) return;

                n = list[0].GetDimension();

                for (unsigned i=0; i<m; i++) {
                    assert(list[i].GetDimension() == n);
                    for (int j=0; j<n; j++) {
                        if (list[i][j] != 0) {
                            values.insert({ MatrixIndex(j,i), list[i][j] });
                        }
                    }
                }
            }

            Matrix(const Matrix& other) : n(other.n), m(other.m), values(other.values) { }

            Matrix(Matrix&& other) : n(std::move(other.n)), m(std::move(other.m)), values(std::move(other.values)) { }
        public:
            Matrix& operator=(const Matrix& other) {
                n = other.n;
                m = other.m;
                values = other.values;
                return *this;
            }

            Matrix& operator=(Matrix&& other) {
                n = std::move(other.n);
                m = std::move(other.m);
                values = std::move(other.values);
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
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                return values[MatrixIndex(i,j)];
            }

            inline float At(int i, int j) const {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                auto it = values.find(MatrixIndex(i,j));
                if (it == values.end()) return 0;
                return it->second;
            }

            inline float& operator()(int i, int j) {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                return values[MatrixIndex(i,j)];
            }

            inline float operator()(int i, int j) const {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                auto it = values.find(MatrixIndex(i,j));
                if (it == values.end()) return 0;
                return it->second;
            }
        public:
            Vector GetRowVector(int i) const {
                if (i >= n) throw OutOfBoundariesException();

                Vector result(m);
                for (int j=0; j<m; j++) {
                    result[j] = At(i,j);
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
                        if (At(j,i) != 0) {
                            result(i,j) = At(j,i);
                        }
                    }
                }
                return result;
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
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        float z = other(i,j);
                        if (z != 0) At(i,j) += z;
                    }
                }
                return *this;
            }

            Matrix operator+(const Matrix& other) const {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                Matrix result (n, m);

                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        float x = At(i,j);
                        float y = other(i,j);
                        if (x != 0 || y != 0) result(n,m) = x + y;
                    }
                }
                return result;
            }

            Matrix& operator-=(const Matrix& other) {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        float z = other(i,j);
                        if (z != 0) At(i,j) -= z;
                    }
                }
                return *this;
            }

            Matrix operator-(const Matrix& other) const {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                Matrix result (n, m);

                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        float x = At(i,j);
                        float y = other(i,j);
                        if (x != 0 || y != 0) result(n,m) = x - y;
                    }
                }
                return result;
            }

            Matrix& operator*=(float c) {
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != 0) At(i,j) *= c;
                    }
                }
                return *this;
            }

            Matrix operator*(float c) const {
                Matrix result (n,m);
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != 0) result(i,j) = c * At(i,j);
                    }
                }
                return result;
            }

            Matrix operator*(const Matrix& other) const {
                if (m != other.n) throw CannotMultiplyMatricesException();
                Matrix result(n, other.m);

                for (int i=0; i<n; i++) {
                    for (int j=0; j<other.m; j++) {
                        for (int k=0; k<m; k++) {
                            float z = At(i,k)*other(k,j);
                            if (z != 0) result(i,j) = z;
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
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != 0) At(i,j) *= c;
                    }
                }
                return *this;
            }

            Matrix operator/(float c) const {
                c = 1.0/c;
                Matrix result (n,m);
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != 0) result(i,j) = c * At(i,j);
                    }
                }
                return result;
            }
        public:
            Matrix Resize(unsigned n, unsigned m) const {
                Matrix result (n, m);

                for (unsigned i=0; i<std::min(n, this->n); i++) {
                    for (unsigned j=0; j<std::min(m, this->m); j++) {
                        if (At(i,j) != 0) result(i,j) = At(i,j);
                    }
                }

                return result;
            }
        public:
            /**
                \brief Returns the reduced row echelon form of the matrix
             */
            Matrix GetRowEchelonForm() const {
                Matrix result = *this;

                // Find rows that are completely zero
                bool* zeroRow = new bool[result.GetNumberOfRows()];
                for (unsigned r=0; r<result.GetNumberOfColumns(); r++) {
                    bool isZero = true;
                    for (unsigned c=0; c<result.GetNumberOfColumns(); c++) {
                        if (result(r,c) != 0) {
                            isZero = false;
                            break;
                        }
                    }

                    if (isZero) zeroRow[r] = true;
                    else zeroRow[r] = false;
                }

                unsigned lead=0;
                for (unsigned r=0; r<result.GetNumberOfRows(); r++) {
                    if (lead >= result.GetNumberOfColumns()) return result;
                    if (zeroRow[r]) continue;

                    unsigned i = r;
                    while (result(i, lead) == 0) {
                        i++;
                        if (result.GetNumberOfRows() <= i && !zeroRow[i]) {
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
                // Find rows that are completely zero
                unsigned numRows = GetNumberOfRows();
                for (unsigned r=0; r<numRows; r++) {
                    bool isZero = true;
                    for (unsigned c=0; c<GetNumberOfColumns(); c++) {
                        if (At(r,c) != 0) {
                            isZero = false;
                            break;
                        }
                    }

                    // If row is zero
                    if (isZero) {
                        // swap to the end of the matrix
                        SwapRows(r,numRows-1);

                        // decrease the effective number of rows
                        numRows--;

                        // check that entry again
                        r--;
                    }
                }

                // Do magic
                unsigned lead=0;
                for (unsigned r=0; r<numRows; r++) {
                    if (lead >= GetNumberOfColumns()) return;

                    unsigned i = r;
                    while (At(i, lead) == 0) {
                        i++;
                        if (numRows <= i) {
                            i = r;
                            lead++;
                            if (lead >= GetNumberOfColumns()) return;
                        }
                    }
                    SwapRows(i,r);

                    // Divide row r through M[r,lead]
                    if (At(r, lead) != 0) {
                        float x = At(r, lead);
                        for (int k=0; k<GetNumberOfColumns(); k++) {
                            float z = At(r,k);
                            if (z != 0) At(r,k) = z / x;
                        }
                    }

                    for (unsigned i=0; i<numRows; i++) {
                        if (i != r) {
                            float x = At(i, lead);
                            for (unsigned k=0; k<GetNumberOfColumns(); k++) {
                                float y = x*At(r,k);
                                if (y != 0) At(i,k) -= y;
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
                    auto it1 = values.find(MatrixIndex(i,k));
                    float value1 = (it1 != values.end()) ? it1->second : 0;
                    if (it1 != values.end()) values.erase(it1);

                    auto it2 = values.find(MatrixIndex(j,k));
                    float value2 = (it2 != values.end()) ? it2->second : 0;
                    if (it2 != values.end()) values.erase(it2);

                    if (value1 != 0) values.insert({ MatrixIndex(j,k), value1 });
                    if (value2 != 0) values.insert({ MatrixIndex(i,k), value2 });                    
                }
            }

            void SwapColumns(unsigned i, unsigned j) {
                assert(i < m && j < m);
                if (i == j) return;

                for (int k=0; k<n; k++) {
                    auto it1 = values.find(MatrixIndex(k,i));
                    float value1 = (it1 != values.end()) ? it1->second : 0;
                    if (it1 != values.end()) values.erase(it1);

                    auto it2 = values.find(MatrixIndex(k,j));
                    float value2 = (it2 != values.end()) ? it2->second : 0;
                    if (it2 != values.end()) values.erase(it2);

                    if (value1 != 0) values.insert({ MatrixIndex(k,j), value1 });
                    if (value2 != 0) values.insert({ MatrixIndex(k,i), value2 });
                }
            }
        public:
            friend std::ostream& operator<<(std::ostream& os, const Matrix& v) {
                os << "[";
                for (int i=0; i<v.n; i++) {
                    if (i > 0) os << " ";
                    os << "[";
                    for (int j=0; j<v.m; j++) {
                        os << std::setw(3) << std::setprecision(1) << v(i,j);
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

            std::map<MatrixIndex, float> values;
        };

    }
}