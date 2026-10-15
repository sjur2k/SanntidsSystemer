# Butterfly Response-Time Tester

The BRTT is a 3-channel latency tester for 5V devices, developed for use in the exercises in TTK4147.

The response time is a measure of the time it takes a device to respond to an incoming pin change (*from* the BRTT) with a pin change of its own (*to* the BRTT). Since the BRTT is external to the device being tested, it does not introduce disturbances such as increased CPU usage and also measures hardware-level pin I/O delays.

The BRTT can test up to three independent channels, where each channel consists of a *test* pin and a *response* pin. It can also perform multiple tests in rapid succession and reports all results as text over a serial port.

## Hardware

Port B on the Butterfly (bottom left) is used for response-time testing.

| 1 | 3 | 5 | (unused) | VCC |
|---|---|---|----------|-----|
| 0 | 2 | 4 | (unused) | GND |

The six numbered pins can be configured in several ways as either response or test pins, with any combination of the three channels and with either active-high or active-low polarity.

By default, the following layout is used with active-low polarity:

| Resp A | Resp B | Resp C | (unused) | VCC |
|---------|---------|---------|----------|-----|
| Test A | Test B | Test C | (unused) | GND |

The two unused pins are wired to the joystick and should either remain disconnected or be configured as inputs on the device under test.

> **Warning:** Pin 5 is connected to the Butterfly's piezoelectric speaker, meaning you will hear a clicking sound when this pin is used in a test.

### Menu

The menu is navigated using the joystick:

- Right: Open menu
- Left: Close menu
- Up/Down: Cycle through menus or options

Opening a menu also sets the currently displayed option.

Settings are **not** saved when the device is powered down.

### Testing and Results

To start a test:

- Open the main `BRTTv4` menu, or
- Press the joystick button while on any menu page

If the tested device fails to respond before the Butterfly's internal timer overflows, the remaining test sequence is aborted.

### Serial Port Settings

| Setting | Value |
|----------|--------|
| Baud Rate | 9600 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |

The serial port pins are located to the left of the display:

| Pins |
|------|
| Rx |
| Tx |
| GND |

## BRTT Configuration

*Default values are highlighted in blue on the original interface.*

### Channel Selection (`CHNNLS`)

Selects which of channels A, B, and/or C are used for testing.

**Default:** `C + B + A`

Available options:

- `A`
- `B + A`
- `C + B`
- `C`
- `B`
- `C + A`

### Number of Sub-tests (`NUMBER`)

Sets the number of sub-tests performed in sequence.

Once started, a test sequence cannot be aborted except by causing a timeout (for example, by not responding).

Options:

- **Default:** `10`
- `100`
- `1000`
- `10000`

### Advanced Configuration (`CONFIG`)

#### Pin IO Method (`SAMPLE`)

Selects between polling and interrupt-based pin I/O.

| Option | Description |
|----------|-------------|
| **`POLL`** | Response pins are read (and test pins set) in a tight loop. |
| `INTRPT` | Response pins are detected with pin-change interrupts while test pins are set in a tight loop. |

> **Note on overlapping pin-change interrupts**
>
> The pin-change ISR blocks other interrupts while running. If responses arrive too close together, measurement artifacts may occur because one response must wait for the previous ISR to complete.
>
> To reduce this effect:
>
> - The BRTT introduces a minimum inter-channel release offset in interrupt mode.
> - Setting `RESET` to `DEFER` reduces unnecessary pin-change events and lowers the chance of overlapping interrupts.

#### Timer Prescaler (timeout/resolution) (`PRESCA`)

The timer prescaler controls the tradeoff between maximum test duration and measurement resolution.

All measurements use a 16-bit unsigned timer.

| Setting | Maximum Duration | Resolution |
|----------|-----------------|------------|
| `8388MS` | 8388 ms | 128 µs |
| `2097MS` | 2097 ms | 32 µs |
| `524MS` | 524 ms | 8 µs |
| `65MS` | 65 ms | ~7 µs (poll) / 1 µs (interrupt) |
| **`8MS`** | 8 ms | ~7 µs (poll) / 125 ns (interrupt) |

> **Polling mode has lower resolution** because each loop iteration consumes CPU cycles.
>
> The BRTT compensates for:
>
> - Loop iteration time in polling mode
> - Interrupt context-switch overhead in interrupt mode
>
> Connecting test and response pins directly together should therefore produce a measured response time of approximately **0**.

#### Channel Release Trigger (`RELEAS`)

Controls how test pins are activated.

| Option | Description |
|----------|-------------|
| **`SEPAR`** | Test pins are triggered separately in a random order. |
| `GROUP` | All test pins are triggered simultaneously. |

> **Important artifacts**
>
> 1. The randomly generated release time differs slightly from the actual release time. The BRTT compensates by reading back the actual trigger time.
> 2. In `SEPAR` mode, loop execution introduces a minimum delay between channel releases that cannot be compensated.

#### Max Test Pin Release Delay (`MAXREL`)

Sets the maximum delay before a test pin is released.

Values are relative to the timeout selected through the timer prescaler.

Options:

- `40%`
- **`20%`**
- `10%`
- `5%`

#### Test Pin Reset Timing (`RESET`)

Controls when test pins return to their inactive state.

| Option | Description |
|----------|-------------|
| **`IMMEDI`** | Test pin resets immediately after its response is received. |
| `DEFER` | All test pins reset together after all responses have been received. |

> If the device under test reacts to both rising and falling edges, `DEFER` can prevent reset events from affecting timing measurements.

#### Test/Response Pin Selection (`PINSEL`)

Selects which pins are used as test outputs. All remaining pins become response inputs.

Channels are assigned in the order:

**A, B, C**

| Option | Output Pins |
|----------|------------|
| **`EVEN`** | 0, 2, 4 |
| `ODD` | 1, 3, 5 |
| `LOWER` | 0, 1, 2 |
| `UPPER` | 3, 4, 5 |

#### Pin IO Polarity (`POLARI`)

Sets the active state of test and response pins.

| Option | Description |
|----------|-------------|
| **`LOW`** | Active low (GND) |
| `HIGH` | Active high (VCC) |

> **Important:** Response pins must be in the inactive state (default: inactive-high) before a test begins.