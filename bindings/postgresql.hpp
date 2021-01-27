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
#include <postgresql/libpq-fe.h>

#include <recorder/configurations.hpp>

namespace recorder {

class PostgreSqlDatabase {
 private:
  PGconn* connection_ = nullptr;
  PGresult* result_ = nullptr;

 public:
  explicit PostgreSqlDatabase(const config::Database& db)
      : connection_(PQsetdbLogin(db.host().c_str(),
                                 std::to_string(db.port()).c_str(), nullptr,
                                 nullptr, db.database().c_str(),
                                 db.user().c_str(), db.password().c_str())) {
    if (PQstatus(connection_) == CONNECTION_BAD) {
      std::string message = "connection cannot be established. ";
      message += PQerrorMessage(connection_);
      throw std::runtime_error(message);
    }
  }

  ~PostgreSqlDatabase() { close(); }

 public:
  void close() noexcept {
    if (result_) {
      PQclear(result_);
      result_ = nullptr;
    }

    if (connection_) {
      PQfinish(connection_);
      connection_ = nullptr;
    }
  }

 public:
  void execute(const std::string& query) {
    result_ = PQexec(connection_, std::string(query).c_str());
    const auto res = PQresultStatus(result_);
    if (res != PGRES_COMMAND_OK && res != PGRES_TUPLES_OK) {
      std::cerr << "exec error: " << PQresultErrorMessage(result_) << std::endl;
      throw std::runtime_error("exec error");
    }
  }
};

}  // namespace recorder
