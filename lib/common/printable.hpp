#pragma once

#include <string>

namespace Construction {
	namespace Common {

		/**
			\class Printable
		
			Class for printable texts
		 */
		class Printable {
		public:
			Printable() = default;

			Printable(const std::string& printed) : printed_text(printed) { }
		public:
			std::string GetPrintedText() const { return printed_text; }
			void SetPrintedText(const std::string& text) { printed_text = text; }
		public:
			virtual std::string ToString() const { return printed_text; }
			operator std::string() const { return ToString(); }
			
			friend std::ostream& operator<<(std::ostream& os, const Printable& pr) {
				os << pr.ToString();
				return os;
			}
		protected:
			std::string printed_text;
		};
	
	}
}