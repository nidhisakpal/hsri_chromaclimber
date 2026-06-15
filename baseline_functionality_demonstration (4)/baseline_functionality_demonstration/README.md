# Baseline Functionality Demonstration

This Arduino sketch is a baseline robot-control program for an Adafruit ESP32-S3 Feather based robot using separate sensor, logic, and output modules. The program reads battery, IMU, magnetometer, color sensor, and button inputs; stores the interpreted state in a `robot` object; runs the active behavior; and then writes LED and motor outputs.

## Hardware Target

- Main board: Adafruit ESP32-S3 Feather.
- Battery monitor: Adafruit MAX17048.
- IMU: Adafruit MPU6050.
- Color sensor: Adafruit TCS34725.
- Magnetometer: QMC5883L.
- Status LED: onboard NeoPixel.
- Drive output: dual H-bridge motor driver pins defined in `def_pins.h`.
- User input: pull-up button on `DI_USER_BUTTON`.

## Program Flow

The main sketch follows this loop:

1. `setup()` creates/configures hardware with `boardSetup(...)`.
2. The controller starts in `BEHAVIOR_HEARTBEAT`.
3. `loop()` calls `inputs(...)` to update the controller from physical sensors.
4. `controller.runLogic()` clears outputs and runs the selected behavior state machine.
5. `outputs(...)` applies safety hooks, updates the NeoPixel/color-sensor LED, and drives motors.

The commented calls in `loop()` (`serialInterjection`, `commHandlerIn`, `serverHandler`, and `commHandlerOut`) show intended extension points for runtime control or communication, but they are inactive in the current sketch.

## Folder Structure

```text
baseline_functionality_demonstration/
  baseline_functionality_demonstration.ino
  def_config.h
  def_friendlynames.h
  def_includes.h
  def_pins.h
  def_states_and_phases.h
  inputs.cpp
  inputs.h
  outputs.cpp
  outputs.h
  README.md
  README.txt
  robot.cpp
  robot.h
  serial_helper_programs.cpp
  serial_helper_programs.h
  setup.cpp
  setup.h
  simple_helper_functions.cpp
  simple_helper_functions.h
  user_control.cpp
  user_control.h
  build/
  desktop.ini
```

`build/` and `desktop.ini` are not part of the robot program logic. `README.txt` contains the original setup/library links; this `README.md` expands the code documentation.

## File Reference

### `baseline_functionality_demonstration.ino`

The Arduino entry point. It includes `def_includes.h`, creates all global hardware objects, creates the global `robot controller`, and defines `setup()` and `loop()`.

Functions:

- `setup()`: calls `boardSetup(...)` with pointers to hardware objects, then sets `controller.behavior` to `BEHAVIOR_HEARTBEAT`.
- `loop()`: updates sensor inputs, runs robot behavior logic, and writes outputs.

Global objects:

- `Adafruit_MAX17048 lipo_monitor`: battery fuel gauge.
- `Adafruit_NeoPixel rgb_led`: onboard RGB LED object.
- `Adafruit_MPU6050 imu`: accelerometer/gyroscope.
- `Adafruit_TCS34725 color_sensor`: RGB color sensor.
- `QMC5883LCompass compass`: magnetometer.
- `robot controller`: shared state and behavior logic container.

### `def_includes.h`

Central include file used to keep the main `.ino` small. It imports Arduino, Wire/I2C, Adafruit device libraries, helper definitions, and the local modules.

Important definitions:

- `DEFAULT_I2C_PORT`: set to `&Wire`.
- `SECONDARY_I2C_PORT`: conditionally set to `&Wire1` for boards with a secondary I2C port.

### `def_config.h`

Project-level configuration constants.

Constants:

- `SERIAL_BAUD_RATE`: serial speed, currently `115200`.
- `BATTERY_CAPACITY_MAH`: nominal battery capacity, currently `850`.
- `PWM_FREQUENCY`: PWM frequency for motor pins, currently `4000`.
- `PWM_BIT_RESOLUTION`: PWM resolution, currently `8`.

### `def_pins.h`

Pin map for the ESP32-S3 robot hardware.

Motor/H-bridge pins:

- `DO_HB_SLP`: H-bridge sleep/enable pin.
- `DO_HB_AIN1`, `DO_HB_AIN2`: motor A direction/PWM pins.
- `DO_HB_BIN1`, `DO_HB_BIN2`: motor B direction/PWM pins.
- `PWM_CH_A1`, `PWM_CH_A2`, `PWM_CH_B1`, `PWM_CH_B2`: PWM channel identifiers retained for older/commented LEDC code.

LED and input pins:

- `DO_ONBOARD_LED`: onboard LED pin.
- `DO_COLOR_SENSOR_LED`: color sensor illumination LED pin.
- `DI_USER_BUTTON`: user pushbutton input.
- `PIN_NEOPIXEL`: onboard NeoPixel data pin.

### `def_friendlynames.h`

Human-readable constants for motor directions, LED states, and packed RGB colors.

Constants:

- `MOTORDIR_FWD`, `MOTORDIR_REV`, `MOTORDIR_STOP`.
- `LED_ON`, `LED_OFF`.
- `RED`, `YELLOW`, `GREEN`, `BLUE`, `TEAL`, `PURPLE`, `WHITE`.

### `def_states_and_phases.h`

Behavior, action, role, and phase IDs used by the controller state machine.

Defined behavior families:

- `BEHAVIOR_IDLE`: do nothing.
- `BEHAVIOR_HEARTBEAT`: serial status reporting.
- `COLOR_SENSE_TEST`: color reporting.
- `WHERE_TO_GO`: movement/correction testing.
- `BEHAVIOR_SPATIAL_PROGRAM`: color-driven spatial movement.
- `BEHAVIOR_EXAMPLE`: simple forward/spin example.
- `BEHAVIOR_SHADOW_INSTRUCTION`: white/black guide behavior.
- `BEHAVIOR_FOLLOW_LINE`: defined but not implemented in `robot.cpp`.
- `BEHAVIOR_FRACTIAL_PLAID_GPS`: fractal-plaid GPS behavior.
- `BEHAVIOR_GESTURE_CONTROL`: IDs are defined, but no handler is active in this baseline `runLogic()`.

### `setup.h` and `setup.cpp`

Own board startup, pin configuration, and serial initialization.

Functions:

- `boardSetup(Adafruit_NeoPixel*, Adafruit_MAX17048*, Adafruit_MPU6050*, Adafruit_TCS34725*, QMC5883LCompass*)`: configures secondary I2C pins, calls `pinConfig()`, initializes the battery monitor, IMU, compass, and NeoPixel, waits for the user button before continuing, starts serial, and sets the NeoPixel green when initialization completes.
- `pinConfig()`: configures color-sensor LED, H-bridge sleep, user button, PWM resolution/frequency, initializes PWM outputs to zero, and drives default output states low.
- `serialSetup()`: starts `Serial` at `SERIAL_BAUD_RATE` and prints compile file/date/time information.

### `inputs.h` and `inputs.cpp`

Read physical hardware into the shared `robot` object.

Functions:

- `inputs(...)`: top-level input function. Calls `inputsFromDevices(...)` and `inputsFromHardwarePins(...)`.
- `inputsFromDevices(...)`: updates time, reads MPU6050 acceleration/gyro/temperature, reads QMC5883L magnetometer axes, reads MAX17048 battery voltage/percentage, reads TCS34725 raw RGB/clear values, and calls `robot::inputIMU(...)` and `robot::inputColor(...)` to interpret selected values.
- `inputsFromHardwarePins(robot*)`: reads `DI_USER_BUTTON` and stores the inverted pull-up result in `controller->user_button_pushed`.

### `outputs.h` and `outputs.cpp`

Translate controller output variables into hardware actions.

Functions:

- `outputs(robot*, Adafruit_NeoPixel*)`: calls `safetyControl(...)`, writes LEDs, then calls `runMotors(...)` using `controller->drive_forward` and `controller->drive_spin`.
- `setLEDs(robot*, Adafruit_NeoPixel*)`: starts the NeoPixel object, writes the controller RGB values, calls `show()`, and writes the color-sensor white LED output.
- `runMotors(float forward, float spin)`: converts normalized forward/spin commands in the range `-1..1` into two motor speeds, enables/disables the H-bridge sleep pin, and writes PWM to the four motor input pins.
- `safetyControl(robot*)`: placeholder for future output safety logic. It currently does nothing.

Note: `setPixelColor(controller->rgb_r, controller->rgb_g, controller->rgb_b, 0)` passes `rgb_r` as the pixel index. For a one-pixel NeoPixel, typical usage would be index `0` followed by RGB values. The README documents the current code as written.

### `robot.h` and `robot.cpp`

The main state container and behavior implementation. Most variables are public by design, which is common in small embedded sketches where separate modules cooperatively read/write shared state.

Important fields:

- Identity/state: `selfID`, `role`, `program_tick`, `behavior`, `action`, `phase`.
- Time: `this_time`, `last_time`, `dt`.
- Sensors: `ax`, `ay`, `az`, `gx`, `gy`, `gz`, `mx`, `my`, `mz`, `temperature`, `battery_voltage`, `battery_percent`, `sense_red`, `sense_green`, `sense_blue`, `sense_clear`.
- Calculated color/orientation: `_hue`, `_brightness`, `_color`, `_theta`, `de_theta`.
- GPS: `gps_x`, `gps_y`, `step_size`, `gps_iter`, `gps_color`.
- Outputs: `rgb_r`, `rgb_g`, `rgb_b`, `color_sensor_white_led`, `drive_forward`, `drive_spin`.
- Stability timers: `previous_color`, `c_c`, `h_ms`, `h1_ms`, `h2_ms`.

