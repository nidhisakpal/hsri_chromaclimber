# ChromaClimber

ChromaClimber is an Arduino/C++ robot-control program for the Pololu 3pi+ 32U4 OLED Robot with an added Adafruit TCS34725 color sensor. The program presents a menu on the OLED display, lets the user choose a behavior, samples the robot's sensors, computes interpreted state, runs behavior logic, and converts requested movement/LED/sound outputs into Pololu hardware commands.

## Hardware Target

- Main robot: Pololu 3pi+ 32U4 OLED Robot.
- Display: Pololu OLED.
- Input buttons: Pololu buttons A, B, and C.
- Motion hardware: motors and encoders through the Pololu 3pi+ library.
- Sensors: Pololu IMU, line sensors, bump sensors, battery reading.
- External color sensor: Adafruit TCS34725 over I2C.
- Audio: Pololu buzzer.

## Program Flow

The main loop is structured around a menu and a robot-controller pipeline:

1. `setup()` initializes serial, the Pololu menu, I2C, IMU, custom display characters, and the global `robot controller`.
2. The menu is shown when button B is pressed.
3. When the menu is not active, `loop()` calls `inputs(&controller)`.
4. `controller.processSensorData()` converts raw sensor readings into hue, color, orientation, battery percentage, and encoder speeds.
5. `controller.runLogic()` clears outputs and runs the selected behavior.
6. `controller.processOutputs()` converts behavior-level commands into motor speeds.
7. `outputs(&controller)` writes LEDs, speaker commands, and motor speeds to hardware.

This creates a four-stage control pattern: raw input, calculated state, behavior decisions, and hardware output.

## Folder Structure

```text
ChromaClimber/
  Bak/
    20260327ChromaClimber.zip
  ChromaClimber.ino
  def_friendlynames.h
  def_playtones.h
  def_robot_properties.h
  def_states_and_phases.h
  extended_lcd_font.h
  font.cpp
  menu_stuff.h
  README.md
  robot.cpp
  robot.h
  serial_helper_programs.cpp
  serial_helper_programs.h
  simple_helper_functions.cpp
  simple_helper_functions.h
```

`Bak/20260327ChromaClimber.zip` is a backup archive and is not compiled directly by the Arduino sketch.

## File Reference

### `ChromaClimber.ino`

The Arduino entry point and top-level hardware integration file. It creates global Pololu hardware objects, configures the menu, gathers inputs, and writes outputs.

Global objects:

- `OLED display`: robot display.
- `Buzzer buzzer`: speaker/tone output.
- `ButtonA buttonA`, `ButtonB buttonB`, `ButtonC buttonC`: physical buttons.
- `LineSensors lineSensors`: five line sensor array.
- `BumpSensors bumpSensors`: front bump switches.
- `IMU imu`: accelerometer, gyroscope, and magnetometer.
- `Motors motors`: motor driver interface.
- `Encoders encoders`: wheel encoder interface.
- `PololuMenu<typeof(display)> mainMenu`: OLED menu controller.
- `Adafruit_TCS34725 color_sensor`: RGB color sensor.
- `robot controller`: shared state and behavior logic object.

Setup and loop functions:

- `setup()`: starts serial, defines the main menu items, binds menu display/buzzer/buttons, starts I2C, initializes and enables the IMU, loads the back-arrow custom character, and gives the controller a pointer to the OLED display.
- `loop()`: if button B is not being used to open the menu, runs the robot pipeline. If B opens the menu, shows a short "Main Menu" welcome screen, waits for behavior selection, and redraws the back-arrow prompt.

Menu behavior configuration:

- `config_behavior_spatial_programming()`: sets `BEHAVIOR_SPATIAL_PROGRAM` and `ACTION_SPATIAL_INIT`.
- `config_behavior_heartbeat()`: sets `BEHAVIOR_HEARTBEAT` and `ACTION_INIT`.
- `config_behavior_shadow_instructions()`: sets `BEHAVIOR_GUIDE_TO_GOAL` and `ACTION_GUIDE_INIT`.
- `config_behavior_virtual_joysticks()`: sets `BEHAVIOR_GESTURE_CONTROL` and `ACTION_GESTURE_INIT`.
- `config_behavior_fractal_gps()`: sets `BEHAVIOR_FRACTIAL_PLAID_GPS` and `ACTION_GPS_INIT`.
- `config_behavior_color_reporting()`: sets `COLOR_SENSE_TEST` and `ACTION_INIT`.

