#include "Halite.h"

using boost::asio::ip::tcp;

unsigned char Halite::getNextFrame()
{
	if(game_map.map_width == 0 || game_map.map_height == 0) return 255;

    //Create threads to send/receive data to/from players. The threads should return a float of how much time passed between the end of their message being sent and the end of the AI's message being sent.
    std::vector< std::future<double> > frameThreads(number_of_players);
    for(unsigned char a = 0; a < number_of_players; a++)
    {
        frameThreads[a] = std::async(handleFrameNetworking, player_connections[a], game_map, &player_moves[a]);
    }

	//Find the locations of all of the pieces the players had before they made their moves
	std::vector<unsigned short> numSentient(number_of_players, 0);
	std::map<hlt::Location, hlt::Site> oldPieces;
	for(unsigned short a = 0; a < game_map.map_height; a++) for(unsigned short b = 0; b < game_map.map_width; b++)
	{
		//If sentient
		if(game_map.contents[a][b].owner != 0)
		{
			//Add to map of sentient pieces.
			oldPieces.insert(std::pair<hlt::Location, hlt::Site>({ b, a }, game_map.contents[a][b]));
			//Add to number of sentients controlled by player.
			numSentient[game_map.contents[a][b].owner - 1]++;
		}
	}

    //Figure out how long each AI is permitted to respond.
    std::vector<double> allowableTimesToRespond(number_of_players);
    for(unsigned char a = 0; a < number_of_players; a++) allowableTimesToRespond[a] = FLT_MAX;
        //For the time being we'll allow infinte time (debugging purposes), but eventually this will come into use):
        //allowableTimesToRespond[a] = 0.01 + (double(game_map.map_height)*game_map.map_width*.00001) + (double(numSentient[a]) * numSentient[a] * .0001);

	std::vector< std::map<hlt::Location, unsigned char> > newPieces(number_of_players);
    
    //Join threads. Figure out if the player responded in an allowable amount of time.
    std::vector<bool> permissibleTime(number_of_players);
    for(unsigned char a = 0; a < number_of_players; a++)
    {
        permissibleTime[a] = frameThreads[a].get() <= allowableTimesToRespond[a];
    }

	//Set everything's age to 0 on the map.
	clearMap();

	//For each player, use their moves to create the newPieces map.
	for(unsigned short a = 0; a < number_of_players; a++)
	{
		//Add in pieces according to their moves.
		for(auto b = player_moves[a].begin(); b != player_moves[a].end(); b++) if(oldPieces[b->loc].owner == a + 1)
		{
			if(newPieces[a].count(b->loc))
			{
				if(short(newPieces[a][b->loc]) + oldPieces[b->loc].strength <= 255) newPieces[a][b->loc] += oldPieces[b->loc].strength;
				else newPieces[a][b->loc] = 255;
			}
			else
			{
				newPieces[a].insert(std::pair<hlt::Location, unsigned char>(b->loc, oldPieces[b->loc].strength));
			}

			//Erase from oldPieces.
			oldPieces.erase(b->loc);
		}
	}

	//Add in all of the remaining pieces whose moves weren't specified. 
	for(auto b = oldPieces.begin(); b != oldPieces.end(); b++)
	{
		if(newPieces[b->second.owner - 1].count(b->first))
		{
			if(short(newPieces[b->second.owner - 1][b->first]) + b->second.strength <= 255) newPieces[b->second.owner - 1][b->first] += b->second.strength;
			else newPieces[b->second.owner - 1][b->first] = 255;
		}
		else
		{
			newPieces[b->second.owner - 1].insert(std::pair<hlt::Location, unsigned char>(b->first, b->second.strength));
		}
	}

	oldPieces.clear(); //Take back that memory (although in actuality really insignificant).

	std::vector< std::map<hlt::Location, unsigned short> > toInjure(number_of_players); //This is a short so that we don't have to worry about 255 overflows.

	//Sweep through locations and find the correct damage for each piece. accordingly.
	for(unsigned char a = 0; a != game_map.map_height; a++) for(unsigned short b = 0; b < game_map.map_width; b++)
	{
		hlt::Location l = { b, a };
		for(unsigned short c = 0; c < number_of_players; c++) if(newPieces[c].count(l))
		{
			for(unsigned short d = 0; d < number_of_players; d++) if(d != c)
			{
				hlt::Location tempLoc = l;
				//Check 'STILL' square:
				if(newPieces[d].count(tempLoc))
				{
					//Apply damage:
					if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += newPieces[c][l];
					else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, newPieces[c][l]));
				}
				//Check 'NORTH' square:
				tempLoc = game_map.getLocation(l, NORTH);
				if(newPieces[d].count(tempLoc))
				{
					//Apply damage:
					if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += newPieces[c][l];
					else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, newPieces[c][l]));
				}
				//Check 'EAST' square:
				tempLoc = game_map.getLocation(l, EAST);
				if(newPieces[d].count(tempLoc))
				{
					//Apply damage:
					if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += newPieces[c][l];
					else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, newPieces[c][l]));
				}
				//Check 'SOUTH' square:
				tempLoc = game_map.getLocation(l, SOUTH);
				if(newPieces[d].count(tempLoc))
				{
					//Apply damage:
					if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += newPieces[c][l];
					else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, newPieces[c][l]));
				}
				//Check 'WEST' square:
				tempLoc = game_map.getLocation(l, WEST);
				if(newPieces[d].count(tempLoc))
				{
					//Apply damage:
					if(toInjure[d].count(tempLoc)) toInjure[d][tempLoc] += newPieces[c][l];
					else toInjure[d].insert(std::pair<hlt::Location, unsigned short>(tempLoc, newPieces[c][l]));
				}
			}
		}
	}

	//Injure and/or delete pieces.
	for(unsigned char a = 0; a != number_of_players; a++)
	{
		for(auto b = toInjure[a].begin(); b != toInjure[a].end(); b++)
		{
			if(b->second >= newPieces[a][b->first]) newPieces[a].erase(b->first);
			else newPieces[a][b->first] -= b->second;
		}
	}

	//Add pieces back into the map.
	for(unsigned char a = 0; a != number_of_players; a++)
	{
		for(auto b = newPieces[a].begin(); b != newPieces[a].end(); b++)
		{
			game_map.getSite(b->first, STILL) = { a + 1, b->second };
		}
	}
    
    //Add game map to full game
    full_game.push_back(new hlt::Map());
    *full_game.back() = game_map;
    
    //Increment turn number:
    turn_number++;
    
    //Check if the game is over:
    unsigned char first_found = 0;
    for(auto a = game_map.contents.begin(); a != game_map.contents.end(); a++) for(auto b = a->begin(); b != a->end(); b++)
    {
        if(b->owner != first_found && b->owner != 0)
        {
            if(first_found == 0) first_found = b->owner;
            else return 0; //Multiple people still alive
        }
    }
    return first_found; //If returns 0, that means NOBODY is alive. If it returns something else, they are the winner.
}

