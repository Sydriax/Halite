#include "Halite.h"

using boost::asio::ip::tcp;

//Consts -----------------------------

//Default port number.
const unsigned short DEFAULT_PORT = 2000;

//Graph constants:
const float TERRITORY_GRAPH_TOP = 0.98, TERRITORY_GRAPH_BOTTOM = 0.01, TERRITORY_GRAPH_LEFT = 0.51, TERRITORY_GRAPH_RIGHT = 0.98;
const float STRENGTH_GRAPH_TOP = -0.01, STRENGTH_GRAPH_BOTTOM = -0.98, STRENGTH_GRAPH_LEFT = 0.51, STRENGTH_GRAPH_RIGHT = 0.98;

//Map constants:
const float MAP_TOP = 0.98, MAP_BOTTOM = -0.98, MAP_LEFT = -0.98, MAP_RIGHT = 0.49;

//Private Functions ------------------

void Halite::loadColorCodes()
{
	std::fstream colorFile;
	colorFile.open("ColorCodes.txt", std::ios_base::in);
	color_codes.clear();
	int n; float r, g, b;
	while(!colorFile.eof())
	{
		colorFile >> n >> r >> g >> b;
		color_codes.insert(std::pair<unsigned char, hlt::Color>(unsigned char(n), { r, g, b }));
	}
	colorFile.close();
}