Input/output functions:

- `inputs(robot*)`: reads elapsed time, IMU axes, battery voltage, color sensor RGB/clear values, encoders, buttons, line sensors, and bump sensors into the controller.
- `outputs(robot*)`: writes LEDs, controls buzzer play mode, and sends scaled motor speed commands to the Pololu motor driver.

Display helper functions:

- `loadCustomCharacters()`: loads only the back-arrow custom character.
- `loadCustomCharactersBarGraph()`: loads custom bar-graph level characters into display slots 0 through 6.
- `loadCustomCharactersMotorDirs()`: loads motor-direction arrow characters into display slots 0 through 3.
- `displayBackArrow()`: clears the OLED and displays a back-arrow plus `B` on the second line.
- `showSplash()`: shows a larger splash sequence using a graphics buffer, blinks LEDs, displays "ChromaClimber", waits for button B, plays a thank-you beep, and returns to the compact display layout.
- `mainMenuWelcome()`: turns LEDs off, displays "Main Menu", and pauses briefly.

Note: `ChromaClimber.ino` includes `splash.h`, but that file is not present in this folder. If the sketch does not compile, that missing include is one likely cause.

### `robot.h` and `robot.cpp`

The controller class stores raw inputs, calculated sensor values, behavior state, and requested outputs. The class is intentionally public-field heavy so the Arduino sketch can read/write hardware state directly.

Important fields:

- Identity/state: `selfID`, `role`, `program_tick`, `behavior`, `action`, `phase`.
- Flags/timers: `flag_0`, `flag_1`, `this_time`, `last_time`, `dt`, `logic_timer_0`.
- Motion commands: `move_mode`, `drive_forward`, `drive_spin`, `move_x`, `move_y`, `de_theta`.
- Display pointer: `OLED* display`.
- Raw IMU inputs: `input_ax`, `input_ay`, `input_az`, `input_gx`, `input_gy`, `input_gz`, `input_mx`, `input_my`, `input_mz`.
- Raw color inputs: `input_sense_red`, `input_sense_green`, `input_sense_blue`, `input_sense_clear`.
- Buttons and contact inputs: `input_button_a`, `input_button_b`, `input_button_c`, `input_bumper_left`, `input_bumper_right`.
- Encoders: `encoder_count_left`, `encoder_count_right`, previous counts, and calculated speeds.
- Line sensors: `input_line_sensor_values[5]`.
- Calculated values: `calc_battery_percent`, `calc_hue`, `calc_brightness`, `calc_color`, `calc_theta`.
- GPS state: `gps_x`, `gps_y`, `step_size`, `gps_iter`, `gps_color`.
- Outputs: `led_r`, `led_g`, `led_b`, `speaker_frequency`, `speaker_volume`, `speaker_duration`, `play_mode`, `motorspeed_left`, `motorspeed_right`.
- Color stability: `previous_color`, `c_c`, `h_ms`, `h1_ms`, `h2_ms`.

Methods:

