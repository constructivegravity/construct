#pragma once

#include <cobalt.hpp>

#include <cmd/solve.hpp>
#include <cmd/cli.hpp>

namespace Construction {
    namespace Cmd {

        class RootCommand : public Cobalt::Command<RootCommand, SolveCommand, CliCommand> {
        public:
            static std::string Use() {
                return "apple";
            }

            static std::string Short() {
                return "Solve gravitational construction equations";
            }

            static std::string Long() {
                return "Solve tensorial equations for arbitrary objects generated with background structure.";
            }

            void RegisterFlags() {
                AddPersistentFlag<bool>(debugMode, "debug", "d", false, "Print everything that is happening");
            }

            void PersistentPreRun(const Cobalt::Arguments& args) {
                std::cerr << "The infamous Apple Program" << std::endl;
                std::cerr << "(c) 2016 Constructive Gravity Group Erlangen" << std::endl;
                std::cerr << "All rights reserved." << std::endl << std::endl;

                std::cerr << "        ,---,_          ,          " << std::endl;
                std::cerr << "         _>   `'-.  .--'/          " << std::endl;
                std::cerr << "    .--'` ._      `/   <_          " << std::endl;
                std::cerr << "     >,-' ._'.. ..__ . ' '-.       " << std::endl;
                std::cerr << "  .-'   .'`         `'.     '.     " << std::endl;
                std::cerr << "   >   / >`-.     .-'< \\ , '._\\  " << std::endl;
                std::cerr << "  /    ; '-._>   <_.-' ;  '._>     " << std::endl;
                std::cerr << "  `>  ,/  /___\\ /___\\  \\_  /    " << std::endl;
                std::cerr << "  `.-|(|  \\o_/  \\o_/   |)|`      " << std::endl;
                std::cerr << "      \\;        \\      ;/        " << std::endl;
                std::cerr << "        \\  .-,   )-.  /           " << std::endl;
                std::cerr << "         /`  .'-'.  `\\            " << std::endl;
                std::cerr << "        ;_.-`.___.'-.;             " << std::endl << std::endl;
            }
        private:
            bool debugMode;
        };

    }

    // Syntactic sugar
    using Execute = Cobalt::Execute<Cmd::RootCommand>;
}
