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

#include <csignal>
#include <cstring>
#include <iostream>

#include "execution_stopper.hpp"

namespace {

void Handler(int signum) {
  std::clog << "caught signal: " << signum << " " << strsignal(signum)
            << std::endl;
  if (signum == SIGINT) {
    recorder::execution::Stop();
  }
}

}  // namespace

namespace recorder::signal {

void RegisterHandler() {
  struct ::sigaction sa;
  sa.sa_handler = Handler;

  std::clog << "press Ctrl-C to stop recording" << std::endl;
  sigaction(SIGINT, &sa, nullptr);
}

}  // namespace recorder::signal
