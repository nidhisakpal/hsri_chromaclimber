#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Robot logic is often broken down into a set of
                higherarchical ACTION information. Typically,
                the way I handle it is to have "behaviors" 
                that consist of a variety of ACTIONs as well as
                "phases" within these ACTIONs.     
*//////////////////////////////////////////////////////////////
// Behavior where nothing happens
#define BEHAVIOR_IDLE               0x00
#define ACTION_IDLE_IDLE            0x00
#define PHASE_IDLE_IDLE             0x00

/////////////////////////////////////////////////////
// Behavior where reports happen
#define BEHAVIOR_HEARTBEAT          0x01
#define COLOR_SENSE_TEST            0x02
#define WHERE_TO_GO                 0x08
#define BEHAVIOR_1                  0x09
#define BEHAVIOR_2                  0x11
#define BEHAVIOR_4                  0x10

/////////////////////////////////////////////////////
// Behavior where different colors correspond to different actions to be performed (different directions to move)
#define BEHAVIOR_SPATIAL_PROGRAM    0x03
#define ACTION_SPATIAL_INIT         0x00
#define ACTION_SPATIAL_WAIT         0x10
#define ACTION_SPATIAL_MOVE         0x20

/////////////////////////////////////////////////////
// Behavior where baseline behaviors are demonstrated
#define BEHAVIOR_EXAMPLE            0x04
#define ACTION_INIT                 0x00
#define ACTION_EXAMPLE_FORWARD      0x01
#define ACTION_EXAMPLE_SPIN         0x03
#define GREEN_ACTION                0x05
#define BLUE_ACTION                 0x06
#define RED_ACTION                  0x07
#define WHITE_ACTION                0x08
#define BLACK_ACTION                0X09

/////////////////////////////////////////////////////
// Ricochet off of shadows until hit green
#define BEHAVIOR_SHADOW_INSTRUCTION      0x05
#define ACTION_GUIDE_INIT           0x00 
#define ACTION_GUIDE_FORWARD        0x11
#define ACTION_GUIDE_REVERSE        0x12
#define ACTION_GUIDE_NEW_DIRECTION  0x13
#define ACTION_GUIDE_FREE_MOVE      0x14
#define ACTION_GUIDE_GOAL           0xFF

/////////////////////////////////////////////////////
// Follows a line based on the observed colors
#define BEHAVIOR_FOLLOW_LINE        0x06
#define ACTION_FOLLOW_INIT          0x00
#define ACTION_FOLLOW_FORWARD       0x10   
#define ACTION_FOLLOW_SEARCH        0x11
#define ACTION_FOLLOW_GOAL          0xFF


/////////////////////////////////////////////////////
// Behavior where flashing lights are used as a means of GPS
#define BEHAVIOR_FRACTIAL_PLAID_GPS 0x07
#define ACTION_GPS_INIT             0x00
#define ACTION_GPS_WAIT_WHITE_START 0x01
#define ACTION_GPS_WAIT_NONWHITE    0x02
#define ACTION_GPS_WAIT_WHITE       0x03
#define ACTION_GPS_REPORT           0x04
#define ACTION_GPS_COMPLETE         0x05

/////////////////////////////////////////////////////
// Multi-robot behavior where different robots have different roles
#define BEHAVIOR_GESTURE_CONTROL    0x08
#define ROLE_GESTURE_CENTER         0x10
#define ROLE_GESTURE_UP             0x11
#define ROLE_GESTURE_RIGHT          0x12
#define ROLE_GESTURE_MOVE           0x20

#define ACTION_GESTURE_INIT         0x00
#define ACTION_GESTURE_REPORTER     0x10
#define ACTION_GESTURE_MOVE         0x20
#define ACTION_GESTURE_PAUSE        0x21
