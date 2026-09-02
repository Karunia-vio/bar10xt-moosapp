#--------------------------------------------------------
# The CMakeLists.txt for:                       pbar10xt
# Author(s):                              
#--------------------------------------------------------

# 1. Cari library libmodbus di sistem Linux (Raspberry Pi)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MODBUS REQUIRED libmodbus)

SET(SRC
  bar10xt.cpp
  bar10xt_Info.cpp
  main.cpp
  ModbusTCP.cpp
)

ADD_EXECUTABLE(pbar10xt ${SRC})

TARGET_LINK_LIBRARIES(pbar10xt
   MOOS
  #  AppCastingMOOS     
   ${MODBUS_LIBRARIES} # Link ke libmodbus
   apputil
   mbutil
   m
   pthread
)