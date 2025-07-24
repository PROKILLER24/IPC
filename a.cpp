#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include "shared_memory/SharedData.hh"

int main() {
    // Create + open SHM
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "[A] shm_open failed\n";
        return 1;
    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        std::cerr << "[A] ftruncate failed\n";
        return 1;
    }

    void* ptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "[A] mmap failed\n";
        return 1;
    }

    auto* data = static_cast<SharedWheelData*>(ptr);
    std::memset(data, 0, sizeof(*data));

    std::cout << "[A] Writing to SHM: " << SHM_NAME << " (" << SHM_SIZE << " bytes)\n";

    uint64_t seq = 0;
    float t = 0.0f;

    while (true) {
        float rpm = 100.0f + 20.0f * std::sin(t); // mock signal
        data->left_rpm  = rpm;
        data->right_rpm = rpm;  // if you need it
        data->seq       = ++seq;

        std::cout << "[A] Write left_rpm=" << rpm << " seq=" << seq << std::endl;

        t += 0.1f;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    return 0;
}