void Halite::setupMapRendering(unsigned short width, unsigned short height)
{
	//Delete buffers and vaos
	glDeleteBuffers(1, &map_vertex_buffer);
	glDeleteBuffers(1, &map_color_buffer);
	glDeleteBuffers(1, &map_strength_buffer);
	glDeleteVertexArrays(1, &map_vertex_attributes);
	//Ensure that shaders are deleted:
	glDeleteShader(map_vertex_shader);
	glDeleteShader(map_geometry_shader);
	glDeleteShader(map_fragment_shader);
	glDeleteProgram(map_shader_program);
	//Generate buffers and vaos.
	glGenBuffers(1, &map_vertex_buffer);
	glGenBuffers(1, &map_color_buffer);
	glGenBuffers(1, &map_strength_buffer);
	glGenVertexArrays(1, &map_vertex_attributes);

	//Generate vertices of centers of squares.
	std::vector<float> vertexLocations(unsigned int(width) * height * 2); //2 because there are x and y values for every vertex.
	float xLoc = MAP_LEFT + (MAP_RIGHT - MAP_LEFT) / (2*width), yLoc = MAP_TOP - (MAP_TOP - MAP_BOTTOM) / (2 * height), dX = (MAP_RIGHT - MAP_LEFT) / width, dY = (MAP_TOP - MAP_BOTTOM) / height;
	for(unsigned int a = 0; a < vertexLocations.size(); a += 2)
	{
		vertexLocations[a] = xLoc;
		vertexLocations[a + 1] = yLoc;

		xLoc += dX;
		if(xLoc > MAP_RIGHT)
		{
			xLoc = MAP_LEFT + (MAP_RIGHT - MAP_LEFT) / (2 * width);
			yLoc -= dY;
		}
	}

	//Bind vertex attribute object.
	glBindVertexArray(map_vertex_attributes);

	//Setup vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, map_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertexLocations.size() * sizeof(float), vertexLocations.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Create vector of floats (0.0) to reserve the memory for the color buffer and allow us to set the mode to GL_DYNAMIC_DRAW.
	std::vector<float> colors(unsigned int(width) * height * 3); //r, g, and b components.

	//Setup color buffer
	glBindBuffer(GL_ARRAY_BUFFER, map_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//Create vector of unsigned ints (0) to reserve the memory for the strength buffer and allow us to set the mode to GL_DYNAMIC_DRAW.
	std::vector<unsigned int> strengths(unsigned int(width) * height, 0); //r, g, and b components.

	//Setup strength buffer
	glBindBuffer(GL_ARRAY_BUFFER, map_strength_buffer);
	glBufferData(GL_ARRAY_BUFFER, strengths.size() * sizeof(GL_UNSIGNED_INT), strengths.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 0, NULL);

	//Setup shaders:
	map_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	shaderFromFile(map_vertex_shader, "Classes/shaders/mapvertexshader.glsl", "map_vertex_shader");
	map_geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	shaderFromFile(map_geometry_shader, "Classes/shaders/mapgeometryshader.glsl", "map_geometry_shader");
	map_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFromFile(map_fragment_shader, "Classes/shaders/mapfragmentshader.glsl", "map_fragment_shader");

	//Setup shader program:
	map_shader_program = glCreateProgram();
	glAttachShader(map_shader_program, map_vertex_shader);
	glAttachShader(map_shader_program, map_geometry_shader);
	glAttachShader(map_shader_program, map_fragment_shader);
	glLinkProgram(map_shader_program);
	glDetachShader(map_shader_program, map_vertex_shader);
	glDetachShader(map_shader_program, map_geometry_shader);
	glDetachShader(map_shader_program, map_fragment_shader);

	//Set uniforms:
	glUseProgram(map_shader_program);
	const float SPACE_FACTOR = 0.8;
	GLint widthLoc = glGetUniformLocation(map_shader_program, "width"), heightLoc = glGetUniformLocation(map_shader_program, "height");
	glUniform1f(widthLoc, dX * SPACE_FACTOR * 0.5);
	glUniform1f(heightLoc, dY * SPACE_FACTOR * 0.5);

	//Cleanup - delete shaders
	glDeleteShader(map_vertex_shader);
	glDeleteShader(map_geometry_shader);
	glDeleteShader(map_fragment_shader);
}

void Halite::setupGraphRendering()
{
	//Delete buffers and vaos
	glDeleteBuffers(1, &graph_territory_vertex_buffer);
	glDeleteBuffers(1, &graph_strength_vertex_buffer);
	glDeleteBuffers(1, &graph_color_buffer);
	glDeleteBuffers(1, &graph_border_buffer);
	glDeleteVertexArrays(1, &graph_territory_vertex_attributes);
	glDeleteVertexArrays(1, &graph_strength_vertex_attributes);
	//Ensure that shaders are deleted:
	glDeleteShader(graph_vertex_shader);
	glDeleteShader(graph_fragment_shader);
	glDeleteProgram(graph_shader_program);
	//Generate buffers and vaos.
	glGenBuffers(1, &graph_territory_vertex_buffer);
	glGenBuffers(1, &graph_strength_vertex_buffer);
	glGenBuffers(1, &graph_color_buffer);
	glGenBuffers(1, &graph_border_buffer);
	glGenVertexArrays(1, &graph_territory_vertex_attributes);
	glGenVertexArrays(1, &graph_strength_vertex_attributes);
	glGenVertexArrays(1, &graph_border_vertex_attributes);

	//Set the number of frames the graph will handle. Also prevents race conditions with full_game by not using iterators, but rather up to a numeric frame.
	graph_frame_number = full_game.size();

//Setup graph border:

	//Bind vertex attribute object.
	glBindVertexArray(graph_border_vertex_attributes);

	//Floats representing contents of the buffer.
	std::vector<float> graphBorderBufferValues(95);

	//First 8 floats represent position vertices in game. Their values are undefined for now, since they're set every frame. Next 30 floats represent actual border. Next 12 floats represent colors of position vertices, and final 45 floats represent colors of border vertices.

	//Create territory borders:
	graphBorderBufferValues[8] = TERRITORY_GRAPH_LEFT; graphBorderBufferValues[9] = TERRITORY_GRAPH_TOP; graphBorderBufferValues[10] = TERRITORY_GRAPH_LEFT; graphBorderBufferValues[11] = TERRITORY_GRAPH_BOTTOM; graphBorderBufferValues[12] = TERRITORY_GRAPH_RIGHT; graphBorderBufferValues[13] = TERRITORY_GRAPH_BOTTOM; graphBorderBufferValues[14] = TERRITORY_GRAPH_RIGHT; graphBorderBufferValues[15] = TERRITORY_GRAPH_TOP; graphBorderBufferValues[16] = TERRITORY_GRAPH_LEFT; graphBorderBufferValues[17] = TERRITORY_GRAPH_TOP;

	//Create strength borders:
	graphBorderBufferValues[18] = STRENGTH_GRAPH_LEFT; graphBorderBufferValues[19] = STRENGTH_GRAPH_TOP; graphBorderBufferValues[20] = STRENGTH_GRAPH_LEFT; graphBorderBufferValues[21] = STRENGTH_GRAPH_BOTTOM; graphBorderBufferValues[22] = STRENGTH_GRAPH_RIGHT; graphBorderBufferValues[23] = STRENGTH_GRAPH_BOTTOM; graphBorderBufferValues[24] = STRENGTH_GRAPH_RIGHT; graphBorderBufferValues[25] = STRENGTH_GRAPH_TOP; graphBorderBufferValues[26] = STRENGTH_GRAPH_LEFT; graphBorderBufferValues[27] = STRENGTH_GRAPH_TOP;

	//Create map borders:
	graphBorderBufferValues[28] = MAP_LEFT; graphBorderBufferValues[29] = MAP_TOP; graphBorderBufferValues[30] = MAP_LEFT; graphBorderBufferValues[31] = MAP_BOTTOM; graphBorderBufferValues[32] = MAP_RIGHT; graphBorderBufferValues[33] = MAP_BOTTOM; graphBorderBufferValues[34] = MAP_RIGHT; graphBorderBufferValues[35] = MAP_TOP; graphBorderBufferValues[36] = MAP_LEFT; graphBorderBufferValues[37] = MAP_TOP;

	for(unsigned short a = 38; a < 95; a++) graphBorderBufferValues[a] = 1.0;

	//Bind graph border buffer
	glBindBuffer(GL_ARRAY_BUFFER, graph_border_buffer);
	glBufferData(GL_ARRAY_BUFFER, graphBorderBufferValues.size()*sizeof(float), graphBorderBufferValues.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(38*sizeof(float)));

//Setup territory graph:

	//Bind vertex attribute object.
	glBindVertexArray(graph_territory_vertex_attributes);

	//Find the greatest territory_count existent.
	unsigned int maxTerritoryValue = 0;
	for(unsigned char a = 0; a < number_of_players; a++) for(unsigned short b = 0; b < graph_frame_number; b++) if(full_game[b]->territory_count.size() > a && full_game[b]->territory_count[a] > maxTerritoryValue) maxTerritoryValue = full_game[b]->territory_count[a];

	//Create vector of graph vertices.
	std::vector<float> graphTerritoryVertices(unsigned int(number_of_players) * graph_frame_number * 2);

	//Set vertices by player:
	unsigned int graphTerritoryVerticesLoc = 0; //Location in graphTerritoryVertices.
	for(unsigned char a = 0; a < number_of_players; a++) for(unsigned short b = 0; b < graph_frame_number; b++)
	{
		graphTerritoryVertices[graphTerritoryVerticesLoc] = (float(b) / (graph_frame_number - 1)) * (TERRITORY_GRAPH_RIGHT - TERRITORY_GRAPH_LEFT) + TERRITORY_GRAPH_LEFT;
		if(full_game[b]->territory_count.size() > a) graphTerritoryVertices[graphTerritoryVerticesLoc + 1] = (1 - (float(full_game[b]->territory_count[a]) / maxTerritoryValue)) * (TERRITORY_GRAPH_BOTTOM - TERRITORY_GRAPH_TOP) + TERRITORY_GRAPH_TOP;
		else graphTerritoryVertices[graphTerritoryVerticesLoc + 1] = TERRITORY_GRAPH_BOTTOM;
		graphTerritoryVerticesLoc += 2;
	}

	//Set vertices in buffer object
	glBindBuffer(GL_ARRAY_BUFFER, graph_territory_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, graphTerritoryVertices.size() * sizeof(float), graphTerritoryVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Create vector representing color data:
	std::vector<float> graphColors(unsigned int(number_of_players) * graph_frame_number * 3);

	//Set color data:
	unsigned int graphColorsLoc = 0; //Location in graphColors.
	for(unsigned char a = 0; a < number_of_players; a++)
	{
		hlt::Color c = color_codes[a + 1];
		for(unsigned short b = 0; b < graph_frame_number; b++)
		{
			graphColors[graphColorsLoc] = c.r;
			graphColors[graphColorsLoc + 1] = c.g;
			graphColors[graphColorsLoc + 2] = c.b;
			graphColorsLoc += 3;
		}
	}

	//Set colors in buffer object
	glBindBuffer(GL_ARRAY_BUFFER, graph_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, graphColors.size()*sizeof(float), graphColors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

//Setup strength graph:

	//Bind Vertex Array:
	glBindVertexArray(graph_strength_vertex_attributes);

	//Find the greatest strength_count existent.
	unsigned int maxStrengthValue = 0;
	for(unsigned char a = 0; a < number_of_players; a++) for(unsigned short b = 0; b < graph_frame_number; b++) if(full_game[b]->strength_count.size() > a && full_game[b]->strength_count[a] > maxStrengthValue) maxStrengthValue = full_game[b]->strength_count[a];

	//Create vector of graph vertices.
	std::vector<float> graphStrengthVertices(unsigned int(number_of_players) * graph_frame_number * 2);

	//Set vertices by player:
	unsigned int graphStrengthVerticesLoc = 0; //Location in graphStrengthVertices.
	for(unsigned char a = 0; a < number_of_players; a++) for(unsigned short b = 0; b < graph_frame_number; b++)
	{
		graphStrengthVertices[graphStrengthVerticesLoc] = (float(b) / (graph_frame_number - 1)) * (STRENGTH_GRAPH_RIGHT - STRENGTH_GRAPH_LEFT) + STRENGTH_GRAPH_LEFT;
		if(full_game[b]->strength_count.size() > a) graphStrengthVertices[graphStrengthVerticesLoc + 1] = (1 - (float(full_game[b]->strength_count[a]) / maxStrengthValue)) * (STRENGTH_GRAPH_BOTTOM - STRENGTH_GRAPH_TOP) + STRENGTH_GRAPH_TOP;
		else graphStrengthVertices[graphStrengthVerticesLoc + 1] = STRENGTH_GRAPH_BOTTOM;
		graphStrengthVerticesLoc += 2;
	}

	//Set vertices in buffer object
	glBindBuffer(GL_ARRAY_BUFFER, graph_strength_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, graphStrengthVertices.size() * sizeof(float), graphStrengthVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Add in color buffer as well:
	glBindBuffer(GL_ARRAY_BUFFER, graph_color_buffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//Setup shaders:
	graph_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	shaderFromFile(graph_vertex_shader, "Classes/shaders/graphvertexshader.glsl", "graph_vertex_shader");
	graph_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFromFile(graph_fragment_shader, "Classes/shaders/graphfragmentshader.glsl", "graph_fragment_shader");

	//Setup shader program:
	graph_shader_program = glCreateProgram();
	glAttachShader(graph_shader_program, graph_vertex_shader);
	glAttachShader(graph_shader_program, graph_fragment_shader);
	glLinkProgram(graph_shader_program);
	glDetachShader(graph_shader_program, graph_vertex_shader);
	glDetachShader(graph_shader_program, graph_fragment_shader);

	//Cleanup - delete shaders
	glDeleteShader(graph_vertex_shader);
	glDeleteShader(graph_fragment_shader);
}

void Halite::clearFullGame()
{
	for(auto a = full_game.begin(); a != full_game.end(); a++) delete *a;
	full_game.clear();
}

std::vector<bool> Halite::getNextFrame(std::vector<bool> alive)
{
	if(game_map.map_width == 0 || game_map.map_height == 0) return std::vector<bool>(0);

	//Create threads to send/receive data to/from players. The threads should return a float of how much time passed between the end of their message being sent and the end of the AI's message being sent.
	std::vector< std::future<double> > frameThreads(std::count(alive.begin(), alive.end(), true));
	unsigned char threadLocation = 0; //Represents place in frameThreads.
	for(unsigned char a = 0; a < number_of_players; a++)
	{
		if(alive[a])
		{
			frameThreads[threadLocation] = std::async(handleFrameNetworking, player_connections[a], game_map, &player_moves[a]);
			threadLocation++;
		}
	}

	//Figure out how long each AI is permitted to respond.
	std::vector<double> allowableTimesToRespond(number_of_players);
	for(unsigned char a = 0; a < number_of_players; a++) allowableTimesToRespond[a] = FLT_MAX;
	//For the time being we'll allow infinte time (debugging purposes), but eventually this will come into use):
	//allowableTimesToRespond[a] = 0.2 + (double(game_map.map_height)*game_map.map_width*.0001) + (double(game_map.territory_count[a]) * game_map.territory_count[a] * .001);

	std::vector< std::map<hlt::Location, unsigned char> > pieces(number_of_players + 1);

	//Join threads. Figure out if the player responded in an allowable amount of time.
	std::vector<bool> permissibleTime(number_of_players, false);
	threadLocation = 0; //Represents place in frameThreads.
	for(unsigned char a = 0; a < number_of_players; a++)
	{
		if(alive[a])
		{
			permissibleTime[a] = frameThreads[threadLocation].get() <= allowableTimesToRespond[a];
			threadLocation++;
		}
	}

	//For each player, use their moves to create the pieces map.
	for(unsigned char a = 0; a < number_of_players; a++) if(alive[a])
	{
		//Add in pieces according to their moves. Also add in a second piece corresponding to the piece left behind.
		for(auto b = player_moves[a].begin(); b != player_moves[a].end(); b++) if(game_map.getSite(b->loc, STILL).owner == a + 1)
		{
			if(b->dir == STILL && game_map.getSite(b->loc, STILL).strength != 255) game_map.getSite(b->loc, STILL).strength++;
			hlt::Location newLoc = game_map.getLocation(b->loc, b->dir);
			if(pieces[a + 1].count(newLoc))
			{
				if(short(pieces[a + 1][newLoc]) + game_map.getSite(b->loc, STILL).strength <= 255) pieces[a + 1][newLoc] += game_map.getSite(b->loc, STILL).strength;
				else pieces[a + 1][newLoc] = 255;
			}
			else
			{
				pieces[a + 1].insert(std::pair<hlt::Location, unsigned char>(newLoc, game_map.getSite(b->loc, STILL).strength));
			}

			//Add in a new piece with a strength of 0 if necessary.
			if(!pieces[a + 1].count(b->loc))
			{
				pieces[a + 1].insert(std::pair<hlt::Location, unsigned char>(b->loc, 0));
			}

			//Erase from the game map so that the player can't make another move with the same piece. Essentially, I need another number which will never be in use, and there is unlikely to ever be 255 players, so I'm utilizing 255 to ensure that there aren't problems. This also means that one can have at most 254 players, but that is really not that dissimilar from having 255 players, and would be unbearably slow, so I'm willing to sacrifice that for simplicity.
			game_map.getSite(b->loc, STILL) = { 255, 0 };
		}
	}

	//Add in all of the remaining pieces whose moves weren't specified. 
	for(unsigned short a = 0; a < game_map.map_height; a++) for(unsigned short b = 0; b < game_map.map_width; b++)
	{
		hlt::Location l = { b, a };
		if(game_map.getSite(l, STILL).strength != 255 && !(game_map.getSite(l, STILL).strength == 1 && game_map.getSite(l, STILL).owner == 0)) game_map.getSite(l, STILL).strength++;
		hlt::Site s = game_map.getSite(l, STILL);
		if(s.owner != 255)
		{
			if(pieces[s.owner].count(l))
			{
				if(short(pieces[s.owner][l]) + s.strength <= 255) pieces[s.owner][l] += s.strength;
				else pieces[s.owner][l] = 255;
			}
			else
			{
				pieces[s.owner].insert(std::pair<hlt::Location, unsigned char>(l, s.strength));
			}
		}
	}

	std::vector< std::map<hlt::Location, unsigned short> > toInjure(number_of_players + 1); //This is a short so that we don't have to worry about 255 overflows.

	//Sweep through locations and find the correct damage for each piece.
	for(unsigned char a = 0; a != game_map.map_height; a++) for(unsigned short b = 0; b < game_map.map_width; b++)
	{
		hlt::Location l = { b, a };
		for(unsigned short c = 0; c < number_of_players + 1; c++) if((c == 0 || alive[c - 1]) && pieces[c].count(l))
		{
			for(unsigned short d = 0; d < number_of_players + 1; d++) if(d != c && (d == 0 || alive[d-1]))
			{
				hlt::Location tempLoc = l;
				//Check 'STILL' square:
				if(pieces[d].count(tempLoc))
				{
					//Add to damage total, but only if it's not the null player:
					if(c != 0) attack_count[c - 1] += pieces[d][tempLoc] > pieces[c][l] ? pieces[c][l] : pieces[d][tempLoc];
					//Apply damage, but not more than they have strength:
					if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += pieces[c][l];
					else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, pieces[c][l]));
				}
				//Only resolve adjacent squares if both players involved are not the NULL player.
				if(c != 0 && d != 0)
				{
					//Check 'NORTH' square:
					tempLoc = game_map.getLocation(l, NORTH);
					if(pieces[d].count(tempLoc))
					{
						//Add to damage total:
						attack_count[c - 1] += pieces[d][tempLoc] > pieces[c][l] ? pieces[c][l] : pieces[d][tempLoc];
						//Apply damage, but not more than they have strength:
						if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += pieces[c][l];
						else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, pieces[c][l]));
					}
					//Check 'EAST' square:
					tempLoc = game_map.getLocation(l, EAST);
					if(pieces[d].count(tempLoc))
					{
						//Add to damage total:
						attack_count[c - 1] += pieces[d][tempLoc] > pieces[c][l] ? pieces[c][l] : pieces[d][tempLoc];
						//Apply damage, but not more than they have strength:
						if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += pieces[c][l];
						else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, pieces[c][l]));
					}
					//Check 'SOUTH' square:
					tempLoc = game_map.getLocation(l, SOUTH);
					if(pieces[d].count(tempLoc))
					{
						//Add to damage total:
						attack_count[c - 1] += pieces[d][tempLoc] > pieces[c][l] ? pieces[c][l] : pieces[d][tempLoc];
						//Apply damage, but not more than they have strength:
						if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += pieces[c][l];
						else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, pieces[c][l]));
					}
					//Check 'WEST' square:
					tempLoc = game_map.getLocation(l, WEST);
					if(pieces[d].count(tempLoc))
					{
						//Add to damage total:
						attack_count[c - 1] += pieces[d][tempLoc] > pieces[c][l] ? pieces[c][l] : pieces[d][tempLoc];
						//Apply damage, but not more than they have strength:
						if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += pieces[c][l];
						else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, pieces[c][l]));
					}
				}
			}
		}
	}

	//Injure and/or delete pieces. Note >= rather than > indicates that pieces with a strength of 0 are killed.
	for(unsigned char a = 0; a < number_of_players + 1; a++) if(a == 0 || alive[a - 1])
	{
		for(auto b = toInjure[a].begin(); b != toInjure[a].end(); b++)
		{
			if(a != 0) attack_count[a - 1] += b->second;
			if(b->second >= pieces[a][b->first]) pieces[a].erase(b->first);
			else pieces[a][b->first] -= b->second;
		}
	}

	//Clear the map (everything to {0, 1})
	for(auto a = game_map.contents.begin(); a != game_map.contents.end(); a++) for(auto b = a->begin(); b != a->end(); b++) *b = { 0, 1 };

	//Add pieces back into the map.
	for(unsigned char a = 0; a < number_of_players + 1; a++)
	{
		for(auto b = pieces[a].begin(); b != pieces[a].end(); b++)
		{
			game_map.getSite(b->first, STILL) = { a, b->second };
		}
	}

	//Calculate statistics:
	game_map.getStatistics();

	//Add game map to full game
	full_game.push_back(new hlt::Map(game_map));

	//Check if the game is over:
	std::vector<bool> stillAlive(number_of_players, false);
	unsigned char first_found = 0;
	for(unsigned char a = 0; a < game_map.territory_count.size(); a++) if(game_map.territory_count[a] != 0) stillAlive[a] = true;
	for(unsigned char a = 0; a < permissibleTime.size(); a++) if(!permissibleTime[a]) stillAlive[a] = false;
	return stillAlive;
}

