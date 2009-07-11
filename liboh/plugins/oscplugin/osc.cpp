// ===========================================================
// START osc.cpp
// ===========================================================

#include "osc.h"

#include <string>
#include <sstream>
//#include "../ui/menudef.h" // rkh
#include <iostream> //rkh
#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/ip/UdpSocket.h"
#include "oscpack/ip/IpEndpointName.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
//#define ADDRESS "127.0.0.1"
//#define ADDRESS "192.168.1.77"
#define PORT 7000

//osc_input_vars gOSCvars;

namespace osc{

class PacketListener : public osc::OscPacketListener {
protected:

    virtual void ProcessMessage( const osc::ReceivedMessage& m, 
const IpEndpointName& remoteEndpoint )
    {
        try{
            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.

            
            if( strcmp( m.AddressPattern(), "/g_gravity" ) == 0 ){
                osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
                float a3;
                args >> a3 >> osc::EndMessage;
gOSCvars.g_gravity = a3;
gOSCvars.g_gravity_update = 1;
                std::cout << "received '/g_gravity': "
<< a3 << "\n";
    }else if( strcmp( m.AddressPattern(), "/g_speed" ) == 0 ){
                osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
                float a3;
                args >> a3 >> osc::EndMessage;
gOSCvars.g_speed = a3;
gOSCvars.g_speed_update = 1;

                
                std::cout << "received '/g_speed': "
                    << a3 << "\n";


            }else if( strcmp( m.AddressPattern(), "/g_homing_speed" ) == 0 ){

                // example #1 -- argument stream interface
                osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
                //bool a1;
                //osc::int32 a2;
                float a3;
                //const char *a4;
                args >> a3 >> osc::EndMessage;
gOSCvars.g_homing_speed = a3;
gOSCvars.g_homing_speed_update = 1;

                
                std::cout << "received '/g_homing_speed': "
                    << a3 << "\n";


            }else if( strcmp( m.AddressPattern(), "/test2" ) == 0 ){
                // example #2 -- argument iterator interface, supports
                // reflection for overloaded messages (eg you can call 
                // (*arg)->IsBool() to check if a bool was passed etc).
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                bool a1 = (arg++)->AsBool();
                int a2 = (arg++)->AsInt32();
                float a3 = (arg++)->AsFloat();
                const char *a4 = (arg++)->AsString();
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();

                
                std::cout << "received '/test2' message with arguments: "
                    << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";
            }
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }
};

} // end namespace osc


// if dynamic IP is enabled, ip's will be selected from an array populated by the user
// currently, ips are set in the client and assed in 
void getIPAddress (int index){

}

#define OUTPUT_BUFFER_SIZE 2048
void sendOSCmessage_projectile(osc_projectile_vars currentProjectile) //, char *type)
{
// std::string classname = currentProjectile.classname;
// std::string slash = "/";

std::string str(currentProjectile.port);
std::istringstream strin(str);
int port;
strin >> port;
char buffer[OUTPUT_BUFFER_SIZE];
osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

UdpTransmitSocket socket( IpEndpointName( currentProjectile.hostname, port ));
p.Clear();

// p << osc::BeginMessage((slash + classname).c_str())
p << osc::BeginMessage("/projectile")
            << currentProjectile.classname
    << (int)currentProjectile.ownernum
    << (int)currentProjectile.projectilenum
    << (float)currentProjectile.origin[0]
    << (float)currentProjectile.origin[1]
    << (float)currentProjectile.origin[2]
    << (int)currentProjectile.targetnum
    << (int)currentProjectile.bounce
    << (int)currentProjectile.explode
  << osc::EndMessage;


if(p.IsReady()){ socket.Send( p.Data(), p.Size() );}



  
}

void sendOSCbundle_projectile(osc_projectile_vars currentProjectile)
{
std::string str(currentProjectile.port);
std::istringstream strin(str);
int port;
strin >> port;
char buffer[OUTPUT_BUFFER_SIZE];
osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );

UdpTransmitSocket socket( IpEndpointName( currentProjectile.hostname, port ));
p.Clear();

p << osc::BeginBundle()
<< osc::BeginMessage( "/classname" ) << currentProjectile.classname << osc::EndMessage
<< osc::BeginMessage( "/projectilenum" ) << (int)currentProjectile.projectilenum << osc::EndMessage
<< osc::BeginMessage( "/origin" ) << (float)currentProjectile.origin[0] << (float)currentProjectile.origin[1] << (float)currentProjectile.origin[2] << osc::EndMessage
// << osc::BeginMessage( "/originy" ) << currentProjectile.origin[1] << osc::EndMessage
// << osc::BeginMessage( "/originz" ) << currentProjectile.origin[2] << osc::EndMessage
<< osc::BeginMessage( "/ownernum" ) << currentProjectile.ownernum << osc::EndMessage
<< osc::BeginMessage( "/targetnum" ) << currentProjectile.targetnum << osc::EndMessage
<< osc::BeginMessage( "/bounce" ) << currentProjectile.bounce << osc::EndMessage
<< osc::BeginMessage( "/explode" ) << currentProjectile.explode << osc::EndMessage
<< osc::EndBundle;

if(p.IsReady()){ socket.Send( p.Data(), p.Size() );}
}

