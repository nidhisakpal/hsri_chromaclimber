# ChromaClimber

ChromaClimber is Arduino firmware for a color-responsive robot built on the
Pololu 3pi+ 32U4 OLED Robot platform. The robot reads motion, color, bumper,
line-sensor, encoder, button, and battery data, converts those inputs into a
shared `robot` controller state, and runs one of several selectable behaviors
from an on-robot OLED menu.

The project is designed for interactive robotics experiments where color cues
act as instructions. Depending on the selected mode, the robot can report live
sensor data, identify colors, move in directions associated with observed
colors, ricochet away from obstacles while following light/dark instructions,
or estimate a location through a color-sequence "fractal plaid GPS" routine.

## Repository Contents

```text
.
|-- ChromaClimber/
|   |-- ChromaClimber.ino
|   |-- robot.h
|   |-- robot.cpp
|   |-- simple_helper_functions.h
|   |-- simple_helper_functions.cpp
|   |-- serial_helper_programs.h
|   |-- serial_helper_programs.cpp
|   |-- def_friendlynames.h
|   |-- def_playtones.h
|   |-- def_robot_properties.h
|   |-- def_states_and_phases.h
|   |-- extended_lcd_font.h
|   |-- font.cpp
|   |-- menu_stuff.h
|   `-- Bak/
|       `-- 20260327ChromaClimber.zip
|-- ChromaClimber (1).pdf
`-- ChromaClimber_ Electrical Hardware.pdf
```

The two PDF files are retained as supporting project documentation. The
firmware itself lives in `ChromaClimber/`.

## Hardware Target

The code targets a Pololu 3pi+ 32U4 OLED Robot and assumes the standard Pololu
robot interfaces are available:

- OLED display
- Buzzer
- Buttons A, B, and C
- Line sensors
- Bump sensors
- IMU
- Motors
- Wheel encoders
- Battery voltage reader

The firmware also uses an Adafruit TCS34725 RGB color sensor over I2C.

## Software Dependencies

Install these libraries in the Arduino IDE or Arduino CLI environment:

- `Pololu3piPlus32U4`
- `Pololu3piPlus32U4IMU`
- `PololuMenu`
- `Adafruit_TCS34725`
- `Wire`, included with Arduino

The sketch includes:

```cpp
#include <Wire.h>
#include <Pololu3piPlus32U4.h>
#include <Pololu3piPlus32U4IMU.h>
#include <PololuMenu.h>
#include <Adafruit_TCS34725.h>
```

## Important Compile Note

`ChromaClimber/ChromaClimber.ino` currently includes `splash.h`:

```cpp
#include "splash.h"
```

The uploaded repository contents do not include `ChromaClimber/splash.h`. If
your Arduino build fails with an error such as `splash.h: No such file or
directory`, either restore that file from the original Pololu demo/project
source or remove/comment the include and any splash-screen code that depends on
it.

The existing sketch defines splash-related functions such as `showSplash()`,
but the main loop uses `mainMenuWelcome()` when entering the menu. If the splash
graphics are not needed, removing the missing include may be enough after
checking that no symbols from `splash.h` are still required by your local build.

## Opening and Uploading

1. Install the required Arduino libraries.
2. Connect the Pololu 3pi+ 32U4 OLED Robot over USB.
3. Open `ChromaClimber/ChromaClimber.ino` in the Arduino IDE.
4. Select the correct board and port for the 3pi+ 32U4.
5. Compile the sketch.
6. Upload it to the robot.
7. Open the Serial Monitor at `115200` baud if you want to inspect behavior
   reports or color/GPS debug output.

## Runtime Flow

`ChromaClimber.ino` owns the Arduino `setup()` and `loop()` functions.

During `setup()` the firmware:

- Starts serial output at `115200` baud.
- Builds the OLED menu.
- Registers behavior configuration callbacks.
- Initializes I2C with `Wire.begin()`.
- Initializes and enables the IMU.
- Loads custom OLED characters.
- Gives the global `robot controller` access to the OLED display.

During each `loop()` iteration, the firmware either:

- Runs the active behavior, or
- Opens the main menu when button B is pressed.

The main control loop is:

```text
read inputs
process sensor data
run selected behavior logic
convert behavior outputs into motor/LED/speaker commands
write outputs to hardware
```

That pipeline is implemented through these calls:

```cpp
inputs(&controller);
controller.processSensorData();
controller.runLogic();
controller.processOutputs();
outputs(&controller);
```

## Menu Behaviors

The OLED menu exposes six behavior choices:

| Menu label | Internal behavior | Purpose |
| --- | --- | --- |
| `Spatial` | `BEHAVIOR_SPATIAL_PROGRAM` | Move according to detected color instructions. |
| `Heartbeat` | `BEHAVIOR_HEARTBEAT` | Print sensor and computed state data over serial. |
| `Shadow` | `BEHAVIOR_GUIDE_TO_GOAL` | Move on white, spin on black, avoid bumps, stop at green. |
| `Joysticks` | `BEHAVIOR_GESTURE_CONTROL` | Configured in the menu, with constants reserved for gesture roles. |
| `Fractal GPS` | `BEHAVIOR_FRACTIAL_PLAID_GPS` | Estimate position from a sequence of stable color readings. |
| `Color Rep` | `COLOR_SENSE_TEST` | Print interpreted color information over serial. |

Button B opens the menu. The menu uses Pololu's `PololuMenu` with buttons A,
B, and C for navigation and selection.

## Sensor Inputs

The `inputs(robot* controller)` function gathers all hardware readings and
stores them on the shared controller object.

### IMU

The firmware reads accelerometer, gyroscope, and magnetometer values:

- `input_ax`, `input_ay`, `input_az`
- `input_gx`, `input_gy`, `input_gz`
- `input_mx`, `input_my`, `input_mz`

The controller computes an orientation angle from the acceleration values using
`orientation(input_ax, input_ay)`.

### Color Sensor

The Adafruit TCS34725 sensor provides raw:

- red
- green
- blue
- clear

The controller then computes:

- `calc_hue`
- `calc_brightness`
- `calc_color`

Color classification is based on brightness thresholds and hue cutoffs in
`simple_helper_functions.h`.

### Encoders

The firmware reads left and right encoder counts and calculates approximate
encoder speeds by comparing the current counts against the previous counts over
the loop time delta.

### Buttons and Bumpers

The controller tracks:

- Button A
- Button B
- Button C
- Left bumper
- Right bumper

Bumper inputs are also used as a safety layer in `processOutputs()` to prevent
continued forward motion into an obstacle.

### Line Sensors

All five Pololu line sensors are read into:

```cpp
input_line_sensor_values[5]
```

The current behavior code does not heavily use these values, but they are
available for future line-following or floor-pattern behaviors.

### Battery

The firmware reads battery millivolts through Pololu's
`readBatteryMillivolts()`. The controller computes a battery percentage using
the configured min and max values in `def_robot_properties.h`.

## Color Classification

Color classification is implemented in `simple_helper_functions.cpp`.

The raw RGB values are transformed into:

- Hue, using the maximum and minimum channel values.
- Brightness, using the average of the maximum and minimum RGB channels.

Brightness is checked first:

- Brightness above `THRESHOLD_WHITE` becomes `COLOR_WHITE`.
- Brightness below `THRESHOLD_BLACK` becomes `COLOR_BLACK`.

If the reading is neither white nor black, hue cutoffs classify it as:

- red
- yellow
- green
- cyan
- blue
- magenta

The thresholds are defined in `simple_helper_functions.h`:

```cpp
#define THRESHOLD_WHITE  400.00f
#define THRESHOLD_BLACK  100.00f

