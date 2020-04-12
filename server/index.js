"use strict" 
const express = require('express');
const bodyParser = require('body-parser');
const io = require('socket.io');
const compress = require('compression');
const http = require('http');

const PORT = 8080;

// App
var app = express();

app.use(compress());  

app.use(express.static(__dirname + '/public/bioreactor-monitor/dist/bioreactor-monitor'));

app.use(bodyParser.json());

app = http.createServer(app);

app.listen(PORT, function() {
  console.log('server up and running at %s port at https://localhost', PORT);
  console.log('version is %s', process.version);
});