#include <array>

#include <language/api.hpp>
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

        WHEN(" symmetrizing a tensor") {
            auto tensor = Construction::Tensor::Tensor::EpsilonGamma(0,2, Construction::Tensor::Indices::GetRomanSeries(4, {1,3}));

            REQUIRE(tensor.Symmetrize({ {"a", {1,3}}, {"c", {1,3}} }).ToString() == "1/2 * (\\gamma_{ab}\\gamma_{cd} + \\gamma_{ad}\\gamma_{bc})");
        }

        WHEN(" evaluating the tensor components") {

            std::array<double, 9> components;

            for (unsigned i=1; i<=3; ++i) {
                for (unsigned j=1; j<=3; ++j) {
                    components[3*(i-1)+j-1] = T(i,j).ToDouble();
                }
            }

            THEN(" all components should be zero") {
                for (unsigned i=0; i<9; i++) {
                    REQUIRE(components[i] == 0);
                }
            }

        }

        WHEN(" going out of scope, all memory is freed") {

        }

        WHEN(" serializing the tensor") {

            std::stringstream ss;
            auto tensor = Construction::Tensor::Tensor::EpsilonGamma(1,2, Construction::Tensor::Indices::GetRomanSeries(7, {1,3}));

            tensor.Serialize(ss);

	        std::string content = ss.str();

            THEN(" the deserialized tensor is correct") {
                std::stringstream is(content);
                auto read = Construction::Tensor::Tensor::Deserialize(is);

		        REQUIRE(read);

		        REQUIRE(read->ToString() == tensor.ToString());
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
        auto epsilon = Construction::Tensor::Tensor::Epsilon(Construction::Tensor::Indices::GetRomanSeries(3, {1,3}));

        // Look at the indices
        WHEN(" looking at the indices") {
            auto indices = epsilon.GetIndices();

            // Only allow three indices
            THEN(" we have exactly three") {
                REQUIRE(indices.Size() == 3);
            }

        }

        WHEN(" initializing the epsilon with all indices up") {
            auto indices = Construction::Tensor::Indices::GetRomanSeries(3, {1,3});
            indices[0].SetContravariant(true);
            indices[1].SetContravariant(true);
            indices[2].SetContravariant(true);

            REQUIRE(indices.ToString() == "^{abc}");

            auto epsilon2 = Construction::Tensor::Tensor::Epsilon(indices);

            THEN(" we get the proper result") {
                REQUIRE(epsilon2.ToString() == "\\epsilon^{abc}");
            }
        }

        // All index combinations
        WHEN(" looking at the tensor components with two or more indices being equal") {
            THEN(" we get zero due to antisymmetry") {
                // Same indices
                REQUIRE(epsilon(1,1,1) == 0.0);
                REQUIRE(epsilon(2,2,2) == 0.0);
                REQUIRE(epsilon(3,3,3) == 0.0);

                REQUIRE(epsilon(1,1,2) == 0.0);
                REQUIRE(epsilon(1,1,3) == 0.0);
                REQUIRE(epsilon(1,2,1) == 0.0);
                REQUIRE(epsilon(1,3,1) == 0.0);
                REQUIRE(epsilon(2,1,1) == 0.0);
                REQUIRE(epsilon(3,1,1) == 0.0);

                REQUIRE(epsilon(2,2,1) == 0.0);
                REQUIRE(epsilon(2,2,3) == 0.0);
                REQUIRE(epsilon(2,1,2) == 0.0);
                REQUIRE(epsilon(2,3,2) == 0.0);
                REQUIRE(epsilon(1,2,2) == 0.0);
                REQUIRE(epsilon(3,2,2) == 0.0);

                REQUIRE(epsilon(3,3,1) == 0.0);
                REQUIRE(epsilon(3,3,2) == 0.0);
                REQUIRE(epsilon(3,1,3) == 0.0);
                REQUIRE(epsilon(3,2,3) == 0.0);
                REQUIRE(epsilon(1,3,3) == 0.0);
                REQUIRE(epsilon(2,3,3) == 0.0);
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
                REQUIRE(epsilon.IsEpsilon());
            }

            THEN(" IsEpsilon returns true even after cast") {
                Construction::Tensor::Tensor t = epsilon;
                REQUIRE(t.IsEpsilon());
            }
        }
    }

    GIVEN(" epsilon in four dimensions") {
        auto epsilon = Construction::Tensor::Tensor::Epsilon(Construction::Tensor::Indices::GetRomanSeries(4,{0,3}));

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
        auto gamma = Construction::Tensor::Tensor::Gamma(Construction::Tensor::Indices::GetRomanSeries(2,{1,3}));

        WHEN(" considering the indices") {
            auto indices = gamma.GetIndices();

            THEN(" it should have two") {
                REQUIRE(indices.Size() == 2);
            }

            THEN(" it should print to {ab}") {
                REQUIRE(indices.ToString() == "_{ab}");
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
        auto T = Construction::Tensor::Tensor::EpsilonGamma(1,3, Construction::Tensor::Indices::GetRomanSeries(9, {1,3}));

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

            auto S = Construction::Tensor::Tensor::EpsilonGamma(1,2, indices);

            THEN("We get the correct result") {
                auto canon = S.Canonicalize();
                REQUIRE(canon.ToString() == "-\\epsilon_{abc}\\gamma_{de}\\gamma_{fg}");
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

        auto S = Construction::Tensor::Tensor::EpsilonGamma(1,1, indices);

        WHEN(" canonicalizing this tensor") {
            auto canon = S.Canonicalize();

            REQUIRE(canon.ToString() == "-\\epsilon_{abc}\\gamma_{de}");
        }
    }

}


SCENARIO("Addition", "[tensor-addition]") {

    auto gamma = Construction::Tensor::Tensor::Gamma(Construction::Tensor::Indices::GetRomanSeries(2,{1,3}));
    auto gamma2 = Construction::Tensor::Tensor::Gamma(Construction::Tensor::Indices::GetRomanSeries(2,{1,3}));

    GIVEN(" two tensors (twice the metric)") {

        auto a = gamma + gamma2;

        WHEN(" printing the TeX code") {

            THEN(" get twice the metric \\gamma") {
                REQUIRE(a.ToString() == "\\gamma_{ab} + \\gamma_{ab}");
            }
        }

        WHEN(" simpliyifing") {
            THEN(" we really get just twice the metric \\gamma") {
                auto b = a.Simplify();
                REQUIRE(b.ToString() == "2 * \\gamma_{ab}");

                auto c = (Construction::Tensor::Scalar::Variable("x") * gamma + Construction::Tensor::Scalar::Variable("y") * gamma2);
                REQUIRE(c.Simplify().ToString() == "(x + y) * \\gamma_{ab}");

                auto d = c.Simplify().RedefineVariables("x");
                REQUIRE(d.ToString() == "x_1 * \\gamma_{ab}");
            }
        }

        WHEN(" symmetrizing (manually)") {
            Construction::Tensor::Indices indices = { {"b", {1,3}}, {"a", {1,3}} };
            auto permuted_gamma = Construction::Tensor::Tensor::Gamma({ { "b", {1,3} }, { "a", {1,3} } });

            auto symmetrized = Construction::Tensor::Scalar(1,2) * ( gamma + permuted_gamma );
            auto expanded = symmetrized.Expand();
            auto simplified = expanded.Simplify();

            THEN(" we get the symmetrized expression when not simpliyifing") {
                REQUIRE(symmetrized.ToString() == "1/2 * (\\gamma_{ab} + \\gamma_{ba})");
            }

            THEN(" the expanded term looks correct") {
                REQUIRE(expanded.ToString() == "1/2 * \\gamma_{ab} + 1/2 * \\gamma_{ba}");
            }

            THEN(" we get the metric again after simplification") {
                REQUIRE(simplified.ToString() == "\\gamma_{ab}");
            }
        }

        WHEN(" considering the indices") {
            auto indices = a.GetIndices();

            THEN(" should also have two indices") {
                REQUIRE(indices.Size() == 2);
            }

            THEN(" should be {ab}") {
                REQUIRE(indices.ToString() == "_{ab}");
            }
        }

        WHEN(" serializing an addition of two tensors") {
            auto tensor = Construction::Tensor::Scalar("x") * Construction::Tensor::Tensor::EpsilonGamma(0, 3, Construction::Tensor::Indices::GetRomanSeries(6, {1,3})) +
                     Construction::Tensor::Scalar("y") * Construction::Tensor::Tensor::EpsilonGamma(2, 0, Construction::Tensor::Indices::GetRomanSeries(6, {1,3}));

            std::string content;

            // Serialize
            {
                std::stringstream ss;
                tensor.Serialize(ss);
                content = ss.str();
            }

            THEN(" the deserialized gives the same tensor again") {
                std::stringstream ss(content);
                auto deserialized = Construction::Tensor::Tensor::Deserialize(ss);

                REQUIRE(deserialized);
                REQUIRE(deserialized->ToString() == tensor.ToString());
            }
        }

        WHEN(" creating a delta tensor") {
            auto indices = Construction::Tensor::Indices::GetRomanSeries(2, {1,3});
            indices[0].SetContravariant(true);
            indices[1].SetContravariant(false);

            REQUIRE(indices.ToString() == "^{a}_{b}");

            REQUIRE(indices.ContainsIndex({"a", {1,3}}));

            auto delta = Construction::Language::API::Delta(indices);

            REQUIRE(delta.ToString() == "\\delta^{a}_{b}");
        }

        WHEN(" calculating the trace of delta") {
            auto delta = Construction::Language::API::Delta({
                {"a", {1,3}},
                {"a", {1,3}}
            });

            REQUIRE(delta.ToString() == "\\delta^{a}_{a}");
        }

        WHEN(" contracting two indices") {
            auto A = Construction::Tensor::Indices::GetRomanSeries(2, {1,3});
            Construction::Tensor::Indices B = { {"a", {1,3}}, {"c", {1,3}}, {"d", {1,3}} };
            B[0].SetContravariant(true);

            REQUIRE(A.ToString() == "_{ab}");
            REQUIRE(B.ToString() == "^{a}_{cd}");
            REQUIRE(A.Contract(B).ToString() == "_{bcd}");

            A[0].SetContravariant(true);
            REQUIRE_THROWS(A.Contract(B));

            B[0].SetContravariant(false);
            REQUIRE(A.Contract(B).ToString() == "_{bcd}");

            REQUIRE(A.Contract(Construction::Tensor::Indices::GetRomanSeries(2, {1,3}, 2)).ToString() == "^{a}_{bcd}");
        }

        WHEN(" contracting with delta") {
            auto delta = Construction::Language::API::Delta({ {"a", {1,3}}, {"d", {1,3}}});
            auto epsilon = Construction::Language::API::Epsilon(Construction::Tensor::Indices::GetRomanSeries(3, {1,3}));

            REQUIRE(delta.ToString() == "\\delta^{a}_{d}");

            auto contracted = delta * epsilon;

            REQUIRE(contracted.ToString() == "\\epsilon_{dbc}");
            //REQUIRE(contracted() == 3);
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
