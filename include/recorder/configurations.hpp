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

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <string>

namespace recorder {

namespace config {

class Database {
 private:
  std::string binding_;
  std::string host_;
  std::uint16_t port_;
  std::string database_;
  std::string user_;
  std::string password_;

 public:
  Database(std::string binding, std::string host, std::uint16_t port,
           std::string database, std::string user, std::string password)
      : binding_(std::move(binding)),
        host_(std::move(host)),
        port_(port),
        database_(std::move(database)),
        user_(std::move(user)),
        password_(std::move(password)) {}
  Database(const Database&) = default;
  Database(Database&&) noexcept = default;

  Database& operator=(const Database&) = default;
  Database& operator=(Database&&) noexcept = default;

 public:
  static Database FromYamlNode(const YAML::Node& node) {
    auto binding = node["binding"].as<std::string>();
    auto host = node["host"].as<std::string>();
    const auto port = node["port"].as<std::uint16_t>();
    auto database = node["database"].as<std::string>();
    auto user = node["user"].as<std::string>();
    auto password = node["password"].as<std::string>();
    return Database(std::move(binding), std::move(host), port,
                    std::move(database), std::move(user), std::move(password));
  }

 public:
  [[nodiscard]] const std::string& binding() const { return binding_; }
  [[nodiscard]] const std::string& host() const { return host_; }
  [[nodiscard]] std::uint16_t port() const noexcept { return port_; }
  [[nodiscard]] const std::string& database() const { return database_; }
  [[nodiscard]] const std::string& user() const { return user_; }
  [[nodiscard]] const std::string& password() const { return password_; }
};

}  // namespace config

class Configurations {
 private:
  int thread_count_;
  config::Database db_;
  std::string query_;

 public:
  Configurations(int thread_count, config::Database db, std::string query)
      : thread_count_(thread_count),
        db_(std::move(db)),
        query_(std::move(query)) {}

  Configurations(const Configurations&) = default;
  Configurations(Configurations&&) noexcept = default;

  Configurations& operator=(const Configurations&) = default;
  Configurations& operator=(Configurations&&) noexcept = default;

 public:
  static Configurations FromStream(std::istream& is) {
    YAML::Node config = YAML::Load(is);
    const auto threads = config["threads"].as<int>();
    auto db = config::Database::FromYamlNode(config["database"]);
    auto query = config["query"].as<std::string>();
    return Configurations(threads, std::move(db), std::move(query));
  }

  static Configurations FromFile(const std::string& config_file) {
    std::ifstream ifs(config_file);
    return FromStream(ifs);
  }

 public:
  [[nodiscard]] int threadCount() const noexcept { return thread_count_; }
  [[nodiscard]] const config::Database& database() const { return db_; }
  [[nodiscard]] const std::string& query() const { return query_; }
};

}  // namespace recorder
