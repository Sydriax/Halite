{-# LANGUAGE DeriveFunctor #-}

module Halite.Types
  ( Direction(..)
  , Location(..)
  , Map(..)
  , Site(..)
  , Move(..)
  , ID
  , Effects(runEffects) -- you don't want to export constructor
  , Test(runTest)
  ) where


import Control.Monad (ap)
import System.Random (StdGen, randomR)
import Halite.Classes

data Direction
    = Still
    | North
    | East
    | South
    | West
     deriving (Show, Eq, Ord, Enum)

data Location = Location
   { posX :: Int
   , posY :: Int
   } deriving (Show, Eq)

data Site = Site
    { siteOwner :: Int
    , siteStrength :: Int
    , siteProduction :: Int
    , siteLocation :: Location
    } deriving (Show, Eq)

data Move = Move
    { movelocation :: Location
    , moveDirection :: Direction
    } deriving (Show, Eq)

data Map = Map
    { mapWidth :: Int
    , mapHeight :: Int
    , mapContents :: [[Site]]
    } deriving (Show, Eq)

type ID = Int

-- these two monads allow you to use side effects in your algorithm

data Effects a = Effects {runEffects :: StdGen -> (a, StdGen)}
   deriving (Functor)

instance Applicative Effects where
    pure x = Effects (\y -> (x, y))
    (<*>) = ap

instance Monad Effects where
    m >>= k = Effects $ \s ->
        let (a, s') = runEffects m s
        in runEffects (k a) s'

instance RandomReader Effects where
   rand i = Effects $ randomR (0, i-1)

data Test a = Test {runTest :: [Int] -> (a, [Int])}
   deriving (Functor)

instance Applicative Test where
    pure x = Test (\y -> (x, y))
    (<*>) = ap

instance Monad Test where
    m >>= k = Test $ \s ->
        let (a, s') = runTest m s
        in runTest (k a) s'

instance RandomReader Test where
   rand i = Test $ \(x:xs) -> (mod x i, xs)
