# A Privacy-preserving Distributed Filtering Framework for NLP Artifacts

This repository contains the source code for the project "A Privacy-preserving Distributed Filtering Framework for NLP Artifacts". 
The software manual is also available in pdf format. Please see manual.pdf for details.
This software has been tested using Google cloud’s n1-standard-8 (Ubuntu 16.04.3 LTS) and Amazon’s
r3.xlarge (Ubuntu 16.04.2 LTS). However, this package should work in lower configuration machines as
long as these machines have multiple cores. The following software modules are required.  An installation script (script.sh) is provided with the package to install these modules.  This script should be executed in the project directory.
  - libssl
  - NFLlib
  - GMP
  - MPFR
  - OpenMP
  
GCC 5.4.1 or later is recommended

# Entities in the System Architecture and Corresponding Executable
![System Architecture] (http://www.sharelatex.com/project/5a8df40a4e5235777c589661/file/5a8e0e374e5235777c589b6a)
| Entity | Executable |
| ------ | ------ |
| Crypto Service Provider (CSP) | csp.cpp |
| Central Server | central_server.cpp |
|  Data Owner | data_owner.cpp |

# Directory Structure
Common directory and files:
- common:  directory containing some common utilities
- include:  directory containing nfl source
- lib:  directory containing nfl source
- FV_2.hpp:  library for FV cryptosystem
- fvnamespace2.h:  for FV cryptosystem
- script.sh:  script for dependency installation
Entity specific directory and files are described in the following subsections.

## Central Server
- central_server:  directory for central server files
- central_server.cpp
## CSP
- csp:  directory for CSP files
- csp.cpp
## Data Owner
- data_owner_id,  (id  =  1,  2,  3...):   directory  for  data  owner  files.   This  directory  contains  some sub-directories  for  organizing  datasets  used  in  different  executions  of  the  system  protocol. Each sub-directory  is  identified  by  an  experiment  code  (A,  B,  C  etc.).   This  experiment  code  can  be specified in the configuration file (please see Section "Configuration file").
- data_owner.cpp

# Compilation
- Compiling the executable for central server:
```sh
g++ central_server.cpp -o cs.o a.out -std=c++11 -fopenmp  -I./include/ -I./include/nfl/ -I./include/nfl/prng/ -I./lib/prng/ -I./lib/params/ -I./include/nfl/opt/arch/ -lgmpxx -lgmp -lmpfr -m64 -DNTT_AVX -DNTT_SSE
```
- Compiling the executable for CSP:
```sh
g++ csp.cpp -o csp.o a.out -std=c++11  -I./include/ -I./include/nfl/ -I./include/nfl/prng/ -I./lib/prng/ -I./lib/params/ -I./include/nfl/opt/arch/ -lgmpxx -lgmp  -lmpfr -m64 -DNTT_AVX -DNTT_SSE
```
- Compiling the executable for data owner:
```sh
g++ data_owner.cpp -o do.o a.out -std=c++11 -lcrypto  -I./include/ -I./include/nfl/ -I./include/nfl/prng/ -I./lib/prng/ -I./lib/params/ -I./include/nfl/opt/arch/ -lgmpxx -lgmp  -lmpfr -m64 -DNTT_AVX -DNTT_SSE
```
# Configuration File
At the beginning of the system protocol, as part of system initialization process, the central server sends a  configuration  file  to  each  party.   This  configuration  file  contains  IP  address  and  port  of  the  parties. This configuration file should be placed in the central server’s directory (central_server/config.ini). A sample configuration file is provided with this package. If software is deployed in multiple machines, then IP addresses should be changed accordingly (Please see "Deploying the Software in Multiple Machines" for details).

# Format of Input Data
This software expects the input data of data owner in the following format:
```sh
bigram_1 = count_1
bigram_2 = count_2
bigram_3 = count_3
..................
```
For instance, blood group = 21.
The filename containing the input data should be ExperimentCode_data owner id_pt_count_map.txt
For instance, for experiment code A, data owner id 1, filename will be
A_1_pt_count_map.txt. Also, this file will be placed in data owner 1’s directory for experiment code A (data_owner1/A/).

# Output for Participating Data Owners
The final output is stored in the plaintext_output_bigrams.txt file in the data owners’ directory.  This file contains the bigrams meeting the threshold criteria.

# Running the Software
The central server executable should run at the end because it will expect all other entities to be ready for receiving configuration file. For example, for three data owners, the following sequence can be followed (in five different terminals).
```sh
./do.o 1
./do.o 2
./do.o 3
./csp.o
./cs.o
```
It should be noted that ID of a data owner is passed as a command line argument.

# Deploying the Software in Multiple Machines
To deploy each entity in different machine, the executable and the directory should be copied to that machine. For instance, to deploy central server in a different machine, only common files and central_server.cpp, central_server directory are required.  Similarly, for data owner 1, only common files, data_owner.cpp, and data_owner directory are required.

# Supplementary Codes
The default number of data owners is three. src_additional directory contains codes to handle number of data owners other than three.

# License
GPLv3
This software uses the following projects (licensed under GPLv3):
-  NFLlib
- FV-NFLlib
