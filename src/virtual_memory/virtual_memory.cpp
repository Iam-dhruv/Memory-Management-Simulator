#include "../../include/VirtualMemory.h"
#include <iomanip>

MMU::MMU(int pg_size, CacheController* c, MemorySimulator* sm) 
    : page_size(pg_size), cache(c), std_mem(sm), timer(0) {
    std::cout << "MMU Initialized with Page Size: " << page_size << " bytes\n";
}

void MMU::access(int virtual_address, std::string type) {
    timer++;
    
    // 1. Calculate VPN and Offset [cite: 96, 121]
    int vpn = virtual_address / page_size;
    int offset = virtual_address % page_size;

    // 2. Lookup in Page Table
    if (!page_table[vpn].valid) {
        std::cout << ">> Page Fault! VPN " << vpn << " not in memory.\n";
        
        // 3. Handle Page Fault (Bring data into Physical RAM) [cite: 108]
        if (!handle_page_fault(vpn)) {
            std::cout << "CRITICAL: Cannot resolve Page Fault. Memory Full?\n";
            return;
        }
    }

    // 4. Translation Success: Get Physical Address
    int frame_addr = page_table[vpn].frame_start_addr;
    int physical_address = frame_addr + offset;
    
    // Update Page Metadata (LRU)
    page_table[vpn].last_access_time = timer;
    if (type == "write") page_table[vpn].dirty = true;

    std::cout << "   [MMU] VA " << virtual_address << " -> VPN " << vpn 
              << " -> PA " << physical_address << "\n";

    // 5. Forward to Cache (Physical Address) [cite: 119]
    if (cache) {
        cache->access(physical_address, type);
    } else {
        std::cout << "   [MMU] Warning: No Cache connected. Access complete.\n";
    }
}

bool MMU::handle_page_fault(int vpn) {
    int allocated_addr = -1;

    // Try to allocate a new frame in Physical Memory
    if (std_mem) {
        allocated_addr = std_mem->my_malloc(page_size);
    }

    // If allocation failed (Memory Full), we need to EVICT a page [cite: 102, 106]
    if (allocated_addr == -1) {
        std::cout << ">> Physical Memory Full. Evicting a victim page...\n";
        evict_victim();
        
        // Retry allocation
        if (std_mem) allocated_addr = std_mem->my_malloc(page_size);

        if (allocated_addr == -1) return false; // Still failed
    }

    // Update Page Table with new Frame
    page_table[vpn].valid = true;
    page_table[vpn].frame_start_addr = allocated_addr;
    page_table[vpn].dirty = false;
    page_table[vpn].last_access_time = timer;
    
    loaded_pages.push_back(vpn); // Track for replacement logic
    std::cout << ">> Page " << vpn << " loaded into Frame at " << allocated_addr << "\n";
    
    return true;
}

void MMU::evict_victim() {
    // Simple LRU Policy for Page Replacement
    int victim_vpn = -1;
    int min_time = 2147483647;
    int victim_index = -1;

    for (size_t i = 0; i < loaded_pages.size(); i++) {
        int vpn = loaded_pages[i];
        if (page_table[vpn].last_access_time < min_time) {
            min_time = page_table[vpn].last_access_time;
            victim_vpn = vpn;
            victim_index = i;
        }
    }

    if (victim_vpn != -1) {
        // Free the Physical Memory
        int frame_addr = page_table[victim_vpn].frame_start_addr;
        if (std_mem) std_mem->my_free(frame_addr); 
        
        // Update Page Table
        page_table[victim_vpn].valid = false;
        if (page_table[victim_vpn].dirty) {
            std::cout << "   (Saving Dirty Page " << victim_vpn << " to disk...)\n";
        }
        
        // Remove from loaded list
        loaded_pages.erase(loaded_pages.begin() + victim_index);
        std::cout << ">> Evicted Page " << victim_vpn << " (Frame " << frame_addr << " freed)\n";
    }
}

void MMU::print_page_table() {
    std::cout << "--- Page Table ---\n";
    std::cout << "VPN   | Valid | Frame | Dirty | LRU Time\n";
    for (auto const& [vpn, entry] : page_table) {
        if (entry.valid) {
            std::cout << std::setw(5) << vpn << " | " 
                      << std::setw(5) << entry.valid << " | " 
                      << std::setw(5) << entry.frame_start_addr << " | " 
                      << std::setw(5) << entry.dirty << " | " 
                      << std::setw(8) << entry.last_access_time << "\n";
        }
    }
    std::cout << "------------------\n";
}