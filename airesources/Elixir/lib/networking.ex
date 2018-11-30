defmodule Networking do
    defp get_string do
        IO.gets("")
        |> String.strip
    end

    defp send_string(string) do
        IO.puts string
    end

    defp int_split(string) do
        String.split(string)
        |> Enum.map(fn(s) ->
            Integer.parse(s)
            |> elem(0)
        end)
    end
    
    defp serialize_move_set(moves) do
        Enum.map(moves, fn({l, d}) ->
            "#{l.x} #{l.y} #{d}"
        end)
        |> Enum.join(" ")
    end

    defp deserialize_game_map_size(string) do
        int_split(string)
        |> List.to_tuple
    end

    defp deserialize_productions(string, width, height) do
        locations = GameMap.locations(width, height)
        productions = int_split(string)
        map = Enum.zip(locations, productions)
        |> Map.new(fn({location, production}) ->
            {location, %Site{production: production}}
        end)
        %GameMap{width: width, height: height, map: map}
    end

    defp deserialize_game_map(string, game_map) do
        split = int_split(string)
        idx = length(split) - game_map.width * game_map.height
        {owners, strengths} = Enum.split(split, idx)
        owners = Enum.chunk(owners, 2)
        |> Enum.flat_map(fn([count, tag]) ->
            List.duplicate(tag, count)
        end)
        locations = GameMap.locations(game_map)
        map = List.zip([locations, owners, strengths])
        |> Enum.reduce(game_map.map, fn({location, owner, strength}, acc) ->
            Map.update!(acc, location, fn(site) ->
                %Site{site | owner: owner, strength: strength}
            end)
        end)
        %GameMap{game_map | map: map}
    end

    def get_init do
        {player_tag, _} = Integer.parse(get_string)
        {width, height} = deserialize_game_map_size(get_string)
        game_map = deserialize_productions(get_string, width, height)
        game_map = deserialize_game_map(get_string, game_map)
        {player_tag, game_map}
    end

    def send_init(name) do
        send_string(name)
    end

    def get_frame(game_map) do
        deserialize_game_map(get_string, game_map)
    end

    def send_frame(moves) do
        send_string(serialize_move_set(moves))
    end
end