- `robot()` / `~robot()`: empty constructor/destructor.
- `runLogic()`: clears outputs and dispatches to the active behavior.
- `behaviorReport()`: prints a comma-separated telemetry line containing timing, IMU, heading, battery, raw color, hue, brightness, and color index.
- `whats_the_color()`: prints hue, brightness, and color index in a more readable format.
- `behavior_spatial_programming()`: maps detected colors to movement directions. Blue moves north, green moves east, red moves west, teal moves south, yellow stops and plays a tone, white triggers a brief reverse maneuver, and unknown colors stop.
- `shadow_instructions()`: guide-to-goal behavior. On white it drives forward, on black it spins, on green it stops at the goal, and on bumper contact it reverses then spins for a random duration before continuing.
- `f_p_gps()`: fractal-plaid GPS state machine. It waits for stable white, waits for stable non-white direction colors, updates `gps_x`/`gps_y`, halves `step_size`, waits for white again, and completes after five iterations.
- `processSensorData()`: computes color hue/brightness/index, orientation from acceleration, battery percentage from voltage, and encoder speeds from count deltas.
- `resetOutputsToNull()`: clears motor speeds and LED outputs.
- `processOutputs()`: converts behavior-level movement commands to differential motor speeds. `DRIVEMODE_VECTOR` turns toward `move_x`/`move_y` before driving. `DRIVEMODE_ARC` directly converts forward/spin. `DRIVEMODE_MANUAL` assumes motor speeds were set directly. It also applies a bumper safety override.
- `setMoveVectors(uint8_t direction)`: maps direction constants such as `NORTH`, `EAST`, or `STOP_MOVING` to `move_x`/`move_y`.

Behavior dispatch currently handles:

- `BEHAVIOR_IDLE`.
- `BEHAVIOR_HEARTBEAT`.
- `COLOR_SENSE_TEST`.
- `BEHAVIOR_SPATIAL_PROGRAM`.
- `BEHAVIOR_GUIDE_TO_GOAL`.
- `BEHAVIOR_FRACTIAL_PLAID_GPS`.

`BEHAVIOR_GESTURE_CONTROL` is selectable from the menu, but `runLogic()` does not currently include a case for it.

### `def_states_and_phases.h`

Behavior and action numeric IDs for the state machine.

Defined behavior families:

- `BEHAVIOR_IDLE`: no behavior.
- `BEHAVIOR_HEARTBEAT`: serial telemetry.
- `COLOR_SENSE_TEST`: color reporting.
- `WHERE_TO_GO`, `BEHAVIOR_1`, `BEHAVIOR_2`, `BEHAVIOR_4`: reserved/demo IDs.
- `BEHAVIOR_SPATIAL_PROGRAM`: color-to-direction movement.
- `BEHAVIOR_EXAMPLE`: baseline/demo action IDs retained from earlier code.
- `BEHAVIOR_GUIDE_TO_GOAL`: shadow/guide behavior with bumper recovery.
- `BEHAVIOR_FOLLOW_LINE`: defined but not implemented in `robot.cpp`.
- `BEHAVIOR_FRACTIAL_PLAID_GPS`: color-coded iterative GPS.
- `BEHAVIOR_GESTURE_CONTROL`: role/action IDs are defined, but behavior logic is not implemented in the current dispatch.

### `def_robot_properties.h`

Physical and control tuning constants.

Constants:

- `DIAMETER_WHEEL_MM`: wheel diameter.
- `TRACK_WIDTH_MM`: distance between drive wheels.
- `SPEED_MAX_MMPS`: nominal max speed.
- `DIAMETER_ROBOT_MM`: robot body diameter.
- `BATTERY_VOLTAGE_MAX`, `BATTERY_VOLTAGE_MIN`: voltage range used to estimate battery percentage.
- `FACING_DEADZONE_DEG`: heading error below which the robot drives straight.
- `FACING_ONLY_TURN_DEG`: heading error above which the robot turns in place.

### `def_friendlynames.h`

Readable aliases for drive modes, LED states, colors, and compass-like movement directions.

Constants:

- Motor direction aliases: `MOTORDIR_FWD`, `MOTORDIR_REV`, `MOTORDIR_STOP`.
- Drive modes: `DRIVEMODE_MANUAL`, `DRIVEMODE_ARC`, `DRIVEMODE_VECTOR`.
- LED aliases: `LED_ON`, `LED_OFF`.
- Packed color values: `RED`, `YELLOW`, `GREEN`, `BLUE`, `TEAL`, `PURPLE`, `WHITE`, `BLACK`.
- Direction vectors: `NORTH_X/Y`, `EAST_X/Y`, `SOUTH_X/Y`, `WEST_X/Y`, plus diagonal vectors.
- Direction IDs: `NORTH`, `NORTHEAST`, `EAST`, `SOUTHEAST`, `SOUTH`, `SOUTHWEST`, `WEST`, `NORTHWEST`, `STOP_MOVING`.

