defmodule GameMap do
    defstruct [:width, :height, :map]

    def locations(width, height) do
        for y <- 0..height-1, x <- 0..width-1, do: %Location{x: x, y: y}
    end

    def locations(game_map) do
        locations(game_map.width, game_map.height)
    end

    def in_bounds(game_map, location) do
        location.x in 0..game_map.width-1 and
        location.y in 0..game_map.height-1
    end

    def get_distance(game_map, loc_a, loc_b) do
        dx = abs(loc_a.x - loc_b.x)
        dy = abs(loc_a.y - loc_b.y)
        dx = if dx > game_map.width / 2, do: game_map.width - dx, else: dx
        dy = if dy > game_map.height / 2, do: game_map.height - dy, else: dy
        dx + dy
    end

    def get_angle(game_map, loc_a, loc_b) do
        dx = loc_a.x - loc_b.x
        dy = loc_a.y - loc_b.y
        dx = cond do
            dx > game_map.width - dx -> dx - game_map.width
            -dx > game_map.width + dx -> dx + game_map.width
        end
        dy = cond do
            dy > game_map.height - dy -> dy - game_map.height
            -dy > game_map.height + dy -> dy + game_map.height
        end
        :math.atan2(dy, dx)
    end

    defp wrap(a, b) do
        rem(a + b, b)
    end

    
    def get_location(game_map, location, direction)
    #still
    def get_location(game_map, location, 0), do: location
    
    # north
    def get_location(game_map, location, 1) do
        %Location{x: location.x, y: wrap(location.y - 1, game_map.height)}
    end

    # east
    def get_location(game_map, location, 2) do
        %Location{x: wrap(location.x + 1, game_map.width), y: location.y}
    end

    # south
    def get_location(game_map, location, 3) do
        %Location{x: location.x, y: wrap(location.y + 1, game_map.height)}
    end

    # west
    def get_location(game_map, location, 4) do
        %Location{x: wrap(location.x - 1, game_map.width), y: location.y}
    end

    def get_site(game_map, location, direction \\ 0) do
        game_map.map[get_location(game_map, location, direction)]
    end
end