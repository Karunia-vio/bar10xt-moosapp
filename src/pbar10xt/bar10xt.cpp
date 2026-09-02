/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: bar10xt.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
// #include "ACTable.h"
#include "bar10xt.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

bar10xt::bar10xt()
{
  m_ip_address = "10.5.51.45"; 
  m_port       = 502;

  for(int i=0; i<32; i++) {
        m_modbus_data[i] = 0;
    }

  m_pressure = 0.0;
  m_temperature = 0.0;
  m_depth = 0.0;
  m_heartbeat = 0;
}

//---------------------------------------------------------
// Destructor

bar10xt::~bar10xt()
{
  m_modbus.disconnect();
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool bar10xt::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "FOO") 
       cout << "great!";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool bar10xt::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool bar10xt::Iterate()
{
  AppCastingMOOSApp::Iterate();

    Notify("BAR10XT_TEST_ALIVE", 123);
      
    // Request 4 holding registers mulai dari address 0
    m_modbus.readRegister(0, 4, m_modbus_data);
    
    // Decode dan scale data (sama persis dengan logic di ESP32)
    m_pressure    = (int16_t)m_modbus_data[0] / 10.0;
    m_temperature = (int16_t)m_modbus_data[1] / 100.0;
    m_depth       = (int16_t)m_modbus_data[2] / 100.0;
    m_heartbeat   = m_modbus_data[3];

    // Publish ke MOOSDB
    Notify("BAR10XT_PRESSURE", m_pressure);
    Notify("BAR10XT_TEMPERATURE", m_temperature);
    Notify("BAR10XT_DEPTH", m_depth);
    Notify("BAR10XT_HEARTBEAT", m_heartbeat);
    
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool bar10xt::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "ip_address") {
            m_ip_address = value;
            handled = true;
        }
        else if(param == "port") {
            m_port = atoi(value.c_str());
            handled = true;
        }

        if(!handled)
            reportUnhandledConfigWarning(orig);
    }
    
    registerVariables();

    if (m_modbus.connect(m_ip_address, m_port)) {
        reportEvent("Berhasil terhubung ke Modbus TCP: " + m_ip_address);
    } else {
        reportRunWarning("Gagal terhubung ke Modbus TCP!");
    }

    return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void bar10xt::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool bar10xt::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "Target ESP32 IP : " << m_ip_address << ":" << m_port << endl;
  m_msgs << "Status          : " << (modbus_connect ? "CONNECTED" : "DISCONNECTED") << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Pressure (mbar) : " << m_pressure << endl;
  m_msgs << "Temperature (C) : " << m_temperature << endl;
  m_msgs << "Depth (m)       : " << m_depth << endl;
  m_msgs << "Heartbeat       : " << m_heartbeat << endl;
  return(true);
}




