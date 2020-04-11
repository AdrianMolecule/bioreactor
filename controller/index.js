"use strict";

const {
  EtherPortClient
} = require('etherport-client');
const five = require("johnny-five");

// The overall frequency of checking sensors and logic in ms
const CHECK_INTERVAL = 500;

// The overall signal to stop all activity
var stopSignal = false;

// WIFI Example setup. No arguments for local board probably
const board = new five.Board({
  port: new EtherPortClient({
    host: '10.0.0.49',
    port: 3030
  }),
  repl: false
});

// EXAMPLE PINS: DIGITAL: 13, ANALOG: "A0"

// SENSOR PINS
const TEMP_SENSOR_DIGI_PIN  = ;
const PH_SENSOR_PIN = ;
const O2_SENSOR_PIN = ;

const LIGHT_SENSOR_PIN = ;

// ACTUATOR PINS
const HEATER_CONTROL_PIN = ;

const PHPUMP_CONTROL_PWM_DIGI_PIN = ;
const PHPUMP_CONTROL_DIR_DIGI_PIN = ;
const PHPUMP_CONTROL_CDIR_DIGI_PIN = ;

const O2_CONTROL_DIGI_PIN = ;
const ALARM_CONTROL_DIGI_PIN = ;


class Controller {
  
  constructor (args) {
    this.args = args;
	
	this.args.sensor.on("change", value => {
		if (!this.lastChecked) {
		   this.lastChecked = new Date();
		}
		if (this.lastChecked > ((new Date()) - CHECK_INTERVAL)) {
		  return;
		}
	  
		this.lastChecked = new Date();
		this.value = this.args.read(this.args.sensor);
    });
	
	this.actuatorInterval = setInterval(function () {
		if (!this.lastChecked) {
			return;
		}
		
		if (stopSignal) {
			this.args.stopAction(this.args.actuator);
			clearInterval(this.actuatorInterval);
			return;
		}
		
		if (this.value < this.args.target) {
			this.positiveAction(this.args.actuator);
		}
		else if (this.value > this.args.target) {
			this.negativeAction(this.args.actuator);
		}
	}, CHECK_INTERVAL);
  }
  
  // Property access example if constructor doesn't work
  //  static get CONSTANT() {
    // return "CONSTANT";
  // }
  // static set CONSTANT(value) {
    // throw new Error(`The readOnly property cannot be written. ${value} was passed.`);
  // }
  // set name(name) {
    // this._name = name.charAt(0).toUpperCase() + name.slice(1);
  // }
  // get name() {
    // return this._name;
  // }
}

