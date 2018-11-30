defmodule Direction do
    def still, do: 0
    def north, do: 1
    def east, do: 2
    def south, do: 3
    def west, do: 4

    def directions, do: 0..4
    def cardinals, do: 1..4
end