#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

// Define the available strategies
enum AllocationStrategy {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

struct Block {
    int id;
    int start_address;
    int size;
    bool is_free;
    Block* next;
    
    Block(int i, int start, int s, bool free, Block* n = nullptr)
        : id(i), start_address(start), size(s), is_free(free), next(n) {}
};

class MemorySimulator {
private:
    Block* head;
    int total_memory_size;
    int next_id_counter;
    
    // NEW: Store the current strategy
    AllocationStrategy current_strategy;
    int total_allocations {};
    int successful_allocations {};
    int failed_allocations {};
public:
    MemorySimulator(int size);
    bool is_allocated(int addr);
    // NEW: Function to change strategy
    void set_strategy(AllocationStrategy strategy);
 

    int my_malloc(int requested_size);
    //bool my_free(int block_id);
    bool my_free(int physical_address);
    void dump_memory();
    void print_stats();
private:
    void merge_free_blocks();
};

#endif