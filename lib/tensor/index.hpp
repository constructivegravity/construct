#pragma once

#include <cassert>
#include <vector>
#include <sstream>
#include <map>

#include <common/printable.hpp>
#include <common/range.hpp>

namespace Albus {
	namespace Tensor {
		
		using Common::Printable;
		using Common::Range;
		
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
			//Index(Index&& other) 
			//  : name(std::move(other.name)), Printable(other.GetPrintedText())(std::move(other.printed_text)), range(std::move(other.range)) { }
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
			/**
				Equality operator
			 */
			bool operator==(const Index& other) const {
				return name == other.name && printed_text == other.printed_text;
			}
			
			/**
				Inequality operator
			 */
			bool operator!=(const Index& other) const {
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
			 */
			unsigned operator()(unsigned value) const {
				assert(value >= range.GetFrom() && value <= range.GetTo());
				return value;
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
			
			Index operator[](unsigned id) const {
				assert(id >= 0 && id < indices.size());
				return indices[id];
			}
			
			size_t Size() const { return indices.size(); }
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
			
			static Indices GetSeries(unsigned N, const std::string& name, const std::string& printed, const Range& range) {
				Indices result;
				for (unsigned i=1; i<=N; i++) {
					std::string fullName = "";
					std::string fullPrinted = "";
					
					// Generate full name
					{
						std::stringstream ss;
						ss << name << "_" << i;
						fullName = ss.str();
					}
					
					// Generate full printed text
					{
						std::stringstream ss;
						ss << printed << "_" << i;
						fullPrinted = ss.str();
					}
										
					result.indices.emplace_back(Index(fullName, fullPrinted, range));
				}
				return result;
			}
			
			static Indices GetGreekSeries(unsigned N, const Range& range) {
				assert(N <= GreekIndices.size());
				Indices result;
				for (unsigned i=0; i<N; i++) {
					std::string fullName = GreekIndices[i];
					std::string fullPrinted = GreekSymbols.at(fullName);
					result.indices.emplace_back(Index(fullName, fullPrinted, range));
				}
				return result;
			}
			
			static Indices GetRomanSeries(unsigned N, const Range& range) {
				assert(N <= 52);
				Indices result;
				for (char i=0; i<N; i++) {
					std::string fullName;
					fullName = 'a' + i;
					if (i >= 26) fullName = 'A' + (i-26);
					result.indices.emplace_back(Index(fullName, fullName, range));
				}
				return result;
			}
		private:
			std::vector<Index> indices;
		};
		
	}
}