#include <tensor/tensor.hpp>
#include <tensor/symmetrization.hpp>
#include <generator/symmetrized_tensor.hpp>

SCENARIO("Block symmetrization", "[block-symmetrization]") {

    GIVEN(" four indices") {

        // Generate abcd
        auto indices = Construction::Tensor::Indices::GetRomanSeries(4, {1,3});
        Construction::Tensor::BlockSymmetrization symmetrizer({{1,2},{3,4}});

        WHEN(" block symmetrizing the gammas") {

            auto permuted = symmetrizer.PermuteIndices(indices);
            REQUIRE(permuted.size() == 2);
            REQUIRE(permuted[0].ToString() == "{abcd}");
            REQUIRE(permuted[1].ToString() == "{cdab}");
        }

    }

    GIVEN(" five indices") {

        // Generate abcd
        auto indices = Construction::Tensor::Indices::GetRomanSeries(5, {1,3});
        Construction::Tensor::BlockSymmetrization symmetrizer({{2,3},{4,5}});

        WHEN(" block symmetrizing the gammas") {
            auto permuted = symmetrizer.PermuteIndices(indices);
            REQUIRE(permuted.size() == 2);
            REQUIRE(permuted[0].ToString() == "{abcde}");
            REQUIRE(permuted[1].ToString() == "{adebc}");
        }

    }

    GIVEN(" two gammas") {
        Construction::Tensor::EpsilonGammaTensor gammas(0,2, Construction::Tensor::Indices::GetRomanSeries(4, {1,3}));

        Construction::Tensor::BlockSymmetrization symmetrizer({{1,2},{3,4}});
        WHEN(" block symmetrizing the gammas") {
            auto permuted = symmetrizer(gammas);
            REQUIRE(permuted->ToString() == "\\gamma_{ab}\\gamma_{cd}");
        }
    }

}