#pragma once

#include <cassert>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>

#include <tensor/expression.hpp>

#include <common/error.hpp>
#include <common/printable.hpp>
#include <common/range.hpp>
#include <common/serializable.hpp>
#include <common/task_pool.hpp>

namespace Construction {
	namespace Tensor {

		using Common::Printable;
		using Common::Range;
		using Common::Serializable;

		class IncompleteIndexAssignmentException : public Exception {
		public:
			IncompleteIndexAssignmentException()
				: Exception("Incomplete index assignment") { }
		};

		class IndexOutOfRangeException : public Exception {
		public:
			IndexOutOfRangeException() : Exception("The index assignment is out of range.") { }
		};

		class IndicesIncomparableException : public Exception {
		public:
			IndicesIncomparableException() : Exception("The given indices cannot be compared.") { }
		};

        /**
			\class CannotContractTensorsException
		 */
		class CannotContractIndicesException : public Exception {
		public:
			CannotContractIndicesException() : Exception("Cannot contract the indices. One or multiple indices occur more than once in a non-covariant fashion.") { }
		};


		static const std::map<std::string, std::string> GreekSymbols = {
				{ "alpha", "\\alpha" },
				{ "beta", "\\beta" },
				{ "gamma", "\\gamma" },
				{ "delta", "\\delta" },
				{ "epsilon", "\\epsilon" },
				{ "zeta", "\\zeta" },
				{ "eta", "\\eta" },
				{ "theta", "\\theta" },
				{ "iota", "\\iota" },
				{ "kappa", "\\kappa" },
				{ "lambda", "\\lambda" },
				{ "mu", "\\mu" },
				{ "nu", "\\nu" },
				{ "xi", "\\xi" },
				{ "omicron", "\\omicron" },
				{ "pi", "\\pi" },
				{ "rho", "\\rho" },
				{ "sigma", "\\sigma" },
				{ "tau", "\\tau" },
				{ "upsilon", "\\upsilon" },
				{ "phi", "\\varphi" },
				{ "chi", "\\chi" },
				{ "psi", "\\psi" },
				{ "omega", "\\omega" },

				{ "Alpha", "\\Alpha" },
				{ "Beta", "\\Beta" },
				{ "Gamma", "\\Gamma" },
				{ "Delta", "\\Delta" },
				{ "Epsilon", "\\Epsilon" },
				{ "Zeta", "\\Zeta" },
				{ "Eta", "\\Eta" },
				{ "Theta", "\\Theta" },
				{ "Iota", "\\Iota" },
				{ "Kappa", "\\Kappa" },
				{ "Lambda", "\\Lambda" },
				{ "Mu", "\\Mu" },
				{ "Nu", "\\Nu" },
				{ "Xi", "\\Xi" },
				{ "Omicron", "\\Omicron" },
				{ "Pi", "\\Pi" },
				{ "Rho", "\\Rho" },
				{ "Sigma", "\\Sigma" },
				{ "Tau", "\\Tau" },
				{ "Upsilon", "\\Upsilon" },
				{ "Phi", "\\Varphi" },
				{ "Chi", "\\Chi" },
				{ "Psi", "\\Psi" },
				{ "Omega", "\\Omega" },
		};

		static const std::vector<std::string> GreekIndices = {
				"mu", "nu", "kappa", "lambda", "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "xi", "omicron", "pi", "rho", "sigma", "tau", "upsilon", "phi", "chi", "psi", "omega",
				"Alpha", "Beta", "Gamma", "Delta", "Epsilon", "Zeta", "Eta", "Theta", "Iota", "Kappa", "Lambda", "Mu", "Nu", "Xi", "Omicron", "Pi", "Rho", "Sigma", "Tau", "Upsilon", "Phi", "Chi", "Psi", "Omega",
		};

		/**
			\class Index

			\brief One single index

			Class for a single index. Note that this is abstractly and
			just marks a slot to plug a specific combination for the
			valid range of the index.
		 */
		class Index : public Printable, Serializable<Index> {
		public:
			/**
				\brief Constructor of an index

				Constructor of an index. The (unique) name of an index has to be supplied
				and a printable version in form of LaTeX code. It is also important to give a
				range to the index.
			 */
			Index() : range(1,3) { }

