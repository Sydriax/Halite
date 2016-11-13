module Networking
    ( getTag
    , getMap
    , updateMap
    , sendMoves
    , sendName
    ) where

import Types
import Data.List (intersperse)

showMoves :: [Move] -> String
showMoves = unwords . fmap showMove
   where
      showMove (Move (Location x y) direction) = unwords $ show <$>
         [x, y, case direction of
            Still -> 0
            North -> 1
            East  -> 2
            South -> 3
            West  -> 4
         ]


readOwnerStrength :: Int -> Int -> String -> ([Int], [Int])
readOwnerStrength width height s = splitAt size (expand size data')
   where
      size  = width * height
      data' = read <$> words s
      expand 0 rest   = rest
      expand n (c:o:rest) = replicate c o ++ expand (n -c) rest


sendString :: String -> IO ()
sendString s = do
   putStrLn s
   -- flush

getString :: IO String
getString = readLn

getMap :: IO  GameMap
getMap = do
   [width, height]      <- fmap read . words                <$> getString
   productions          <- fmap read . words                <$> getString
   (owners, strengths)  <- readOwnerStrength height width   <$> getString
   return $ GameMap width height  (zipWith3 Site owners strengths productions)

getTag :: IO Int
getTag = read <$> getString

sendName = sendString

sendMoves :: [Move] -> IO ()
sendMoves = sendString . showMoves

updateMap :: GameMap -> IO GameMap
updateMap (GameMap w h sites) = do
   (owners, strengths) <- readOwnerStrength w h <$> getString
   return $ GameMap w h  (update owners strengths sites)
   where
      update [] [] [] = []
      update (o:os) (s:ss) (Site _ _ p:ss') = Site o s p : update os ss ss'
