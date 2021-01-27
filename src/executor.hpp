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
#include <boost/lockfree/queue.hpp>
#include <chrono>
#include <future>
#include <recorder/configurations.hpp>
#include <shared_mutex>
#include <thread>

#include "execution_stopper.hpp"
#include "signal.hpp"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace recorder {

namespace execution {

class StartingPistol {
 private:
  std::shared_mutex mutex_;

 public:
  StartingPistol() { mutex_.lock(); }

 public:
  void wait() { mutex_.lock_shared(); }

  void start() { mutex_.unlock(); }
};

template <class ClockT>
struct Statistic {
  using time_point = typename ClockT::time_point;
  static_assert(std::is_trivially_copyable_v<time_point>,
                "time_point must be trivially copyable");

  int thread_number;
  time_point begin, end;
};

template <class DatabaseBindingT, class ClockT>
void Worker(const Configurations& conf, StartingPistol& starting_pistol,
            std::atomic<bool>& is_running, int thread_number,
            boost::lockfree::queue<Statistic<ClockT>>& statistics_queue) {
  DatabaseBindingT binding(conf.database());
  const auto& query = conf.query();

  starting_pistol.wait();

  try {
    while (likely(is_running.load(std::memory_order::memory_order_acquire))) {
      const auto begin = ClockT::now();
      binding.execute(query);
      const auto end = ClockT::now();
      const Statistic<ClockT> s = {thread_number, begin, end};
      statistics_queue.push(s);
    }
  } catch (const std::runtime_error& e) {
    printf("runtime error: %s\n", e.what());
    std::terminate();
  }
}

template <class ClockT>
Statistics StatisticsWorker(
    boost::lockfree::queue<Statistic<ClockT>>& statistics_queue,
    std::atomic<bool>& is_running, const int thread_count, const bool verbose) {
  using namespace std::chrono_literals;

  std::vector<
      std::tuple<int, std::chrono::microseconds, typename ClockT::time_point>>
      statistics(thread_count + 1);
  std::vector<
      std::vector<std::tuple<Statistics::Throughput, Statistics::Latency>>>
      results(thread_count + 1);

  static constexpr auto kInterval = 1s;
  const auto whole_index = thread_count;

  const auto update_statistics = [&results, &statistics, whole_index, verbose](
                                     const int index,
                                     const Statistic<ClockT> s) {
    auto& [counter, elapsed, deadline] = statistics[index];
    counter++;
    elapsed +=
        std::chrono::duration_cast<std::chrono::microseconds>(s.end - s.begin);
    if (deadline <= s.end) {
      const auto start_point = deadline - kInterval;
      const auto duration_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(s.end -
                                                                start_point)
              .count();
      const auto duration_s = duration_ms / 1000.0;
      const auto throughput = counter / duration_s;
      const auto latency = Statistics::Latency(elapsed / counter);
      results[index].emplace_back(
          std::make_tuple(Statistics::Throughput(throughput), latency));
      deadline = s.end + kInterval;

      counter = 0;
      elapsed = std::chrono::microseconds(0);
      if (verbose) {
        if (index == whole_index) {
          std::cout << "whole: ";
        } else {
          std::cout << "thread" << index << ": ";
        }
        std::cout << std::fixed << std::setprecision(2) << throughput
                  << " ops, " << latency.latency().count() << " us"
                  << std::endl;
      }
    }
  };

  while (true) {
    Statistic<ClockT> s;
    if (!statistics_queue.pop(s)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (unlikely(!is_running.load(std::memory_order::memory_order_acquire))) {
        break;
      }
      continue;
    }
    update_statistics(s.thread_number, s);
    update_statistics(whole_index, s);
  }
  const auto whole_result = results.back();
  results.pop_back();

  return Statistics(whole_result, results);
}

}  // namespace execution

template <class DatabaseBindingT,
          class ClockT = std::chrono::high_resolution_clock>
Statistics Execute(const Configurations& conf, const bool verbose) {
  execution::StartingPistol starting_pistol;

  std::atomic<bool> is_running(true);
  execution::SetRunningContext(&is_running);
  signal::RegisterHandler();

  boost::lockfree::queue<execution::Statistic<ClockT>> statistics_queue(4096);
  std::vector<std::thread> threads;
  threads.reserve(conf.threadCount());

  for (int i = 0; i < conf.threadCount(); ++i) {
    threads.emplace_back(
        [&conf, &starting_pistol, i, &is_running, &statistics_queue] {
          execution::Worker<DatabaseBindingT, ClockT>(
              conf, starting_pistol, is_running, i, statistics_queue);
        });
  }
  auto future = std::async(
      std::launch::async, [&statistics_queue, &is_running, &conf, verbose] {
        return execution::StatisticsWorker(statistics_queue, is_running,
                                           conf.threadCount(), verbose);
      });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  starting_pistol.start();

  for (auto& thread : threads) {
    thread.join();
  }
  return future.get();
}

}  // namespace recorder

#undef unlikely
#undef likely
