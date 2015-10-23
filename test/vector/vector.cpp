#include <vector/vector.hpp>

#include <sstream>

SCENARIO("Vector class", "[vectors]") {

    GIVEN(" a three-dimensional vector") {

        WHEN(" considering a three-dimensional vector") {

            Construction::Vector::Vector v = {1,3,-2};

            THEN(" the dimension is three") {
                REQUIRE(v.GetDimension() == 3);
            }

            THEN(" the components are correct") {
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 3);
                REQUIRE(v[2] == -2);
            }

            THEN(" the output is fine") {
                std::stringstream ss;
                ss << v;

                REQUIRE(ss.str() == "(1, 3, -2)");
            }

            THEN(" access beyond dimension throws exception") {
                REQUIRE_THROWS_AS(v[3], Construction::Vector::OutOfBoundariesException);
            }

            THEN(" negation is correct") {
                auto w = -v;
                REQUIRE(w[0] == -1);
                REQUIRE(w[1] == -3);
                REQUIRE(w[2] == 2);
            }

            THEN(" scalar multiplication is correct") {
                auto w = 3*v;
                REQUIRE(w[0] == 3);
                REQUIRE(w[1] == 9);
                REQUIRE(w[2] == -6);
            }

            THEN(" scalar division is correct") {
                auto w = v/3;
                REQUIRE(w[0] == Approx(1.0/3));
                REQUIRE(w[1] == 1);
                REQUIRE(w[2] == Approx(-2.0/3));
            }

        }

        WHEN(" considering two vectors") {

            Construction::Vector::Vector v = {1,3,-2};
            Construction::Vector::Vector w = {3,1,4};

            THEN(" addition is correct") {
                auto x = v+w;
                REQUIRE(x[0] == 4);
                REQUIRE(x[1] == 4);
                REQUIRE(x[2] == 2);
            }

        }

    }

}