void sendOSCmessage(osc_client_vars currentClient)
{
std::string str(currentClient.port);
std::istringstream strin(str);
int port;
strin >> port;

   char buffer[OUTPUT_BUFFER_SIZE];
   osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
   UdpTransmitSocket socket( IpEndpointName( currentClient.hostname, port ));
   p.Clear();

p << osc::BeginMessage( "/player" )
    << currentClient.classname
    << (int)currentClient.clientnum
    << (float)currentClient.origin[0]
    << (float)currentClient.origin[1]
    << (float)currentClient.origin[2]

    << (float)currentClient.velocity[0]
    << (float)currentClient.velocity[1]
    << (float)currentClient.velocity[2]

 
<< (float)currentClient.viewangles[0]
<< (float)currentClient.viewangles[1]
<< (float)currentClient.viewangles[2]

<< (float)currentClient.delta_angles[0]
<< (float)currentClient.delta_angles[1]
<< (float)currentClient.delta_angles[2]

 
<< (int)currentClient.weapon
<< (int)currentClient.weaponstate
  << (int)currentClient.jumppad_ent

 
  << (int)currentClient.damageEvent
<< (int)currentClient.damageYaw
<< (int)currentClient.damagePitch
<< (int)currentClient.damageCount
<< (int)currentClient.groundEntityNum

 /*
 currentClient.velocity[0] = pm.ps->velocity[0];
 currentClient.velocity[1] = pm.ps->velocity[1];
 currentClient.velocity[2] = pm.ps->velocity[2];
 currentClient.viewangles[0] = pm.ps->viewangles[0];
 currentClient.viewangles[1] = pm.ps->viewangles[1];
 currentClient.viewangles[2] = pm.ps->viewangles[2];
 currentClient.delta_angles[0] = pm.ps->delta_angles[0];
 currentClient.delta_angles[1] = pm.ps->delta_angles[1];
 currentClient.delta_angles[2] = pm.ps->delta_angles[2];
 currentClient.weapon = pm.ps->weapon;
 currentClient.weaponstate = pm.ps->weaponstate;
 currentClient.jumppad_ent = pm.ps->jumppad_ent;
 currentClient.damageEvent = pm.ps->damageEvent;
 currentClient.damageYaw = pm.ps->damageYaw;
 currentClient.damagePitch = pm.ps->damagePitch;
 currentClient.damageCount = pm.ps->damageCount;
 currentClient.groundEntityNum = pm.ps->groundEntityNum;
 // currentClient.surfaceFlags = pm.surfaceFlags;
 */
  << osc::EndMessage;

   if(p.IsReady()){ socket.Send( p.Data(), p.Size() );}
}

