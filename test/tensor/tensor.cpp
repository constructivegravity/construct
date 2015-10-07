#include <array>

#include <tensor/tensor.hpp>

SCENARIO("General tensors", "[tensor]") {

    GIVEN(" a tensor with two indices") {

        Albus::Tensor::Tensor T("T", "T", Albus::Tensor::Indices::GetRomanSeries(2, {1,3}));

        WHEN(" printing the TeX code") {
            std::stringstream ss;
            ss << T;
            auto code = ss.str();

            THEN(" we get") {
                REQUIRE(code == "T_{ab}");
            }

        }

        WHEN(" evaluating the tensor components") {

            std::array<double, 9> components;

            for (unsigned i=1; i<=3; ++i) {
                for (unsigned j=1; j<=3; ++j) {
                    components[3*(i-1)+j-1] = T(i,j);
                }
            }

            THEN(" all components should be zero") {
                for (unsigned i=0; i<9; i++) {
                    REQUIRE(components[i] == 0);
                }
            }

        }

    }

}

/**
    Testing of the Levi-Civita symbol
 */
SCENARIO("Epsilon tensor", "[epsilon-tensor]") {

    // Levi-Civita symbol in three dimensions, i.e. on a spatial slice
    GIVEN(" epsilon in three dimensions") {

        auto epsilon = Albus::Tensor::EpsilonTensor::Space();

        // Look at the indices
        WHEN(" looking at the indices") {
            auto indices = epsilon.GetIndices();

            // Only allow three indices
            THEN(" we have exactly three") {
                REQUIRE(indices.Size() == 3);
            }

        }

        // All index combinations
        WHEN(" looking at the tensor components with two or more indices being equal") {
            THEN(" we get zero due to antisymmetry") {
                // Same indices
                REQUIRE(epsilon(1,1,1) == 0);
                REQUIRE(epsilon(2,2,2) == 0);
                REQUIRE(epsilon(3,3,3) == 0);

                REQUIRE(epsilon(1,1,2) == 0);
                REQUIRE(epsilon(1,1,3) == 0);
                REQUIRE(epsilon(1,2,1) == 0);
                REQUIRE(epsilon(1,3,1) == 0);
                REQUIRE(epsilon(2,1,1) == 0);
                REQUIRE(epsilon(3,1,1) == 0);

                REQUIRE(epsilon(2,2,1) == 0);
                REQUIRE(epsilon(2,2,3) == 0);
                REQUIRE(epsilon(2,1,2) == 0);
                REQUIRE(epsilon(2,3,2) == 0);
                REQUIRE(epsilon(1,2,2) == 0);
                REQUIRE(epsilon(3,2,2) == 0);

                REQUIRE(epsilon(3,3,1) == 0);
                REQUIRE(epsilon(3,3,2) == 0);
                REQUIRE(epsilon(3,1,3) == 0);
                REQUIRE(epsilon(3,2,3) == 0);
                REQUIRE(epsilon(1,3,3) == 0);
                REQUIRE(epsilon(2,3,3) == 0);
            }
        }

        WHEN(" looking at the tensor components with cyclic permutations of (1,2,3)") {
            THEN(" we get one") {
                REQUIRE(epsilon(1,2,3) == 1);
                REQUIRE(epsilon(2,3,1) == 1);
                REQUIRE(epsilon(3,1,2) == 1);
            }
        }

        WHEN(" looking at the tensor components with odd permutations of (1,2,3)") {
            THEN(" we get minus one") {
                REQUIRE(epsilon(2,1,3) == -1);
                REQUIRE(epsilon(3,2,1) == -1);
                REQUIRE(epsilon(1,3,2) == -1);
            }
        }

        WHEN(" considering the type of the tensor") {
            THEN(" IsEpsilon returns true") {
                REQUIRE(epsilon.IsEpsilonTensor());
            }

            THEN(" IsEpsilon returns true even after cast") {
                Albus::Tensor::Tensor t = epsilon;
                REQUIRE(t.IsEpsilonTensor());
            }
        }
    }

    GIVEN(" epsilon in four dimensions") {

        auto epsilon = Albus::Tensor::EpsilonTensor::SpaceTime();

        // Look at the indices
        WHEN(" looking at the indices") {
            auto indices = epsilon.GetIndices();

            // Only allow three indices
            THEN(" we have exactly four") {
                REQUIRE(indices.Size() == 4);
            }

        }

    }

}

SCENARIO("Metric tensor", "[gamma-tensor]") {

    GIVEN(" a spatial metric") {

        auto gamma = Albus::Tensor::GammaTensor::SpatialMetric();

        WHEN(" considering the indices") {
            auto indices = gamma.GetIndices();

            THEN(" it should have two") {
                REQUIRE(indices.Size() == 2);
            }

            THEN(" it should print to {ab}") {
                REQUIRE(indices.ToString() == "{ab}");
            }
        }

        WHEN(" evaluating the components") {

            THEN(" we have a diagonal tensor") {
                REQUIRE(gamma(1, 1) == 1);
                REQUIRE(gamma(2, 2) == 1);
                REQUIRE(gamma(3, 3) == 1);

                REQUIRE(gamma(1, 2) == 0);
                REQUIRE(gamma(1, 3) == 0);
                REQUIRE(gamma(2, 1) == 0);
                REQUIRE(gamma(2, 3) == 0);
                REQUIRE(gamma(3, 1) == 0);
                REQUIRE(gamma(3, 2) == 0);
            }
        }

    }

}


SCENARIO("Addition", "[tensor-addition]") {

    auto gamma = Albus::Tensor::GammaTensor::SpatialMetric();
    auto gamma2 = Albus::Tensor::GammaTensor::SpatialMetric();

    GIVEN(" two tensors (twice the metric)") {

        auto a = gamma + gamma2;

        WHEN(" printing the TeX code") {

            THEN(" get twice the metric \\gamma") {
                REQUIRE(a.ToString() == "\\gamma_{ab} + \\gamma_{ab}");
            }
        }

        WHEN(" considering the indices") {
            auto indices = a.GetIndices();

            THEN(" should also have two indices") {
                REQUIRE(indices.Size() == 2);
            }

            THEN(" should be {ab}") {
                REQUIRE(indices.ToString() == "{ab}");
            }
        }

        WHEN(" adding them") {

            THEN(" we get the double") {
                REQUIRE(a(1,1) == 2);
                REQUIRE(a(1,2) == 0);
                REQUIRE(a(1,3) == 0);
                REQUIRE(a(2,1) == 0);
                REQUIRE(a(2,2) == 2);
                REQUIRE(a(2,3) == 0);
                REQUIRE(a(3,1) == 0);
                REQUIRE(a(3,2) == 0);
                REQUIRE(a(3,3) == 2);
            }

        }

    }

}