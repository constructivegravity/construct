#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>

SCENARIO("Tensor containers", "[tensor-container]") {

    GIVEN(" a tensor container") {
        Construction::Tensor::TensorContainer container;

        WHEN(" looking at the empty container") {
            THEN(" the size should be zero") {
                REQUIRE(container.Size() == 0);
            }

            THEN(" IsEmpty() should return true") {
                REQUIRE(container.IsEmpty());
            }
        }

        WHEN(" inserting a tensor") {
            auto gamma = Construction::Tensor::GammaTensor::SpatialMetric();
            container.Insert(gamma);

            THEN(" the size should be one") {
                REQUIRE(container.Size() == 1);
            }

            THEN(" the container is not empty") {
                REQUIRE(!container.IsEmpty());
            }

            THEN(" the first element should be a gamma") {
                REQUIRE(container[0].IsGammaTensor());
                REQUIRE(container[0](1,1) == 1);
                REQUIRE(container[0](1,2) == 0);
            }
        }

        WHEN(" serializing and deserializing a container") {
            Construction::Tensor::EpsilonGammaTensor epsilonGamma (1,2, Construction::Tensor::Indices::GetRomanSeries(7, {1,3}));
            Construction::Tensor::EpsilonGammaTensor epsilonGamma2 (0,4, Construction::Tensor::Indices::GetRomanSeries(8, {1,3}));

            container.Insert(epsilonGamma);
            container.Insert(epsilonGamma2);

            std::stringstream ss;
            container.Serialize(ss);

            std::stringstream is (ss.str());
            auto deserialized = *Construction::Tensor::TensorContainer::Deserialize(is);

            THEN(" the size matches") {
                REQUIRE(deserialized.Size() == 2);
            }

            THEN(" the string is correct") {
                REQUIRE(deserialized.ToString() == container.ToString());
            }
        }
    }

}