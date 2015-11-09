#pragma once

#include <vector>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <common/serializable.hpp>

#include <tensor/tensor.hpp>

namespace Construction {
    namespace Tensor {

        using Common::Serializable;

        /**
            \class TensorContainer

            \brief Container class to manage several tensors

            Container class to manage several tensors. It especially
            allows streaming in and out of a file.
         */
        class TensorContainer : public Serializable<TensorContainer> {
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

            virtual void Serialize(std::ostream& os) const {
                // Write size of container
                size_t size = data.size();
                os.write(reinterpret_cast<const char*>(&size), sizeof(size));

                // Store every tensor
                for (auto& t : data) {
                    t->Serialize(os);
                }
            }

            static std::shared_ptr<TensorContainer> Deserialize(std::istream& is) {
                size_t size;
                is.read(reinterpret_cast<char*>(&size), sizeof(size));

                auto result = std::make_shared<TensorContainer>();

                for (int i=0; i<size; i++) {
                    auto tensor =  Tensor::Deserialize(is);
                    result->Insert(std::move(tensor));
                }

                return std::move(result);
            }

            template<class Archive>
            void serialize(Archive& ar, const unsigned version) {
                ar & data;
            }

            void SaveToFile(const std::string& filename) {

            }

            void LoadFromFile(const std::string& filename) {

            }
        public:
            std::string ToString() const {
                std::stringstream ss;
                ss << "[";
                for (int i=0; i<data.size(); i++) {
                    ss << data[i]->ToString();
                    if (i != data.size()-1) ss << ", ";
                }
                ss << "]";
                return ss.str();
            }

            friend std::ostream& operator<<(std::ostream& os, const TensorContainer& container) {
                os << container.ToString();
                return os;
            }
        private:
            ContainerType data;
        };

    }
}