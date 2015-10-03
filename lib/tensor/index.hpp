#pragma once

#include <cassert>
#include <vector>
#include <sstream>
#include <map>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <common/error.hpp>
#include <common/printable.hpp>
#include <common/range.hpp>

namespace Albus {
	namespace Tensor {
		
		using Common::Printable;
		using Common::Range;

		class IncompleteIndexAssignmentException : public Exception {
		public:
			IncompleteIndexAssignmentException()
				: Exception("Incomplete index assignment") { }
		};

		class IndexOutOfRangeException : public Exception {
		public:
			IndexOutOfRangeException() : Exception("The index assignment is out of range.") { }
		};
		
		/**
			\class Index
		
			\brief One single index
		
			Class for a single index. Note that this is abstractly and
			just marks a slot to plug a specific combination for the
			valid range of the index.
		 */
		class Index : public Printable {
		public:
			/**
				\brief Constructor of an index
			
				Constructor of an index. The (unique) name of an index has to be supplied
				and a printable version in form of LaTeX code. It is also important to give a 
				range to the index.
			 */
			Index(const std::string& name, const std::string& printable, const Range& range) 
				: Printable(printable), name(name), range(range) { }
			
			Index(const std::string& name, const Range& range) 
				: Printable(name), name(name), range(range) { }
			
			Index(const std::string& name) 
				: Printable(name), name(name), range(Range::SpaceRange()) { }
			
			// Copy constructor
			Index(const Index& other) : name(other.name), Printable(other.GetPrintedText()), range(other.range) { }
			// Move constructor (TODO)
			Index(Index&& other)
			  : name(std::move(other.name)), Printable(std::move(other.printed_text)), range(std::move(other.range)) { }
		public:
			/**
				Copy assignment operator
			 */
			Index& operator=(const Index& other) {
				name = other.name;
				printed_text = other.printed_text;
				range = other.range;
				return *this;
			}
			
			/**
				Move assignment operator
			 */
			Index& operator=(Index&& other) {
				name = std::move(other.name);
				printed_text = std::move(other.printed_text);
				range = std::move(other.range);
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
		private:
			friend class boost::serialization::access;

			template<class Archive>
			void serialize(Archive& ar, const unsigned int version) {
				ar & name;
				ar & printed_text;
				ar & range;
				ar & up;
			}
		private:
			std::string name;
			Range range;
			bool up = false;
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
		class Indices : public Printable {
		public:
			Indices() = default;
		
			Indices(std::initializer_list<Index> indices) {
				for (auto index : indices) {
					this->indices.emplace_back(index);
				}
			}
			
			Indices(const Index& index) {
				indices.push_back(index);
			}
			
			Indices(Index&& index) {
				indices.emplace_back(std::move(index));
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
			Indices Partial(const Range& range) const {
				Indices result;
				for (auto i : range) {
					result.indices.emplace_back(indices[i]);
				}
				return result;
			}
		public:
			void Insert(const Index& index) {
				indices.push_back(index);
			}
			
			void Insert(Index&& index) {
				indices.emplace_back(std::move(index));
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
				
				result = Partial(Range(1, indices.size()))(args...);

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
				assert(id >= 0 && id < indices.size());
				return indices[id];
			}

			Index operator[](const std::string& name) const {
				for (auto& index : indices) {
					if (index.GetName() == name) return index;
				}
				assert(false);
			}

			Index& operator[](unsigned id) {
				assert(id >= 0 && id < indices.size());
				return indices[id];
			}

			Index& operator[](const std::string& name) {
				for (auto& index : indices) {
					if (index.GetName() == name) return index;
				}
				assert(false && "Index not present");
			}

			size_t Size() const { return indices.size(); }
			int IndexOf(const Index& index) const {
				for (int i=0; i<indices.size(); i++)
					if (indices[i] == index) return i;
				return -1;
			}
 		public:
			virtual std::string ToString() const {
				std::stringstream ss;
				if (indices.size() > 1) ss << "{";
				
				for (auto& index : indices) {
					ss << index;
				}
				if (indices.size() > 1) ss << "}";
	
				return ss.str();
			}
		public:
			/**
				Get a partition of n indices. Note that this is up to permutation.

			 	TODO: parallelize this
			 */
			std::vector<std::pair<Indices, Indices>> GetAllPartitions(unsigned n) const {
				// Check if the number of indices we wish to draw from the indices is
				// smaller than the total number of indices.
				assert(n <= indices.size());

				std::vector< std::pair<Indices, Indices> > result;

				// Use a helper method
				std::function<void(const std::vector<unsigned>&, const std::vector<unsigned>&)> fn = [&](const std::vector<unsigned>& usedIndices, const std::vector<unsigned>& neglectedIndices) -> void {
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
						result.push_back({ used, remaining });
						return;
					}

					// If all the indices were used, but no partition is found yet, cancel
					if (usedIndices.size() + neglectedIndices.size() == indices.size()) {
						return;
					}

					// Obtain current index
					unsigned current = usedIndices.size() + neglectedIndices.size();

					// Consider it as used index
					{
						std::vector<unsigned> newUsedIndices = usedIndices;
						newUsedIndices.push_back(current);

						fn(newUsedIndices, neglectedIndices);
					}

					// Consider it as neglected index
					{
						std::vector<unsigned> newNeglectedIndices = neglectedIndices;
						newNeglectedIndices.push_back(current);

						fn(usedIndices, newNeglectedIndices);
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
						if (commutativityCheck(used))
							result.push_back(used);
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

			bool ContainsIndex(const Index& index) const {
				return std::find(indices.begin(), indices.end(), index) != indices.end();
			}
		private:
			friend class boost::serialization::access;

			template<class Archive>
			void serialize(Archive& ar, const unsigned int version) {
				ar & indices;
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