#include <vector/matrix.hpp>
#include <vector/vector.hpp>

#include <sstream>

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
            THEN(" we should find 5 rows and 5 columns") {
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

    GIVEN(" An even larger matrix P") {
        Construction::Vector::Matrix P = {
            {1, 6, 1, 1, 1, 1, 1, 1, 7}, 
            {2, 2, 2, 3, 2, 2, 2, 2, 2}, 
            {3, 3, 3, 3, 3, 3, 8, 3, 3}, 
            {4, 4, 4, 4, 0, 4, 4, 4, 4}, 
            {5, 0, 0, 5, 5, 5, 5, 5, 4}, 
            {6, 6, 6, 6, 6, 6, 6, 6, 6}, 
            {7, 7, 7, 7, 7, 7, 7, 7, 7}, 
            {8, 8, 8, 8, 2, 8, 8, 0, 8}, 
            {9, 0, 9, 9, 9, 9, 9, 9, 9}, 
            {10, 10, 10, 15, 10, 10, 1, 0, 10}
        };

        WHEN(" we consider the number of rows and columns") {
            THEN(" we should find 9 rows and 10 columns") {
                REQUIRE(P.GetNumberOfColumns()==10);
                REQUIRE(P.GetNumberOfRows()==9);
            }
        }

        WHEN(" calculating the row echelon form") {
            P.ToRowEchelonForm();

            REQUIRE(P.GetColumnVector(0) == Construction::Vector::Vector({1,0,0,0,0,0,0,0,0}));
            REQUIRE(P.GetColumnVector(1) == Construction::Vector::Vector({0,1,0,0,0,0,0,0,0}));
            REQUIRE(P.GetColumnVector(2) == Construction::Vector::Vector({0,0,1,0,0,0,0,0,0}));
            REQUIRE(P.GetColumnVector(3) == Construction::Vector::Vector({0,0,0,1,0,0,0,0,0}));
            REQUIRE(P.GetColumnVector(4) == Construction::Vector::Vector({0,0,0,0,1,0,0,0,0}));
            REQUIRE(P.GetColumnVector(5) == Construction::Vector::Vector({0,0,0,0,0,1,0,0,0}));
            REQUIRE(P.GetColumnVector(7) == Construction::Vector::Vector({0,0,0,0,0,0,1,0,0}));
            REQUIRE(P.GetColumnVector(8) == Construction::Vector::Vector({0,0,0,0,0,0,0,1,0}));
            
            std::vector<int> nullEntries = {0,1,2,3,4,6,7,8};
            for(auto i:nullEntries)
            {
                REQUIRE(P.GetColumnVector(6)[i]==0);
            }
            REQUIRE(P.GetColumnVector(6)[5]==Approx(7.0/6));

            REQUIRE(P.GetColumnVector(9)[0]==0);
            REQUIRE(P.GetColumnVector(9)[1]==5);
            REQUIRE(P.GetColumnVector(9)[2]==Approx(-9.0/5));
            REQUIRE(P.GetColumnVector(9)[3]==Approx(-15.0/8));
            REQUIRE(P.GetColumnVector(9)[4]==0);
            REQUIRE(P.GetColumnVector(9)[5]==Approx(29.0/60));
            REQUIRE(P.GetColumnVector(9)[6]==Approx(5.0/4));
            REQUIRE(P.GetColumnVector(9)[7]==0);
            REQUIRE(P.GetColumnVector(9)[8]==0);
        }
    }
}