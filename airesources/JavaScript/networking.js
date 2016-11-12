'use strict'
const hlt = require('./hlt.js');
const readline = require('readline');
var rl = readline.createInterface({
	input: process.stdin,
	output: process.stdout,
	terminal: false
});
var lines = []
rl.on('line', function(line){
	lines.push(line);
})
var _productions = []
var _width = -1
var _height = -1

function serializeMoveSet(moves) {
	var returnString = "";
	for (var move in move) {
		returnString += move.loc.x.toString() + " " + move.loc.y.toString() + " " + move.direction.toString() + " ";
	}
	return returnString;
}
function deserializeMapSize(inputString) {
	splitString = inputString.split(" ");
	_width = parseInt(splitString.shift());
	_height = parseInt(splitString.shift());
}
function deserializeProductions(inputString) {
	splitString = inputString.split(" ");
	for (var a = 0; a < _height; a++) {
		row = [];
		for (var b = 0; b < _width; b++) {
			row.push(parseInt(splitString.shift()));
		}
		_productions.push(row);
	}
}
function deserializeMap(inputString) {
	splitString = inputString.split(" ");
	m = new hlt.GameMap(_width, _height);
	y = 0;
	x = 0;
	counter = 0;
	owner = 0;
	while (y != m.height) {
		counter = parseInt(splitString.shift());
		owner = parseInt(splitString.shift());
		for (var a = 0; a < counter; a++) {
			m.contents[y][x].owner = owner;
			x+=1;
			if (x==m.width) {
				x=0;
				y+=1;
			}
		}
	}
	for (var a = 0; a < _height; a++) {
		for (var b = 0; b < _width; b++) {
			m.contents[a][b].strength = parseInt(splitString.shift());
			m.contents[a][b].production = _productions[a][b];
		}
	}
	return m;
}
function sendString(toBeSent) {
	toBeSent += '\n';
	process.stdout.write(toBeSent);
}
function getString() {
	while (true) {
		if (lines.len > 0) {
			return lines.shift();
		}
	}
}
/*function hiddenGetInit() { // for sync reasons
	// var playerTag, m;
	// return getString().then(function (a) {
	// 	playerTag = parseInt(a);
	// 	return getString().then(function (a) {
	// 		deserializeMapSize(a);
	// 		return getString().then(function (a) {
	// 			deserializeProductions(a);
	// 			return getString().then(function (a) {
	// 				m = deserializeMap(d);
	// 				return [playerTag, d];
	// 			})
	// 		})
	// 	})
	// })
	var a = getString(),
		b = getString(),
		c = getString(),
		d = getString(),
	playerTag = a,
	a = deserializeMapSize(b),
	b = deserializeProductions(c),
	m = deserializeMap(d);
	return [playerTag, m]
}*/
function getInit() {
	var playerTag = parseInt(getString());
	deserializeMapSize(getString());
	deserializeProductions(getString());
	var m = deserializeMap(getString())
	return [playerTag, m]

	/* // Attempt at a better sync thing
	return new Promise(function(resolve, reject) {
		var playerTag, m;
		resolve(
			getString().then(function (a) {
				playerTag = parseInt(a);
				return getString().then(function (a) {
					deserializeMapSize(a);
					return getString().then(function (a) {
						deserializeProductions(a);
						return getString().then(function (a) {
							m = deserializeMap(d);
							return [playerTag, m];
						})
					})
				})
			}))
	}); */

	//return setTimeout(hiddenGetInit,200);
}
function sendInit(name) {
	sendString(name);
}
function hiddenGetFrame() {
	return getString().then(function (a) {
		return deserializeMap(a);
	});
}
function getFrame() {
	return new Promise(function(resolve, reject) {
		resolve( setTimeout(hiddenGetFrame,200));
	});
}
function sendFrame(moves) {
	sendString(serializeMoveSet(moves));
}
module.exports = {getInit, sendInit, getFrame, sendFrame}