			Index(const std::string& name, const std::string& printable, const Range& range)
				: Printable(printable), name(name), range(range) { }

			Index(const std::string& name, const Range& range)
				: Printable(name), name(name), range(range) { }

			Index(const std::string& name)
				: Printable(name), name(name), range(Range::SpaceRange()) { }

			// Copy constructor
			Index(const Index& other) : name(other.name), Printable(other.GetPrintedText()), range(other.range), up(other.up) { }
			// Move constructor (TODO)
			Index(Index&& other)
			  : name(std::move(other.name)), Printable(std::move(other.printed_text)), range(std::move(other.range)), up(std::move(other.up)) { }
		public:
			/**
				Copy assignment operator
			 */
			Index& operator=(const Index& other) {
				name = other.name;
				printed_text = other.printed_text;
				range = other.range;
                up = other.up;
				return *this;
			}

			/**
				Move assignment operator
			 */
			Index& operator=(Index&& other) {
				name = std::move(other.name);
				printed_text = std::move(other.printed_text);
				range = std::move(other.range);
                up = std::move(other.up);
				return *this;
			}
		public:
			inline std::string GetName() const {
				return name;
			}

			inline Range GetRange() const {
				return range;
			}

			inline bool IsContravariant() const {
				return up;
			}

			inline void SetContravariant(bool value) {
				up = value;
			}
		public:
			/**
				Equality operator
			 */
			inline bool operator==(const Index& other) const {
				return name == other.name && printed_text == other.printed_text;
			}

			/**
				Inequality operator
			 */
			inline bool operator!=(const Index& other) const {
				return name != other.name || printed_text != other.printed_text;
			}

			bool operator<(const Index& other) const {
				// Compare roman indices
				if (IsRomanIndex()) {
					if (!other.IsRomanIndex()) throw IndicesIncomparableException();

					int posX = name[0] - 'a';
					if (posX < 0) posX = name[0] - 'A' + 26;

					int posY = other.name[0] - 'a';
					if (posY < 0) posY = other.name[0] - 'A' + 26;

					return posX < posY;
				}
				// Compare greek indices
				else if (IsGreekIndex()) {
					if (!other.IsGreekIndex()) throw IndicesIncomparableException();

					return std::distance(GreekSymbols.find(name), GreekSymbols.find(other.name)) > 0;
				}
				// Compare series indices
				else if (IsSeriesIndex()) {
					if (!other.IsSeriesIndex()) throw IndicesIncomparableException();

					// If the prefix does not match also throw exception
					int posDash = name.find("_");
					if (name.substr(0, posDash) != other.name.substr(0, name.find("_"))) {
						throw IndicesIncomparableException();
					}

					int posX = atoi(name.substr(posDash+1, name.length()-posDash-1).c_str());
					int posY = atoi(other.name.substr(posDash+1, other.name.length()-posDash-1).c_str());

					return posX < posY;
				}
				// cannot compare the indices
				else throw IndicesIncomparableException();
			}

			inline bool operator<=(const Index& other) const {
				return (*this == other) || (*this < other);
			}

			bool operator>(const Index& other) const {
				// Compare roman indices
				if (IsRomanIndex()) {
					if (!other.IsRomanIndex()) throw IndicesIncomparableException();

					int posX = name[0] - 'a';
					if (posX < 0) posX = name[0] - 'A' + 26;

					int posY = other.name[0] - 'a';
					if (posY < 0) posY = other.name[0] - 'A' + 26;

					return posX > posY;
				}
					// Compare greek indices
				else if (IsGreekIndex()) {
					if (!other.IsGreekIndex()) throw IndicesIncomparableException();

					return std::distance(GreekSymbols.find(name), GreekSymbols.find(other.name)) < 0;
				}
					// Compare series indices
				else if (IsSeriesIndex()) {
					if (!other.IsSeriesIndex()) throw IndicesIncomparableException();

					// If the prefix does not match also throw exception
					int posDash = name.find("_");
					if (name.substr(0, posDash) != other.name.substr(0, name.find("_"))) {
						throw IndicesIncomparableException();
					}

					int posX = atoi(name.substr(posDash, name.length()-posDash).c_str());
					int posY = atoi(other.name.substr(posDash, other.name.length()-posDash).c_str());

					return posX > posY;
				}
					// cannot compare the indices
				else throw IndicesIncomparableException();
			}

