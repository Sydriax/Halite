module Halite.Networking
  ( communicate
  ) where

import Halite.Types
import Data.List (zipWith4)
import System.Random (getStdGen)
import System.IO (hFlush, stdout)

communicate
   :: Monad m
   => String
   -> (ID -> Map -> m [Move])
   -> (m [Move] -> a -> ([Move], a))
   -> a
   -> IO ()
communicate name algorithm runMonad input = do
    (myID, gameMap) <- getInit
    sendInit name
    loop (algorithm myID) gameMap input runMonad

loop algorithm gameMap input runMonad = do
   gameMap' <- getFrame gameMap
   let (moves, input') = runMonad (algorithm gameMap') input
   sendFrame moves
   loop algorithm gameMap' input' runMonad

getInit :: IO (ID, Map)
getInit = do
    userID <- read <$> getLine
    [w, h] <- map read . words <$> getLine
    prod <- map read . words <$> getLine
    mapc <- parseMapContents (w, h) prod <$> getLine
    return (userID, Map w h mapc)

sendInit :: String -> IO ()
sendInit s = putStrLn s >> hFlush stdout

getFrame :: Map -> IO Map
getFrame (Map w h cont) =
    Map w h . parseMapContents (w, h) (getProds cont) <$> getLine

sendFrame' :: [Move] -> IO ()
sendFrame' = putStrLn . unwords . map showMove

sendFrame :: [Move] -> IO ()
sendFrame s = sendFrame' s >> hFlush stdout


parseMapContents :: (Int, Int) -> [Int] -> String -> [[Site]]
parseMapContents (w, h) pds s = splitEvery w $ zipWith4 Site ons sts pds locations
  where
    (owners, sts) = splitMapContents (read <$> words s) (w * h)
    ons = stretch owners
    stretch (a:b:bs) = replicate a b ++ stretch bs
    stretch [] = []
    xs = concat $ replicate h [0..w - 1]
    ys = concatMap (replicate w) [0..h - 1]
    locations = zipWith Location xs ys

splitMapContents :: [Int] -> Int -> ([Int], [Int])
splitMapContents xs area = splitAt (length xs - area) xs

splitEvery :: Int -> [a] -> [[a]]
splitEvery _ [] = []
splitEvery n xs = first : splitEvery n rest
  where
    (first, rest) = splitAt n xs

getProds :: [[Site]] -> [Int]
getProds = map siteProduction . concat

showMove :: Move -> String
showMove (Move (Location x y) d) =
    show x ++ " " ++ show y ++ " " ++ show (fromEnum d)
