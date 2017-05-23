#include <tensor/scalar.hpp>
#include <tensor/fraction.hpp>
#include <tensor/variable.hpp>

using Construction::Tensor::Scalar;

SCENARIO("Scalars", "[scalar]") {

	GIVEN("a floating point number") {

		Scalar zero = 0;

		Scalar s1 = 3.3;
		Scalar s2 = 7.9;

		Scalar f1 (5, 8);
		Scalar f2 (1, 3);

		Scalar v1 ("x");
		Scalar v2 ("y");

		WHEN(" converting to double, get zero") {
			REQUIRE(zero.ToDouble() == 0);
		}

		WHEN(" multiplying zero with another scalar gives zero") {
			REQUIRE( (zero * Scalar(3) ).ToDouble() == 0);

			// Multiply by a variable
			auto s = zero * Scalar::Variable("a");
			REQUIRE(s.IsNumeric());
			REQUIRE(s.ToDouble() == 0);
		}

        WHEN(" creating from a double") {
            REQUIRE(Scalar::Fraction(0.5).ToString() == "1/2");
            REQUIRE(Scalar::Fraction(0.25).ToString() == "1/4");
        }

		WHEN(" comparing two scalars") {
			REQUIRE(zero == 0);
			REQUIRE(Scalar(1) == 1);
			REQUIRE(Scalar(1,2) == 0.5);
		}

		WHEN(" printing scalars") {
			REQUIRE( s1.ToString() == "3.3" );
			REQUIRE( f2.ToString() == "1/3");
			REQUIRE( v1.ToString() == "x");

			REQUIRE( (s1*f2).ToString() == "1.1");
			REQUIRE( (f1*v1).ToString() == "5/8 * x");
		}

		WHEN(" addition") {

			REQUIRE((s1 + zero).ToString() == s1.ToString());
			REQUIRE((f1 + zero).ToString() == f1.ToString());
			REQUIRE((v1 + zero).ToString() == v1.ToString());

			REQUIRE((f1 + f2).ToString() == Scalar(23,24).ToString());
			REQUIRE((s1 + s2).ToString() == "11.2");
			REQUIRE((v1 + v2).ToString() == "x + y");

			REQUIRE((s1 + f1).ToString() == "3.925");
			REQUIRE((s1 + v1).ToString() == "3.3 + x");
			REQUIRE((f1 + v1).ToString() == "5/8 + x");

			auto s = f1*v1;
			REQUIRE((f1*v1).ToString() == "5/8 * x");

			REQUIRE((s1 + (f1*v1)).ToString() == "3.3 + 5/8 * x");

			REQUIRE( (Scalar("x") - Scalar("x")).ToString() == "0" );

			/*Scalar t = Scalar(1,3) * Scalar("x");
			Scalar s = 3.3 + t;
			//mREQUIRE(s.IsAdded());
			REQUIRE(s.ToString() == "3.3 + 1/3 * x");*/
		}

	}

	GIVEN(" an expression containing a variable") {

		Scalar s = Scalar(3) - Scalar(5) * Scalar("x");

		WHEN(" printing the expression") {
			THEN(" we get the proper TeX code") {
				REQUIRE(s.ToString() == "3 - 5 * x");
			}
		}

		/*WHEN(" serializing this") {
			THEN(" we get something") {
				std::string content;
				{
					std::stringstream ss;
					s.Serialize(ss);
					content = ss.str();
				}

				{
					std::stringstream ss(content);
					auto pointer = Scalar::Deserialize(ss);
					REQUIRE(pointer != nullptr);

					Scalar deserialized = *pointer;
					REQUIRE(deserialized.ToString() == s.ToString());
				}
			}
		}*/

		WHEN(" checking if it contains a variable") {
			THEN(" we get yes") {
				REQUIRE(s.HasVariables());
			}
		}

		WHEN(" substituting the variable") {
			THEN(" for x=2 we have a number") {
				Scalar substituted = s.Substitute(Scalar("x"), Scalar(2));

				REQUIRE(substituted.IsNumeric());
				REQUIRE(substituted.ToDouble() == -7);
			}

			THEN(" inserting zero gives 3") {
				REQUIRE(s.Substitute(Scalar("x"), Scalar(0)).ToString() == "3");
			}

			THEN(" inserting 3/5 gives zero") {
				REQUIRE(s.Substitute(Scalar("x"), Scalar(3,5)).ToString() == "0");
			}

			THEN(" inserting a variable renames it") {
				REQUIRE(s.Substitute(Scalar("x"), Scalar("y")).ToString() == "3 - 5 * y");
			}

			THEN(" trying to replace the 3 does not work") {
				REQUIRE(s.Substitute(Scalar(3), Scalar(2)).ToString() == "3 - 5 * x");
			}
		}

	}

}