//Public Functions -------------------

Halite::Halite()
{
	graph_frame_number = 0;
    last_turn_output = 0;
    color_codes = std::map<unsigned char, hlt::Color>();
    number_of_players = 0;
    game_map = hlt::Map();
    turn_number = 0;
    player_names = std::vector<std::string>();
    full_game = std::vector<hlt::Map * >();
    player_connections = std::vector<tcp::socket * >();
    player_moves = std::vector< std::set<hlt::Move> >();
    //Init Color Codes:
    loadColorCodes();
}

Halite::Halite(unsigned short w, unsigned short h)
{
	graph_frame_number = 0;
    last_turn_output = 0;
    player_moves = std::vector< std::set<hlt::Move> >();
    full_game = std::vector<hlt::Map * >();
    
    //Init Color Codes:
    loadColorCodes();

    turn_number = 0;
    
    //Connect to players
    number_of_players = 0;
    player_names = std::vector<std::string>();
    player_connections = std::vector<tcp::socket * >();
    
    std::string in;
    //Ask if the user would like to use the default ports?
    bool useDefaultPorts = true;
    std::cout << "Would you like to use the default ports? Please enter Yes or No: ";
    while(true)
    {
        std::getline(std::cin, in);
        std::transform(in.begin(), in.end(), in.begin(), ::tolower);
        if(in == "n" || in == "no" || in == "nope" || in == "y" || in == "yes" || in == "yep") break;
        std::cout << "That isn't a valid input. Please enter Yes or No: ";
    }
    if(in == "n" || in == "no" || in == "nope") useDefaultPorts = false;
    
    bool done = false;
    while(!done)
    {
        in;
        //If less than 2, bypass this step: Ask if the user like to add another AI
        if(number_of_players >= 2)
        {
            std::cout << "Would you like to add another player? Please enter Yes or No: ";
            while(true)
            {
                std::getline(std::cin, in);
                std::transform(in.begin(), in.end(), in.begin(), ::tolower);
                if(in == "n" || in == "no" || in == "nope" || in == "y" || in == "yes" || in == "yep") break;
                std::cout << "That isn't a valid input. Please enter Yes or No: ";
            }
            if(in == "n" || in == "no" || in == "nope") break;
        }
        
        unsigned short portNumber;
        if(useDefaultPorts) portNumber = number_of_players + DEFAULT_PORT;
        else
        {
            std::cout << "What port would you like to connect player " << number_of_players + 1 << " on? Please enter a valid port number: ";
            while(true)
            {
                std::getline(std::cin, in);
                std::transform(in.begin(), in.end(), in.begin(), ::tolower);
                try
                {
                    portNumber = std::stoi(in);
                    break;
                }
                catch(std::exception e)
                {
                    std::cout << "That isn't a valid input. Please enter a valid port number: ";
                }
            }
        }
        std::cout << "Waiting for a connection on port " << portNumber << ".\n";
        
        boost::asio::io_service *io_service = new boost::asio::io_service();
		tcp::acceptor acceptor(*io_service, tcp::endpoint(tcp::v4(), portNumber));

        tcp::socket *socket = new tcp::socket(*io_service);
		tcp::socket &referenceSocket = *socket;

		std::cout << "Waiting to accept\n";
		acceptor.accept(*socket);
		std::cout << "Accepted\n";

        player_connections.push_back(socket);
        
        std::cout << "Connected to player " << number_of_players + 1 << " at " << socket->remote_endpoint().address().to_string() << std::endl << "How should I refer to this player? Please enter their name: ";
	while(true)
	{
		std::getline(std::cin, in);
		if(in == "") std::cout << "Each player requires a name to be uniquely identifiable. Please enter a name for this player: ";
		else
		{
			bool good = true;
			for(auto a = player_names.begin(); a != player_names.end(); a++) if(*a == in)
			{
				good = false;
				break;
			}
			if(good) break;
			else std::cout << "That name is already taken. Please enter another name for this player: ";
		}
	}
        player_names.push_back(in);
        
        number_of_players++;
    }

    getColorCodes();
    
    //Initialize map:
    game_map = hlt::Map(w, h, number_of_players);
	game_map.getStatistics();
    
    //Initialize player moves vector
    player_moves.resize(number_of_players);

	//Initialize player attack_count vector.
	attack_count = std::vector<unsigned int>(number_of_players, 0);
    
    //Add game map to full game
	full_game.push_back(new hlt::Map(game_map));
}

