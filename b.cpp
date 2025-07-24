#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include "shared_memory/SharedData.hh"

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::connection_hdl;

server ws_server;
connection_hdl current_connection;
bool client_connected = false;

// --- WS callbacks ---
void on_open(server* s, connection_hdl hdl) {
    std::cout << "[B] Client connected\n";
    current_connection = hdl;
    client_connected = true;
}

void on_close(server* s, connection_hdl hdl) {
    std::cout << "[B] Client disconnected\n";
    client_connected = false;
}

void start_websocket_server() {
    ws_server.init_asio();
    ws_server.set_open_handler(std::bind(&on_open, &ws_server, std::placeholders::_1));
    ws_server.set_close_handler(std::bind(&on_close, &ws_server, std::placeholders::_1));

    ws_server.listen(9002);
    ws_server.start_accept();

    std::thread([](){
        ws_server.run();
    }).detach();
}

// --- read from A (SHM) ---
float listen_from_a(SharedWheelData* data, uint64_t& last_seq) {
    if (data->seq != last_seq) {
        last_seq = data->seq;
    }
    float rpm = data->left_rpm;
    std::cout << "[B] Read from SHM: left_rpm=" << rpm << " seq=" << last_seq << std::endl;
    return rpm;
}

// --- send to C (WS) ---
void send_to_c(float rpm) {
    if (!client_connected) return;

    std::string msg = std::to_string(rpm);
    websocketpp::lib::error_code ec;
    ws_server.send(current_connection, msg, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "[B] send() failed: " << ec.message() << std::endl;
    } else {
        std::cout << "[B] Sent to WS: " << msg << std::endl;
    }
}

// --- main loop ---
void main_loop() {
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        std::cerr << "[B] Failed to open shared memory\n";
        return;
    }

    void* ptr = mmap(nullptr, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "[B] mmap failed\n";
        close(shm_fd);
        return;
    }

    auto* data = static_cast<SharedWheelData*>(ptr);
    uint64_t last_seq = 0;

    while (true) {
        float rpm = listen_from_a(data, last_seq);
        send_to_c(rpm);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    munmap(ptr, SHM_SIZE);
    close(shm_fd);
}

int main() {
    std::cout << "[B] Starting WebSocket++ Server on ws://localhost:9002\n";
    start_websocket_server();
    main_loop();
    return 0;
}
