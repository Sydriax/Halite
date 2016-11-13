module Types
    ( Move (..)
    , Location (..)
    , Direction (..)
    , Site (..)
    , GameMap (..)
    , isMy
    ) where

data Direction = Still | North | South | East | West deriving (Enum, Bounded)
data Location = Location Int Int    -- X Y
data Site = Site Int Int Int        -- Owner Strength Production
data Move = Move Location Direction

data GameMap = GameMap Int Int [Site]

isMy :: Int -> Site -> Bool
isMy me (Site owner _ _) = owner == me

inBounds :: GameMap -> Location -> Bool
inBounds (GameMap width height _) (Location x y) =
   x >= 0 && x < width && y >= 0 && y < height

{-
dist :: Location -> Location -> (Int, Int)
dist (Location x1 y1) (Location x2 y2) = (x1 - x2, y1 - y2)

lift :: (a -> b) -> (a, a) -> (b, b)
lift f (a1, a2) = (f a1, f a2)

ifElse :: a -> (a -> Bool) -> (a -> b) -> (a -> b) -> b
ifElse a f1 f2 f3 = if f1 a then f2 a else f3 a
-}

getDistance :: GameMap -> Location -> Location -> Int
getDistance (GameMap width height _) (Location x1 y1) (Location x2 y2) = dx + dy
   where
      dx' = abs $ x1 - x2
      dy' = abs $ y1 - y2
      dx  = if dx' > width `div` 2 -- WARNING maybe I should use (/) instead of div
               then width - dx'
               else dx'
      dy  = if dy' > height `div` 2 -- ^^
               then height - dy'
               else dy'

getAngle :: GameMap -> Location -> Location -> Double
getAngle (GameMap width height _) (Location x1 y1) (Location x2 y2) =
   atan2 (fromIntegral dy) (fromIntegral dx)
   where
      dx' = x1 - x2
      dy' = y1 - y2
      dx  = if dx' > width - dx'
               then dx' - width
               else if -dx' > width + dx'
               then dx' + width
               else dx'
      dy  = if dy' > width - dy'
               then dy' - width
               else if -dy' > width + dy'
               then dy' + width
               else dy'

getLocation :: GameMap -> Location -> Direction -> Location
getLocation _ l Still = l
getLocation (GameMap width height _) (Location x y) direction = case direction of
   North -> if y == 0
      then Location x (height -1)
      else Location x (y -1)
   East -> if x == width -1
      then Location 0 y
      else Location (x +1) y
   South -> if y == height -1
      then Location x 0
      else Location x (y+1)
   West -> if x == 0
      then Location (width -1) y
      else Location (x -1) y

getSite :: GameMap -> Location -> Direction -> Site
getSite g@(GameMap w _ sites) l d = sites !! (y*w + x) --sites[x][y]
   where
      Location x y = getLocation g l d
