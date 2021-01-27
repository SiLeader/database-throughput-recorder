# Throughput recorder - DB throughput recording system

&copy; 2021 SiLeader and Cerussite.

## features

+ DB support
    + PostgreSQL
+ throughput recording
+ latency recording

## how to use

1. clone this repository.
2. build program (see below)
3. run built program

```shell
git submodule update --init --recursive # clone third party libraries
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # Release build
make
```

```shell
./recorder -h # show command line options
```

### configurations

see `test` directory

```yaml
threads: 8
database:
  binding: postgresql
  host: localhost
  port: 5432
  database: recorder
  user: recorder
  password: recorder
query: "SELECT * FROM recorder WHERE rid = 1"
```

+ threads: number of threads
+ database: database configurations
    + binding: database binding (postgresql or delayed support)
    + host: database host
    + port: database port
    + database: database name
    + user: database login user
    + password: password for user
+ query: execute query

## license

GNU GPL version 3.0
