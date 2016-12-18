#!/bin/sh
swiftc -O ./Sources/Halite/Model.swift ./Sources/Halite/Networking.swift Sources/MyBot/main.swift -o MyBot
swiftc -O ./Sources/Halite/Model.swift ./Sources/Halite/Networking.swift Sources/RandomBot/main.swift -o RandomBot
./halite -d "30 30" "./MyBot" "./RandomBot"
