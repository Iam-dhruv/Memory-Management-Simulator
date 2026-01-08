# Memory Management Simulator

A comprehensive C++ simulation of an Operating System's memory management hierarchy. This tool models the interactions between the **Memory Management Unit (MMU)**, a **Multilevel Cache (L1/L2)**, and **Physical Main Memory**.

It is designed to demonstrate key OS concepts including dynamic memory allocation strategies, cache associativity & replacement policies, and virtual memory paging.

## 🚀 Features

* **Physical Memory Allocator**: Implements a linked-list based allocator supporting **First Fit**, **Best Fit**, and **Worst Fit** strategies with automatic free-block coalescing.
* **Multilevel Cache System**: Simulates an L1 and L2 cache hierarchy with configurable block sizes and associativity (Direct Mapped, N-Way Set Associative). Implements **LRU (Least Recently Used)** replacement.
* **Virtual Memory (MMU)**: Simulates paging with a configurable page size. Handles **Virtual-to-Physical address translation**, **Page Faults**, and **Page Replacement** (LRU).
* **Interactive CLI**: A command-line interface to issue instructions and inspect the state of the system in real-time.

---

## 🛠️ Building the Project

This project uses a standard `Makefile` for compilation.

### Prerequisites
* G++ (or any standard C++17 compliant compiler)
* Make

### Compilation
Run the following command in the root directory:
```bash
make 
```
To run the simulator:

```bash
./memsim
```
To clean build artifacts:

```bash

make clean
```
## 📖 Usage Modes
The simulator is flexible and can be run in three distinct modes depending on which components you initialize.

### Mode 1: Physical Memory Allocator (Malloc/Free)
Focus: Understanding fragmentation and allocation strategies.

In this mode, you interact directly with physical memory. You can allocate blocks, free them, and observe how different strategies (First Fit, Best Fit, Worst Fit) affect memory layout and fragmentation.

Example Session:

```bash

# 1. Initialize 1024 bytes of physical RAM
> init standard 1024
Standard Allocator Initialized (1024 bytes).
# 2. Set Strategy to Best Fit (Default is First Fit)
> set allocator best
# 3. Allocate memory
> malloc 100
Allocated 100 bytes at 0 (ID = 1)
> malloc 200
Allocated 200 bytes at 100 (ID = 2)

# 4. Free memory (by Address)
> free 100
Block at Address 100 freed.

# 5. Inspect memory map
> dump
--- Memory Dump ---
[0 - 99] USED (ID=1)
[100 - 1023] FREE
-------------------
```
### Mode 2: Cache Simulation (Physical Addressing)
Focus: Cache hits, misses, and associativity.

In this mode, you enable the Cache Controller. Memory accesses are simulated as Physical Addresses. You can see how data moves from Main Memory → L2 → L1.

Prerequisite: You must initialize memory first.

Example Session:

```bash

# 1. Init Memory
> init standard 1024
Standard Allocator Initialized (1024 bytes).
# 2. Init Cache: Size=64B, Block=16B, Associativity=2-Way
> init_cache 64 16 2
Cache Initialized (L1: 64B, L2: 512B).
-> Linked to Active Memory.
# 3. Access Physical Address 10 (Read)
> access 10 r
--- L2 MISS! Accessing Main Memory ---
>> Main Memory: Fetching data...
Access 10 (read) processed.

# 4. Access Address 10 again (Should be a HIT)
> access 10 r
Access 10 (read) processed.

# 5. View Stats
> cache_stats
L1 Stats: Hits: 1, Misses: 1, Hit Rate: 50.00%
```
### Mode 3: Full System (Virtual Memory + MMU)
Focus: Paging, Address Translation, and Page Faults.

This is the complete simulation. You issue Virtual Addresses. The MMU translates them to Physical Addresses (handling Page Faults if necessary), and then requests data from the Cache.

Flow: User (Virtual Addr) → MMU → Physical Addr → Cache → RAM.

Example Session:

```bash

# 1. Init Memory & Cache
> init standard 1024
> init_cache 256 64 1

# 2. Enable Virtual Memory (Page Size = 64 bytes)
> init_mmu 64

# 3. Write to Virtual Address 0
> access 0 w
>> Page Fault! VPN 0 not in memory.
Allocated 64 bytes at Address 0
>> Page 0 loaded into Frame at 0
[MMU] VA 0 -> VPN 0 -> PA 0
Access 0 (write) processed.

# 4. View Page Table
> pt_dump
VPN   | Valid | Frame | Dirty | LRU Time
    0 |     1 |     0 |     1 |        1
```