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
        };

    }

    // Syntactic sugar
    using Execute = Cobalt::Execute<Cmd::RootCommand>;
}
