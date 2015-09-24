#pragma once

namespace Albus {
	namespace Common {
		
		/**
			\class Range
		
			\brief Representation of a [a,b] range in the natural numbers
		
			This class represents a range [a,b] of the natural numbers. It
			is for example used for the indices of the tensor that may range
			from 1 to D or 0 to D.
		 */
		class Range {
		public:
			Range(unsigned from, unsigned to) : from(from), to(to) { }
			
			// Copy constructor
			Range(const Range& other) : from(other.from), to(other.to) { }
			
			// Move constructor
			Range(Range&& other) : from(std::move(other.from)), to(std::move(other.to)) { }
		public:
			/**
				Copy assignment operator
			 */
			Range& operator=(const Range& other) {
				from = other.from;
				to = other.to;
				return *this;
			}
			
			/**
				Move assignment operator (C++11 only)
			 */
			Range& operator=(Range&& other) {
				from = std::move(other.from);
				to = std::move(other.to);
				return *this;
			}
		public:
			unsigned GetFrom() const { return from; }
			unsigned GetTo() const { return to; }
		public:
			/**
				\class Iterator
			
				\brief Range iterator
			
				An iterator for ranges. In C++11 this allows the following syntax
			
					for (auto i : range) {
						// do calculation
					}
				
				Before one can use the standard iterator syntax
			
					for (Range::Iterator it = range.begin(); it != range.end(); ++it) {
						// do calculations
					}
			
				Internally, both methods are equivalent.
			 */
			class Iterator {
			public:
				Iterator(unsigned pos, unsigned max) : pos(pos), max(max) { }
			public:
				unsigned operator*() {
					return pos;
				}
				
				bool operator!=(const Iterator& it) const {
					return pos != it.pos || max != it.max;
				}
				
				Iterator operator++(int) {
					if (pos < max) pos++;
					return *this;
				}
				
				Iterator& operator++() {
					if (pos < max) pos++;
					return *this;
				}
			private:
				unsigned pos;
				unsigned max;
			};
			
			Iterator begin() const { return Iterator(from, to); }
			Iterator end() const { return Iterator(to, to); }
		public:
			static Range SpacetimeRange(unsigned D=3) {
				return Range(0,D);
			}
			
			static Range SpaceRange(unsigned D=3) {
				return Range(1,D);
			}
			
			std::vector<unsigned> ToVector() const {
				std::vector<unsigned> result;
				for (unsigned i=from; i<=to; i++) {
					result.push_back(i);
				}
				return result;
			}
		private:
			unsigned from;
			unsigned to;
		};
	
	}
}