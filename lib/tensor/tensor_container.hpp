#pragma once

#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <tensor/tensor.hpp>

namespace Albus {
    namespace Tensor {

        /**
            \class TensorContainer

            \brief Container class to manage several tensors

            Container class to manage several tensors. It especially
            allows streaming in and out of a file.
         */
        class TensorContainer {
        public:
            typedef std::vector<Tensor>             ContainerType;
            typedef ContainerType::iterator         Iterator;
            typedef ContainerType::const_iterator   ConstIterator;
        public:
            TensorContainer() = default;

        public:
            void Insert(const Tensor& obj) { data.push_back(obj); }
            void Insert(Tensor&& obj) { data.push_back(std::move(obj)); }

            void Pop() { data.pop_back(); }
            void Remove(unsigned i) { data.erase(data.begin() + i); }
        public:
            size_t Size() const { return data.size(); }

            bool IsEmpty() const { return data.size() == 0; }
        public:
            Tensor& At(unsigned i) { return data.at(i); }
            Tensor At(unsigned i) const { return data.at(i); }

            Tensor& operator[](unsigned i) { return data[i]; }
            Tensor operator[](unsigned i) const { return data[i]; }

            Tensor& Front() { return data[0]; }
            Tensor Front() const { return data[0]; }
            Tensor& Back() { return data[data.size()-1]; }
            Tensor Back() const { return data[data.size()-1]; }
        public:
            Iterator begin() { return data.begin(); }
            Iterator end()   { return data.end(); }

            ConstIterator begin() const { return data.begin(); }
            ConstIterator end()   const { return data.end(); }
        public:
            friend class boost::serialization::access;

            template<class Archive>
            void serialize(Archive& ar, const unsigned version) {

            }

            void SaveToFile(const std::string& filename) {

            }

            void LoadFromFile(const std::string& filename) {

            }
        private:
            ContainerType data;
        };

    }
}