std::string Halite::runGame()
{
    unsigned short result = 0;
    while(result == 0) result = getNextFrame();
	if(result == 255) return "";
    return player_names[result - 1];
}

void Halite::confirmWithinGame(signed short& turnNumber)
{
	if(turnNumber < 0) turnNumber = 0;
    if(turnNumber >= full_game.size()) turnNumber = full_game.size() - 1;
}

void Halite::render(short& turnNumber)
{
    confirmWithinGame(turnNumber);

	if(!full_game.empty())
	{
		hlt::Map * m = full_game[turnNumber];

		std::vector<float> colors(unsigned int(m->map_width) * m->map_height * 3);

		unsigned int colorLocation = 0;
		for(auto a = m->contents.begin(); a != m->contents.end(); a++)
		{
			for(auto b = a->begin(); b != a->end(); b++)
			{
				hlt::Color c = color_codes[b->owner];
				const double BASE_DIMMING_FACTOR = 0.5;
				if(b->strength != 255)
				{
					const double TRUE_DIMMING_FACTOR = 0.15 + 0.85*(BASE_DIMMING_FACTOR*b->strength / 255);
					c.r *= TRUE_DIMMING_FACTOR;
					c.g *= TRUE_DIMMING_FACTOR;
					c.b *= TRUE_DIMMING_FACTOR;
				}
				colors[colorLocation] = c.r;
				colors[colorLocation + 1] = c.g;
				colors[colorLocation + 2] = c.b;
				colorLocation += 3;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(float), colors.data());

		glUseProgram(shader_program);
		glBindVertexArray(vertex_attributes);
		glDrawArrays(GL_POINTS, 0, unsigned int(m->map_width) * m->map_height);
	}
}

void Halite::output(std::string filename)
{
    std::cout << "Beginning to output file from frame #" << last_turn_output + 1 << ".\n";

    std::fstream game_file;
    if(last_turn_output == 0)
    {
        game_file.open(filename, std::ios_base::out);
        game_file  << game_map.map_width << ' ' << game_map.map_height << ' ' << number_of_players << "\n";
        for(auto a = player_names.begin(); a != player_names.end() - 1; a++) game_file  << *a << "\n";
        game_file  << *(player_names.end() - 1);
    }
    else game_file.open(filename, std::ios_base::app);
    
    while(last_turn_output < full_game.size())
    {
        game_file  << std::endl;
        for(auto a = full_game[last_turn_output]->contents.begin(); a != full_game[last_turn_output]->contents.end(); a++) for(auto b = a->begin(); b != a->end(); b++) game_file  << short(b->owner) << ' ' << short(b->strength) << ' ';
        last_turn_output++;
        std::cout << "Finished outputting frame " << last_turn_output + 1 << ".\n";
    }
    
    std::cout << "Output file until frame #" << last_turn_output + 1 << ".\n";
    
    game_file.close();
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
        full_game.push_back(new hlt::Map());
        *full_game.back() = game_map;
        std::cout << "Gotten frame #" << short(full_game.size()) << ".\n";
    }
    
    delete full_game.back();
    full_game.pop_back();

	setupRendering(full_game[0]->map_width, full_game[0]->map_height);
    
    game_file.close();

    return true;
}

