module Halite.Types
  ( Direction(..)
  , Map(..)
  , Site(..)
  , Move(..)
  , ID
  ) where

data Direction
    = Still
    | North
    | East
    | South
    | West
     deriving (Show, Eq, Ord, Enum)

data Site = Site
    { siteOwner :: Int
    , siteStrength :: Int
    , siteProduction :: Int
    , siteX :: Int
    , siteY :: Int
    } deriving (Show, Eq)

data Move = Move
    { moveX :: Int
    , moveY :: Int
    , moveDirection :: Direction
    } deriving (Show, Eq)

data Map = Map
    { mapWidth :: Int
    , mapHeight :: Int
    , mapContents :: [[Site]]
    } deriving (Show, Eq)

type ID = Int
