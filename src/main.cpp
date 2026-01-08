#include <iostream>
#include <string>
#include <vector>
#include <limits> // For numeric_limits
#include "../include/MemoryManager.h"   // Standard Allocator
#include "../include/Cache.h"           // Cache System
#include "../include/VirtualMemory.h"   // Virtual Memory System

// --- Helper Functions for Help Menus ---

void print_generic_help() {
    std::cout << "--- General Help ---\n";
    std::cout << "Available Commands:\n";
    std::cout << "  init standard <size>     : Initialize memory allocator\n";
    std::cout << "  init_cache <size> <block> <ways> : Initialize L1/L2 Cache\n";
    std::cout << "  init_mmu <page_size>             : Initialize Virtual Memory (MMU)\n";
    std::cout << "  help <standard|cache|mmu>  : Specific help menus\n";
    std::cout << "  exit                             : Quit\n";
}

void print_standard_help() {
    std::cout << "--- Standard Allocator Help ---\n";
    std::cout << "Current Mode: Standard (Linked List)\n";
    std::cout << "Commands:\n";
    std::cout << "  malloc <size>                    : Allocate memory\n";
    std::cout << "  free <address>                        : Free block by address\n";
    std::cout << "  set allocator <first|best|worst> : Change strategy\n";
    std::cout << "  dump                             : Show memory map\n";
    std::cout << "  stats                            : Show fragmentation stats\n";
}


void print_cache_help() {
    std::cout << "--- Cache Simulation Help ---\n";
    std::cout << "Usage: init_cache <TotalSize> <BlockSize> <Associativity>\n";
    std::cout << "  <TotalSize>    : Total bytes in L1 (L2 is auto-set to 8x this)\n";
    std::cout << "  <BlockSize>    : Bytes per cache line (e.g., 64)\n";
    std::cout << "  <Associativity>: 1 = Direct Mapped, 2 = 2-Way Set Assoc, etc.\n";
    std::cout << "\nCommands:\n";
    std::cout << "  access <addr> <r|w> : Simulate access (Virtual if MMU active, else Physical)\n";
    std::cout << "  cache_stats         : Show Hits, Misses, and Hit Rate\n";
}

void print_mmu_help() {
    std::cout << "--- Virtual Memory Help ---\n";
    std::cout << "Usage: init_mmu <page_size>\n";
    std::cout << "  <page_size> : Size of a page/frame in bytes (e.g., 64)\n";
    std::cout << "\nCommands:\n";
    std::cout << "  access <v_addr> <r|w> : Access a Virtual Address (triggers translation)\n";
    std::cout << "  pt_dump               : Dump the Page Table\n";
}

// --- Main Function ---

