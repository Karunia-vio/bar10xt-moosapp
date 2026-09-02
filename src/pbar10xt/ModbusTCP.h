#ifndef MB_TCP_H
#define MB_TCP_H

#include <cstring>
#include <errno.h>
#include <iostream>
#include <modbus/modbus.h>
#include <unistd.h>

class ModbusTCP {
private:
  modbus_t *mb;
  uint16_t tab_reg[32];

  std::string current_ip;
  int current_port;
  bool m_connected;

public:
  ModbusTCP();
  ~ModbusTCP();

  bool connect(const std::string &ip, int port);
  void disconnect();
  bool reconnect(uint8_t max_count);
  void readRegister(int addr, int nb, uint16_t *value);
  void writeRegister(int addr, const int16_t value);

protected:
};

#endif // MODBUS_TCP_H
