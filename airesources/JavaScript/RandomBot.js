// Basic bot
'use strict'
const fs = require('fs');
const hlt = require('./hlt');
const networking = require('./networking');
var myID = setTimeout(networking.getInit,200)[0],
	a = networking.sendInit("NodeJS Random Bot");

while (true) {
	var moves = [];
	networking.getFrame().then(function (gameMap) {
		for (var y = 0; y < gameMap.height; y++) {
			for (var x = 0; x < gameMap.width; x++) {
				if (gameMap.getSite(new hlt.Location(x,y)).owner == myID) {
					moves.append(new hlt.Move(new hlt.Location(x,y), (Math.floor(Math.random()*5))))
				}
			}
		}
		if (moves.len < 1) {
			console.log("NO MOVES")
		} else {
			networking.sendFrame(moves)
		}
	});


}
