#!/bin/sh
swiftc -O Model.swift Networking.swift main.swift -o MyBot
./halite -d "30 30" "./MyBot" "./MyBot"
