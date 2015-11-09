#include <array>

#include <tensor/tensor.hpp>

#include <boost/archive/text_oarchive.hpp>

SCENARIO("General tensors", "[tensor]") {

    GIVEN(" a tensor with two indices") {

        Construction::Tensor::Tensor T("T", "T", Construction::Tensor::Indices::GetRomanSeries(2, {1,3}));

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

        WHEN(" serializing the tensor") {

            std::stringstream ss;
            Construction::Tensor::EpsilonGammaTensor tensor(1,2, Construction::Tensor::Indices::GetRomanSeries(7, {1,3}));
            Construction::Tensor::GammaTensor gamma(Construction::Tensor::Indices::GetRomanSeries(2, {1,3}));

            tensor.Serialize(ss);
            //gamma.Serialize(ss);
            //std::cout << ss.str() << std::endl;
            //std::cout << "Length: " << ss.str().size() << std::endl;

            THEN(" the deserialized tensor is correct") {
                std::stringstream is(ss.str());
                auto read = Construction::Tensor::Tensor::Deserialize(is);
                //std::cout << read->ToString() << std::endl;

                /*auto read2 = Construction::Tensor::Tensor::Deserialize(is);
                std::cout << read2->ToString() << std::endl;*/
            }

            /*std::stringstream ss;
            {
                boost::archive::text_oarchive oa(ss);
                oa << T;
            }
            std::cout << ss.str() << std::endl;*/

        }

    }

}

/**
    Testing of the Levi-Civita symbol
 */
SCENARIO("Epsilon tensor", "[epsilon-tensor]") {

    // Levi-Civita symbol in three dimensions, i.e. on a spatial slice
    GIVEN(" epsilon in three dimensions") {

        auto epsilon = Construction::Tensor::EpsilonTensor::Space();

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
                Construction::Tensor::Tensor t = epsilon;
                REQUIRE(t.IsEpsilonTensor());
            }
        }
    }

    GIVEN(" epsilon in four dimensions") {

        auto epsilon = Construction::Tensor::EpsilonTensor::SpaceTime();

        // Look at the indices
        WHEN(" looking at the indices") {
            auto indices = epsilon.GetIndices();

            // Only allow three indices
            THEN(" we have exactly four") {
                REQUIRE(indices.Size() == 4);
            }

            THEN(" cyclic permutation is not 1") {
                REQUIRE(epsilon(1,2,3,0) == -1);
            }

        }

    }

}

SCENARIO("Metric tensor", "[gamma-tensor]") {

    GIVEN(" a spatial metric") {

        auto gamma = Construction::Tensor::GammaTensor::SpatialMetric();

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

SCENARIO("Epsilon Gamma", "[epsilon-gamma]") {

    GIVEN(" a tensor with one epsilon and three gammas") {

        Construction::Tensor::EpsilonGammaTensor T(1, 3, Construction::Tensor::Indices::GetRomanSeries(9, {1,3}));

        WHEN(" printing the TeX code") {
            REQUIRE(T.ToString() == "\\epsilon_{abc}\\gamma_{de}\\gamma_{fg}\\gamma_{hi}");
        }

        WHEN(" considering some components") {
            REQUIRE(T(1, 1, 2, 3, 1, 2, 1, 2, 1) == 0.0);
            REQUIRE(T(1, 2, 3, 1, 1, 2, 2, 3, 3) == 1.0);
            REQUIRE(T(1, 3, 2, 1, 1, 2, 2, 3, 3) == -1);
        }

        /*WHEN(" serializing the tensor") {

            std::stringstream ss;

            // Scope based output
            {
                boost::archive::binary_oarchive oa(ss);
                oa << T;
            }

            std::string output = ss.str();
            std::cout << T << std::endl;
            std::cout << output << std::endl;

            THEN(" the deserialized tensor has the correct indices") {

                Construction::Tensor::Tensor t;

                // Scope based input
                {
                    boost::archive::binary_iarchive ia(ss);
                    ia >> t;
                }

                std::cout << t << std::endl;
            }

        }*/

        WHEN(" considering the canonicalization") {
            Construction::Tensor::Indices indices;
            indices.Insert(Construction::Tensor::Index("a", {1,3}));
            indices.Insert(Construction::Tensor::Index("c", {1,3}));
            indices.Insert(Construction::Tensor::Index("b", {1,3}));
            indices.Insert(Construction::Tensor::Index("g", {1,3}));
            indices.Insert(Construction::Tensor::Index("f", {1,3}));
            indices.Insert(Construction::Tensor::Index("e", {1,3}));
            indices.Insert(Construction::Tensor::Index("d", {1,3}));

            Construction::Tensor::EpsilonGammaTensor S(1,2, indices);

            THEN("We get the correct result") {
                auto canon = S.Canonicalize();
                REQUIRE(canon->ToString() == "-\\epsilon_{abc}\\gamma_{de}\\gamma_{fg}");
            }

        }

    }

    GIVEN(" a tensor with 1 epsilon and 1 gamma with non-standard indices") {

        Construction::Tensor::Indices indices;
        indices.Insert(Construction::Tensor::Index("a", {1,3}));
        indices.Insert(Construction::Tensor::Index("c", {1,3}));
        indices.Insert(Construction::Tensor::Index("b", {1,3}));
        indices.Insert(Construction::Tensor::Index("e", {1,3}));
        indices.Insert(Construction::Tensor::Index("d", {1,3}));

        Construction::Tensor::EpsilonGammaTensor S(1,1, indices);

        WHEN(" canonicalizing this tensor") {
            auto canon = S.Canonicalize();

            REQUIRE(canon->ToString() == "-\\epsilon_{abc}\\gamma_{de}");
        }
    }

}


SCENARIO("Addition", "[tensor-addition]") {

    auto gamma = Construction::Tensor::GammaTensor::SpatialMetric();
    auto gamma2 = Construction::Tensor::GammaTensor::SpatialMetric();

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