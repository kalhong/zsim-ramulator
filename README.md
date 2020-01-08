zsim-ramulator
==============

modified zsim src : init.cpp, ramulator_mem_ctrl.h, ramulator_mem_ctrl.cpp

modified ramulator src : Gem5Wrapper.cpp, Gem5Wrapper.h, StatType.cpp

zsim: https://github.com/s5z/zsim

ramulator: https://github.com/CMU-SAFARI/ramulator

Setup
-----

External dependencies: `gcc-4.8, pin, scons, libconfig, libhdf5, libelfg0, clang-3.4`
Use Vagrantfile for easy install (ubuntu12.04)
- `$ vagrant up`
- `$ vagrant ssh`

Compile
-------

- Ramulator compile: `$ cd ramulator; make -j libramulator`
- Zsim compile: `$ scons -j40`
- compile both: `$ ./compile_ZnR.sh`

Happy hacking, and hope you find zsim-ramulator useful!

