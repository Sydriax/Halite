defmodule RandomBot do   
    defp loop(id, game_map) do
        GameMap.locations(game_map)
        |> Enum.filter(&(GameMap.get_site(game_map, &1).owner == id))
        |> Enum.map(fn(location) -> {location, Enum.random(Direction.directions)} end)
        |> Networking.send_frame
        loop(id, Networking.get_frame(game_map))
    end

    def main do
        {id, game_map} = Networking.get_init
        Networking.send_init "ElixirBot"
        loop(id, Networking.get_frame(game_map))
    end
end