Methods:

- `robot()` / `~robot()`: empty constructor/destructor.
- `runLogic()`: resets outputs, then dispatches to the active behavior based on `behavior`.
- `resetOutputsToNull()`: clears drive commands, RGB values, and color-sensor LED command.
- `behaviorReport()`: prints time, IMU, heading, magnetometer, temperature, battery, color, hue, and color index values over serial.
- `whats_the_color()`: prints hue, brightness, and color index only.
- `move_test()`: uses `correction(...)` to turn toward `de_theta` and prints current heading, desired heading, forward, and spin values.
- `behaviorExample()`: simple action state machine that drives forward/spins based on detected color indices.
- `behavior_spatial_programming()`: alternates between green and blue action states, sets a desired movement vector, converts it to `de_theta`, and uses heading correction. It transitions when stable target colors are detected.
- `shadow_instructions()`: drives forward on white and ramps forward/spin behavior on black, using stable color detection to switch actions.
- `f_p_gps()`: fractal-plaid GPS state machine. It waits for stable white, waits for a stable non-white direction color, updates `gps_x`/`gps_y`, halves the step size, waits for white again, and repeats until five iterations complete.
- `inputIMU(...)`: stores accelerometer and gyro values and computes `_theta` from accelerometer axes.
- `inputMag(...)`: stores magnetometer values. This exists but `inputsFromDevices(...)` writes magnetometer fields directly instead of calling it.
- `inputTemperature(...)`: stores temperature.
- `inputBattery(...)`: stores voltage and percentage.
- `setInputTime(uint32_t)`: updates `last_time`, `this_time`, and `dt`.
- `inputColor(...)`: computes hue, brightness, and color index from raw RGB values.
- `cali_thresh(...)`: declared in `robot.h` but not implemented in `robot.cpp`.

### `simple_helper_functions.h` and `simple_helper_functions.cpp`

General-purpose utility functions used by robot logic, color sensing, timing, serial helpers, and movement.

Math and normalization:

- `bound(input, minimum, maximum)`: clamps a value.
- `adjustMaxMin(measurement, max_meas, min_meas)`: updates observed max/min values.
- `normalizedNumber0to1(x, x_max, x_min)`: updates max/min and normalizes `x` to `0..1`.
- `normalizedNumberNeg1to1(x, x_max, x_min)`: normalizes `x` to `-1..1`.
- `getNthPrime(nth)`: returns the nth prime.
- `getFloatSign(x)` / `getIntSign(x)`: returns `-1`, `0`, or `1`.
- `getMidof3(a, b, c)`: returns the middle value of three numbers.
- `getSlope(...)` / `getYIntercept(...)`: linear regression helpers for two points.
- `isqrt(N)`: integer square root approximation.
- `wrapToPi(val)`: wraps an angle-like value into roughly `-PI..PI`.
- `getAbsentByte(...)`: returns the option not present among two selected byte values.

Vector helpers:

- `vectorAdd_f(...)`: adds a scaled vector into a target 3-vector.
- `getVectorMagnitude(x, y, z)`: returns 3D vector magnitude.
- `vectorDotProduct(...)`: returns dot product of two 3D vectors.
- `normalizeVector(...)`: normalizes a 3D vector in place.
- `getAngleBetweenVectors(...)`: returns angle between two 3D vectors.

Analog measurement:

- `getAvgReading(pin, numMeas)`: average analog reading as float.
- `getAvgReading_int(pin, numMeas)`: average analog reading as integer.
- `getAvgReading_int_notHiOrLo(pin, numMeas)`: average after dropping highest and lowest samples.
- `getMaxofN(pin, numMeas)`: maximum analog reading across samples.

Timing/filtering:

- `checkTime(startTime, nowTime, targetDuration_seconds)`: returns true after the requested duration.
- `susdFilter(meas, last, stepsize)`: limits change per update.
- `expFilter(newVal, oldVal, alpha)`: exponential smoothing.

Encoding and memory:

- `floatToFourChars(...)` / `fourCharsToFloat(...)`: reinterpret floats as bytes and back.
- `int16ToTwoChars(...)` / `twoCharsToInt16(...)`: convert 16-bit integer values to/from bytes.
- `floatToEncodedByte(...)` / `encodedByteToFloat(...)`: encode/decode scaled floats in a byte.
- `s2u_int8(...)` / `u2s_int8(...)`: signed/unsigned 8-bit reinterpret helpers.
- `clearArray(...)`: zeroes a byte array.
- `getFreeMemory()`: estimates remaining heap/stack memory.

