# **C Cache Simulator**  

##  **What This Project Does**  
This C-based cache simulator efficiently models cache behavior, tracking hits, misses, and evictions based on memory access patterns.  

## **Features**
- Implements a fully customized cache model
- Tracks memory hits, misses, and evictions
- Uses LRU (Least Recently Used) replacement strategy**
- Dynamically allocates and deallocates memory on the heap**
- Supports both read & write operations**  

##  **Setup guide**  
Clone the Repository  
```bash  
git clone https://github.com/siddharthr2004/cache-simulator.git  
cd cache-simulator  
```  

## Compile the Code  
```bash  
gcc -o cache_simulator cache_simulator.c lru_cache.c memory_manager.c -Wall -Wextra  
```  

## Run the Simulator  
```bash  
./cache_simulator cachegrind_test.txt  
```  

## View Results  
The program will output **cache hits, misses, and evictions** based on the input file.  

---  




