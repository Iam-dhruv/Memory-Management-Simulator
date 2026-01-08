#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cmath>
#include <iostream>
#include <string>
class MemorySimulator;
//class BuddyAllocator;
// 1. A Single Cache Block (The "Line")
struct CacheLine {
    bool valid;
    unsigned long long tag;
    unsigned long long last_access_time; // For LRU Policy

    CacheLine() : valid(false), tag(0), last_access_time(0) {}
};

// 2. A Single Level of Cache (e.g., L1)
class CacheLevel {
private:
    int level_id;           // 1 for L1, 2 for L2
    size_t size;            // Total bytes
    size_t block_size;      // Bytes per block
    int associativity;      // Ways per set (1 = Direct Mapped)
    
    int num_sets;
    int index_bits;
    int offset_bits;

    // The Storage: A Vector of Sets, where each Set is a Vector of Lines
    std::vector<std::vector<CacheLine>> sets;

    // Stats
    long long hits = 0;
    long long misses = 0;
    unsigned long long access_counter = 0; // Global time for LRU

public:
    CacheLevel(int id, size_t s, size_t bs, int assoc);
    
    // Returns true if HIT, false if MISS
    bool access(unsigned long long address, bool is_write);
    
    void print_stats();
    void reset_stats();

private:
    // Helper to extract parts of the address
    unsigned long long get_tag(unsigned long long addr);
    unsigned long long get_index(unsigned long long addr);
};

// 3. The Controller (Manages L1 -> L2)
class CacheController {
private:
    CacheLevel* L1;
    CacheLevel* L2; // Optional, can be nullptr
    MemorySimulator* std_mem;
public:
    CacheController(CacheLevel* l1, CacheLevel* l2 = nullptr,MemorySimulator* sm = nullptr);
    ~CacheController();

    void access(unsigned long long address, std::string type); // "read" or "write"
    void dump_stats();
};

#endif