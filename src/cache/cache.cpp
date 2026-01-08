#include "../../include/Cache.h"
#include "../../include/MemoryManager.h"
#include <iomanip>

// --- CacheLevel Implementation ---

CacheLevel::CacheLevel(int id, size_t s, size_t bs, int assoc) {
    level_id = id;
    size = s;
    block_size = bs;
    associativity = assoc;

    num_sets = size / (block_size * associativity);
    offset_bits = std::log2(block_size);
    index_bits = std::log2(num_sets);

    // Initialize Storage
    sets.resize(num_sets);
    for (int i = 0; i < num_sets; i++) {
        sets[i].resize(associativity);
    }
}

unsigned long long CacheLevel::get_index(unsigned long long addr) {
    // Shift out offset, mask to get index
    return (addr >> offset_bits) & (num_sets - 1);
}

unsigned long long CacheLevel::get_tag(unsigned long long addr) {
    // Shift out offset and index
    return addr >> (offset_bits + index_bits);
}

bool CacheLevel::access(unsigned long long address, bool is_write) {
    access_counter++; // Increment "time"
    
    unsigned long long index = get_index(address);
    unsigned long long tag = get_tag(address);

    // 1. Search the Set for the Tag
    for (int i = 0; i < associativity; i++) {
        if (sets[index][i].valid && sets[index][i].tag == tag) {
            // HIT!
            hits++;
            sets[index][i].last_access_time = access_counter; // Update LRU
            return true;
        }
    }

    // 2. MISS! We need to allocate a line.
    misses++;

    // Find a victim (Invalid line OR Least Recently Used)
    int victim_way = -1;
    unsigned long long min_time = -1; // Max Value

    // First pass: Check for empty slots
    for (int i = 0; i < associativity; i++) {
        if (!sets[index][i].valid) {
            victim_way = i;
            break;
        }
    }

    // Second pass: If full, find LRU
    if (victim_way == -1) {
        for (int i = 0; i < associativity; i++) {
            if (sets[index][i].last_access_time < min_time) {
                min_time = sets[index][i].last_access_time;
                victim_way = i;
            }
        }
    }

    // Replace
    sets[index][victim_way].valid = true;
    sets[index][victim_way].tag = tag;
    sets[index][victim_way].last_access_time = access_counter;

    return false; // Return false to signal a MISS (so we can check L2)
}

void CacheLevel::print_stats() {
    std::cout << "L" << level_id << " Stats: ";
    std::cout << "Hits: " << hits << ", Misses: " << misses;
    if (hits + misses > 0) {
        double ratio = (double)hits / (hits + misses) * 100.0;
        std::cout << ", Hit Rate: " << std::fixed << std::setprecision(2) << ratio << "%";
    }
    std::cout << "\n";
}

// --- CacheController Implementation ---

CacheController::CacheController(CacheLevel* l1, CacheLevel* l2, MemorySimulator* sm) 
    : L1(l1), L2(l2), std_mem(sm) {}

CacheController::~CacheController() {
    delete L1;
    if (L2) delete L2;
}

void CacheController::access(unsigned long long address, std::string type) {
    bool is_write = (type == "write");
    
    // 1. Check L1
    if (L1->access(address, is_write)) {
        std::cout << "--- L1 HIT ---" <<std::endl;
        return; 
    }

    // 2. Check L2 (if exists)
    if (L2 && L2->access(address, is_write)) {
         std::cout << "--- L2 HIT ---" <<std::endl;
        return;
    }

    // 3. CACHE MISS on all levels! Access Main Memory.
    std::cout << "--- L2 MISS! Accessing Main Memory at " << address << " ---\n";
    
    bool valid_access = false;
    
    if (std_mem) {
        valid_access = std_mem->is_allocated(address);
    }
    if (valid_access) {
        std::cout << ">> Main Memory: Fetching data from valid block.\n";
        // In a real OS, we would bring this line into L2/L1 here.
    } else {
        std::cout << ">> SEGMENTATION FAULT: Attempted to access unallocated memory!\n";
    }
}

void CacheController::dump_stats() {
    std::cout << "--- Cache Statistics ---\n";
    L1->print_stats();
    if (L2) L2->print_stats();
    std::cout << "------------------------\n";
}