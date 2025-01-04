I never got to implement malloc on my own in college, so this was me doing it myself.

Basic allocator that will resize memory blocks when necessary.
- will split blocks into smaller blocks, so small amounts of memory don't take up big blocks
- will combine blocks into larger blocks; initial heap fragmentation handling
