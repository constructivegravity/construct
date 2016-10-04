#include <language/api.hpp>

SCENARIO("Metric case", "[metric]") {

    // Calculate all coefficients
    auto lambda1 = Construction::Tensor::Scalar("a_1");
    auto lambda2 = Construction::Language::API::Coefficient(0,0,2,0).RedefineVariables("b");
    auto lambda3 = Construction::Language::API::Coefficient(0,0,2,1).RedefineVariables("c");
    auto lambda4 = Construction::Language::API::Coefficient(2,0,2,0).RedefineVariables("d");
    auto theta   = Construction::Language::API::Coefficient(2,0,2,0).RedefineVariables("e");
    auto lambda5 = Construction::Language::API::Coefficient(0,0,2,2).RedefineVariables("f");
    auto lambda6 = Construction::Language::API::Coefficient(2,0,2,1).RedefineVariables("g");
    auto lambda7 = Construction::Language::API::Coefficient(2,0,2,2).RedefineVariables("h");
    auto lambda8 = Construction::Language::API::Coefficient(2,1,2,1).RedefineVariables("i");

    // TODO: check if the coefficients are correct

    // Solve equation 10a
    auto ten_a = lambda1 * Construction::Language::API::Gamma(Construction::Tensor::Indices::GetRomanSeries(2, {1,3})) - 2 * lambda2;
    {
        auto subst = Construction::Language::API::HomogeneousSystem(ten_a);
        REQUIRE(subst.ToString() == "a_1 = 2 * b_1\n");
    }

    // Solve equation 2a
    auto two_a = lambda3;
    {
        auto subst = Construction::Language::API::HomogeneousSystem(two_a);
        REQUIRE(subst.ToString() == "");
    }

    // Solve equation 10b
    auto ten_b = Construction::Language::API::Gamma(Construction::Tensor::Indices::GetRomanSeries(2, {1,3})) * lambda2 +
            2 * Construction::Language::API::Symmetrize(Construction::Language::API::Gamma(Construction::Tensor::Indices::GetNamed({"c a"})) * Construction::Language::API::RenameIndices(lambda2, Construction::Tensor::Indices::GetNamed({"a b"}), Construction::Tensor::Indices::GetNamed({"b d"})), Construction::Tensor::Indices::GetNamed({"a b"}))
            -2 * lambda4;
    {
        auto subst = Construction::Language::API::HomogeneousSystem(ten_b);
        REQUIRE(subst.ToString() == "");
    }


}