void Halite::init()
{
    //Send initial package 
    std::vector<std::thread> initThreads(number_of_players);
    for(unsigned char a = 0; a < number_of_players; a++)
    {
        initThreads[a] = std::thread(handleInitNetworking, player_connections[a], a + 1, player_names[a], game_map);
    }
    for(unsigned char a = 0; a < number_of_players; a++)
    {
    	initThreads[a].join();
    }
    
    setupMapRendering(game_map.map_width, game_map.map_height);
}

void Halite::confirmWithinGame(signed short & turnNumber)
{
	if(turnNumber < 0) turnNumber = 0;
	if(turnNumber >= full_game.size()) turnNumber = full_game.size() - 1;
}

std::vector< std::pair<std::string, float> > Halite::runGame()
{
	std::vector<bool> result(number_of_players, true);
	while(std::count(result.begin(), result.end(), true) > 1 && turn_number <= 1000)
	{
		//Increment turn number:
		turn_number++;
		//Frame logic.
		result = getNextFrame(result);
	}
	unsigned int maxValue = 2 * *std::max_element(attack_count.begin(), attack_count.end());
	std::vector< std::pair<std::string, float> > relativeScores(number_of_players);
	for(unsigned char a = 0; a < number_of_players; a++)
	{
		relativeScores[a] = std::pair<std::string, float>(player_names[a], round(1000.0 * float(attack_count[a]) / maxValue) / 1000.0);
	}
	for(unsigned char a = 0; a < number_of_players; a++) if(result[a]) relativeScores[a].second += 0.5;
	std::sort(relativeScores.begin(), relativeScores.end(), [](const std::pair<std::string, float> & a, const std::pair<std::string, float> & b) -> bool { return a.second > b.second; });
	return relativeScores;
}

