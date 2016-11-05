// HLT
'use strict'
var STILL = 0;
var NORTH = 1;
var EAST = 2;
var SOUTH = 3;
var WEST = 4;

var DIRECTIONS = [0,1,2,3,4];
var CARDINALS = [1,2,3,4];

var ATTACK = 0;
var STOP_ATTACK = 1;

class Location {
	constructor(x,y) {
		if (typeof(x)==='undefined') x = 0;
		if (typeof(y)==='undefined') y = 0;
		this.x = x;
		this.y = y;
	}
}
class Site {
	constructor(owner, strength, production) {
		if (typeof(owner)==='undefined') owner = 0;
		if (typeof(strength)==='undefined') strength = 0;
		if (typeof(production)==='undefined') production = 0;
		this.owner = owner;
		this.strength = strength;
		this.production = production;
	}
}
class Move{
	constructor(loc, direction) {
		if (typeof(loc)==='undefined') loc = 0;
		if (typeof(direction)==='undefined') direction = 0;
		this.loc = loc;
		this.direction = direction;
	}
}
class GameMap {
	constructor(width,height,numberOfPlayers) {
		if (typeof(width)==='undefined') width = 0;
		if (typeof(height)==='undefined') height = 0;
		if (typeof(numberOfPlayers)==='undefined') numberOfPlayers = 0;
		this.width = width;
		this.height = height;
		this.contents = [];

		for (var y = 0; y < this.height; y++) {
			row = [];
			for (var x = 0; x < this.width; x++) {
				row.push(new Site(0,0,0));
			}
			this.contents.push(row);
		}
	}
	inBounds(l) {
		return (l.x >= 0) && (l.x < this.width) && (l.y >= 0) && (l.y < this.height);
	}
	getDistance(l1,l2) {
		dx = Math.abs(l1.x-l2.x);
		dy = Math.abs(l1.y-l2.y);
		if (dx > this.width/2) {
			dx = this.witdh - dx;
		}
		if (dy > this.height/2) {
			dy = this.height - dy;
		}
		return dx+dy;
	}
	getAngle(l1,l2) {
		dx = l2.x - l1.x;
		dy = l2.y - l1.y;

		if (dx > this.width - dx) {
			dx -= this.width;
		} else if (-dx > this.width + dx) {
			dx += this.width;
		}
		if (dy > this.height - dy){
			dy -= this.height;
		} else if (-dy > this.height + dy) {
			dy += this.height;
		}
		return Math.atan2(dy, dx);
	}
	getLocation(loc, direction) {
		switch (direction) {
			case NORTH:
				if (l.y == 0) {
					l.y = this.height-1;
				} else {
					l.y -= 1;
				}
				break;
			case EAST:
				if (l.x == this.width-1) {
					l.x = 0;
				} else {
					l.x += 1;
				}
				break;
			case SOUTH:
				if (l.y == this.height-1) {
					l.y = 0;
				} else {
					l.y += 1;
				}
				break;
			case WEST:
				if (l.x == 0) {
					l.x = this.width-1;
				} else {
					l.x -= 1;
				}
				break;
			case STILL:
			default:
				break;
		}
		return loc;
	}
	getSite(l, direction) {
		if (typeof(direction)==='undefined') direction = STILL;
		l = this.getLocation(l, direction);
		return this.contents[l.y][l.x];
	}
}

module.exports = {STILL, NORTH, SOUTH, EAST, WEST, DIRECTIONS, CARDINALS, ATTACK, STOP_ATTACK, Location, Site, Move, GameMap}
