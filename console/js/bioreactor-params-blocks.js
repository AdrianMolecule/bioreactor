Blockly.Blocks['temperature'] = {
  init: function() {
    this.appendValueInput("temperature")
        .setCheck("Number")
        .appendField(new Blockly.FieldLabelSerializable("temperature"), "temperature");
    this.setColour(20);
 this.setTooltip("Temperature");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['ph'] = {
  init: function() {
    this.appendValueInput("ph")
        .setCheck("Number")
        .appendField(new Blockly.FieldLabelSerializable("pH"), "ph");
    this.setColour(65);
 this.setTooltip("Target pH");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['optical_density'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("Number")
        .appendField(new Blockly.FieldLabelSerializable("Optical Density"), "opticalDensity");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['shaking'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("Number")
        .appendField(new Blockly.FieldLabelSerializable("Shaking"), "shaking");
    this.setColour(180);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['blue_light'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("Number")
        .appendField(new Blockly.FieldLabelSerializable("Blue Light"), "blueLight");
    this.setColour(180);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['product'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("Number")
        .appendField(new Blockly.FieldLabelSerializable("Product"), "product");
    this.setColour(180);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};