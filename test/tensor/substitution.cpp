#include <language/api.hpp>
#include <tensor/tensor.hpp>
#include <tensor/scalar.hpp>
#include <tensor/substitution.hpp>

SCENARIO("Substitution", "[substitution]") {

    GIVEN(" a tensorial equation") {

        auto tensor = Construction::Language::API::Coefficient(2, 0, 2, 0);

        auto subst = Construction::Tensor::Substitution();
        subst.Insert(Construction::Tensor::Scalar("e_1"), -Construction::Tensor::Scalar("e_2"));

        auto substituted = subst(tensor);
        REQUIRE(substituted.ToString() == "e_2 * (-\\gamma_{ab}\\gamma_{cd} + \\gamma_{ac}\\gamma_{bd} + \\gamma_{ad}\\gamma_{bc})");

    }

}