board.on("ready", () => {
	// What's the sensor range like? Just a guess
	const TEMP_MIN = 0; 
	const TEMP_MAX = 255; 
	var temperatureSetting = 0;
	
	var temperature = new Controller({
		target: 37,
		sensor: new five.thermometer({
			controller: "ds18b20",
			pin: TEMP_SENSOR_DIGI_PIN
		}),
		read: function (sensor) {
			const {address, celsius, fahrenheit, kelvin} = sensor;
			return celsius;
		},
		actuator: new five.pin(HEATER_CONTROL_PIN),
		positiveAction: function (actuator) {
			if (temperatureSetting === TEMP_MAX) {
				return;
			}
			temperatureSetting = temperatureSetting + 1;
			actuator.writeAnalog(temperatureSetting);
		},
		negativeAction: function (actuator) {
			if (temperatureSetting === TEMP_MIN) {
				return;
			}
			temperatureSetting = temperatureSetting - 1;
			actuator.writeAnalog(temperatureSetting);
		},
		stopAction: function (actuator) {
			actuator.writeAnalog(TEMP_MIN);
		},
	});
	
	var pH = new Controller({
		target: 7,
		sensor: new sensor({
			pin: PH_SENSOR_PIN
		}),
		read: function (sensor) {
			return sensor.value;
		},
		actuator: new motor({
			pins: {
			  pwm: PHPUMP_CONTROL_PWM_DIGI_PIN,
			  dir: PHPUMP_CONTROL_DIR_DIGI_PIN,
			  cdir: PHPUMP_CONTROL_CDIR_DIGI_PIN
			}
		}),
		positiveAction: function (actuator) {
			actuator.start();
		},
		negativeAction: function (actuator) {
			actuator.brake();
		},
		stopAction: function (actuator) {
			actuator.brake();
		},
	});
	
	const O2_MIN = 0; 
	const O2_MAX = 90; 
	var o2Setting = 0;
	
	var O2 = new Controller({
		target: 4,
		sensor: new sensor({
			pin: O2_SENSOR_PIN
		}),
		read: function (sensor) {
			return sensor.value;
		},
		actuator: new five.servo(O2_CONTROL_DIGI_PIN),
		positiveAction: function (actuator) {
			if (o2Setting === O2_MAX) {
				return;
			}
			o2Setting = o2Setting + 1;
			actuator.to(o2Setting);
		},
		negativeAction: function (actuator) {
			if (o2Setting === O2_MIN) {
				return;
			}
			o2Setting = o2Setting - 1;
			actuator.to(o2Setting);
		},
		stopAction: function (actuator) {
			actuator.to(O2_MIN);
		},
	});
	
	var light = new Controller({
		target: .4,
		sensor: new light({
			controller: "bh1750"
		}),
		read: function (sensor) {
			return sensor.level;
		},
		actuator: new five.Piezo(ALARM_CONTROL_DIGI_PIN),
		positiveAction: function (actuator) {
			// NOOP
		},
		negativeAction: function (actuator) {
			alarm.frequency(262, 20000);
			stopSignal = true;
		},
		stopAction: function (actuator) {
			alarm.frequency(1047, 20000);
		},
	});

  // // analog io expander 
  // var expander = new five.expander({
    // controller: "pcf8591",
    // address: 0x??
  // });

  // // virtual board
  // var virtual = new five.board.virtual({
    // io: expander
  // });
 
  // // sensors
  // // thermometer sensor
  // var temperature = new five.thermometer({
    // controller: "ds18b20",
    // pin: TEMP_SENSOR_DIGI_PIN
  // });
  // temperature.on("data", function() {
    // console.log("celsius: %d", this.c);
    // console.log("fahrenheit: %d", this.f);
    // console.log("kelvin: %d", this.k);
  // });
  

  // // analog light sensor
  // var light = new light({
    // controller: "bh1750",
    // // pin: LIGHT_SENSOR_PIN
  // });
  // light.on("change", () => {
    // console.log("ambient light level: ");
    // console.log("  level  : ", light.level);
    // console.log("-----------------");
  // });

  // // generic analog ph sensor. also for oxygen?
  // // create a new generic sensor instance for
  // // a sensor connected to an analog (adc) pin
  // var ph = new sensor({
    // pin: PH_SENSOR_PIN,
    // // board: virtual
  // });
  // ph.on("change", value => {
    // console.log("sensor: ");
    // console.log("  value  : ", ph.value);
    // console.log("-----------------");
  // });
 
  // // actuators
  // // heater
  // var heater = new five.pin(HEATER_CONTROL_PIN);
  // heater.write(0x01);

  // // pump motor
  // var pump = new motor({
    // pins: {
      // pwm: PHPUMP_CONTROL_PWM_DIGI_PIN,
      // dir: PHPUMP_CONTROL_DIR_DIGI_PIN,
      // cdir: PHPUMP_CONTROL_CDIR_DIGI_PIN
    // }
  // });
  // motor.start();
  // motor.brake();

  // // O2 flow servo
  // var flow = new five.servo(o2_control_digi_pin);
  // flow.to(90);
  
  // // Piezo alarm
  // var alarm = new five.Piezo(ALARM_CONTROL_DIGI_PIN);
  // alarm.frequency(1047, 20000);
});