### `def_playtones.h`

Constants for speaker output modes and stored song snippets.

Constants:

- `SILENT`: stop speaker output.
- `PLAY_TONE`: play a single frequency/duration/volume command.
- `CONTINUE_PLAYING`: leave the current sound running.
- `SONG_FUGUE`: command ID for the stored fugue melody.
- `NOTESEQUENCE_FUGUE`: Pololu buzzer note string.

### `menu_stuff.h`

Small stored sounds and custom OLED character bitmaps used by the menu and splash helpers.

Contents:

- `beepBrownout`, `beepWelcome`, `beepThankYou`, `beepFail`, `beepPass`, `beepReadySetGo`: short Pololu buzzer note strings stored in program memory.
- `backArrow`: custom LCD/OLED character used to indicate the B/back action.
- `forwardArrows`, `reverseArrows`, `forwardArrowsSolid`, `reverseArrowsSolid`: custom characters for motor-direction display modes.

### `font.cpp` and `extended_lcd_font.h`

Custom OLED font support.

- `extended_lcd_font.h`: an X Bitmap style byte array called `font_bits` containing font data.
- `font.cpp`: uses a macro rename trick so including `extended_lcd_font.h` replaces the Pololu OLED font symbol with the custom font data.

This is display infrastructure only; it does not contain robot behavior logic.

### `simple_helper_functions.h` and `simple_helper_functions.cpp`

General-purpose utility library shared with the baseline sketch.

Math and normalization:

- `bound(input, minimum, maximum)`: clamps a value.
- `adjustMaxMin(measurement, max_meas, min_meas)`: updates max/min observations.
- `normalizedNumber0to1(x, x_max, x_min)`: updates max/min and normalizes to `0..1`.
- `normalizedNumberNeg1to1(x, x_max, x_min)`: normalizes to `-1..1`.
- `getNthPrime(nth)`: returns the nth prime.
- `getFloatSign(x)` / `getIntSign(x)`: returns numeric sign.
- `getMidof3(a, b, c)`: returns the middle of three values.
- `getSlope(...)` / `getYIntercept(...)`: two-point line helpers.
- `isqrt(N)`: integer square root approximation.
- `wrapToPi(val)`: wraps an angle-like value by one `2*PI` step.
- `getAbsentByte(...)`: returns whichever option was not selected.

Vector helpers:

- `vectorAdd_f(...)`: scaled vector addition.
- `getVectorMagnitude(x, y, z)`: 3D magnitude.
- `vectorDotProduct(...)`: 3D dot product.
- `normalizeVector(...)`: normalize a 3D vector in place.
- `getAngleBetweenVectors(...)`: angle between two 3D vectors.

Analog/timing/filtering:

- `getAvgReading(pin, numMeas)`: average analog reading as float.
- `getAvgReading_int(pin, numMeas)`: average analog reading as int.
- `getAvgReading_int_notHiOrLo(pin, numMeas)`: average after removing high and low sample.
- `getMaxofN(pin, numMeas)`: maximum reading.
- `checkTime(startTime, nowTime, targetDuration_seconds)`: elapsed-time check.
- `susdFilter(meas, last, stepsize)`: slew-rate style limiter.
- `expFilter(newVal, oldVal, alpha)`: exponential smoothing.

Encoding, serial, and memory:

- `floatToFourChars(...)` / `fourCharsToFloat(...)`: float-byte reinterpret conversion.
- `int16ToTwoChars(...)` / `twoCharsToInt16(...)`: 16-bit byte conversion.
- `floatToEncodedByte(...)` / `encodedByteToFloat(...)`: scaled byte encoding.
- `s2u_int8(...)` / `u2s_int8(...)`: signed/unsigned 8-bit reinterpret helpers.
- `waitForKeypress()`: blocking serial key read.
- `printArray(...)`: prints or writes an array over serial.
- `getFreeMemory()`: estimates free memory.
- `clearArray(...)`: zeroes byte array contents.

