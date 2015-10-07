#include <vector/matrix.hpp>

SCENARIO("Matrix", "[matrix]") {

    GIVEN(" A matrix M") {
        //  1   2   -1   -4
        //  2   3   -1   -11
        // -2   0   -3    22
        Albus::Vector::Matrix M = {
            {1, 2, -2},
            {2, 3, 0},
            {-1, -1, -3},
            {-4, -11, 22}
        };

        WHEN(" we look at the number of rows and columns") {
            THEN("  r is equal to 3") {
                REQUIRE(M.GetNumberOfRows());
            }

            THEN(" c is equal to 4") {
                REQUIRE(M.GetNumberOfColumns());
            }
        }

        WHEN(" calculating the row echelon form") {
            auto echelon = M.GetRowEchelonForm();

            REQUIRE(echelon.GetColumnVector(0) == Albus::Vector::Vector({1,0,0}));
            REQUIRE(echelon.GetColumnVector(1) == Albus::Vector::Vector({0,1,0}));
            REQUIRE(echelon.GetColumnVector(2) == Albus::Vector::Vector({0,0,1}));
            REQUIRE(echelon.GetColumnVector(3) == Albus::Vector::Vector({-8,1,-2}));
        }

    }

}