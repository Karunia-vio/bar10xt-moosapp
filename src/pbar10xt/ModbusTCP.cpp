#include "ModbusTCP.h"
#include <chrono>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <thread>
#include <unistd.h>
ModbusTCP::ModbusTCP() {}

ModbusTCP::~ModbusTCP() { disconnect(); }

bool ModbusTCP::connect(const std::string &ip, int port) {
  disconnect();

  current_ip = ip;
  current_port = port;

  mb = modbus_new_tcp(ip.c_str(), port);
  if (!mb) {
    std::cerr << "Unable to create Modbus context" << std::endl;
  }
  if (modbus_connect(mb) == -1) {

    std::cerr << "Connection failed: " << modbus_strerror(errno) << std::endl;
    return false;
  }
  std::cout << "Connected to Modbus TCP Server!" << std::endl;

  return true;
}

void ModbusTCP::disconnect() {
  if (mb) {
    modbus_close(mb);
    modbus_free(mb);
    mb = nullptr;
  }
}

bool ModbusTCP::reconnect(uint8_t max_count) {
  bool connected = false;
  uint8_t reconnect_count = 0;
  std::cout << "Reconnect..." << std::endl;
  disconnect();
  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (connect(current_ip, current_port)) {
      std::cout << "Reconnect berhasil " << std::endl;
      return true;
    } else {
      reconnect_count++;
      std::cout << "Coba Reconnect ulang" << std::endl;
    }
  } while (reconnect_count < max_count);
  return false;
}

void ModbusTCP::readRegister(int addr, int nb, uint16_t *value)

{
  int rc = modbus_read_registers(mb, addr, nb, tab_reg);
  if (rc == -1) {
    std::cerr << "Failed to read registers:" << modbus_strerror(errno)
              << std::endl;

    *value = -1;

    if (errno == EBADF || errno == EPIPE || errno == ECONNRESET ||
        errno == ECONNREFUSED) {
      std::cout << "Koneksi terputus. Mencoba reconnect..." << std::endl;
      if (!reconnect(3)) {
        std::cerr << "Reconnect gagal. Tidak bisa baca register." << std::endl;
        return;
      }

      rc = modbus_read_registers(mb, addr, nb, tab_reg);
      if (rc == -1) {
        std::cerr << "Baca ulang setelah reconnect gagal: "
                  << modbus_strerror(errno) << std::endl;
        return;
      }
    } else {
      return;
    }
  }
  *value = tab_reg[0];
}

void ModbusTCP::writeRegister(int addr, const int16_t value) {
  int rw = modbus_write_register(mb, addr, value);

  if (rw == -1) {
    std::cerr << "gagal write register: " << modbus_strerror(errno)
              << std::endl;
    if (errno == EBADF || errno == EPIPE || errno == ECONNRESET ||
        errno == ECONNREFUSED) {
      std::cout << "Koneksi terputus. Mencoba reconnect..." << std::endl;
      if (!reconnect(3)) {
        std::cerr << "Reconnect gagal. Tidak bisa tulis register." << std::endl;
        return;
      }

      rw = modbus_write_register(mb, addr, value);
      if (rw == -1) {
        std::cerr << "write ulang setelah reconnect gagal: "
                  << modbus_strerror(errno) << std::endl;
        return;
      }
    } else {
      return;
    }
  }
  std::cout << "berhasil write register = " << value << std::endl;
}
