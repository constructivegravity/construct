#include <tensor/index.hpp>

SCENARIO("Tensor indices", "[indices]") {

    GIVEN(" an indices object") {

        auto indices = Construction::Tensor::Indices::GetRomanSeries(3, {1,3});

        WHEN(" printing the TeX code") {
            THEN(" we have {abc}") {
                REQUIRE(indices.ToString() == "_{abc}");
            }
        }

        WHEN(" taking a partial") {
            auto partial = indices.Partial({1,2});

            THEN(" the TeX code has no a anymore") {
                REQUIRE(partial.ToString() == "_{bc}");
            }
        }

        WHEN(" applying components to the indices") {

            THEN(" assignments in the range return the assignment") {
                auto res = indices(1,2,3);
                std::vector<unsigned> assignment = {1,2,3};

                REQUIRE(res == assignment);
            }

            THEN(" and out-of range index should throw an exception") {
                REQUIRE_THROWS_AS(indices(0,1,1), Construction::Tensor::IndexOutOfRangeException);
            }

        }

        WHEN(" comparing two indices") {

            auto indexRoman1 = Construction::Tensor::Indices::GetRomanSeries(1, {1,3}, 3)[0];
            auto indexRoman2 = Construction::Tensor::Indices::GetRomanSeries(1, {1,3}, 8)[0];

            auto indexGreek1 = Construction::Tensor::Indices::GetGreekSeries(1, {1,3}, 2)[0];
            auto indexGreek2 = Construction::Tensor::Indices::GetGreekSeries(1, {1,3}, 15)[0];

            auto indexSeries1 = Construction::Tensor::Indices::GetSeries(1, "alpha", "\\alpha", {1,3}, 4)[0];
            auto indexSeries2 = Construction::Tensor::Indices::GetSeries(1, "alpha", "\\alpha", {1,3}, 7)[0];

            THEN(" they are all unequal") {
                REQUIRE(indexRoman1 != indexRoman2);
                REQUIRE(indexGreek1 != indexGreek2);
                REQUIRE(indexSeries1 != indexSeries2);

                REQUIRE(indexRoman1 != indexGreek1);
                REQUIRE(indexRoman1 != indexSeries1);
            }

            THEN(" the first is smaller than the second") {
                REQUIRE(indexRoman1 < indexRoman2);
                REQUIRE(indexGreek1 < indexGreek2);
                REQUIRE(indexSeries1 < indexSeries2);
            }

            THEN(" greeks and romans are incomparable") {
                REQUIRE_THROWS_AS(indexRoman1 < indexGreek1, Construction::Tensor::IndicesIncomparableException);
            }

        }

        WHEN(" considering normal ordering") {

            Construction::Tensor::Indices permuted;
            permuted.Insert(indices[2]);
            permuted.Insert(indices[0]);
            permuted.Insert(indices[1]);

            THEN(" the original indices are normal ordered") {
                REQUIRE(indices.IsNormalOrdered());
            }

            THEN(" the permuted indices are not") {
                REQUIRE(!permuted.IsNormalOrdered());
            }

        }

        WHEN(" seralizing indices") {

            std::stringstream ss;
            indices.Serialize(ss);
            std::string output = ss.str();

            THEN(" the deserialized indices are the same") {
                std::stringstream st (output);
                auto newIndices = Construction::Tensor::Indices::Deserialize(st);

                REQUIRE(*newIndices == indices);
            }

        }

        WHEN(" looking at all partitions") {
            auto newIndices = Construction::Tensor::Indices::GetRomanSeries(6, {1,3});
            auto result = newIndices.GetAllPartitions({2,2,2}, true);

            REQUIRE(result.size() == 15);

        }

    }

}