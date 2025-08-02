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

    // Write to shared memory
    const char* message = "Data from shared memory A\n";
    memcpy(ptr, message, strlen(message) + 1);

    // Open PTY and send
    const char* device = "/dev/pts/5";
    int fd = open(device, O_WRONLY);
    if (fd < 0) { perror("PTY open"); return 1; }

    write(fd, (char*)ptr, strlen((char*)ptr));
    std::cout << "Script A sent via PTY: " << (char*)ptr << std::endl;

    munmap(ptr, 1024);
    close(shm_fd);
    close(fd);
    return 0;
}
