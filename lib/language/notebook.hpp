#pragma once

#include <string>

namespace Construction {
    namespace Language {

        class Notebook {
        public:
        	void Append(const std::string& line) {
        		lines.push_back(line);
        	}

        	void Clear() {
        		lines.clear();
        	}
		public:
			size_t Size() const {
				return lines.size();
			}

			std::string operator[](unsigned int id) const {
				return lines.at(id);
			}

			std::vector<std::string>::iterator begin() {
				return lines.begin();
			}

			std::vector<std::string>::iterator end() {
				return lines.end();
			}

			std::vector<std::string>::const_iterator begin() const {
				return lines.begin();
			}

			std::vector<std::string>::const_iterator end() const {
				return lines.end();
			}
        private:
            std::vector<std::string> lines;
        };

    }
}
