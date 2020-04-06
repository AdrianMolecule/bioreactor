# Schema for bioreactor protocols

This page describes a JSON schema for representing bioreactor protocols. A protocol is a sequence of steps that the bioreactor should go through. For example, a step might involve heating bringing the bioreactor to a certain temperature or to a certain pH level, or it might involve simply waiting for a certain amount of time.

If you are not familiar with JSON (javascript object notation), read this [primer](https://www.json.org/json-en.html) first.

This schema is written in terms of [Golang structs](https://blog.golang.org/json) but can be used from any language.

### Example protocol

Here is an example of a program that:
 - brings the temperature to 220°C and holds it there for 3 hours, then
 - brings pH to 3, then
 - turns on a cyan light for 10 minutes

```json
{
    "name": "test protocol",
    "root": {
        "steps": [
            "hold_temperature_at": {
                "temperature": 220,
                "duration": 10800,
            },
            "bring_ph_to": {
                "ph": 3,
            },
            "hold_lighting_at": {
                "color": {
                    "red": 0,
                    "green": 255,
                    "blue": 255,
                }
                "duration": 600,
            }
        ]
    }
}
```


### BringTemperatureTo

BringTemperatureTo is a step that causes the bioreactor to bring the temperature to a certain level and then move on to the next step.

```go
type BringTemperatureTo struct {
    TemperatureCelsuis float64  // the desired temperature in degrees Celsius
    MaxRate float64             // the maximum rate of heating, in degrees Celsius per second
}
```


Example
```json
{
    "temperature_celsius": 220,   // 220°C
    "max_rate": 3,                // heat at 3 degrees per second
}
```

### HoldTemperatureAt

HoldTemperatureAt is a step that causes the bioreactor to bring the temperature to a certain level and then hold it there for a certain amount of time.

```go
type HoldTemperatureAt struct {
    TemperatureCelsuis float64  // the desired temperature in degrees Celsius
    MaxRate *float64            // the maximum rate of heating, in degrees Celsius per second
    Duration time.Duration      // the amount of time to hold the temperature at this level
}
```

Example
```json
{
    "temperature_celsius": 220,   // 220°C
    "max_rate": 3,                // heat at 3 degrees per second
    "duration": 10800,            // 10800 seconds = 3 hours
}
```


### BringPhTo

BringPhTo is a step that causes the bioreactor to bring the pH to a certain level and then move on to the next step.

```go
type BringPhTo struct {
    PH float64      // the desired pH
    MaxRate float64 // the maximum rate of heating, in degrees Celsius per second
}
```


Example
```json
{
    "ph": 5,                      // 220°C
    "max_rate": 3,                // heat at 3 degrees per second
    "duration": 10800,            // 10800 seconds = 3 hours
}
```


### HoldPhAt

HoldPhAt is a step that causes the bioreactor to bring the pH to a certain level and then hold it there for a certain amount of time.

```go
type HoldTemperatureAt struct {
    PH float64               // the desired temperature in degrees Celsius
    MaxRate float64          // the maximum rate of heating, in degrees Celsius per second
    Duration time.Duration   // the amount of time to hold the temperature at this level
}
```


Example
```json
{
    "ph": 5,
    "max_rate": 3,                // heat at 3 degrees per second
    "duration": 10800,            // 10800 seconds = 3 hours
}
```

### HoldLightingAt

HoldLightingAt holds the lighting at a set color and brightness for a certain amount of time.

```go
type Color struct {
    Red int    // The red component of color (0..255)
    Green int  // The green component of color (0..255)
    Blue int   // The blue component of color (0..255)
}

type HoldLightingAt struct {
    Color *Color   // The red, green, and blue components of the color
    Lumens int     // Brightness in lumens
    Duration time.Duration  // duration to hold the lighting at this point
}
```

Example:
```json
{
    "color": {
        "red": 0,
        "green": 255,
        "blue": 255,
    },
    "lumens": 600,
    "duration": 600,   // 600 seconds = 10 minutes
}
```

### Wait

Wait instructs the bioreactor to wait passively for a set amount of time

```go
type Wait struct {
    Duration time.Duration
}
```

Example:
```json
{
    "duration": 10800,   // 3 hours
}
```

### Sequence

A sequence instructs the bioreactor to execute a series of steps, one after another

```go
type Sequence struct {
    Steps []*Step
}
```

Example:
```json
{
    "steps": [
        "hold_temperature_at": {
            "temperature": 220,
            "duration": 10800,
        },
        "bring_ph_to": {
            "ph": 3,
        },
        "hold_lighting_at": {
            "color": {
                "red": 0,
                "green": 255,
                "blue": 255,
            }
            "duration": 600,
        }
    ]
}
```

### Step

A step is a union type that can be one of several types. Exactly one of the pointer members is non-nil.

```go
type Step struct {
    BringTemperatureTo *BringTemperatureTo
    HoldTemperatureAt *HoldTemperatureAt
    BringPhTo *BringPhTo
    HoldPhAt *HoldPhAt
    HoldLightingAt *HoldLightingAt
    Wait *Wait
    Sequence *Sequence
}
```

### Program

A program has a name and a root step. Exactly one of the pointer members are non-nil.

```go
type Program struct {
    Name string
    Root *Step
}
```

Example:
```json
{
    "name": "test protocol",
    "root": {
        "steps": [
            "hold_temperature_at": {
                "temperature": 220,
                "duration": 10800,
            },
            "bring_ph_to": {
                "ph": 3,
            },
            "hold_lighting_at": {
                "color": {
                    "red": 0,
                    "green": 255,
                    "blue": 255,
                }
                "duration": 600,
            }
        ]
    }
}
```
