#include <iostream>
#include "../../include/MemoryManager.h" // Import the blueprint

// Constructor Implementation
MemorySimulator::MemorySimulator(int size) {
    total_memory_size = size;
    next_id_counter = 1;
    head = new Block(-1, 0, size, true, nullptr);
    current_strategy = FIRST_FIT;
}

void MemorySimulator::set_strategy(AllocationStrategy strategy){
    current_strategy = strategy;
}
// Malloc Implementation (First Fit)
int MemorySimulator::my_malloc(int requested_size) {
    total_allocations++;
    Block* best_block = nullptr;
    Block* current = head;

    // --- STRATEGY SELECTION LOGIC ---
    
    if (current_strategy == FIRST_FIT) {
        // Stop at the very first match
        while (current != nullptr) {
            if (current->is_free && current->size >= requested_size) {
                best_block = current;
                break; 
            }
            current = current->next;
        }
    } 
    
    else if (current_strategy == BEST_FIT) {
        // Scan ALL blocks. Find the smallest one that fits.
        // Goal: Minimize wasted space (leftover).
        while (current != nullptr) {
            if (current->is_free && current->size >= requested_size) {
                if (best_block == nullptr || current->size < best_block->size) {
                    best_block = current;
                }
            }
            current = current->next;
        }
    } 
    
    else if (current_strategy == WORST_FIT) {
        // Scan ALL blocks. Find the largest one that fits.
        // Goal: Leave a gap big enough to be useful later.
        while (current != nullptr) {
            if (current->is_free && current->size >= requested_size) {
                if (best_block == nullptr || current->size > best_block->size) {
                    best_block = current;
                }
            }
            current = current->next;
        }
    }

    // --- ALLOCATION LOGIC (Common to all) ---

    if (best_block == nullptr) {
        failed_allocations++;
        std::cout << "Allocation failed: Not enough memory!\n";
        return -1;
    }

    // Split the block if it's too big
    if (best_block->size > requested_size) {
        int remaining_size = best_block->size - requested_size;
        int new_block_start = best_block->start_address + requested_size;

        Block* new_free_block = new Block(
            -1, new_block_start, remaining_size, true, best_block->next
        );
        best_block->next = new_free_block;
    }

    // Update the chosen block
    best_block->size = requested_size;
    best_block->is_free = false;
    best_block->id = next_id_counter;
    
    int allocated_id = next_id_counter;
    next_id_counter++;
    successful_allocations++;
    std::cout << "Allocated " << best_block->size << " bytes at " << best_block->start_address << " (ID = " <<allocated_id<< ")\n";
    return best_block->start_address;
}

void MemorySimulator::merge_free_blocks() {
    Block* current = head;

    while (current != nullptr && current->next != nullptr) {
        // Check if THIS block and the NEXT block are both FREE
        if (current->is_free && current->next->is_free) {
            
            // 1. Combine sizes
            current->size += current->next->size;
            
            // 2. Remove the next block from the list
            Block* temp = current->next;
            current->next = current->next->next;
            
            // 3. Delete the C++ object (cleanup)
            delete temp; 

            // IMPORTANT: Do NOT advance 'current' yet.
            // We just created a bigger block. We need to check if 
            // this NEW bigger block can merge with the *next* one too.
        } else {
            // Only move forward if we didn't merge
            current = current->next;
        }
    }
}
// bool MemorySimulator::my_free(int block_id) {
//     Block* current = head;
//     bool found = false;

//     // Step 1: Find the block
//     while (current != nullptr) {
//         if (!current->is_free && current->id == block_id) {
//             // Found it! Mark as free.
//             current->is_free = true;
//             current->id = -1; // Reset ID
//             found = true;
//             std::cout << "Block " << block_id << " freed.\n";
//             break; 
//         }
//         current = current->next;
//     }

//     // Step 2: If found, merge neighbors
//     if (found) {
//         merge_free_blocks();
//         return true;
//     } else {
//         std::cout << "Error: Block ID " << block_id << " not found or already free.\n";
//         return false;
//     }
// }

// Add to MemorySimulator class
bool MemorySimulator::is_allocated(int addr) {
    Block* current = head;
    while (current != nullptr) {
        // Check if address is within this block's range
        if (addr >= current->start_address && addr < (current->start_address + current->size)) {
            return !current->is_free; // True if USED, False if FREE
        }
        current = current->next;
    }
    return false; // Address out of bounds
}

bool MemorySimulator::my_free(int physical_address) {
    Block* current = head;
    bool found = false;

    // Search for the block starting at this specific address
    while (current != nullptr) {
        // We look for a USED block that starts EXACTLY at the given address
        if (!current->is_free && current->start_address == physical_address) {
            current->is_free = true;
            current->id = -1; // Reset ID
            found = true;
            std::cout << "Block at address " << physical_address << " freed.\n";
            break; 
        }
        current = current->next;
    }

    if (found) {
        merge_free_blocks(); // Coalesce neighbors immediately
        return true;
    } else {
        std::cout << "Error: MMU tried to free invalid address " << physical_address << "\n";
        return false;
    }
}
// Dump Memory Implementation
void MemorySimulator::dump_memory() {
    std::cout << "\n--- Memory Dump ---\n";
    Block* current = head;
    while (current != nullptr) {
        std::cout << "[" << current->start_address << " - " 
                  << (current->start_address + current->size - 1) << "] ";
        
        if (current->is_free) {
            std::cout << "FREE\n";
        } else {
            std::cout << "USED (ID=" << current->id << ")\n";
        }
        current = current->next;
    }
    std::cout << "-------------------\n";
}

void MemorySimulator::print_stats(){
    int used_memory {};
    int free_memory {};
    int largest_free_block {};
    int internal_fragmentation {};

    Block* current = head;
    while(current!= nullptr){
        if(current->is_free) {
            free_memory += current->size;
            if(current->size > largest_free_block){
                largest_free_block = current->size;
            }
        } else {
            used_memory += current->size;
        }
        current = current->next;
    }

    double ext_frag_percent = 0.0;
    if(free_memory > 0 ){
        ext_frag_percent = 1.0 - ((double)largest_free_block/free_memory);

    }
    std::cout << "\n--- Statistics ---\n";
    std::cout << "Total Memory:       " << total_memory_size << "\n";
    std::cout << "Used Memory:        " << used_memory << " (" 
              << ((double)used_memory / total_memory_size) * 100 << "%)\n";
    std::cout << "Free Memory:        " << free_memory << "\n";
    std::cout << "Total Requests:     " << total_allocations << "\n";
    std::cout << "Success Rate:       " << successful_allocations << "/" 
              << total_allocations << "\n";
    std::cout << "Internal Frag:      " << internal_fragmentation << "\n";
    std::cout << "External Frag:      " << ext_frag_percent * 100 << "%\n";
    std::cout << "------------------\n";
}