void sendOSCbundle(osc_client_vars currentClient)
{

std::string str(currentClient.port);
std::istringstream strin(str);
int port;
strin >> port;

   char buffer[OUTPUT_BUFFER_SIZE];
   osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
   UdpTransmitSocket socket( IpEndpointName( currentClient.hostname, port ));
   p.Clear();

   p << osc::BeginBundle()
<< osc::BeginMessage( "/classname" ) << currentClient.classname << osc::EndMessage
<< osc::BeginMessage( "/client" ) << currentClient.clientnum << osc::EndMessage
<< osc::BeginMessage( "/originx" ) << currentClient.origin[0] << osc::EndMessage
<< osc::BeginMessage( "/originy" ) << currentClient.origin[1] << osc::EndMessage
<< osc::BeginMessage( "/originz" ) << currentClient.origin[2] << osc::EndMessage

<< osc::BeginMessage( "/velocity/x" ) << currentClient.velocity[0] << osc::EndMessage
<< osc::BeginMessage( "/velocity/y" ) << currentClient.velocity[1] << osc::EndMessage
<< osc::BeginMessage( "/velocity/z" ) << currentClient.velocity[2] << osc::EndMessage

<< osc::BeginMessage( "/viewangles/x" ) << currentClient.viewangles[0] << osc::EndMessage
<< osc::BeginMessage( "/viewangles/y" ) << currentClient.viewangles[1] << osc::EndMessage
<< osc::BeginMessage( "/viewangles/z" ) << currentClient.viewangles[2] << osc::EndMessage

<< osc::BeginMessage( "/delta_angles/x" ) << currentClient.delta_angles[0] << osc::EndMessage
<< osc::BeginMessage( "/delta_angles/y" ) << currentClient.delta_angles[1] << osc::EndMessage
<< osc::BeginMessage( "/delta_angles/z" ) << currentClient.delta_angles[2] << osc::EndMessage

<< osc::BeginMessage( "/weapon" ) << currentClient.weapon << osc::EndMessage
<< osc::BeginMessage( "/weaponstate" ) << currentClient.weaponstate << osc::EndMessage  
<< osc::BeginMessage( "/jumppad_ent" ) << currentClient.jumppad_ent << osc::EndMessage  

<< osc::BeginMessage( "/damageEvent" ) << currentClient.damageEvent << osc::EndMessage  
<< osc::BeginMessage( "/damageYaw" ) << currentClient.damageYaw << osc::EndMessage  
<< osc::BeginMessage( "/damagePitch" ) << currentClient.damagePitch << osc::EndMessage  
<< osc::BeginMessage( "/damageCount" ) << currentClient.damageCount << osc::EndMessage  

<< osc::BeginMessage( "/groundEntityNum" ) << currentClient.groundEntityNum << osc::EndMessage  

/*  
currentClient.velocity[0] = pm.ps->velocity[0];
currentClient.velocity[1] = pm.ps->velocity[1];
currentClient.velocity[2] = pm.ps->velocity[2];
currentClient.viewangles[0] = pm.ps->viewangles[0];
currentClient.viewangles[1] = pm.ps->viewangles[1];
currentClient.viewangles[2] = pm.ps->viewangles[2];
currentClient.delta_angles[0] = pm.ps->delta_angles[0];
currentClient.delta_angles[1] = pm.ps->delta_angles[1];
currentClient.delta_angles[2] = pm.ps->delta_angles[2];
currentClient.weapon = pm.ps->weapon;
currentClient.weaponstate = pm.ps->weaponstate;
currentClient.jumppad_ent = pm.ps->jumppad_ent;
currentClient.damageEvent = pm.ps->damageEvent;
currentClient.damageYaw = pm.ps->damageYaw;
currentClient.damagePitch = pm.ps->damagePitch;
currentClient.damageCount = pm.ps->damageCount;
currentClient.groundEntityNum = pm.ps->groundEntityNum;
// currentClient.surfaceFlags = pm.surfaceFlags;
*/
<< osc::EndBundle;

   if(p.IsReady()){ socket.Send( p.Data(), p.Size() );}
}

void receiveOSCmessage()
{


    std::cout << "============================================ \n";
    std::cout << "============================================ \n";
    std::cout << "============================================ \n";
    std::cout << "RKH - TESTING receiveOSCmessage() in osc.cpp \n";
    std::cout << "============================================ \n";
    std::cout << "============================================ \n";
    std::cout << "============================================ \n";
    std::cout <<"Hello World" << std::endl;

    osc::PacketListener listener;

    UdpListeningReceiveSocket s(
            IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
            &listener );
   //s.Break();
    s.RunUntilSigInt();
  return;

}

osc_input_vars getOSCmessage()
{
//osc_input_vars teststruct;
//teststruct.testvar = 11;
gOSCvars.testvar = 12;
return gOSCvars;
}

