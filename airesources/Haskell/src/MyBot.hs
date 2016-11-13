module MyBot
    ( algorithm
    , name
    ) where

import Types
import Classes

name :: String
name = "Haskell Bot"

algorithm :: RandomReader r => Int -> GameMap -> r [Move]
algorithm me gameMap = undefined