void Halite::render(short & turnNumber)
{
	confirmWithinGame(turnNumber);

	if(!full_game.empty())
	{
		hlt::Map * m = full_game[turnNumber];

		std::vector<float> colors(unsigned int(m->map_width) * m->map_height * 3);
		std::vector<unsigned int> strengths(unsigned int(m->map_width) * m->map_height);

		unsigned int loc = 0;
		unsigned int colorLoc = 0;
		for(auto a = m->contents.begin(); a != m->contents.end(); a++)
		{
			for(auto b = a->begin(); b != a->end(); b++)
			{
				hlt::Color c = color_codes[b->owner];
				colors[colorLoc] = c.r;
				colors[colorLoc + 1] = c.g;
				colors[colorLoc + 2] = c.b;
				strengths[loc] = b->strength;
				colorLoc += 3;
				loc++;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, map_color_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(float), colors.data());

		glBindBuffer(GL_ARRAY_BUFFER, map_strength_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, strengths.size() * sizeof(unsigned int), strengths.data());

		//Draw map:
		glUseProgram(map_shader_program);
		glBindVertexArray(map_vertex_attributes);
		glDrawArrays(GL_POINTS, 0, unsigned int(m->map_width) * m->map_height);

		if(turnNumber >= graph_frame_number) setupGraphRendering();

		//Draw graphs:
		glUseProgram(graph_shader_program);
		glBindVertexArray(graph_territory_vertex_attributes);
		for(unsigned char a = 0; a < number_of_players; a++) glDrawArrays(GL_LINE_STRIP, a * graph_frame_number, graph_frame_number);
		glBindVertexArray(graph_strength_vertex_attributes);
		for(unsigned char a = 0; a < number_of_players; a++) glDrawArrays(GL_LINE_STRIP, a * graph_frame_number, graph_frame_number);

		//Edit border buffer
		float xPos = (float(turnNumber) / (graph_frame_number - 1)) * (TERRITORY_GRAPH_RIGHT - TERRITORY_GRAPH_LEFT) + TERRITORY_GRAPH_LEFT;
		glBindBuffer(GL_ARRAY_BUFFER, graph_border_buffer);
		float positionVertices[8];
		positionVertices[0] = xPos; positionVertices[1] = TERRITORY_GRAPH_BOTTOM; positionVertices[2] = xPos; positionVertices[3] = TERRITORY_GRAPH_TOP; positionVertices[4] = xPos; positionVertices[5] = STRENGTH_GRAPH_BOTTOM; positionVertices[6] = xPos; positionVertices[7] = STRENGTH_GRAPH_TOP;
		glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), positionVertices);

		//Draw borders:
		glBindVertexArray(graph_border_vertex_attributes);
		glDrawArrays(GL_LINE_STRIP, 4, 5);
		glDrawArrays(GL_LINE_STRIP, 9, 5);
		glDrawArrays(GL_LINE_STRIP, 14, 5);
		glDrawArrays(GL_LINES, 0, 4);
	}
}