#define RED_MIN_0    0.00f
#define YELLOW_MIN   0.125f
#define GREEN_MIN    0.25f
#define CYAN_MIN     0.35f
#define BLUE_MIN     0.53f
#define MAGENTA_MIN  0.70f
#define RED_MIN_1    0.85f
```

These values will likely need calibration for room lighting, sensor mounting,
surface reflectivity, and the colors used in the physical experiment.

## Movement Model

The controller supports three drive modes:

- `DRIVEMODE_VECTOR`
- `DRIVEMODE_ARC`
- `DRIVEMODE_MANUAL`

### Vector Drive

Vector drive uses a target movement vector, compares it to the robot's computed
orientation, and turns that into forward and spin components.

The behavior sets:

```cpp
move_x
move_y
```

Then `processOutputs()` computes:

```cpp
drive_forward
drive_spin
motorspeed_left
motorspeed_right
```

The robot turns in place when the heading error is large, blends turning and
forward motion when the error is moderate, and drives forward when the heading
error is within the dead zone.

Heading behavior is tuned by:

```cpp
#define FACING_DEADZONE_DEG     5.0
#define FACING_ONLY_TURN_DEG    45.0
```

### Arc Drive

Arc drive uses `drive_forward` and `drive_spin` directly and converts them to
differential motor speeds.

### Manual Drive

Manual drive assumes the behavior has already set the motor-speed variables.

### Motor Scaling

Final motor commands are scaled in `outputs()`:

```cpp
motors.setSpeeds(400 * controller->motorspeed_left,
                 400 * controller->motorspeed_right);
