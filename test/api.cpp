#include <language/api.hpp>

SCENARIO("API methods", "[api]") {

	GIVEN(" two epsilons") {
		auto epsilon1 = Construction::Language::API::Epsilon({ {"a", {1,3}}, {"b", {1,3}}, {"c", {1,3}} });
		auto epsilon2 = Construction::Language::API::Epsilon({ {"b", {1,3}}, {"a", {1,3}}, {"c", {1,3}} });

		WHEN(" adding them") {
			auto t = epsilon1 + epsilon2;

			REQUIRE(t.IsZero());
		}
	}

	GIVEN(" four indices") {
		auto indices = Construction::Tensor::Indices::GetRomanSeries(4, {1,3});

		WHEN(" creating an arbitrary tensor") {
			auto arbitrary = Construction::Language::API::Arbitrary(indices);

			THEN(" we get two gammas") {
				REQUIRE(arbitrary.ToString() == "e_1 * \\gamma_{ab}\\gamma_{cd} + \ne_2 * \\gamma_{ac}\\gamma_{bd} + \ne_3 * \\gamma_{ad}\\gamma_{bc}");
			}

		}

		WHEN(" symmetrizing the arbitrary tensor") {
			auto arbitrary = Construction::Language::API::Arbitrary(Construction::Tensor::Indices::GetRomanSeries(4, {1,3}));
			auto symmetrized = Construction::Language::API::Symmetrize(arbitrary, { {"a", {1,3}}, {"c", {1,3}} }).Simplify();

			REQUIRE(symmetrized.ToString() == "(1/2 * e_1 + 1/2 * e_3) * (\\gamma_{ab}\\gamma_{cd} + \\gamma_{cb}\\gamma_{ad}) + e_2 * \\gamma_{ac}\\gamma_{bd}");

			auto redefined = symmetrized.RedefineVariables("e");
			REQUIRE(redefined.ToString() == "e_1 * (\\gamma_{ab}\\gamma_{cd} + \\gamma_{cb}\\gamma_{ad}) + e_2 * \\gamma_{ac}\\gamma_{bd}");

		}
	}

	GIVEN(" five indices") {
		auto indices = Construction::Tensor::Indices::GetRomanSeries(5, {1,3});

		using Construction::Tensor::Scalar;

		WHEN(" creating an arbitrary tensor") {
			auto arbitrary = Construction::Language::API::Arbitrary(indices);

			THEN(" we have 10 summands") {
				REQUIRE(arbitrary.GetSummands().size() == 10);
			}

			THEN(" we have less linear independent terms") {
				auto independent = arbitrary.Simplify();
				REQUIRE(independent.ToString() == "(e_1 + e_7 + e_8) * \\epsilon_{abc}\\gamma_{de} + (e_2 - e_7 + e_9) * \\epsilon_{abd}\\gamma_{ce} + (e_3 - e_8 - e_9) * \\epsilon_{abe}\\gamma_{cd} + (e_4 + e_7 + e_10) * \\epsilon_{acd}\\gamma_{be} + (e_5 + e_8 - e_10) * \\epsilon_{ace}\\gamma_{bd} + (e_6 + e_9 + e_10) * \\epsilon_{ade}\\gamma_{bc}");

				// With redefined variables, get six terms
				auto redefined = independent.RedefineVariables("e");
				REQUIRE(redefined.ToString() == "e_1 * \\epsilon_{abc}\\gamma_{de} + e_2 * \\epsilon_{abd}\\gamma_{ce} + e_3 * \\epsilon_{abe}\\gamma_{cd} + e_4 * \\epsilon_{acd}\\gamma_{be} + e_5 * \\epsilon_{ace}\\gamma_{bd} + e_6 * \\epsilon_{ade}\\gamma_{bc}");
			}

		}

	}

}