int main() {
    // Subsystem Pointers
    MemorySimulator* standard_mem = nullptr;
    CacheController* cache_system = nullptr;
    MMU* mmu_system = nullptr;

    std::string command;
    std::cout << "========================================\n";
    std::cout << "   Memory & Cache Simulator Started\n";
    std::cout << "========================================\n";
    std::cout << "Type 'help' for commands.\n";
    
    while (true) {
        std::cout << "> "; 
        std::cin >> command;

        if (command == "exit") break; 
        
        // --- HELP COMMANDS ---
        else if (command == "help") {
            std::string sub;
            if (std::cin.peek() == ' ') { 
                std::cin >> sub;
                if (sub == "standard") print_standard_help();
                else if (sub == "cache") print_cache_help();
                else if (sub == "mmu") print_mmu_help();
                else print_generic_help();
            } else {
                print_generic_help();
            }
        }

        // --- MEMORY ALLOCATOR INIT ---
        else if (command == "init") {
            std::string type;
            int size;
            std::cin >> type >> size;

            // Cleanup old systems if re-initializing
            if (standard_mem) { delete standard_mem; standard_mem = nullptr; }
            if (cache_system) { 
                std::cout << "Note: Cache reset due to memory change.\n"; 
                delete cache_system; cache_system = nullptr; 
            }
            if (mmu_system) {
                std::cout << "Note: MMU reset due to memory change.\n";
                delete mmu_system; mmu_system = nullptr;
            }

            if (type == "standard") {
                standard_mem = new MemorySimulator(size);
                std::cout << "Standard Allocator Initialized (" << size << " bytes).\n";
            }else {
                std::cout << "Unknown type. Use 'standard'.\n";
            }
        }

        // --- CACHE INIT ---
        else if (command == "init_cache") {
            int size, block_size, assoc;
            if (std::cin >> size >> block_size >> assoc) {
                if (cache_system) delete cache_system;
                
                // Create L1 (Size) and L2 (Size * 8)
                CacheLevel* l1 = new CacheLevel(1, size, block_size, assoc);
                CacheLevel* l2 = new CacheLevel(2, size * 8, block_size, assoc);
                
                // Link Cache to currently active memory
                cache_system = new CacheController(l1, l2, standard_mem);
                
                std::cout << "Cache Initialized (L1: " << size << "B, L2: " << size*8 << "B).\n";
                if (standard_mem ) std::cout << "-> Linked to Active Memory.\n";
                else std::cout << "-> Warning: No Memory Initialized yet.\n";
                
            } else {
                std::cout << "Invalid arguments.\n";
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

        // --- MMU INIT ---
        else if (command == "init_mmu") {
            int page_size;
            if (std::cin >> page_size) {
                if (!standard_mem) {
                    std::cout << "Error: Initialize memory (init standard) first.\n";
                } else {
                    if (mmu_system) delete mmu_system;
                    // Pass Cache and Memory to MMU
                    mmu_system = new MMU(page_size, cache_system, standard_mem);
                    std::cout << "Virtual Addressing Enabled.\n";
                }
            } else {
                std::cout << "Invalid arguments.\n";
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        
        // --- DATA ACCESS COMMAND (The Hub) ---
        else if (command == "access") {
            int addr; 
            std::string type;
            std::cin >> addr >> type;
            
            if (type == "r") type = "read";
            if (type == "w") type = "write";

            if (mmu_system) {
                // 1. Virtual Memory Mode
                // Flow: User -> MMU -> Cache -> Memory
                mmu_system->access(addr, type);
            } 
            else if (cache_system) {
                // 2. Physical Cache Mode (Legacy)
                // Flow: User -> Cache -> Memory
                std::cout << "[Physical Access] ";
                cache_system->access(addr, type);
                std::cout << "Access processed.\n";
            } 
            else {
                std::cout << "Error: Neither MMU nor Cache is initialized.\n";
            }
        }
        
        // --- STATS & DUMPS ---
        else if (command == "cache_stats") {
            if (cache_system) cache_system->dump_stats();
            else std::cout << "Cache not initialized.\n";
        }
        else if (command == "pt_dump") {
            if (mmu_system) mmu_system->print_page_table();
            else std::cout << "MMU not initialized.\n";
        }
        
        // --- MEMORY OPERATIONS ---
        else if (command == "malloc") {
            int size;
            if (std::cin >> size) {
                if (standard_mem) standard_mem->my_malloc(size);
                else std::cout << "Error: System not initialized. Use 'init'.\n";
            } else {
                 std::cout << "Invalid size.\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } 
        
        else if (command == "free") {
            int val;
            if (std::cin >> val) {
                if (standard_mem) standard_mem->my_free(val); // Val is ID
                else std::cout << "Error: System not initialized.\n";
            } else {
                 std::cout << "Invalid input.\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } 
        
        else if (command == "dump") {
             if (standard_mem) standard_mem->dump_memory();
            else std::cout << "System not initialized.\n";
        }

        else if (command == "stats") {
             if (standard_mem) standard_mem->print_stats();
            else std::cout << "System not initialized.\n";
        }

        else if (command == "set") {
            std::string sub_cmd, strategy;
            std::cin >> sub_cmd; 
            if (sub_cmd == "allocator") {
                std::cin >> strategy;
                if (standard_mem) {
                    if (strategy == "first") standard_mem->set_strategy(FIRST_FIT);
                    else if (strategy == "best") standard_mem->set_strategy(BEST_FIT);
                    else if (strategy == "worst") standard_mem->set_strategy(WORST_FIT);
                    else std::cout << "Unknown strategy.\n";
                } else {
                    std::cout << "Command not available in current mode.\n";
                }
            }
        }
        
        else {
            std::cout << "Unknown command. Type 'help'.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    // Cleanup

    if (standard_mem) delete standard_mem;
    if (cache_system) delete cache_system;
    if (mmu_system) delete mmu_system;

    return 0;
}