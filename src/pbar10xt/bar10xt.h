/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: bar10xt.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef bar10xt_HEADER
#define bar10xt_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "ModbusTCP.h"
#include <string>

class bar10xt : public AppCastingMOOSApp
{
 public:
   bar10xt();
   ~bar10xt();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
    std::string m_ip_address;
    int m_port;

 private: // State variables
    ModbusTCP    m_modbus;
    uint16_t     m_modbus_data[32];

    //data sensor
    double m_pressure;
    double m_temperature;
    double m_depth;
    int m_heartbeat;
};

#endif 
