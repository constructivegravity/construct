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
        }
    }

}