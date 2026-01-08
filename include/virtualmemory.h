#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <map>
#include <vector>
#include <iostream>
#include "Cache.h"
#include "MemoryManager.h"

// Struct for a single entry in the Page Table
struct PageTableEntry {
    bool valid;          // Is it in Physical Memory?
    int frame_start_addr; // The Physical Address where this page starts
    bool dirty;          // Has it been written to?
    int last_access_time;// For LRU Page Replacement

    PageTableEntry() : valid(false), frame_start_addr(-1), dirty(false), last_access_time(0) {}
};

class MMU {
private:
    // Config
    int page_size;
    int timer; // Global counter for LRU

    // The Page Table: Maps VPN (Virtual Page Number) -> PTE
    std::map<int, PageTableEntry> page_table;

    // References to Hardware
    CacheController* cache;
    MemorySimulator* std_mem;

    // Track allocated VPNs to help with replacement
    std::vector<int> loaded_pages; 

public:
    MMU(int pg_size, CacheController* c, MemorySimulator* sm);

    // Main Interface: Accepts a Virtual Address
    void access(int virtual_address, std::string type);

    // Debugging
    void print_page_table();

private:
    // Helper to handle Page Faults
    bool handle_page_fault(int vpn);
    
    // Helper to perform Page Replacement (if memory is full)
    void evict_victim();
};

#endif