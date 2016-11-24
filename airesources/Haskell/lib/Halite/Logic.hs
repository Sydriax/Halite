module Halite.Logic
    ( toLocation
    , distance
    , angle
    , moveToLocation
    , site
    ) where

import Halite.Types

toLocation :: Map -> (Int, Int) -> Location
toLocation (Map width height _) (x, y) =
   Location (mod x width) (mod y height)

distance :: Map -> Location -> Location -> Int
distance (Map width height _) (Location x1 y1) (Location x2 y2) =
   (x2 - x1) `mod` width + (y2 - y1) `mod` height

angle :: Map -> Location -> Location -> Double
angle (Map width height _) (Location x1 y1) (Location x2 y2) =
   atan2 (fromIntegral $ foo dx width) (fromIntegral $ foo dy height)
   where
      dx = x2 - x1
      dy = y2 - y1
      foo a b
           |  a > b - a = a - b
           | -a > b + a = a + b
           | otherwise  = a

moveToLocation :: Map -> Move -> Location
moveToLocation _ (Move location Still) = location
moveToLocation (Map width height _) (Move (Location x y) dir) = case dir of
   North -> if y == 0
      then Location x (height -1)
      else Location x (y +1)
   South -> if y == height -1
      then Location x 0
      else Location x (y -1)
   West  -> if x == 0
      then Location (width -1) y
      else Location (x +1)     y
   East  -> if x == width -1
      then Location 0      y
      else Location (x +1) y

site :: Map -> Location -> Site
site (Map _ _ sites) (Location x y) = sites !! y !! x
