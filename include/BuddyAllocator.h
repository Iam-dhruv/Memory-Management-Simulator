#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <vector>
#include <map>
#include <cmath>

class BuddyAllocator {
private:
    // Stores free blocks. free_lists[i] holds blocks of size 2^i
    std::vector<std::vector<int>> free_lists; 
    
    // Remembers the size (order) of allocated blocks
    // Key: Address, Value: Order (k)
    std::map<int, int> allocation_map; 
    std::map<int, int> requested_size_map;
    int total_memory_size;
    int max_order; // The power of 2 that covers total memory

    int used_memory_actual;
    int used_memory_padded;
public:
    BuddyAllocator(int size);
    bool is_allocated(int addr);
    int allocate(int size);
    void deallocate(int address);
    void dump();
    
    void print_stats();
private:
    // Helper to calculate the exponent (order) for a given size
    int get_order(int size);
};

#endif