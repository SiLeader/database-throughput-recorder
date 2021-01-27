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
#include <vector>

namespace recorder {

class Statistics {
 public:
  class Throughput {
   private:
    double throughput_;

   public:
    Throughput() = default;
    explicit Throughput(double throughput) : throughput_(throughput) {}
    Throughput(const Throughput&) = default;
    Throughput(Throughput&&) = default;

    Throughput& operator=(const Throughput&) = default;
    Throughput& operator=(Throughput&&) = default;

   public:
    [[nodiscard]] double throughput() const noexcept { return throughput_; }
  };

  class Latency {
   private:
    std::chrono::microseconds latency_;

   public:
    Latency() = default;
    explicit Latency(std::chrono::microseconds latency) : latency_(latency) {}
    Latency(const Latency&) = default;
    Latency(Latency&&) = default;

    Latency& operator=(const Latency&) = default;
    Latency& operator=(Latency&&) = default;

   public:
    [[nodiscard]] std::chrono::microseconds latency() const noexcept {
      return latency_;
    }
  };

 private:
  std::vector<std::tuple<Throughput, Latency>> whole_throughput_;
  std::vector<std::vector<std::tuple<Throughput, Latency>>> thread_throughput_;

 public:
  Statistics(std::vector<std::tuple<Throughput, Latency>> whole,
             std::vector<std::vector<std::tuple<Throughput, Latency>>> thread)
      : whole_throughput_(std::move(whole)),
        thread_throughput_(std::move(thread)) {}

 public:
  void dumpToStreamAsCsv(std::ostream& os) const {
    os << "time(sec),whole(ops)";
    for (std::size_t t = 0; t < thread_throughput_.size(); ++t) {
      os << ",thread" << t << "(ops),thread" << t << "(us)";
    }
    os << std::endl;

    const auto dump_impl =
        [](std::ostream& os,
           const std::vector<std::tuple<Throughput, Latency>>& t,
           std::size_t i) {
          os << ",";
          if (i >= t.size()) {
            os << ",";
            return false;
          }
          const auto& [tp, lt] = t[i];
          os << std::fixed << std::setprecision(2) << tp.throughput() << ","
             << lt.latency().count();
          return true;
        };

    for (std::size_t i = 1;; ++i) {
      std::stringstream ss;
      ss << i;
      bool is_continue = false;

      is_continue |= dump_impl(ss, whole_throughput_, i);
      for (const auto& tt : thread_throughput_) {
        is_continue |= dump_impl(ss, tt, i);
      }

      if (!is_continue) {
        break;
      }
      os << ss.str() << std::endl;
    }
  }
};

}  // namespace recorder
