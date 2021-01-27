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

import argparse
import json


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-o', '--output', help='output file path', required=True)
    parser.add_argument(
        '-e', '--executor',
        help='executor header include path (default: executor.hpp)',
        default='executor.hpp'
    )
    parser.add_argument('configuration', help='configuration JSON file')

    args = parser.parse_args()
    output_header = args.output
    conf = args.configuration
    executor_header = args.executor

    with open(conf) as fp:
        conf = json.load(fp)

    with open(output_header, 'w') as fp:
        print('// auto generated file', file=fp)
        print('#pragma once', file=fp)
        print('', file=fp)
        print('#include <recorder/statistics.hpp>', file=fp)
        print(f'#include "{executor_header}"', file=fp)
        print('// database includes', file=fp)
        for ic in conf.values():
            print(f'#include <{ic["include"]}> // for {ic["class"]}', file=fp)
        print('', file=fp)
        print('namespace recorder {', file=fp)
        print('', file=fp)
        print('template<class ClockT>', file=fp)
        print('::recorder::Statistics BindingsCaller(const ::recorder::Configurations& conf, const bool verbose) {',
              file=fp)
        print('  ', end='', file=fp)
        for name, ic in conf.items():
            print(f'if (conf.database().binding() == "{name}")' + '{', file=fp)
            print(f'    return Execute<{ic["class"]}, ClockT>(conf, verbose);', file=fp)
            print('  }', end='', file=fp)
        print('else {', file=fp)
        print('    throw std::runtime_error("unknown database binding");', file=fp)
        print('  }', file=fp)
        print('}', file=fp)
        print('', file=fp)
        print('} // recorder', file=fp)


if __name__ == '__main__':
    main()