```

The `motorspeed_*` values are expected to be normalized around `-1.0` to `1.0`.

## Behavior Details

### Heartbeat

`BEHAVIOR_HEARTBEAT` calls `behaviorReport()`, which prints a comma-separated
stream of live state over serial, including:

- loop time delta
- accelerometer values
- gyroscope values
- magnetometer values
- calculated orientation
- battery voltage and percentage
- raw color sensor values
- calculated hue and brightness
- calculated color index

Use this mode when calibrating sensors or checking that the robot is reporting
reasonable values.

### Color Reporting

`COLOR_SENSE_TEST` calls `whats_the_color()`, which prints:

- hue
- brightness
- color index

This is the simplest mode for tuning color thresholds.

### Spatial Programming

`BEHAVIOR_SPATIAL_PROGRAM` maps detected colors to directional movement.

Current mapping:

| Detected color | Action |
| --- | --- |
| Blue | Move north |
| Green | Move east |
| Red | Move west |
| Teal/cyan | Move south |
| Yellow | Stop, play a tone, turn on green LED |
| White | Reverse briefly |
| Other | Stop |

The direction constants are defined in `def_friendlynames.h`, and the behavior
logic lives in `robot::behavior_spatial_programming()`.

### Shadow Instructions

`BEHAVIOR_GUIDE_TO_GOAL` is implemented by `robot::shadow_instructions()`.

This mode uses color and bumper inputs as instructions:

- White: drive forward.
- Black: spin in place.
- Green: stop at goal.
- Bumper hit: reverse, then spin for a random 1-3 second duration.
- Unknown color: stop.

When the robot reaches the goal state, it turns on all LEDs and prepares a tone.

### Fractal Plaid GPS

`BEHAVIOR_FRACTIAL_PLAID_GPS` is implemented by `robot::f_p_gps()`.

This behavior treats a sequence of stable colors as positional updates. It:

1. Waits for stable white to begin.
2. Waits for a stable non-white color.
3. Updates `gps_x` and `gps_y` according to the color.
4. Halves `step_size`.
5. Waits for stable white again.
6. Repeats for five iterations.

Current color-to-position updates:

| Stable color | Position update |
| --- | --- |
| Red | `x -= step_size`, `y += step_size` |
| Blue | `x += step_size`, `y += step_size` |
| Green | `x -= step_size`, `y -= step_size` |
| Magenta | `x += step_size`, `y -= step_size` |

The stability helper `color_count()` requires the same measured color to persist
for a configured hold time before the state machine accepts it.

### Gesture Control / Virtual Joysticks

The menu maps `Joysticks` to `BEHAVIOR_GESTURE_CONTROL` and initializes
`ACTION_GESTURE_INIT`. Constants for roles and gesture actions exist in
`def_states_and_phases.h`, but the current `runLogic()` switch does not yet
dispatch a gesture-control behavior implementation.

This means the menu option is present, but the behavior appears to be reserved
or incomplete in the current code.

## Key Configuration Values

Robot geometry and general tuning live in `def_robot_properties.h`:

```cpp
#define DIAMETER_WHEEL_MM   32
#define TRACK_WIDTH_MM      90
#define SPEED_MAX_MMPS      40

#define DIAMETER_ROBOT_MM   90

#define BATTERY_VOLTAGE_MAX  6.0
#define BATTERY_VOLTAGE_MIN  4.0

