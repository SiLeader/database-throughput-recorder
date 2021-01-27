# Copyright 2021 SiLeader and Cerussite.
#
# This file is part of throughput-recorder.
#
# throughput-recorder is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# throughput-recorder is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with throughput-recorder.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)
project(throughput-recorder)

set(CMAKE_CXX_STANDARD 17)

# third party libraries
add_subdirectory(third_party/yaml-cpp)

# recorder target configurations
add_executable(recorder
        src/main.cc
        include/recorder/configurations.hpp
        src/executor.hpp
        include/recorder/statistics.hpp
        bindings/delayed.hpp
        src/signal.cc
        src/execution_stopper.cc
        src/execution_stopper.hpp
        src/signal.hpp bindings/postgresql.hpp)

# third party includes
target_include_directories(recorder PRIVATE third_party/argparse third_party/yaml-cpp/include)

# recorder includes
target_include_directories(recorder PUBLIC include)
target_include_directories(recorder PRIVATE bindings)

# recorder libraries
target_link_libraries(recorder pthread yaml-cpp)

# database libraries
target_link_libraries(recorder pq)

# bindings caller generator
add_custom_command(
        OUTPUT ${PROJECT_SOURCE_DIR}/src/bindings_caller.hpp
        COMMAND python3 ${PROJECT_SOURCE_DIR}/generate_binding_caller.py -o src/bindings_caller.hpp bindings/bindings.json
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS ${PROJECT_SOURCE_DIR}/generate_binding_caller.py ${PROJECT_SOURCE_DIR}/bindings/bindings.json
)

add_custom_target(
        generate-binding-caller
        DEPENDS ${PROJECT_SOURCE_DIR}/src/bindings_caller.hpp)
add_dependencies(recorder generate-binding-caller)