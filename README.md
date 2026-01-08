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