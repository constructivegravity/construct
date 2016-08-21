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
        REQUIRE(substituted.ToString() == "e_2 * (-\\gamma_{ac}\\gamma_{bd} - \\gamma_{ad}\\gamma_{bc} + \\gamma_{ab}\\gamma_{cd})");

    }

    GIVEN(" merging two substitutions") {
        Construction::Tensor::Substitution substitution;
        substitution.Insert(Construction::Tensor::Scalar("a"), Construction::Tensor::Scalar("m") + Construction::Tensor::Scalar("n") );
        substitution.Insert(Construction::Tensor::Scalar("c"), Construction::Tensor::Scalar("b"));

        Construction::Tensor::Substitution substitution2;
        substitution2.Insert(Construction::Tensor::Scalar("b"), Construction::Tensor::Scalar("m") - Construction::Tensor::Scalar("n") );

        auto merged = Construction::Tensor::Substitution::Merge({ substitution, substitution2 });

        REQUIRE(merged.ToString() == "a = m + n\nc = m - n\nb = m - n\n");
    }

}
