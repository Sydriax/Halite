#!/bin/bash

if hash stack 2>/dev/null; then
    stack build
    build="$(stack path --dist-dir)/build"
else
    cabal configure
    cabal build
    build="./dist/build"
fi

./halite -d "30 30" "$build/MyBot/MyBot" "$build/RandomBot/RandomBot"
