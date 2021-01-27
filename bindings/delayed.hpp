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

#pragma once

#include <chrono>
#include <recorder/configurations.hpp>
#include <thread>

namespace recorder {

class DelayedDatabase {
 public:
  explicit DelayedDatabase(const config::Database&) {}

 public:
  void execute(const std::string&) {
    using namespace std::chrono_literals;

    static constexpr auto kDuration = 10ms;

    std::this_thread::sleep_for(kDuration);
  }
};

}  // namespace recorder
