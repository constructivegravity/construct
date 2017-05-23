#pragma once

#include <common/logger.hpp>
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

        template<typename T>
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

            Matrix(std::initializer_list<Vector<T>> list) {
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

            Matrix(const std::vector<Vector<T>>& list) {
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
            inline T& At(int i, int j) {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                return values[MatrixIndex(i,j)];
            }

            inline T At(int i, int j) const {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                auto it = values.find(MatrixIndex(i,j));
                if (it == values.end()) return T(0);
                return it->second;
            }

            inline T& operator()(int i, int j) {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                return values[MatrixIndex(i,j)];
            }

            inline T operator()(int i, int j) const {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                auto it = values.find(MatrixIndex(i,j));
                if (it == values.end()) return T(0);
                return it->second;
            }

            inline void Set(int i, int j, T value) {
                if (i >= n || j >= m || i < 0 || j < 0) throw OutOfBoundariesException();
                auto it = values.find(MatrixIndex(i,j));

                // If the new value is zero, delete the entry if necessary
                if (value == T(0)) {
                    if (it == values.end()) return;
                    values.erase(it);
                } else {
                    if (it == values.end()) values.insert({ MatrixIndex(i,j), value });
                    else it->second = value;
                }
            }
        public:
            Vector<T> GetRowVector(int i) const {
                if (i >= n) throw OutOfBoundariesException();

                Vector<T> result(m);
                for (int j=0; j<m; j++) {
                    result[j] = At(i,j);
                }
                return result;
            }

            Vector<T> GetColumnVector(int i) const {
                if (i >= m) throw OutOfBoundariesException();

                Vector<T> result(n);
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
                        double z = other(i,j);
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
                        double x = At(i,j);
                        double y = other(i,j);
                        if (x != T(0) || y != T(0)) result(n,m) = x + y;
                    }
                }
                return result;
            }

            Matrix& operator-=(const Matrix& other) {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        double z = other(i,j);
                        if (z != T(0)) At(i,j) -= z;
                    }
                }
                return *this;
            }

            Matrix operator-(const Matrix& other) const {
                if (other.n != n || other.m != m) throw DimensionsDoNotMatchException();
                Matrix result (n, m);

                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        double x = At(i,j);
                        double y = other(i,j);
                        if (x != T(0) || y != T(0)) result(n,m) = x - y;
                    }
                }
                return result;
            }

            Matrix& operator*=(double c) {
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != T(0)) At(i,j) *= c;
                    }
                }
                return *this;
            }

            Matrix operator*(double c) const {
                Matrix result (n,m);
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != T(0)) result(i,j) = c * At(i,j);
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
                            double z = At(i,k)*other(k,j);
                            if (z != T(0)) result(i,j) = z;
                        }
                    }
                }

                return result;
            }

            Vector<T> operator*(const Vector<T>& v) const {
                if (m != v.GetDimension()) throw OutOfBoundariesException();
                Vector<T> result(n);
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        result[i] = At(i,j) * v[j];
                    }
                }
                return result;
            }

            Matrix& operator/=(T c) {
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != T(0)) At(i,j) /= c;
                    }
                }
                return *this;
            }

            Matrix operator/(T c) const {
                Matrix result (n,m);
                for (int i=0; i<n; i++) {
                    for (int j=0; j<m; j++) {
                        if (At(i,j) != T(0)) result(i,j) = At(i,j) / c;
                    }
                }
                return result;
            }
        public:
            Matrix Resize(unsigned n, unsigned m) const {
                Matrix result (n, m);

                for (unsigned i=0; i<std::min(n, this->n); i++) {
                    for (unsigned j=0; j<std::min(m, this->m); j++) {
                        if (At(i,j) != T(0)) result(i,j) = At(i,j);
                    }
                }

                return result;
            }

            double GetDensity() const {
                size_t value = 0;
                for (int r=0; r<GetNumberOfRows(); r++) {
                    for (int c=0; c<GetNumberOfColumns(); c++) {
                        if (At(r,c) != T(0)) value++;
                    }
                }

                return (static_cast<double>(value) / GetNumberOfRows()) / GetNumberOfColumns();
            }
        public:
            /**
                \brief Returns the reduced row echelon form of the matrix
             */
            Matrix GetRowEchelonForm() const {
                Matrix result = *this;

                bool* zeroRow = new bool[result.GetNumberOfRows()];
                for (unsigned r=0; r<result.GetNumberOfColumns(); r++) {
                    bool isZero = true;
                    for (unsigned c=0; c<result.GetNumberOfColumns(); c++) {
                        if (result(r,c) != T(0)) {
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
                    while (result(i, lead) == T(0)) {
                        i++;
                        if (result.GetNumberOfRows() <= i && !zeroRow[i]) {
                            i = r;
                            lead++;
                            if (lead >= result.GetNumberOfColumns()) return result;
                        }
                    }
                    result.SwapRows(i,r);

                    // Divide row r through M[r,lead]
                    if (result(r, lead) != T(0)) {
                        T x = result(r, lead);
                        for (int k=0; k<result.GetNumberOfColumns(); k++) result(r, k) /= x;
                    }

                    for (unsigned i=0; i<result.GetNumberOfRows(); i++) {
                        if (i != r) {
                            T x = result(i, lead);
                            for (unsigned k=0; k<result.GetNumberOfColumns(); k++) {
                                T y = result(r,k);
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
                // Get the number of rows
                unsigned numRows = GetNumberOfRows();

                // Find rows that are completely zero
                for (unsigned r=0; r<numRows; ++r) {
                    bool isZero = true;
                    for (unsigned c=0; c<GetNumberOfColumns(); c++) {
                        if (At(r,c) != T(0)) {
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
                for (unsigned r=0; r<numRows; ++r) {
                    if (lead >= GetNumberOfColumns()) return;

                    // Search for first line that has a non-zero entry as pivot element
                    unsigned i = r;
                    while (At(i, lead) == T(0)) {
                        ++i;
                        if (numRows <= i) {
                            i = r;
                            ++lead;
                            if (lead >= GetNumberOfColumns()) return;
                        }
                    }
                    SwapRows(i,r);

                    // Divide row r through M[r,lead]
                    if (At(r, lead) != T(0)) {
                        T x = At(r, lead);

                        for (int k=0; k<GetNumberOfColumns(); k++) {
                            T z = At(r,k) / x;

                            // Update the value, remove zeros from the memory
                            // automatically
                            Set(r,k, z);
                        }
                    }

                    for (unsigned i=0; i<numRows; i++) {
                        if (i != r) {
                            T x = At(i, lead);
                            for (unsigned k=0; k<GetNumberOfColumns(); k++) {
                                T y = x*At(r,k);

                                // Update the value, remove zeros from the memory
                                // automatically
                                Set(i,k, At(i,k) - y);
                            }
                        }
                    }
                    lead++;

                    Construction::Logger::Debug("Gauss step: ", ToString(false));
                }
            }
        public:
            void SwapRows(unsigned i, unsigned j) {
                assert(i < n && j < n);
                if (i == j) return;

                for (int k=0; k<m; k++) {
                    auto it1 = values.find(MatrixIndex(i,k));
                    T value1 = (it1 != values.end()) ? it1->second : T(0);
                    if (it1 != values.end()) values.erase(it1);

                    auto it2 = values.find(MatrixIndex(j,k));
                    T value2 = (it2 != values.end()) ? it2->second : T(0);
                    if (it2 != values.end()) values.erase(it2);

                    if (value1 != T(0)) values.insert({ MatrixIndex(j,k), value1 });
                    if (value2 != T(0)) values.insert({ MatrixIndex(i,k), value2 });
                }
            }

            void SwapColumns(unsigned i, unsigned j) {
                assert(i < m && j < m);
                if (i == j) return;

                for (int k=0; k<n; k++) {
                    auto it1 = values.find(MatrixIndex(k,i));
                    double value1 = (it1 != values.end()) ? it1->second : T(0);
                    if (it1 != values.end()) values.erase(it1);

                    auto it2 = values.find(MatrixIndex(k,j));
                    double value2 = (it2 != values.end()) ? it2->second : T(0);
                    if (it2 != values.end()) values.erase(it2);

                    if (value1 != T(0)) values.insert({ MatrixIndex(k,j), value1 });
                    if (value2 != T(0)) values.insert({ MatrixIndex(k,i), value2 });
                }
            }
        public:
            std::string ToString(bool includeEmptyRows=true) const {
                std::stringstream os;

                os << "[";
                for (int i=0; i<n; i++) {
                    std::stringstream is;
                    bool isEmpty=true;

                    if (i > 0) is << " ";
                    is << "[";
                    for (int j=0; j<m; j++) {
                        auto v = At(i,j);
                        if (v != T(0)) isEmpty = false;

                        is << std::setw(3) << std::setprecision(2) << At(i,j);
                        if (j != m-1) is << ", ";
                    }
                    is << "]"; //<< std::endl;
                    if (i != n-1) is << ", " << std::endl;

                    if (includeEmptyRows || !isEmpty) {
                        os << is.str();
                    }
                }
                os << "]";

                return os.str();
            }

            friend std::ostream& operator<<(std::ostream& os, const Matrix& v) {
                os << v.ToString();
                return os;
            }
        private:
            unsigned n;
            unsigned m;

            std::map<MatrixIndex, T> values;
        };

    }
}
