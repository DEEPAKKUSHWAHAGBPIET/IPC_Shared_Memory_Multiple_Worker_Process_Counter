This program demonstrates Inter-Process Communication (IPC) using System V shared memory and atomic operations in C.

The design simulates a master process that spawns multiple worker processes.

Each worker updates its own counter in shared memory.

The master aggregates all counters and prints the results.
