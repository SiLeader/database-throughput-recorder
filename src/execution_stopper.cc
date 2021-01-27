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

#include "execution_stopper.hpp"

#include <atomic>
#include <iostream>

namespace {

std::atomic<bool>* is_running_context = nullptr;

}  // namespace

namespace recorder::execution {

void SetRunningContext(std::atomic<bool>* is_running) {
  is_running_context = is_running;
}

void Stop() {
  if (is_running_context != nullptr) {
    std::clog << "sending stop signal to worker threads" << std::endl;
    is_running_context->store(false, std::memory_order::memory_order_release);
    is_running_context = nullptr;
  }
}

}  // namespace recorder::execution
