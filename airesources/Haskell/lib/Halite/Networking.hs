module Halite.Networking
  ( getInit
  , sendInit
  , getFrame
  , sendFrame
  ) where

import Halite.Types
import Data.List (zipWith5)
import System.IO (hFlush, stdout)

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
parseMapContents (w, h) pds s = splitEvery w $ zipWith5 Site ons sts pds xs ys
  where
    (owners, sts) = splitMapContents (read <$> words s) (w * h)
    ons = stretch owners
    stretch (a:b:bs) = replicate a b ++ stretch bs
    stretch [] = []
    xs = concat $ replicate h [0..w - 1]
    ys = concatMap (replicate w) [0..h - 1]

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
showMove (Move x y d) =
    show x ++ " " ++ show y ++ " " ++ show (fromEnum d)
