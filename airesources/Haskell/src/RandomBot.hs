module RandomBot
    ( algorithm
    , name
    ) where

import Types
import Classes

name :: String
name = "Random Haskell Bot"

algorithm :: RandomReader r => Int -> GameMap -> r [Move]
algorithm me g@(GameMap width height sites) =
   randomMoves
      [Location x y | x <- [1..width], y <- [1..height]] $
      fmap (isMy me) sites

randomMoves :: RandomReader r => [Location] -> [Bool] -> r [Move]
randomMoves l = traverse randMove . filter' l
   where
      filter' :: [a] -> [Bool] -> [a]
      filter' [] [] = []
      filter' (a:as) (True:bs)  = a : filter' as bs
      filter' (_:as) (False:bs) = filter' as bs
      randMove :: RandomReader r => Location -> r Move
      randMove location = do
         direction <- toEnum <$> randR 5
         return $ Move location direction
