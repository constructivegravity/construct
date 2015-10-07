#include <tensor/index.hpp>

/*
SCENARIO("Tensor indices", "[indices]") {

    GIVEN(" an indices object") {

        auto indices = Albus::Tensor::Indices::GetRomanSeries(3, {1,3});

        WHEN(" printing the TeX code") {
            THEN(" we have {abc}") {
                REQUIRE(indices.ToString() == "{abc}");
            }
        }

        WHEN(" taking a partial") {
            auto partial = indices.Partial({1,2});

            THEN(" the TeX code has no a anymore") {
                REQUIRE(partial.ToString() == "{bc}");
            }
        }

        WHEN(" applying components to the indices") {

            THEN(" assignments in the range return the assignment") {
                auto res = indices(1,2,3);
                std::vector<unsigned> assignment = {1,2,3};

                REQUIRE(res == assignment);
            }

            THEN(" and out-of range index should throw an exception") {
                REQUIRE_THROWS_AS(indices(0,1,1), Albus::Tensor::IndexOutOfRangeException);
            }

        }

    }

}
 */