Halite::Halite()
{
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
    color_codes = std::map<unsigned char, hlt::Color>();
    color_codes.insert(std::pair<unsigned char, hlt::Color>(0, { 0.05f, 0.05f, 0.05f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(1, { 1.0f, 0.0f, 0.0f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(2, { 0.0f, 1.0f, 0.0f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(3, { 0.0f, 0.0f, 1.0f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(4, { 1.0f, 1.0f, 1.0f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(5, { 1.0f, 0.0f, 1.0f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(6, { 0.0f, 1.0f, 1.0f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(7, { 1.0f, 1.0f, 1.0f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(8, { .87f, .72f, .53f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(9, { 1.0f, .50f, .50f }));
    color_codes.insert(std::pair<unsigned char, hlt::Color>(10, { 1.0f, .65f, .0f }));
}

Halite::Halite(unsigned short w, unsigned short h)
{
    last_turn_output = 0;
    player_moves = std::vector< std::set<hlt::Move> >();
    full_game = std::vector<hlt::Map * >();
    
    //Init Color Codes:
	color_codes = std::map<unsigned char, hlt::Color>();
	color_codes.insert(std::pair<unsigned char, hlt::Color>(0, { 0.05f, 0.05f, 0.05f }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(1, { 1.0, 0.0, 0.0 }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(2, { 0.0, 1.0, 0.0 }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(3, { 0.0, 0.0, 1.0 }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(4, { 1.0, 1.0, 1.0 }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(5, { 1.0, 0.0, 1.0 }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(6, { 0.0, 1.0, 1.0 }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(7, { 1.0, 1.0, 1.0 }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(8, { .87f, .72f, .53f }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(9, { 1.0f, .50f, .50f }));
	color_codes.insert(std::pair<unsigned char, hlt::Color>(10, { 1.0f, .65f, .0f }));
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
        if(useDefaultPorts) portNumber = number_of_players + 2000;
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
		acceptor.accept(referenceSocket);

		boost::asio::socket_base::keep_alive option(true);
		socket->set_option(option);
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
    
    //Initialize player moves vector
    player_moves.resize(number_of_players);
    
    //Add it to the full game:
    full_game.push_back(new hlt::Map());
    *full_game.back() = game_map;
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

	setupRendering(game_map.map_width, game_map.map_height);
}

void Halite::getColorCodes()
{
    for(unsigned short a = 0; a < number_of_players; a++)
    {
        hlt::Color c = color_codes[a+1];
        std::cout << "Player " << player_names[a] << " has color: r = " << short(c.r) << ", g = " << short(c.g) << ", and b = " << short(c.b) << ".\n";
    }
}

void Halite::clearFullGame()
{
	for(auto a = full_game.begin(); a != full_game.end(); a++) delete *a;
	full_game.clear();
}

void Halite::setupRendering(unsigned short width, unsigned short height)
{
	//Delete buffers and vaos
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &color_buffer);
	glDeleteVertexArrays(1, &vertex_attributes);
	//Generate buffers and vaos.
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &color_buffer);
	glGenVertexArrays(1, &vertex_attributes);

	//Generate vertices of centers of squares:
	std::vector<float> vertexLocations(unsigned int(width) * height * 2); //2 because there are x and y values for every vertex.
	float xLoc = -1.0 + 1.0 / width, yLoc = 1.0 - 1.0 / height, dX = 2.0 / width, dY = 2.0 / height;
	for(unsigned int a = 0; a < vertexLocations.size(); a += 2)
	{
		vertexLocations[a] = xLoc;
		vertexLocations[a + 1] = yLoc;

		xLoc += dX;
		if(xLoc > 1.0)
		{
			xLoc = -1.0 + 1.0 / width;
			yLoc -= dY;
		}
	}

	//Bind vertex attribute object.
	glBindVertexArray(vertex_attributes);

	//Setup vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertexLocations.size() * sizeof(float), vertexLocations.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Create vector of floats (0.0) to reserve the memory for the color buffer and allow us to set the mode to GL_DYNAMIC_DRAW.
	std::vector<float> colors(unsigned int(width) * height * 3); //r, g, and b components.

	//Setup color buffer
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//Setup shaders:
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	shaderFromFile(vertex_shader, "Classes/shaders/vertexshader.glsl", "vertex_shader");
	geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	shaderFromFile(geometry_shader, "Classes/shaders/geometryshader.glsl", "geometry_shader");
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFromFile(fragment_shader, "Classes/shaders/fragmentshader.glsl", "fragment_shader");

	//Setup shader program:
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, geometry_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glDetachShader(shader_program, vertex_shader);
	glDetachShader(shader_program, geometry_shader);
	glDetachShader(shader_program, fragment_shader);

	//Set uniform:
	glUseProgram(shader_program);
	const float SPACE_FACTOR = 0.6;
	GLint widthLoc = glGetUniformLocation(shader_program, "width"), heightLoc = glGetUniformLocation(shader_program, "height");
	glUniform1f(widthLoc, dX * SPACE_FACTOR * 0.5);
	glUniform1f(heightLoc, dY * SPACE_FACTOR * 0.5);
	
	//Cleanup - delete shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);
}

void Halite::clearMap()
{
	for(auto a = game_map.contents.begin(); a != game_map.contents.end(); a++) for(auto b = a->begin(); b != a->end(); b++) b->strength = 0;
}

Halite::~Halite()
{
	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(shader_program);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &color_buffer);
	glDeleteVertexArrays(1, &vertex_attributes);
}