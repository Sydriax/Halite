module Halite.Classes
    ( RandomReader(..)
    ) where

-- use type classes to manage side effects for your algorithm function
-- if you want to add your custom side effect just create a new class
-- class Monad m => MyCustomSideEffect m where ...
-- and change type signature of your algorithm to
-- algorithm :: (MyCustomSideEffect m, RandomReader m) => .... -> m [Move]

class Monad m => RandomReader m where
   rand :: Int -> m Int
