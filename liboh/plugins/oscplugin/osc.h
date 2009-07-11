//
// C++ Interface: osc
//
// Description: 
//
//
// Author: Robert Hamilton <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


#if defined(__cplusplus) || defined(_cplusplus)
   extern "C" {
#endif

typedef struct {
  char *hostname;
  char *port;
  int clientnum;
  float origin[3];
  int pm_flags;
  int weapon;
  int weaponstate;
  int jumppad_ent;
  int damageEvent;
  int damageYaw;
  int damagePitch;
  int damageCount;
  int surfaceFlags;
  int groundEntityNum;
  char *classname;
  char *hostnames[20]; // rkh - added array of client IPs
/* additional Data points used in q3apd and for future use in q3osc: */
  int velocity[3];
  int viewangles[3];
  int delta_angles[3];
} osc_client_vars;


typedef struct {
  char *hostname;
  char *port;
  int ownernum;
  int targetnum;
  int bounce;
  int explode;
  float projectilenum;
  float origin[3];
  char *classname;
  char *hostname1;
  char *hostname2;
  char *hostname3;
  char *hostname4;
  char *hostname5;
} osc_projectile_vars;

typedef struct {
  float g_gravity;
  int g_gravity_update;
  int g_speed;
  int g_speed_update;
  int g_homing_speed;
  int g_homing_speed_update;
  int testvar;
} osc_input_vars;

//void sendOSCmessage(int clientno, char *hostname, char *portnumber);
void sendOSCbundle(osc_client_vars currentClient);
void sendOSCmessage(osc_client_vars currentClient);

void sendOSCbundle_projectile(osc_projectile_vars currentProjectile);
void sendOSCmessage_projectile(osc_projectile_vars currentProjectile); //, char *type);

void receiveOSCmessage( void ); // osc listener method

osc_input_vars getOSCmessage( void ); // test data retrieval

//#ifdef MACOS_X
  extern osc_input_vars gOSCvars;
//#else
//   osc_input_vars gOSCvars;
//#endif

   
#if defined(__cplusplus) || defined(_cplusplus)
   }
#endif

