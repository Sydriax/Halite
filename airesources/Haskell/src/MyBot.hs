module Main where

import Halite
import System.Random (randomRs, getStdGen, StdGen)

main :: IO ()
main = do
    (myID, gameMap) <- getInit
    sendInit "MyHaskellBot"
    dirs <- randomDirections <$> getStdGen
    loop myID gameMap dirs

loop :: ID -> Map -> [Direction] -> IO ()
loop myID gameMap dirs = do
    gameMap' <- getFrame gameMap
    let sites = concat $ mapContents gameMap'
        moves = assignMoves myID sites dirs
        dirs' = drop (length moves) dirs
    sendFrame moves
    loop myID gameMap' dirs'

assignMoves :: ID -> [Site] -> [Direction] -> [Move]
assignMoves myID sites = zipWith3 Move (map siteX sites') (map siteY sites')
  where
    sites' = filter (\s -> siteOwner s == myID) sites

randomDirections :: StdGen -> [Direction]
randomDirections g = toEnum <$> randomRs (0, 4) g
