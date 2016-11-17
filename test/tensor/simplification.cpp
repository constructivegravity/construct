#include <tensor/tensor.hpp>
#include <tensor/symmetrization.hpp>
//#include <generator/symmetrized_tensor.hpp>
#include <language/api.hpp>

SCENARIO("Simplification", "[simplification]") {

    GIVEN(" some arbitrary tensor with symmetries") {
        auto tensor = Construction::Language::API::Coefficient(2,0,2,0);

        WHEN(" Simplify is called again") {
            THEN(" nothing should happen") {
                REQUIRE(tensor.Simplify().ToString()==tensor.ToString());
            }
        }
    }
}
