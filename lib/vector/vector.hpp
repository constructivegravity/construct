#pragma once

#include <cmath>

#include <iostream>

#include <common/error.hpp>

namespace Albus {
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
        class Vector {
        public:
            Vector(size_t dimension) {
                this->dimension = dimension;
                data = new float[dimension];

                for (int i=0; i<dimension; i++) {
                    data[i] = 0.0;
                }
            }

            ~Vector() {
                if (data != nullptr) {
                    delete[] data;
                }
            }

            Vector(std::initializer_list<float> list) {
                dimension = list.size();
                data = new float[dimension];

                int i=0;
                for (auto& e : list) {
                    data[i] = e;
                    i++;
                }
            }

            /**
                Copy constructor
             */
            Vector(const Vector& other) {
                dimension = other.GetDimension();

                data = new float[dimension];
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
                delete[] data;

                size_t dim = other.GetDimension();
                data = new float[dim];
                for (int i=0; i<dim; i++) {
                    data[i] = other.data[i];
                }

                return *this;
            }

            Vector& operator=(Vector&& other) {
                delete[] data;
                data = other.data;
                other.data = nullptr;
                return *this;
            }
        public:
            inline size_t GetDimension() const {
                return dimension;
            }
        public:
            float At(unsigned i) const {
                if (i >= GetDimension()) throw OutOfBoundariesException();
                return data[i];
            }

            float& At(unsigned i) {
                if (i >= GetDimension()) throw OutOfBoundariesException();
                return data[i];
            }

            float& operator[](unsigned i) {
                if (i >= GetDimension()) throw OutOfBoundariesException();
                return data[i];
            }

            float operator[](unsigned i) const {
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

            float operator*(const Vector& other) const {
                size_t dim =  GetDimension();
                if (other.GetDimension() != dim) {
                    throw IncompatibleDimensionsException();
                }

                float result = 0.0;
                for (int i=0; i<dim; i++) {
                    result += data[i]*other.data[i];
                }
                return result;
            }

            Vector& operator*=(float c) {
                for (int i=0; i<GetDimension(); i++) {
                    data[i] *= c;
                }
                return *this;
            }

            Vector operator*(float c) const {
                Vector result = *this;
                for (int i=0; i<GetDimension(); i++) {
                    result.data[i] *= c;
                }
                return result;
            }

            inline friend Vector operator*(float c, const Vector& v) {
                return v*c;
            }

            Vector& operator/=(float c) {
                c = 1.0/c;
                for (int i=0; i<GetDimension(); i++) {
                    data[i] *= c;
                }
                return *this;
            }

            Vector operator/(float c) const {
                Vector result = *this;
                c = 1.0/c;
                for (int i=0; i<GetDimension(); i++) {
                    result.data[i] *= c;
                }
                return result;
            }
        public:
            bool IsZero() const {
                for (int i=0; i<GetDimension(); i++) {
                    if (data[i] != 0.0f) return false;
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
            float LengthSquared() const {
                float result = 0.0;
                for (int i=0; i<GetDimension(); i++) {
                    result += data[i]*data[i];
                }
                return result;
            }

            inline float Length() const {
                return sqrt(LengthSquared());
            }

            inline Vector Normalized() const {
                return (*this)/Length();
            }

            void Normalize() {
                double c = 1.0/Length();
                for (int i=0; i<GetDimension(); i++) {
                    data[i] *= c;
                }
            }

            inline friend float Dot(const Vector& v, const Vector& w) {
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
            float* data;
        };

    }
}