#define FACING_DEADZONE_DEG     5.0
#define FACING_ONLY_TURN_DEG    45.0
```

Behavior IDs, action IDs, and phase IDs live in `def_states_and_phases.h`.

Friendly constants for colors, directions, drive modes, and motor directions
live in `def_friendlynames.h`.

Tone and song constants live in `def_playtones.h`.

## File Guide

### `ChromaClimber.ino`

Main Arduino sketch. It creates all Pololu and Adafruit hardware objects,
builds the OLED menu, initializes sensors, and runs the main input-logic-output
loop.

### `robot.h` and `robot.cpp`

The `robot` class is the central state container and behavior engine. It stores
sensor inputs, computed values, behavior/action state, movement commands, LED
state, speaker state, and motor outputs.

### `simple_helper_functions.h` and `.cpp`

General helper library for math, serial utilities, memory checks, color
classification, orientation calculation, timing checks, vector math, and color
stability filtering.

### `serial_helper_programs.h` and `.cpp`

Utility functions for serial input/output, including waiting for serial data,
reading keypresses, reading numbers, confirmation prompts, and compile-info
printing.

### `def_states_and_phases.h`

Defines numeric IDs for the behavior state machines. This includes the active
behaviors and many reserved or experimental states.

### `def_robot_properties.h`

Defines physical robot dimensions, battery limits, and heading-control
tolerances.

### `def_friendlynames.h`

Defines readable constants for colors, directions, motor directions, and drive
modes.

### `def_playtones.h`

Defines speaker play modes and a stored Fugue note sequence.

### `menu_stuff.h`

Defines simple tunes and custom OLED characters used by the menu/display code.

### `extended_lcd_font.h` and `font.cpp`

Support files for extended display glyphs.

### `Bak/20260327ChromaClimber.zip`

An archived copy of a ChromaClimber source package.

## Serial Output

Open Serial Monitor at `115200` baud.

Recommended modes for serial debugging:

- `Heartbeat`: broad sensor/status stream.
- `Color Rep`: compact color classification stream.
- `Fractal GPS`: prints GPS state-machine progress and estimated coordinates.

The heartbeat stream is comma-separated but does not currently print a header
row. If you are logging to a CSV file, add a header matching the print order in
`robot::behaviorReport()`.

## Calibration Suggestions

Color classification is highly sensitive to the test surface and lighting.
Before running navigation behaviors:

1. Upload the firmware.
2. Select `Color Rep`.
3. Place the robot over each expected color.
4. Record hue, brightness, and color index.
5. Adjust `THRESHOLD_WHITE`, `THRESHOLD_BLACK`, and hue cutoffs if needed.
6. Re-test under the same lighting conditions used for the final demo.

If color readings flicker between categories, increase the hold duration used
by `color_count()` in behaviors that require stable readings.

## Troubleshooting

### The sketch cannot find `splash.h`

The repository currently does not include `splash.h`, although the sketch
includes it. Restore the missing file or remove/comment the include and
splash-specific code after confirming your build does not need those graphics.

### The color sensor always reports white or black

Check lighting and sensor distance from the surface. Then tune:

- `THRESHOLD_WHITE`
- `THRESHOLD_BLACK`

These are in `simple_helper_functions.h`.

### The robot turns the wrong way

Check the orientation calculation and the physical robot orientation. The
controller uses accelerometer-derived orientation and maps desired vectors to
heading error. If the coordinate system is flipped for your setup, inspect:

- `orientation()`
- `setMoveVectors()`
- `processOutputs()`

### The robot drives too fast

Motor speed is scaled in `outputs()` by `400`. Lower that value or adjust the
behavior-level normalized speeds.

### The robot does not move in a selected mode

Some menu options are configured before all behavior implementations are wired
into `runLogic()`. In the current code, `BEHAVIOR_GESTURE_CONTROL` constants
exist, but the run loop does not dispatch a gesture behavior.

### Bumper hits interrupt motion

This is intentional. `processOutputs()` includes a safety override that prevents
forward movement when a bumper is pressed.

## Development Notes

- Keep behavior-specific state transitions in `robot.cpp`.
- Keep numeric IDs in `def_states_and_phases.h`.
- Keep hardware setup and menu registration in `ChromaClimber.ino`.
- Prefer adding new color thresholds to `simple_helper_functions.h` and the
  implementation in `simple_helper_functions.cpp`.
- Add a serial header if you plan to log heartbeat data for analysis.

## Authors and Attribution

The source comments credit Petras Swissler and Roberto Torres as authors of the
ChromaClimber code. The robot control code is adapted from Pololu 3pi+ 32U4
OLED Robot example code.

