#include <vector/matrix.hpp>

SCENARIO("Matrix", "[matrix]") {

    GIVEN(" A matrix M") {
        //  1   2   -1   -4
        //  2   3   -1   -11
        // -2   0   -3    22
        Construction::Vector::Matrix M = {
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
            M.ToRowEchelonForm();

            REQUIRE(M.GetColumnVector(0) == Construction::Vector::Vector({1,0,0}));
            REQUIRE(M.GetColumnVector(1) == Construction::Vector::Vector({0,1,0}));
            REQUIRE(M.GetColumnVector(2) == Construction::Vector::Vector({0,0,1}));
            REQUIRE(M.GetColumnVector(3) == Construction::Vector::Vector({-8,1,-2}));
        }

    }

    GIVEN(" A larger matrix N") {
        Construction::Vector::Matrix N = {
            {1,4,7,12,1},
            {2,5,8,1,3},
            {3,6,9,2,6},
            {4,7,10,3,7},
            {5,8,11,4,8}
        };

        WHEN(" we consider the number of rows and columns") {
            THEN(" we should find 4 rows and 5 columns") {
                REQUIRE(N.GetNumberOfColumns()==5);
                REQUIRE(N.GetNumberOfRows()==5);
            }
        }

        WHEN(" calculating the row echelon form") {
            N.ToRowEchelonForm();

            REQUIRE(N.GetColumnVector(0) == Construction::Vector::Vector({1,0,0,0,0}));
            REQUIRE(N.GetColumnVector(1) == Construction::Vector::Vector({0,1,0,0,0}));
            REQUIRE(N.GetColumnVector(2) == Construction::Vector::Vector({0,0,1,0,0}));
            REQUIRE(N.GetColumnVector(3) == Construction::Vector::Vector({0,0,0,1,0}));
            REQUIRE(N.GetColumnVector(4) == Construction::Vector::Vector({0,0,-1,2,0}));
        }
    }

}