# HCSim 
# HOST-COMPILED MULTI-CORE SYSTEM SIMULATOR:
This is the release of the HCSim-1.1 simulator.

This folder contains abstract models of real-time operating systems (RTOS) and 
high-level multi-core processor models for full-system host-compiled simulation [1]. 


## Building and installing:

Build requirements:
  - SystemC version 2.3.1  http://www.accellera.org/
  - TLM 2.0 http://www.accellera.org/

Preparation before build:
  - Edit Makefile.macros. Set the following variables (external dependencies):
      SYSTEMC_DIR - to the installation directory of SystemC-2.3.1
      TLM_DIR - to the installation directory of SystemC-2.3.1 (Includes TLM)  

Build hcsim library:
  % make clean
  % make all

## Running:
```
Before run an example:
   % source <install_path>/bin/setup.sh
  
To run examples locally:
   % cd <example>
   % make test

<example> can be one of:
  - examples/simple_taskset
  - examples/mix_taskset
  - example/mix_taskset_tlm2.0
```
 

## Directories:

```
  include/                   -- Header files
    OS/                         - OS models
    Processor/                  - processor components models
    Channels/                   - bus and primitive channels models
  src/                       -- Source codes 
    OS/                         - OS model 
    Processor/                  - processor models 
    Channels/                   - channel models 
  examples/                  -- A set of examples running on a dual-core configuration
    simple_taskset/             - artificial periodic task sets
    mix_taskset/                - artificial periodic task sets plus interrupt-driven tasks
    mix_taskset_tlm2.0          - mix_taskset example with TLM2.0-based bus model   
  lib/                       -- symbolic link to hcsim library
  bin/                       -- public executable (set set symbolic link to the library)
  utilities/                 -- Design templates
```
------------
## References:
[1] P. Razaghi, A. Gerstlauer, "Host-Compiled Multi-Core System Simulation
    for Early Real-Time Performance Evaluation," ACM Transactions on Embedded
    Computer Systems, 2014.
    
## Contacts:
* Parisa Razaghi <parisa.r@utexas.edu>
* Zhuoran Zhao <zhuoran@utexas.edu>
* Andreas Gerstlauer <gerst@ece.utexas.edu>    
