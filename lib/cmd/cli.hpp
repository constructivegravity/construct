#pragma once

#include <common/time_measurement.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/permutation.hpp>
#include <tensor/symmetrization.hpp>

#include <tensor/index_parser.hpp>

#include <language/cli.hpp>
#include <language/command.hpp>

#include <vector/matrix.hpp>
//#include <vector/linear_system.hpp>

#include <generator/base_tensor.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <string>
#include <cstdio>

#include <cobalt.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;


#define RECOVER_FROM_EXCEPTIONS 	0

char* command_iterator(const char* text, int state) {
	static std::vector<std::string>::iterator it;
	auto cmds = Construction::Language::CommandManagement::Instance()->GetCommandList();

	if (state == 0) it = begin(cmds);

	while (it != end(cmds)) {
		auto& cmd = *it;
		++it;

		if (cmd.find(text) != std::string::npos) {
			char* completion = new char[cmd.size()];
			strcpy(completion, cmd.c_str());
			return completion;
		}
	}
	return nullptr;
}

char** getCommandCompletions(const char* text, int start, int) {
	char** completionList = nullptr;

	if (start == 0)
		completionList = rl_completion_matches(text, &command_iterator);

	return completionList;
}


namespace Construction {
    namespace Cmd {

        class CliCommand : public Cobalt::Command<CliCommand> {
        public:
            static std::string Use() {
                return "cli";
            }

            static std::string Short() {
                return "Open an interactive session to generate tensors";
            }

            static std::string Long() {
                return "Open an interactive session to generate tensors. Keep in mind that the coefficient syntax is NOT supported here.";
            }

            int Run(const Cobalt::Arguments& args) {
                Construction::Language::CLI cli;

                // Hook readline completion
                rl_attempted_completion_function = &getCommandCompletions;

                // If there is a filename given, execute this
                if (args.size() > 0) {
                    std::string filename = args[0];
                    cli.ExecuteScript(filename);
                    return 0;
                }

                // Create path for crashfile
                std::string crashFile = ".crashfile";
                /*auto crashFile = boost::filesystem::system_complete(argv[0]).remove_filename().string();
    if (crashFile[crashFile.size()-1] != '/') crashFile += "/";
    crashFile += ".crashfile";*/

                // If the file exists
                if (boost::filesystem::exists( crashFile )) {
                    // Ask if the session should be restored
                    std::cout << "Construction can restore the previous session. Should it? [Y/n]: ";

                    while (true) {
                        std::string input;
                        std::cin >> input;

                        if (input == "Y") {
                            Construction::Language::Session::Instance()->LoadFromFile(crashFile);
                            break;
                        } else if (input == "n") break;
                        else
                            std::cout << "Construction can restore the previous session. Should it? [Y/n]: ";
                    }
                }

                // CLI mode
                std::string input;
                while (true) {
                    input = std::string(readline("> "));

                    if (input == "Exit") {
                        // Delete crash file
                        remove(crashFile.c_str());

                        std::cout << "Bye!" << std::endl;
                        break;
                    }

                    if (input != "") {
                        add_history(input.c_str());
                        cli(input);
                    }
                }

                return 0;
            }
        };

    }
}
