/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Friendly names for various constants
*//////////////////////////////////////////////////////////////
// Motor Control
#define MOTORDIR_FWD    1
#define MOTORDIR_REV   -1
#define MOTORDIR_STOP   0

#define DRIVEMODE_MANUAL    0
#define DRIVEMODE_ARC       1
#define DRIVEMODE_VECTOR    2


// LED Control
#define LED_ON          HIGH
#define LED_OFF         LOW

#define RED             0xFF0000
#define YELLOW          0xFFFF00
#define GREEN           0x00FF00
#define BLUE            0x0000FF
#define TEAL            0x00FFFF
#define PURPLE          0xFF00FF
#define WHITE           0xFFFFFF
#define BLACK           0x000000

// Directions
#define NORTH_X        0
#define NORTH_Y        1

#define EAST_X         1
#define EAST_Y         0

#define SOUTH_X        0 
#define SOUTH_Y        -1

#define WEST_X         -1
#define WEST_Y         0

#define NORTHEAST_X    1
#define NORTHEAST_Y    1

#define SOUTHEAST_X    1
#define SOUTHEAST_Y    -1

#define SOUTHWEST_X    -1
#define SOUTHWEST_Y    -1

#define NORTHWEST_X    -1
#define NORTHWEST_Y    1

#define NORTH           0x00
#define NORTHEAST       0x01
#define EAST            0x02
#define SOUTHEAST       0x03
#define SOUTH           0x04
#define SOUTHWEST       0x05
#define WEST            0x06
#define NORTHWEST       0x07
#define STOP_MOVING     0xFF
