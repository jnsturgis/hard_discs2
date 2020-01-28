#! /bin/bash

./config_test
./polygon_test
./topology_test test2.topo

../makeconfig/makeconfig -v 100 100 5
../makeconfig/makeconfig -v 100 100 5 5

../shrinkconfig/shrinkconfig -v test1.config
../shrinkconfig/shrinkconfig -v -s 2.0 test1.config
../shrinkconfig/shrinkconfig -v -s 0.5 test2.config

../NVT/NVT -v -t test1.topo -f test1.ff -c test1.config 100 10 1 1

valgrind ./config_test
valgrind ./polygon_test
valgrind ./topology_test test2.topo

valgrind ../makeconfig/makeconfig -v 100 100 5 5
valgrind ../shrinkconfig/shrinkconfig -v -s 0.5 test2.config

