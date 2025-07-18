
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

int main() {
    const char* name = "/my_shm";
    const int SIZE = 4096;

    // 1. Opening of the shared memory
    int shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to open shared memory.\n";
        return 1;
    }

    // 2. Maping the memory in the format 
    void* ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Mapping failed.\n";
        return 1;
    }

    // 3. showing the memory
    std::cout << "Reader: Received message -> " << static_cast<char*>(ptr) << std::endl;

    // 4. shm_unlink for the delte it.
    munmap(ptr, SIZE);
    close(shm_fd);
    shm_unlink(name);  

    return 0;
}
