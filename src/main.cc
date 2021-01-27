// Copyright 2021 SiLeader and Cerussite.
//
// This file is part of throughput-recorder.
//
// throughput-recorder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// throughput-recorder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with throughput-recorder.  If not, see <http://www.gnu.org/licenses/>.

#include <argparse.hpp>
#include <iostream>
#include <recorder/configurations.hpp>

#include "bindings_caller.hpp"

int main(const int argc, const char* const* const argv) {
  argparse::ArgumentParser parser(
      "recorder", "database throughput and latency recording system",
      "published under GNU General Public License 3.0");
  parser.addArgument({"config"}, "configuration file path");
  parser.addArgument({"--verbose"}, "print verbose log",
                     argparse::ArgumentType::StoreTrue);
  parser.addArgument({"--output", "-o"}, "output CSV file (default: stdout)");

  const auto args = parser.parseArgs(argc, argv);

  const auto config_file = args.get<std::string>("config");

  const auto config = recorder::Configurations::FromFile(config_file);

  const auto statistics =
      recorder::BindingsCaller<std::chrono::high_resolution_clock>(
          config, args.has("verbose"));
  if (args.has("output")) {
    const auto output_csv = args.get<std::string>("output");
    std::ofstream ofs(output_csv);
    statistics.dumpToStreamAsCsv(ofs);
  } else {
    statistics.dumpToStreamAsCsv(std::cout);
  }
}
