Blockly.JavaScript['temperature'] = function(block) {
    var value_temperature = Blockly.JavaScript.valueToCode(block, 'temperature', Blockly.JavaScript.ORDER_ATOMIC);
    // TODO: Assemble JavaScript into code variable.
    var code = 'var temperature='+value_temperature+';\n';
    return code;
  };
  
  Blockly.JavaScript['ph'] = function(block) {
    var value_ph = Blockly.JavaScript.valueToCode(block, 'ph', Blockly.JavaScript.ORDER_ATOMIC);
    // TODO: Assemble JavaScript into code variable.
    var code = 'var ph='+value_ph+';\n';
    return code;
  };
  
  Blockly.JavaScript['optical_density'] = function(block) {
    var value_density = Blockly.JavaScript.valueToCode(block, 'opticalDensity', Blockly.JavaScript.ORDER_ATOMIC);
    // TODO: Assemble JavaScript into code variable.
    var code = 'var opticalDensity='+value_density+';\n';
    return code;
  };

  Blockly.JavaScript['shaking'] = function(block) {
    var value_shaking = Blockly.JavaScript.valueToCode(block, 'shaking', Blockly.JavaScript.ORDER_ATOMIC);
    // TODO: Assemble JavaScript into code variable.
    var code = 'var shaking='+value_shaking+';\n';
    return code;
  };

  Blockly.JavaScript['blue_light'] = function(block) {
    var value_blueLight = Blockly.JavaScript.valueToCode(block, 'blueLight', Blockly.JavaScript.ORDER_ATOMIC);
    // TODO: Assemble JavaScript into code variable.
    var code = 'var blueLight='+value_blueLight+';\n';
    return code;
  };

  Blockly.JavaScript['product'] = function(block) {
    var value_product = Blockly.JavaScript.valueToCode(block, 'product', Blockly.JavaScript.ORDER_ATOMIC);
    // TODO: Assemble JavaScript into code variable.
    var code = 'var product='+value_product+';\n';
    return code;
  };