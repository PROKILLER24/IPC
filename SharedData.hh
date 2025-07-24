#pragma once
#include <cstddef>
#include <cstdint>

struct SharedWheelData {
    float left_rpm;
    float right_rpm;
    uint64_t seq;   
};

constexpr const char* SHM_NAME = "/wheel_shm";
constexpr std::size_t SHM_SIZE = sizeof(SharedWheelData);
