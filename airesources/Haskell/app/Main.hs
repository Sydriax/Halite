module Main where

import Classes (App, Test, runApp, runTest)
import System.Random (getStdGen)
import MyBot (algorithm, name)
import Networking
import Types

main :: IO ()
main = do
   tag      <- getTag
   sendName name
   gameMap  <- getMap
   loop gameMap (algorithm tag)

loop ::  GameMap -> (GameMap -> App [Move]) -> IO ()
loop g fn = do
   g' <- updateMap g
   (moves, _) <- runApp (fn g') <$> getStdGen
   sendMoves moves
   loop g' fn