bool Halite::input(std::string filename, unsigned short& width, unsigned short& height)
{
	std::fstream game_file;
	game_file.open(filename, std::ios_base::in);
	if(!game_file.is_open()) return false;

	std::cout << "Beginning to read in file:\n";

	clearFullGame();
	game_map.map_width = 0;
	game_map.map_height = 0;

	std::string in;
	game_file >> width >> height >> number_of_players;
	game_map.map_width = width;
	game_map.map_height = height;
	std::getline(game_file, in);
	player_names.resize(number_of_players);
	for(unsigned char a = 0; a < number_of_players; a++) std::getline(game_file, player_names[a]);

	game_map.contents.resize(game_map.map_height);
	for(auto a = game_map.contents.begin(); a != game_map.contents.end(); a++) a->resize(game_map.map_width);

	short ownerIn, ageIn;
	while(!game_file.eof())
	{
		for(unsigned short a = 0; a < game_map.map_height; a++) for(unsigned short b = 0; b < game_map.map_width; b++)
		{
			game_file >> ownerIn >> ageIn;
			game_map.contents[a][b] = { static_cast<unsigned char>(ownerIn), static_cast<unsigned char>(ageIn) };
		}
		game_map.getStatistics();
		//Add game map to full game
		full_game.push_back(new hlt::Map(game_map));
		std::cout << "Gotten frame #" << short(full_game.size()) << ".\n";
	}

	delete full_game.back();
	full_game.pop_back();

	setupMapRendering(full_game[0]->map_width, full_game[0]->map_height);
	setupGraphRendering();

	game_file.close();

	return true;
}

