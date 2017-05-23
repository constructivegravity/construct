#pragma once

#include <cmath>

#include <iostream>

#include <common/error.hpp>

namespace Construction {
    namespace Vector {

        class IncompatibleDimensionsException : public Exception {
        public:
            IncompatibleDimensionsException() : Exception("The dimensions of the vectors are incompatible") { }
        };

        class OutOfBoundariesException : public Exception {
        public:
            OutOfBoundariesException() : Exception("The access was out of the range of the vector") { }
        };

        /**
            \class Vector

            \brief Vector for numerical vectors

            Although an abstract vector can be specified with the help
            of the Tensor notion, it is often required for numerical
            calculations to have an efficient implementation without
            the overhead of the index structures and evaluation lambdas.

            Since we cannot give the dimension of the vector at runtime
            we cannot employ template based optimizations and need to
            implement a dynamical version.
         */
        template<typename T>
        class Vector {
        public:
            Vector(size_t dimension) {
                this->dimension = dimension;
                data = (T*) std::calloc(dimension, sizeof(T));
            }

            ~Vector() {
                if (data != nullptr) {
                    std::free(data);
                }
            }

            Vector(std::initializer_list<T> list) {
                dimension = list.size();
                data = (T*) std::calloc(dimension, sizeof(T));

                int i=0;
                for (auto& e : list) {
                    data[i] = e;
                    ++i;
                }
            }

            /**
                Copy constructor
             */
            Vector(const Vector& other) {
                dimension = other.GetDimension();

                data = (T*) std::calloc(dimension, sizeof(T));
                for (int i=0; i<dimension; i++) {
                    data[i] = other.data[i];
                }
            }

            /**
                Move constructor
             */
            Vector(Vector&& other) {
                data = other.data;
                other.data = nullptr;
            }
        public:
            Vector& operator=(const Vector& other) {
                std::free(data);

                size_t dim = other.GetDimension();
                data = (T*) std::calloc(dimension, sizeof(T));
                for (int i=0; i<dim; i++) {
                    data[i] = other.data[i];
                }

                return *this;
            }

            Vector& operator=(Vector&& other) {
                std::free(data);
                data = other.data;
                other.data = nullptr;
                return *this;
            }
        public:
            inline size_t GetDimension() const {
                return dimension;
            }
        public:
            T At(unsigned i) const {
                if (i >= GetDimension()) throw OutOfBoundariesException();
                return data[i];
            }

            T& At(unsigned i) {
                if (i >= GetDimension()) throw OutOfBoundariesException();
                return data[i];
            }

            T& operator[](unsigned i) {
                if (i >= GetDimension()) throw OutOfBoundariesException();
                return data[i];
            }

            T operator[](unsigned i) const {
                if (i >= GetDimension()) throw OutOfBoundariesException();
                return data[i];
            }
        public:
            bool operator==(const Vector& other) const {
                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                for (int i=0; i<dim; i++) {
                    if (data[i] != other.data[i]) return false;
                }
                return true;
            }

            bool operator!=(const Vector& other) const {
                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                for (int i=0; i<dim; i++) {
                    if (data[i] != other.data[i]) return true;
                }
                return false;
            }

            Vector& operator+=(const Vector& other) {
                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                for (int i=0; i<dim; i++) {
                    data[i] += other.data[i];
                }

                return *this;
            }

            Vector operator+(const Vector& other) const {
                Vector result = *this;

                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                for (int i=0; i<dim; i++) {
                    result.data[i] += other.data[i];
                }

                return result;
            }

            Vector& operator-=(const Vector& other) {
                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                for (int i=0; i<dim; i++) {
                    data[i] -= other.data[i];
                }

                return *this;
            }

            Vector operator-(const Vector& other) const {
                Vector result = *this;

                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                for (int i=0; i<dim; i++) {
                    result.data[i] -= other.data[i];
                }

                return result;
            }

            Vector operator-() const {
                Vector result = *this;

                for (int i=0; i<GetDimension(); i++) {
                    result.data[i] = -data[i];
                }

                return result;
            }

            T operator*(const Vector& other) const {
                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                T result;
                for (int i=0; i<dim; i++) {
                    result += data[i]*other.data[i];
                }
                return result;
            }

            Vector& operator*=(T c) {
                for (int i=0; i<GetDimension(); i++) {
                    data[i] *= c;
                }
                return *this;
            }

            Vector operator*(T c) const {
                Vector result = *this;
                for (int i=0; i<GetDimension(); i++) {
                    result.data[i] *= c;
                }
                return result;
            }

            inline friend Vector operator*(T c, const Vector& v) {
                return v*c;
            }

            Vector& operator/=(T c) {
                for (int i=0; i<GetDimension(); i++) {
                    data[i] /= c;
                }
                return *this;
            }

            Vector operator/(T c) const {
                Vector result = *this;
                for (int i=0; i<GetDimension(); i++) {
                    result.data[i] /= c;
                }
                return result;
            }
        public:
            bool IsZero() const {
                for (int i=0; i<GetDimension(); i++) {
                    if (data[i] != T(0)) return false; // Assuming that T(0) is the default value
                }
                return true;
            }

            bool HasNaN() const {
                for (int i=0; i<GetDimension(); i++) {
                    if (std::isnan(data[i])) return true;
                }
                return false;
            }

            bool HasInf() const {
                for (int i=0; i<GetDimension(); i++) {
                    if (std::isinf(data[i])) return true;
                }
                return false;
            }
        public:
            T LengthSquared() const {
                T result = T(0);
                for (int i=0; i<GetDimension(); i++) {
                    result += data[i]*data[i];
                }
                return result;
            }

            inline T Length() const {
                return sqrt(LengthSquared());
            }

            inline Vector Normalized() const {
                return (*this)/Length();
            }

            void Normalize() {
                T c = Length();
                for (int i=0; i<GetDimension(); i++) {
                    data[i] /= c;
                }
            }

            inline friend T Dot(const Vector& v, const Vector& w) {
                return v*w;
            }

            friend Vector Cross(const Vector& v, const Vector& w) {
                assert(v.GetDimension() == 3 && w.GetDimension() == 3);
                Vector result(3);
                result[0] = v.data[1]*w.data[2] - v.data[2]*w.data[1];
                result[1] = v.data[2]*w.data[0] - v.data[0]*w.data[2];
                result[2] = v.data[0]*w.data[1] - v.data[1]*w.data[0];
                return result;
            }
        public:
            friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
                os << "(";
                for (int i=0; i<v.GetDimension(); i++) {
                    os << v[i];
                    if (i != v.GetDimension()-1) os << ", ";
                }
                os << ")";
                return os;
            }
        private:
            size_t dimension;
            T* data;
        };

        typedef Vector<float>       Vecf;
        typedef Vector<double>      Vecd;

    }
}