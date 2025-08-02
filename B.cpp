#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Setup shared memory
    const char* shm_path = "/tmp/shm_comm";
    int shm_fd = open(shm_path, O_RDWR | O_CREAT, 0666);
    ftruncate(shm_fd, 1024);
    void* ptr = mmap(nullptr, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Open PTY and receive
    const char* device = "/dev/pts/6";
    int fd = open(device, O_RDONLY);
    if (fd < 0) { perror("PTY open"); return 1; }

    char buffer[1024] = {0};
    read(fd, buffer, sizeof(buffer));
    std::cout << "Script B received from PTY: " << buffer;

    // Save to shared memory
    memcpy(ptr, buffer, strlen(buffer) + 1);
    std::cout << "Saved to shared memory B: " << (char*)ptr << std::endl;

    munmap(ptr, 1024);
    close(shm_fd);
    close(fd);
    return 0;
}
