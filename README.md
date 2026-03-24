# Parallel and Distributed Processing: High-Performance Computing Implementations

## Navigation and Table of Contents
1. [Overview and Repository Purpose](#overview-and-repository-purpose)
2. [Technical Stack and Requirements](#technical-stack-and-requirements)
3. [Deep Dive: Repository Architecture by Module](#deep-dive-repository-architecture-by-module)
   - [Message Passing Interface (MPI)](#1-message-passing-interface-mpi-distributed-memory-systems)
   - [POSIX Threads (Pthreads)](#2-posix-threads-pthreads-shared-memory-systems)
   - [CUDA (GPU Computing)](#3-cuda-gpu-parallel-computing)
   - [Academic Assignments](#4-academic-assignments-and-applied-problems)
4. [Competencies and Architectural Design Highlights](#competencies-and-architectural-design-highlights)
5. [Compilation and Execution Guide](#compilation-and-execution-guide)

## Overview and Repository Purpose
This repository serves as a comprehensive archive of advanced solutions in the domain of **Parallel and Distributed Processing**. It showcases practical implementations of high-performance computing (HPC) theories utilizing the most prominent parallel programming paradigms in the industry:
- **Message Passing Interface (MPI)** for clustered and distributed-memory environments.
- **POSIX Threads (Pthreads)** for multi-core, shared-memory processing.
- **CUDA** for executing highly dense, massively parallel mathematical computations on NVIDIA GPUs.

Recruiters and engineers reviewing this codebase will find robust, low-level C/C++ architectures designed to overcome traditional Von Neumann bottlenecks, optimize CPU/GPU utilization, mitigate race conditions, ensure safe memory concurrency, and profile execution benchmarks with microsecond precision.

## Technical Stack and Requirements
- **Core Languages**: C99, C11, C++11 and higher.
- **Concurrency API**: POSIX Threads library (`pthread.h`).
- **Distributed API**: Message Passing Interface Standard v3.1 (`mpi.h`), specifically OpenMPI or MPICH.
- **Hardware Acceleration API**: NVIDIA CUDA Toolkit (`cuda_runtime.h`).
- **Compilers**: GNU Compiler Collection (`gcc`, `g++`), MPI C Compiler (`mpicc`), NVIDIA CUDA Compiler (`nvcc`).
- **OS Environment**: Linux/UNIX (Preferred for native Pthreads and MPI execution), Windows via WSL or MS-MPI.

---

## Deep Dive: Repository Architecture by Module

### 1. Message Passing Interface (MPI): Distributed Memory Systems
The MPI directories encapsulate how clustered nodes coordinate complex tasks through passing messages across memory domains that do not share state.

#### Directory: `mpi-1`
An extensive look into fundamental and intermediate MPI functions, showcasing how communication paradigms dramatically alter execution times and code structure.
- **Initialization and Ranks (`Greetings.c` & `Outputs.c`)**: Demonstrates process rank assignments, total process count identification (`MPI_Comm_size`, `MPI_Comm_rank`), and standardized node-to-node communication pipelines.
- **Point-to-Point vs Collective Communication**:
  - `Collective_Communication.c`: Contrasts individual point-to-point sends/receives with optimized collective broadcasts (`MPI_Bcast`).
  - `MPI_Scatter.c` & `MPI_Gather.c`: Exhibits patterns for distributing vast contiguous arrays across individual processing nodes and safely aggregating the altered data back to a root node block by block.
  - `MPI_AllGather.c`: Demonstrates how all nodes can attain the fully aggregated dataset without a secondary scatter operation.
- **Mathematical Parallel Implementations**:
  - `Matrix_Multiplication.c`: A block-distributed matrix multiplication approach dividing rows/columns among respective MPI ranks to balance the arithmetic load.
  - `Sum_elements__array.c` & `Global_max.c`: Uses `MPI_Reduce` to calculate mathematical global properties (summations and global maximums) across disjoint process memories efficiently via tree algorithms.
  - `Trapezoid.c`: A distributed execution of numerical integration via the Trapezoidal rule. The integration area is partitioned across nodes, and the localized areas are merged using reduction operations.
- **Advanced State Representation**:
  - `MPI_Derived_DataType.c`: Maps structured C-datatypes into custom MPI datatypes. This minimizes network overhead allowing users to send complex structs consisting of integers, floats, and chars over the network seamlessly without multiple sequential send operations.

#### Directory: `mpi-2`
- **Advanced Algorithms (`parallel_sorting.c`)**: Implements parallelized sorting topologies across an MPI cluster. Decomposes a primary array to local processes, executes local sorts (such as QuickSort), and utilizes orchestrated sequences like Tree-Merge reduction or Odd-Even transposition sorts. Results in drastic time complexity reduction compared to serial variants.

---

### 2. POSIX Threads (Pthreads): Shared Memory Systems
The Pthreads directories outline execution streams navigating through a unified memory space. These implementations place heavy emphasis on maintaining operational correctness while circumventing concurrency bugs such as deadlocks, livelocks, and race conditions.

#### Directory: `pthreads-1`
- **Synchronization Primitives & Mutexes**:
  - `pi_mutex_lock.c` & `pi_busy_waiting.c`: Highlights the calculation of Pi using Monte Carlo or Leibniz formulas. Directly contrasts the high CPU utilization of active polling ("busy waiting") versus the efficient, yielding nature of POSIX Mutexes (`pthread_mutex_lock` and `unlock`).
- **Thread Barriers (Rendezvous Points)**:
  Consistently stalling threads until a specific synchronization threshold is met, examined through three distinct OS mechanics:
  - `barrier_busy_waiting_mutex.c`: Implementing loop-based condition barriers combined with mutex blocks.
  - `barrier_semaphores.c`: Utilizing POSIX semaphores (`sem_wait`, `sem_post`) to signal execution progression across threads cleanly.
  - `barrier_condition_variables.c`: The industry-standard approach utilizing condition variables (`pthread_cond_wait` and `pthread_cond_broadcast`) to safely suspend and awaken execution queues without wasting CPU cycles.
- **Thread-safe Data Structures & String Modifiers**:
  - `linked_list_r_w_locks.c` & `linked_list_sol2.c`: An implementation of dynamic linked lists completely protected by Read-Write locks (`pthread_rwlock_t`). Allows concurrent readers to access list elements while strictly blocking out any other reader or writer when a structural modification occurs.
  - `tokenizer_strtok.c` & `tokenizer_strtok_r.c`: Exposes how standard C library functions (like `strtok`) maintain dangerous static states that fail catastrophically during concurrency. Rectifies the flaw by implementing reentrant parser functions (`strtok_r`), enabling multiple threads to independently tokenize strings from shared memory buffers.
- **Core Vector Computations**:
  - `matrix_vector_multiplication.c`: Partitions matrix rows statically amongst threads for high-throughput linear algebra calculations.
  - `message_send.c` & `message_send_semaphores.c`: Models consumer-producer pipelines or direct signaling paradigms between varying thread message queues.

#### Directory: `pthreads-2`
Focuses on algorithmic structuring, scheduling distributions, and granular practical optimizations.
- **Thread Management (`hello.c`)**: Validates initialization, runtime joining, and dynamic thread detachment loops.
- **Loop Scheduling (`scheduling.c`)**: Analyzes the performance deltas between static chunk distribution, dynamic queuing, and guided thread block distributions across varying workloads.
- **Algorithmic Mechanics & Parallels**:
  - `trapezoid.c` & `trapezoid_parallel_for.c`: Compares manually coded cyclic threading vs higher-level abstraction patterns that simulate OpenMP-style "parallel for" implementations.
  - `sort_sol1.c` & `sort_sol2.c`: Multi-stage, multi-threaded sorting logic showcasing variable execution granularity and thread-pool load-balancing techniques.

---

### 3. CUDA: GPU Parallel Computing
The `Cuda Programs` directory abstracts complex mathematical matrices and loops into thousands of simultaneous GPU processes, defining heterogeneous architectural models where CPUs instruct high-throughput NVIDIA GPUs.

- **Initialization (`HelloCuda1.cu`)**: Validates the NVCC toolchain, testing fundamental Grid/Block dimensional constraints and foundational `__global__` kernel architecture dispatches.
- **Monte Carlo Estimates (`pi_value_estimate.cu`)**: Implements massive numerical simulations. Derives the mathematical ratio of pi by shooting millions of parallel algorithmic vectors across interconnected thread blocks. Integrates GPU atomics (`atomicAdd`) within the VRAM to safely and quickly tally occurrences in global memory without locking the SMs (Streaming Multiprocessors).
- **Matrix Scaling (`Matrix_Multiplication.cu`)**: Exhibits tremendous speedup metrics over CPU-bound loops. Segments vast memory matrices into sub-matrices, mapping them exactly to geometric Thread Blocks. It commands CPU RAM to GPU VRAM duplication (`cudaMemcpyHostToDevice`), computes thousands of simultaneous inner dot-products directly using the GPU cores, and finalizes with structured retrieval (`cudaMemcpyDeviceToHost`).

---

### 4. Academic Assignments and Applied Problems
These structural assignment directories synthesize isolated theories into functional, multi-component architectural problems.

- **`Assignment 01` (`Code.cpp`)**: Introductory computational theory establishing an analytical baseline of concurrency potential, runtime efficiency, and time complexity mapping based on Amdahl's Law.
- **`Assignment 02` (`exercise_q_2.c`, `programming_q_9.c`, `programming_q_7_wtime.c`...)**:
  - Focuses on benchmarking memory access topography and system cache latency patterns.
  - Deploys performance profiling to find precise micro-variances between default `clock()` systems from the C-standard library and highly-accurate HPC specific timers like `MPI_Wtime()`.
- **`Assignment 03` (`custom_rwl.c`, `my_rand.c`, `pth_cond_bar.c`, `pth_ll_rwl.c`...)**:
  - Highly robust custom synchronization mechanics. Reengineers POSIX barrier struct logic from scratch rather than relying on underlying system macros, demonstrating fundamental systems engineering relating to signaling, mutex locking orders, and thread starvation prevention.
- **`Assignment 04` (`Task 1` & `Task 2`)**: Finalized applied architectural scenarios requiring combined structured approaches spanning multiple concurrency modalities.

---

## Competencies and Architectural Design Highlights
This repository stands as direct proof of deep technical competency navigating the following high-level engineering disciplines:

1. **Deterministic Concurrency Control**: Writing resilient, mathematically predictable threaded logic. Utterly eliminating undefined bounds and heisenbugs through precise formulation of mutex locking structures, conditional wait-and-broadcast cycles, and throttled semaphores.
2. **Algorithmic Load Distribution**: The ability to inspect a linear O(N^2) or O(N log N) algorithm and mathematically partition the data structures (Data Parallelism) or the operational sequence itself (Task Parallelism) dynamically across decentralized compute cores.
3. **Advanced Memory Hierarchy Triage**: Knowing precisely when to exploit heavily localized L1/L2 hardware caches, when to configure broad shared RAM pools safely, and when to package sub-arrays for extreme bandwidth transmission over PCI-e to GPU multiprocessors.
4. **Bandwidth Optimization**: Structuring arbitrary structs into cohesive MPI `MPI_Type_struct` network representations to guarantee that total network packet payloads hit peak limits without devastating the computational flow with repetitive TCP/IP handshake overheads.

---

## Compilation and Execution Guide

Ensure the underlying host Operating System is equipped with the respective toolchains. These examples are mapped for Linux/UNIX Bash shells or Windows WSL domains.

### 1. Compiling MPI Applications (C language)
Requires an implementation matching the MPI 3.1 standard (OpenMPI / MPICH).
```bash
# Compilation flags: -g (debug), -Wall (all warnings)
mpicc -g -Wall -o output_binary program_name.c

# Execution: Request 'N' virtual processors/nodes
mpiexec -n 4 ./output_binary
```

### 2. Compiling Pthreads Applications (C language)
Requires GCC linked with the thread definition libraries.
```bash
# Link the pthread dependency to generate the executable
gcc -g -Wall program_name.c -o output_binary -lpthread

# Execution syntax assumes argument vector input for thread count
./output_binary 8
```

### 3. Compiling CUDA Kernels (.cu language)
Requires NVIDIA GPU architecture and the CUDA toolchain.
```bash
# Compile via NVIDIA CUDA Compiler
nvcc -o output_binary program_name.cu

# Execution
./output_binary
```

*Note: For implementations relying upon localized or customized headers (e.g., `timer.h` or `my_rand.h`), ensure all corresponding header files reside within the immediate compilation directory prior to initiating the compiler build.*
