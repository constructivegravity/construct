#pragma once

#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <tensor/tensor.hpp>

namespace Construction {
    namespace Tensor {

        /**
            \class TensorContainer

            \brief Container class to manage several tensors

            Container class to manage several tensors. It especially
            allows streaming in and out of a file.
         */
        class TensorContainer {
        public:
            typedef std::vector<TensorPointer>      ContainerType;
            typedef ContainerType::iterator         Iterator;
            typedef ContainerType::const_iterator   ConstIterator;
        public:
            TensorContainer() = default;

            TensorContainer(const TensorContainer& other) : data(other.data) { }
            TensorContainer(TensorContainer&& other) : data(std::move(other.data)) { }
        public:
            TensorContainer& operator=(const TensorContainer& other) {
                data = other.data;
                return *this;
            }

            TensorContainer& operator=(TensorContainer&& other) {
                data = std::move(other.data);
                return *this;
            }
        public:
            void Insert(Tensor& obj) {
                data.push_back(std::shared_ptr<Tensor>(std::shared_ptr<Tensor>(), &obj));
            }

            void Insert(const TensorPointer& pointer) {
                data.push_back(std::move(pointer));
            }

            //void Insert(Tensor&& obj) { data.push_back(std::move(obj)); }

            void Pop() { data.pop_back(); }
            void Remove(unsigned i) { data.erase(data.begin() + i); }
        public:
            size_t Size() const { return data.size(); }

            bool IsEmpty() const { return data.size() == 0; }
        public:
            Tensor& At(unsigned i) { return *data.at(i); }
            Tensor At(unsigned i) const { return *data.at(i); }

            TensorPointer Get(unsigned i) const { return data.at(i); }

            Tensor& operator[](unsigned i) { return *data[i]; }
            Tensor operator[](unsigned i) const { return *data[i]; }

            Tensor& Front() { return *data[0]; }
            Tensor Front() const { return *data[0]; }
            Tensor& Back() { return *data[data.size()-1]; }
            Tensor Back() const { return *data[data.size()-1]; }
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
        public:
            friend std::ostream& operator<<(std::ostream& os, const TensorContainer& container) {
                os << "[";
                for (int i=0; i<container.Size(); i++) {
                    os << container.data[i]->ToString();
                    if (i != container.Size()-1) os << ", ";
                }
                os << "]";
                return os;
            }
        private:
            ContainerType data;
        };

    }
}