Serial helpers:

- `waitForKeypress()`: blocks until serial data arrives, returns one character, and clears the buffer.
- `printArray(...)`: prints or writes a byte array over serial.

Color and movement:

- `calchue(R, G, B)`: converts RGB readings to normalized hue.
- `intensity(R, G, B)`: computes brightness as the midpoint of max/min RGB.
- `sense_color(hue, brightness, thresh_white, thresh_black)`: maps hue/brightness to color index constants.
- `orientation(x, y)`: converts a 2D vector to a `0..360` heading.
- `correction(d_theta, c_theta, drive_forward, drive_spin)`: turns toward a desired heading, then drives forward inside a dead zone.
- `color_count(measured_color, color_for_logic, count_of_color, hold_ms)`: returns true only after a color remains stable for `hold_ms`.
- `forward_spin(what_color, forward, spin)`: ramps forward and spin outputs over time while a condition remains true.
- `color_count_2(...)`: declared in the header but not implemented in the source.

### `serial_helper_programs.h` and `serial_helper_programs.cpp`

Blocking serial input and printing helpers.

Functions:

- `clearSerialBuffer()`: discards pending serial bytes.
- `waitForSerialIn()`: blocks until serial data is available.
- `drawHoriz(num, sym)`: prints a repeated character line.
- `getSerialKeypress()`: waits for a non-newline keypress and returns it.
- `getSerialKeypressEcho()`: reads a keypress and echoes it.
- `getSerialInt(timeout, echo)`: reads an integer with timeout.
- `getSerialFloat(timeout, echo)`: reads a float with timeout.
- `getAffirmation(float, timeout)`: asks the user to confirm a value with `y/n`.
- `checkYN(timeout)`: returns true for `y`, false otherwise.
- `getSerialFloat_check(timeout, echo)`: repeatedly reads a float until confirmed.
- `printCompileInfo()`: currently contains commented compile-info printing.

### `user_control.h` and `user_control.cpp`

Serial-control extension point.

Functions:

- `serialInterjection(robot*)`: checks serial input and reacts to a user-entered `x` by printing a message and delaying. This is currently not called because `serialInterjection(&controller)` is commented out in `loop()`.

## Behavior Summary

| Behavior | Function | Purpose |
| --- | --- | --- |
| `BEHAVIOR_IDLE` | none | Leaves outputs cleared. |
| `BEHAVIOR_HEARTBEAT` | `behaviorReport()` | Serial sensor/status dump. |
| `BEHAVIOR_EXAMPLE` | `behaviorExample()` | Demonstrates basic action transitions. |
| `COLOR_SENSE_TEST` | `whats_the_color()` | Reports hue, brightness, and color index. |
| `WHERE_TO_GO` | `move_test()` | Tests desired heading correction. |
| `BEHAVIOR_SPATIAL_PROGRAM` | `behavior_spatial_programming()` | Color-triggered movement state machine. |
| `BEHAVIOR_SHADOW_INSTRUCTION` | `shadow_instructions()` | White/black guided motion behavior. |
| `BEHAVIOR_FRACTIAL_PLAID_GPS` | `f_p_gps()` | Iterative color-coded position estimate. |

## Color Indexes

Color indexes are defined in `simple_helper_functions.h`:

| Name | Value |
| --- | --- |
| `COLOR_BLACK` | `0` |
| `COLOR_WHITE` | `1` |
| `COLOR_RED` | `2` |
| `COLOR_YELLOW` | `3` |
| `COLOR_GREEN` | `4` |
| `COLOR_CYAN` | `5` |
| `COLOR_BLUE` | `6` |
| `COLOR_MAGENTA` | `7` |

## External Libraries

Install these libraries in the Arduino IDE or your Arduino build environment:

- Adafruit MAX1704X.
- Adafruit TCS34725.
- Adafruit MPU6050.
- QMC5883LCompass.
- Adafruit NeoPixel.
- Adafruit TestBed.

The original `README.txt` includes library and board setup links.

## Known Gaps and Notes

- `cali_thresh(...)` is declared but not implemented.
- `color_count_2(...)` is declared but not implemented.
- `safetyControl(...)` is a placeholder.
- `BEHAVIOR_FOLLOW_LINE` and gesture-related IDs are defined but not handled by `runLogic()`.
- The spelling `BEHAVIOR_FRACTIAL_PLAID_GPS` is used consistently in code and should be kept unless all references are changed together.
- The current baseline startup waits while the user button is not pressed, blinking the NeoPixel red during the wait.
