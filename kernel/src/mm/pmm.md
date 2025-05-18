# Soaplin's Physical Memory Manager

The Physical Memory Manager (PMM) in Soaplin uses a lazy-loading design that efficiently manages physical memory pages while minimizing boot time overhead.

## Design Overview

The PMM uses a two-level allocation strategy:
1. Region List - tracks large blocks of available physical memory
2. Free Page List - manages individual pages ready for immediate allocation

### Memory Regions

Each memory region is tracked by a `pmm_region_t` structure that contains:
- Base address of the available memory
- Length of remaining memory
- Pointer to next region

The region structure is cleverly stored in the first page of the region itself, making the overhead minimal (just one 4KB page per region).
When the region has been totally consumed, it's metadata page is turned
into a free page that can be allocated.

### Free Page List

The free page list is a singly-linked list of individual pages that are ready for immediate allocation. It gets refilled from regions only when needed.

## Lazy Loading

Instead of initializing all free pages at boot time, the PMM:
1. Only initializes region structures during boot
2. Adds pages to the free list on-demand
3. Consumes memory regions gradually as needed

This approach provides several benefits:
- Very fast boot times regardless of RAM size
- Memory overhead proportional to number of regions, not total RAM
- No performance penalty during normal operation

## Memory Organization

Physical memory is organized as follows:
- Each region's first page contains the region metadata
- Remaining pages in each region are available for allocation
- Pages are standard 4KB size
- Free pages are linked together in the free list

## Usage

The PMM provides three main functions:
- `pmm_init()` - Initializes the PMM from the bootloader's memory map
- `pmm_alloc_page()` - Allocates a single 4KB page
- `pmm_free_page()` - Returns a page to the free list

## Implementation Details

### Region Initialization
During boot, the PMM:
1. Receives memory map from Limine
2. Identifies usable memory regions
3. Sets up region tracking structures
4. Calculates total available pages

### Page Allocation
When allocating pages:
1. First tries the free list
2. If free list is empty:
   - Takes 4 pages from current region
   - Adds it to free list
   - Updates region metadata
   - If the region has been consumed
    - Let the next region take the head
    - Free the region's metadata page.
3. Returns the page to the caller

### Memory Tracking
The PMM maintains counters for:
- Total available pages
- Currently free pages
This allows for memory usage monitoring and OOM detection.