Color and motion:

- `calchue(R, G, B)`: computes normalized hue from RGB.
- `intensity(R, G, B)`: computes brightness from max/min RGB.
- `sense_color(hue, brightness, thresh_white, thresh_black)`: maps hue/brightness to color index constants.
- `orientation(x, y)`: returns a heading in degrees from a 2D vector.
- `correction(d_theta, c_theta, drive_forward, drive_spin)`: heading correction helper retained from baseline code.
- `color_count(measured_color, color_for_logic, count_of_color, hold_ms)`: stable-color timer.
- `forward_spin(what_color, forward, spin)`: ramps forward and spin outputs while active.
- `color_count_2(...)`: declared in the header but not implemented in the source.

Color constants in this helper:

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

### `serial_helper_programs.h` and `serial_helper_programs.cpp`

Blocking serial helpers. These are available for debugging and calibration, although the main sketch does not currently call them directly.

Functions:

- `clearSerialBuffer()`: discards pending serial bytes.
- `waitForSerialIn()`: blocks until serial data is available.
- `drawHoriz(num, sym)`: prints a repeated character line.
- `getSerialKeypress()`: waits for a non-newline keypress.
- `getSerialKeypressEcho()`: reads and echoes a keypress.
- `getSerialInt(timeout, echo)`: reads an integer.
- `getSerialFloat(timeout, echo)`: reads a float.
- `getAffirmation(float, timeout)`: asks user to confirm a value.
- `checkYN(timeout)`: returns true only for `y`.
- `getSerialFloat_check(timeout, echo)`: repeatedly reads and confirms a float.
- `printCompileInfo()`: currently contains commented compile-info output.

### `Bak/20260327ChromaClimber.zip`

Backup archive. It is useful for recovery/comparison but is not part of the active Arduino build.

## Menu Options and Behaviors

| Menu item | Behavior | Action initialized | Runtime function |
| --- | --- | --- | --- |
| Spatial | `BEHAVIOR_SPATIAL_PROGRAM` | `ACTION_SPATIAL_INIT` | `behavior_spatial_programming()` |
| Heartbeat | `BEHAVIOR_HEARTBEAT` | `ACTION_INIT` | `behaviorReport()` |
| Shadow | `BEHAVIOR_GUIDE_TO_GOAL` | `ACTION_GUIDE_INIT` | `shadow_instructions()` |
| Joysticks | `BEHAVIOR_GESTURE_CONTROL` | `ACTION_GESTURE_INIT` | Not currently handled in `runLogic()` |
| Fractal GPS | `BEHAVIOR_FRACTIAL_PLAID_GPS` | `ACTION_GPS_INIT` | `f_p_gps()` |
| Color Rep | `COLOR_SENSE_TEST` | `ACTION_INIT` | `whats_the_color()` |

## Movement Modes

- `DRIVEMODE_VECTOR`: behavior sets `move_x` and `move_y`; `processOutputs()` turns toward the target direction and then drives forward.
- `DRIVEMODE_ARC`: behavior directly sets `drive_forward` and `drive_spin`; `processOutputs()` converts them to left/right motor speeds.
- `DRIVEMODE_MANUAL`: intended for direct motor-speed control, though current behavior code mostly uses vector or arc mode.

## External Libraries

Install these libraries in the Arduino IDE or compatible build environment:

- Pololu3piPlus32U4.
- Pololu3piPlus32U4IMU.
- PololuMenu.
- Adafruit TCS34725.
- Wire/Arduino core libraries.

## Known Gaps and Notes

- `splash.h` is included but missing from the folder.
- `BEHAVIOR_GESTURE_CONTROL` is exposed in the menu but not implemented in `robot::runLogic()`.
- `BEHAVIOR_FOLLOW_LINE` is defined but not implemented.
- `color_count_2(...)` is declared but not implemented.
- `move_vector_x` and `move_vector_y` are declared in `robot.h`, but the active movement code uses `move_x` and `move_y`.
- The spelling `BEHAVIOR_FRACTIAL_PLAID_GPS` is used by the code and should be changed only with a coordinated rename.
