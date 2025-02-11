# 🚀 **C Cache Simulator**  
### **A Low-Level, High-Performance Cache Modeling Tool**  

##  **What This Project Does**  
This **C-based cache simulator** efficiently models **cache behavior**, tracking **hits, misses, and evictions** based on memory access patterns. Designed as a **lightwieght impplementary model of Valgrind’s cachegrind**, this tool gives insight into memory allocation and cache optimization.  

🔹 **Implements a fully customized cache model**   
🔹 **Tracks memory hits, misses, and evictions**   
🔹 **Uses LRU (Least Recently Used) replacement strategy**   
🔹 **Dynamically allocates and deallocates memory on the heap**   
🔹 **Supports both read & write operations** ✍ 

 **Why?:** This tool is ideal for **low-level memory optimization**, understanding and teaching **cache mechanics**, and understanding **how compilers optimize performance**.  

---  

## ⚙️ **How It Works**  
1️⃣ **Creates a Virtual Cache Model** – Allocates a custom cache structure into memory.  
2️⃣ **Tracks Memory Accesses** – Identifies **hits, misses, and evictions** dynamically.  
3️⃣ **Implements LRU (Least Recently Used) Strategy** – Efficiently manages cache replacement.  
4️⃣ **Processes Read & Write Operations** – Simulates real-world memory access patterns.  

 **Memory is dynamically allocated** using `malloc` & `free`, optimizing for performance.  
 **Output provides insight into cache efficiency** with **hit/miss/eviction statistics**.  

---  

##  **Tech Stack & Concepts Used**  
- **Language:** C   
- **Memory Management:** `malloc`, `free`, `realloc`  
- **Data Structures:** Arrays, Structs, Linked Lists  
- **Algorithms:** Least Recently Used (LRU) Cache Replacement  
- **File Handling:** Reads & writes cache operations from input files  

---  

##  **How to Run**  
### 1️⃣ Clone the Repository  
```bash  
git clone https://github.com/yourusername/cache-simulator.git  
cd cache-simulator  
```  

### 2️⃣ Compile the Code  
```bash  
gcc -o cache_simulator cache_simulator.c lru_cache.c memory_manager.c -Wall -Wextra  
```  

### 3️⃣ Run the Simulator  
```bash  
./cache_simulator cachegrind_test.txt  
```  

### 4️⃣ View Results  
The program will output **cache hits, misses, and evictions** based on the input file.  

---  




