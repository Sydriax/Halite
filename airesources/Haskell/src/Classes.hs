{-# LANGUAGE DeriveAnyClass , DeriveFunctor #-}

module Classes
    ( RandomReader (..)
    , App
    , Test
    , runApp
    , runTest
    ) where

import System.Random

class Monad m => RandomReader m where
   rand  :: m Int
   randR :: Int -> m Int


data App a = App
   { runApp :: StdGen -> (a, StdGen)
   } deriving (Functor, Applicative, Monad)

instance RandomReader App where
   rand    = App random
   randR r = App $ randomR (0, r)

data Test a = Test
   { runTest :: [Int] -> (a, [Int])
   } deriving (Functor, Applicative, Monad)

instance RandomReader Test where
   rand    = Test $ \(i:is) -> (i, is)
   randR r = Test $ \(i:is) -> (mod i r, is)