			inline bool operator>=(const Index& other) const {
				return (*this == other) || (*this > other);
			}
		public:
			/**
				\brief Application functor

				This functor consists of an assertion that garantuees
				that the supplied index value is within the range
				of the index. Otherwise an error is thrown.
				If the test passes, then the index is returned again.

				\param value	The index that may be applied

			 	\throws IndexOutOfRangeException
			 */
			unsigned operator()(unsigned value) const {
				if (!(value >= range.GetFrom() && value <= range.GetTo())) {
					throw IndexOutOfRangeException();
				}
				return value;
			}
		public:
			/**
				\brief Checks if the index is roman

			 	Sometimes it is necessary to know if indices are roman, especially
			 	when comparing indices. Checks if the name has length, is equal to its TeX code
			 	one and lies in the correct range.
			 */
			bool IsRomanIndex() const {
				return name.length() == 1 && name == printed_text && ((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z'));
			}

			/**
				\brief Checks if the index is greek

			 	Sometimes it is necessary to know if indices are roman, especially
			 	when comparing indices. Checks if the name is part of the greek symbols list
			 	and the TeX code matches.
			 */
			bool IsGreekIndex() const {
				return GreekSymbols.find(name) != GreekSymbols.end() && GreekSymbols.at(name) == printed_text;
			}

			/**
				\brief Checks if the index is part of a series

			 	Sometimes it is necessary to know if indices are part of a series, e.g. \alpha_1, especially
			 	when comparing indices. Checks if the name and TeX code contain "_".
			 */
			bool IsSeriesIndex() const {
				return name.find("_") != std::string::npos && printed_text.find("_") != std::string::npos;
			}
		public:
			void Serialize(std::ostream& os) const {
				os << name << ";";
				os << printed_text << ";";
				range.Serialize(os);
			}

			static std::unique_ptr<Index> Deserialize(std::istream& is) {
				// read name
				std::string name;
				std::getline(is, name, ';');

				// read printed_text
				std::string printed_text;
				std::getline(is, printed_text, ';');

				// read range
				auto rangePtr = Range::Deserialize(is);

				return std::move(std::unique_ptr<Index>(new Index(name, printed_text, *rangePtr)));
			}
		private:
			std::string name;
			Range range;
			bool up = false;
		};

		class Indices;

		/**
			\class IndexAssignments
		 */
		class IndexAssignments {
		public:
			IndexAssignments() = default;

			IndexAssignments(const IndexAssignments& other) : assignment(other.assignment) { }
			IndexAssignments(IndexAssignments&& other) : assignment(std::move(other.assignment)) { }
		public:
			IndexAssignments& operator=(const IndexAssignments& other) {
				assignment = other.assignment;
				return *this;
			}

			IndexAssignments& operator=(IndexAssignments&& other) {
				assignment = std::move(other.assignment);
				return *this;
			}
		public:
			unsigned& operator[](const std::string& name) {
				return assignment[name];
			}
		public:
			std::vector<unsigned> operator()(const Indices&) const;

			friend std::ostream& operator<<(std::ostream& os, const IndexAssignments& assignment) {
				os << "{ ";
				int i=0;
				for (auto& it : assignment.assignment) {
					os << "\"" << it.first << "\" => " << it.second;
					if (i != assignment.assignment.size()-1) os << " , ";
					i++;
				}
				os << " }";
				return os;
			}
		private:
			std::map<std::string, unsigned> assignment;
		};

		/**
			\class Indices
		 */
		class Indices : public Serializable<Indices>, public AbstractExpression {
		public:
			Indices() { }

			Indices(std::initializer_list<Index> indices) {
				for (auto index : indices) {
					this->indices.emplace_back(index);
				}
			}

			Indices(const Index& index) {
				indices.push_back(index);
			}

			Indices(Index&& index) {
				indices.push_back(std::move(index));
			}

			Indices(const Indices& other)
				: indices(other.indices) { }

			Indices(Indices&& other)
				: indices(std::move(other.indices)) { }
		public:
			Indices& operator=(const Indices& other) {
				indices = other.indices;
				return *this;
			}

			Indices& operator=(Indices&& other) {
				indices = std::move(other.indices);
				return *this;
			}
		public:
			/**
				\brief Returns the partial indices in the given range
			 */
			Indices Partial(const Range& range) const {
				Indices result;
				for (auto i : range) {
					result.indices.push_back(indices[i]);
				}
				return result;
			}
		public:
			void Insert(const Index& index) {
				indices.push_back(index);
			}

			void Insert(Index&& index) {
				indices.push_back(std::move(index));
			}

			void Remove(unsigned id) {
				//assert(id < indices.size());
				indices.erase(indices.begin() + id);
			}

			void Append(const Indices& other) {
				indices.insert(indices.end(), other.indices.begin(), other.indices.end());
			}

			void Clear() {
				indices.clear();
			}
		public:
			bool operator==(const Indices& other) const {
				if (indices.size() != other.indices.size()) return false;
				for (unsigned i=0; i < indices.size(); i++) {
					if (indices[i] != other.indices[i]) return false;
				}
				return true;
			}

			bool operator!=(const Indices& other) const {
				if (indices.size() != other.indices.size()) return true;
				for (unsigned i=0; i < indices.size(); i++) {
					if (indices[i] != other.indices[i]) return true;
				}
				return false;
			}
		public:
			template<typename T, typename... Args>
			std::vector<unsigned> operator()(T t, Args... args) const {
				std::vector<unsigned> result;

				result = Partial(Range(1, indices.size()-1))(args...);
				result.insert(result.begin(), (indices.at(0))(t));

				return result;
			}

			std::vector<unsigned> operator()() const {
				return std::vector<unsigned>();
			}
		public:
			std::vector<Index>::iterator begin() { return indices.begin(); }
			std::vector<Index>::iterator end() { return indices.end(); }

			std::vector<Index>::const_iterator begin() const { return indices.begin(); }
			std::vector<Index>::const_iterator end() const { return indices.end(); }

			Index operator[](unsigned id) const {
				if (id >= indices.size()) throw IndexOutOfRangeException();
				return indices[id];
			}

			Index operator[](const std::string& name) const {
				for (auto& index : indices) {
					if (index.GetName() == name) return index;
				}
				throw IndexOutOfRangeException();
			}

			Index& operator[](unsigned id) {
				if (id >= indices.size()) throw IndexOutOfRangeException();
				return indices[id];
			}

			Index& operator[](const std::string& name) {
				for (auto& index : indices) {
					if (index.GetName() == name) return index;
				}
				throw IndexOutOfRangeException();
			}

			size_t Size() const { return indices.size(); }
			int IndexOf(const Index& index) const {
				for (int i=0; i<indices.size(); i++)
					if (indices[i] == index) return i;
				return -1;
			}
        public:
            bool operator<(const Indices& other) const {
                for (int i=0; i<std::min(Size(), other.Size()); ++i) {
                    if (indices[i] < other[i]) return true;
                    else if (indices[i] > other[i]) return false;
                }

                // The indices so far were equal
                return Size() < other.Size();
            }

            bool operator>(const Indices& other) const {
                for (int i = 0; i < std::min(Size(), other.Size()); ++i) {
                    if (indices[i] > other[i]) return true;
                    else if (indices[i] < other[i]) return false;
                }

                // The indices so far were equal
                return Size() > other.Size();
            }
        public:
			virtual std::string ToString() const override {
				std::stringstream ss;

				bool lastOneWasDown=true;

                if (indices.size() > 1) {
                    if (indices[0].IsContravariant()) {
                        lastOneWasDown=false;
                        ss << "^{";
                    } else {
                        ss << "_{";
                    }
                } else if (indices.size() == 1) {
                    if (indices[0].IsContravariant()) ss << "^";
                    else ss << "_";
                }

				for (auto& index : indices) {
					if (index.IsContravariant() && lastOneWasDown) {
						lastOneWasDown = false;
						ss << "}^{";
					} else if (!index.IsContravariant() && !lastOneWasDown) {
						lastOneWasDown = true;
						ss << "}_{";
					}
					ss << index;
				}
				if (indices.size() > 1) ss << "}";

				return ss.str();
			}

			std::string ToCommand() const {
				std::stringstream ss;
				ss << "{";

				for (int i=0; i< indices.size(); i++) {
					ss << indices[i];
					if (i != indices.size()-1) ss << " ";
				}

				ss << "}";

				return ss.str();
			}
		public:
			/**
				Get a partition of n indices. Note that this is up to permutation.
			 */
			std::vector<std::pair<Indices, Indices>> GetAllPartitions(unsigned n) const {
				// Check if the number of indices we wish to draw from the indices is
				// smaller than the total number of indices.
				if (n > indices.size()) throw IndexOutOfRangeException();

				std::vector< std::pair<Indices, Indices> > result;

				/*std::mutex mutex, finishedMutex;
				std::vector<std::thread> threads;
				bool finished = false;*/

				// Calculate the number of partitions
				std::function<int(unsigned, unsigned)> binomialCoeff = [&](unsigned n, unsigned k) -> int {
					if (k == n || k == 0) return 1;
					return binomialCoeff(n-1, k-1) + binomialCoeff(n-1, k);
				};
				int size = binomialCoeff(indices.size(), n);

				// Use a helper method
				std::function<void(std::vector<unsigned>, std::vector<unsigned>)> fn = [&](std::vector<unsigned> usedIndices, std::vector<unsigned> neglectedIndices) -> void {
					// If we have enough used indices finish the partition
					if (usedIndices.size() == n) {
						Indices used;
						Indices remaining;

						// Add all the used indices to the used list
						for (auto& index : usedIndices) used.Insert(indices[index]);

						// Add all the so far unused indices to the remaining list
						for (auto& index : neglectedIndices) remaining.Insert(indices[index]);

						// Fill the unused indices
						for (unsigned current = usedIndices.size() + neglectedIndices.size(); current < indices.size(); current++) {
							remaining.Insert(indices[current]);
						}

						// Add partition
						{
							//std::unique_lock<std::mutex> lock(mutex);
							result.push_back({used, remaining});
						}
						return;
					}

					// If all the indices were used, but no partition is found yet, cancel
					if (usedIndices.size() + neglectedIndices.size() == indices.size()) {
						return;
					}

					// Obta in current index
					unsigned current = usedIndices.size() + neglectedIndices.size();

					// Consider it as used index
					{
						std::vector<unsigned> newUsedIndices = usedIndices;
						std::vector<unsigned> newNeglectedIndices = neglectedIndices;

						newUsedIndices.push_back(current);

						fn(newUsedIndices, newNeglectedIndices);
						//pool.Enqueue(fn, newUsedIndices, newNeglectedIndices);
						/*std::unique_lock<std::mutex> lock(finishedMutex);
						if (!finished) threads.emplace_back(std::thread(fn, newUsedIndices, newNeglectedIndices));*/
					}

					// Consider it as neglected index
					{
						std::vector<unsigned> newUsedIndices = usedIndices;
						std::vector<unsigned> newNeglectedIndices = neglectedIndices;

						newNeglectedIndices.push_back(current);

						//pool.Enqueue(fn, newUsedIndices, newNeglectedIndices);
						fn(newUsedIndices, newNeglectedIndices);
						/*std::unique_lock<std::mutex> lock(finishedMutex);
						if (!finished) threads.emplace_back(std::thread(fn, newUsedIndices, newNeglectedIndices));*/
					}
				};

				// Start recursion
				fn({}, {});

				return result;
			}

			/**
				Get all the partitions of a series of indices in
			 */
			std::vector< std::vector<Indices> > GetAllPartitions(const std::vector<unsigned>& partitions, bool commutative=false) const {
				// Check if the request partitions are valid
				unsigned total = 0;
				for (auto& i : partitions) {
					total += i;
				}
				assert(total == indices.size());

				std::vector< std::vector<Indices> > result;

				// Helper function for permutation
				// Based on Heap's algorithm to generate all permutations of an element
				std::function<bool(std::vector<Indices>&, unsigned)> checkAllPermutations;
				checkAllPermutations = [&](std::vector<Indices>& input, unsigned n) -> bool {
					if (n == 1) {
						return std::find(result.begin(), result.end(), input) == result.end();
					} else {
						for (int i=0; i<n; i++) {
							bool res = checkAllPermutations(input, n-1);
							if (!res) return false;
							if (n % 2 == 1) {
								std::swap(input[0], input[n-1]);
							} else {
								std::swap(input[i], input[n-1]);
							}
						}
					}

					return true;
				};

				// Helper function for commutativity check
				std::function<bool(const std::vector<Indices>&)> commutativityCheck;
				if (commutative) {
					commutativityCheck = [&](const std::vector<Indices>& input) -> bool {
						std::vector<Indices> temporary = input;
						return checkAllPermutations(temporary, temporary.size());
					};
				} else {
					commutativityCheck = [&](const std::vector<Indices>&) -> bool {
						return true;
					};
				}

				// Helper function
				std::function< void(const std::vector<unsigned>&, const std::vector<Indices>&, const Indices&) > fn = [&](const std::vector<unsigned>& usedPartitions, const std::vector<Indices>& used, const Indices& remaining) -> void {
					// If we have used all partitions, add partition to the result
					if (usedPartitions.size() == partitions.size() && remaining.Size() == 0) {
						if (commutativityCheck(used)) result.push_back(used);
						return;
					}

					// Get current
					unsigned current = partitions[usedPartitions.size()];

					// Partition the remaining indices
					auto part = remaining.GetAllPartitions(current);
					for (auto& p : part) {
						auto newUsedPartitions = usedPartitions;
						newUsedPartitions.push_back(current);

						auto newUsed = used;
						newUsed.push_back(p.first);

						fn(newUsedPartitions, newUsed, p.second);
					}
				};

				fn({}, {}, *this);
				/*std::vector<unsigned> used1;
				std::vector<Indices> used2;
				pool.Enqueue(fn, used1, used2, *this);*/

				return result;
			}

            /**
				\brief Returns all the possible index combinations for the tensor.

			 	Returns all the possible index combinations for the tensor.
			 	It uses a recursive inline methods that fixes the indices one by one
			 	until all indices have a value. In this case the combination is added to the
			 	result.
			 */
			std::vector<std::vector<unsigned>> GetAllIndexCombinations() const {

				// Result
				std::vector<std::vector<unsigned>> result;

				// Helper method to recursively determine the index combinations
				std::function<void(const std::vector<unsigned>&)> fn = [&](const std::vector<unsigned>& input) -> void {
					// If all indices are fixed, add the combination to the list
					if (input.size() == Size()) {
						result.push_back(input);
						return;
					}

					// Get range of next unfixed index
					auto range = indices[input.size()].GetRange();

					// Iterate over the range
					for (auto i : range) {
						// Add the index to the list
						std::vector<unsigned> newInput = input;
						newInput.push_back(i);

						// Recursive call to go to next index
						fn(newInput);
					}
				};

				// Start recursion
				std::vector<unsigned> input;
				fn({});

				return result;
			}
		public:
			static Indices GetSeries(unsigned N, const std::string& name, const std::string& printed, const Range& range, unsigned offset=0) {
				Indices result;
				for (unsigned i=1; i<=N; i++) {
					std::string fullName = "";
					std::string fullPrinted = "";

					// Generate full name
					{
						std::stringstream ss;
						ss << name << "_" << (i+offset);
						fullName = ss.str();
					}

					// Generate full printed text
					{
						std::stringstream ss;
						ss << printed << "_" << (i+offset);
						fullPrinted = ss.str();
					}

					result.indices.emplace_back(Index(fullName, fullPrinted, range));
				}
				return result;
			}

			static Indices GetGreekSeries(unsigned N, const Range& range, unsigned offset=0) {
				assert(N+offset <= GreekIndices.size());
				Indices result;
				for (unsigned i=0; i<N; i++) {
					std::string fullName = GreekIndices[i+offset];
					std::string fullPrinted = GreekSymbols.at(fullName);
					result.indices.emplace_back(Index(fullName, fullPrinted, range));
				}
				return result;
			}

			static Indices GetRomanSeries(unsigned N, const Range& range, unsigned offset=0) {
				assert(N+offset <= 52);
				Indices result;
				for (char i=0; i<N; i++) {
					std::string fullName;
					fullName = 'a' + i+offset;
					if (i+offset >= 26) fullName = 'A' + (i+offset-26);
					result.indices.emplace_back(Index(fullName, fullName, range));
				}
				return result;
			}

            static Indices GetNamed(const std::vector<std::string>& indices, const Range& range = {1,3}) {
                Indices result;

                for (auto& index : indices) {
                    result.Insert(Index(index, range));
                }

                return result;
            }
        public:
            Indices Shuffle(std::map<Index, Index> transformation) const {
                Indices result;

                for (auto& index : indices) {
                    if (transformation.find(index) == transformation.end()) {
                        result.Insert(index);
                    } else {
						result.Insert(transformation[index]);
					}
                }

                return result;
            }
        public:
			/**
				\brief Returns if the given indices are a permutation of the given ones

			 	In some cases it is necessary to determine if some set of indices contain the same
			 	symbols but are permuted. This is especially essential for the contraction of indices
			 	and generation of multiplied and added tensors.

			 	\param[in] other	The other set of indices
			 	\return is the index of a permutation of the other
			 */
			bool IsPermutationOf(const Indices& other) const {
				// if the number of indices does not match then clearly not
				if (other.indices.size() != indices.size()) return false;

				// Iterate over all indices
				for (auto& index : other.indices) {
					if (std::find(indices.begin(), indices.end(), index) == indices.end()) return false;
				}

				// All indices were present => it is a permutation
				return true;
			}

			static bool IsPermutationOf(const Indices& one, const Indices& other) {
				// if the number of indices does not match then clearly not
				if (other.indices.size() != one.indices.size()) return false;

				// Iterate over all indices
				for (auto& index : other.indices) {
					if (std::find(one.indices.begin(), one.indices.end(), index) == one.indices.end()) return false;
				}

				// All indices were present => it is a permutation
				return true;
			}

			static bool IsPermutationOf(const std::vector<int>& one, const std::vector<int>& other) {
				// if the number of indices does not match then clearly not
				if (other.size() != one.size()) return false;

				// Iterate over all indices
				for (auto& index : other) {
					if (std::find(one.begin(), one.end(), index) == one.end()) return false;
				}

				// All indices were present => it is a permutation
				return true;
			}

			static bool IsPermutationOf(const std::vector<unsigned>& one, const std::vector<unsigned>& other) {
				// if the number of indices does not match then clearly not
				if (other.size() != one.size()) return false;

				// Iterate over all indices
				for (auto& index : other) {
					if (std::find(one.begin(), one.end(), index) == one.end()) return false;
				}

				// All indices were present => it is a permutation
				return true;
			}

			bool ContainsIndex(const Index& index) const {
				return std::find(indices.begin(), indices.end(), index) != indices.end();
			}
        public:
            static Indices FromString(std::string code) {
                Indices indices;

                std::string current;

                // Ignore brackets at the beginning and end if present
                if (code[0] == '{') code = code.substr(1);
                if (code[code.size()-1] == '}') code = code.substr(0, code.size()-1);

                for (int pos=0; pos<code.length(); pos++) {
                    char c = code[pos];

                    if (c == ' ') {
                        if (current.length() == 0) break;

                        Tensor::Index index(current, current, {1,3});
                        if (indices.ContainsIndex(index)) {
                            // throw error
                            return indices;
                        }

                        indices.Insert(index);
                        current = "";
                    } else {
                        current.append(std::string(1,c));
                    }
                }

                if (current.length() != 0) {
                    Tensor::Index index(current, current, {1, 3});
                    if (indices.ContainsIndex(index)) {
                        // throw error
                        return indices;
                    }

                    indices.Insert(index);
                }

                return indices;
            }
		public:
			/**
				\brief Check if the indices are in order

				Check if the indices are in order, i.e. each is
			 	smaller than the next. Order is defined only for
			 	Roman and Greek letters and series ala \beta_i.

			 	Necessary to implement the canonicalization of
			 	indices.

			 	\throws IndicesIncompatibleException
			 */
			bool IsNormalOrdered() const {
				// If no indices present, trivially in order
				if (indices.size() == 0) return true;

				// Dummy variable to keep the last index
				auto last = indices[0];

				// Iterate over the remaining indices
				for (unsigned i=1; i<indices.size(); i++) {
					// if the new index is smaller than the previous
					// return false
					if (indices[i] < last) return false;

					// Change the dummy
					last = indices[i];
				}

				// The indices are in order due to transitivity of order
				return true;
			}

			/**
				\brief Return the ordered
			 */
			Indices Ordered() const {
				// Copy the indices
				Indices result = *this;

				// Sort the indices by order
				std::sort(result.begin(), result.end());

				// Return the ordered indices
				return result;
			}
        public:
            /**

             */
            bool ContainsContractions() const {
                bool result = false;
                std::vector<Index> copy = indices;
                std::vector<Index> duplicates;

                for (int i=0; i<copy.size(); ++i) {
                    // Already used
                    if (std::find(duplicates.begin(), duplicates.end(), copy[i]) != duplicates.end()) {
                        throw CannotContractIndicesException();
                    }

                    // Is contravariant?
                    bool isContravariant = copy[i].IsContravariant();

                    for (int j=i+1; j<copy.size(); ++j) {
                        if (copy[i] == copy[j]) {
                            if ((isContravariant && !copy[j].IsContravariant()) ||
                                (!isContravariant && copy[j].IsContravariant()))
                            {
                                result = true;
                                copy.erase(copy.begin() + j);
                                duplicates.push_back(copy[i]);
                                break;
                            } else {
                                throw CannotContractIndicesException();
                            }
                        }
                    }
                }

                return result;
            }

            /**
                \brief Contract two indices

                Contract two indices. This will return the resulting
                index structure after the tensors with the given indices are
                contracted, i.e.

                    Contract(_{abc}, ^{a}_{de}) = _{bcde}

                \throws CannotContractIndicesException
             */
            Indices Contract(const Indices& other) const {
                std::vector<Index> other_ = other.indices;
                Indices result;

                // Iterate over all indices
                for (auto& index : indices) {
                    // Is contravariant?
                    bool isContravariant = index.IsContravariant();

                    // Look for the index in the other list
                    auto it = std::find(other_.begin(), other_.end(), index);

                    // If it is not contained, add the index to the result
                    if (it == other_.end()) {
                        result.Insert(index);
                        continue;
                    }

                    // If the index is contained, it has to has the other orientation
                    if ((isContravariant && !it->IsContravariant()) ||
                        (!isContravariant && it->IsContravariant()))
                    {
                        // Delete the index from the output
                        other_.erase(it);

                        continue;
                    }

                    // Indices have the same orientation => exception
                    throw CannotContractIndicesException();
                }

                // Add the remaining indices in the other tensor
                for (auto& index : other_) {
                    result.Insert(index);
                }

                return result;
            }
		public:
			virtual std::unique_ptr<AbstractExpression> Clone() const override { return std::move(ExpressionPointer(new Indices(*this))); }
			virtual bool IsIndicesExpression() const override { return true; }
		public:
			void Serialize(std::ostream& os) const override {
				// Write the size of the indices
				unsigned size = indices.size();
				os.write(reinterpret_cast<const char*>(&size), sizeof(size));

				// Write the N indices
				for (auto& index : indices) {
					index.Serialize(os);
				}
			}

			static std::unique_ptr<AbstractExpression> Deserialize(std::istream& is) {
				// Read size
				unsigned size;
				is.read(reinterpret_cast<char*>(&size), sizeof(unsigned));

				// Create result
				auto result = std::unique_ptr<Indices>(new Indices());

				// Read indices
				for (int i=0; i<size; i++) {
					auto indexPtr = Index::Deserialize(is);
					result->Insert(*indexPtr);
				}

				return std::move(result);
			}
		private:
			std::vector<Index> indices;
		};

		std::vector<unsigned> IndexAssignments::operator()(const Indices& indices) const {
			std::vector<unsigned> result;

			// Check if all the indices in the tensor are present in the assignment
			for (auto& index : indices) {
				auto it = assignment.find(index.GetName());

				if (it == assignment.end()) {
					throw IncompleteIndexAssignmentException();
				} else {
					// push the assignment to the list
					result.push_back(it->second);
				}
			}

			return result;
		}

	}
}