void Halite::output(std::string filename)
{
	std::cout << "Beginning to output file from frame #" << last_turn_output + 1 << ".\n";

	std::fstream game_file;
	if(last_turn_output == 0)
	{
		game_file.open(filename, std::ios_base::out);
		game_file << game_map.map_width << ' ' << game_map.map_height << ' ' << number_of_players << "\n";
		for(auto a = player_names.begin(); a != player_names.end() - 1; a++) game_file << *a << "\n";
		game_file << *(player_names.end() - 1);
	}
	else game_file.open(filename, std::ios_base::app);

	while(last_turn_output < full_game.size())
	{
		game_file << std::endl;
		for(auto a = full_game[last_turn_output]->contents.begin(); a != full_game[last_turn_output]->contents.end(); a++) for(auto b = a->begin(); b != a->end(); b++) game_file << short(b->owner) << ' ' << short(b->strength) << ' ';
		last_turn_output++;
		std::cout << "Finished outputting frame " << last_turn_output + 1 << ".\n";
	}

	std::cout << "Output file until frame #" << last_turn_output + 1 << ".\n";

	game_file.close();
}

std::map<unsigned char, hlt::Color> Halite::getColorCodes()
{
	return color_codes;
}

Halite::~Halite()
{
	//Get rid of map OpenGL stuff
	glDeleteShader(map_vertex_shader);
	glDeleteShader(map_geometry_shader);
	glDeleteShader(map_fragment_shader);
	glDeleteProgram(map_shader_program);
	glDeleteBuffers(1, &map_vertex_buffer);
	glDeleteBuffers(1, &map_color_buffer);
	glDeleteBuffers(1, &map_strength_buffer);
	glDeleteVertexArrays(1, &map_vertex_attributes);

	//Get rid of graph OpenGL stuff
	glDeleteShader(graph_vertex_shader);
	glDeleteShader(graph_fragment_shader);
	glDeleteProgram(graph_shader_program);
	glDeleteBuffers(1, &graph_territory_vertex_buffer);
	glDeleteBuffers(1, &graph_strength_vertex_buffer);
	glDeleteBuffers(1, &graph_border_buffer);
	glDeleteBuffers(1, &graph_color_buffer);
	glDeleteVertexArrays(1, &graph_strength_vertex_attributes);
	glDeleteVertexArrays(1, &graph_territory_vertex_attributes);
	glDeleteVertexArrays(1, &graph_border_vertex_attributes);
	
	//Get rid of dynamically allocated memory:
	for(auto a = player_connections.begin(); a != player_connections.end(); a++) if(*a != NULL) delete *a;
	clearFullGame();
}