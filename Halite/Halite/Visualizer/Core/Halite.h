#ifndef HALITE_H
#define HALITE_H

#include <fstream>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>

#include "hlt.h"
#include "../OpenGL.h"

class Halite
{
private:
    std::vector<std::string> player_names;
	std::vector<unsigned int> attack_count;
	std::vector<hlt::Map * > full_game;
	std::map<unsigned char, hlt::Color> color_codes;
	unsigned short number_of_players;

	//Map rendering
	GLuint map_vertex_buffer, map_color_buffer, map_strength_buffer, map_vertex_attributes, map_vertex_shader, map_geometry_shader, map_fragment_shader, map_shader_program;

	//Graph rendering
	GLuint graph_territory_vertex_buffer, graph_strength_vertex_buffer, graph_color_buffer, graph_territory_vertex_attributes, graph_strength_vertex_attributes, graph_vertex_shader, graph_fragment_shader, graph_shader_program;
	//Stats about the graph. This lets us know if we need to redo the setup for the graph.
	unsigned short graph_frame_number, graph_turn_number, graph_turn_min, graph_turn_max;
	float graph_zoom;

	//Border rendering:
	GLuint border_vertex_buffer, border_vertex_attributes, border_vertex_shader, border_fragment_shader, border_shader_program;

	void loadColorCodes(std::string s);
	void setupMapRendering(unsigned short width, unsigned short height);
	void setupGraphRendering(float zoom, short turnNumber);
	void setupBorders();
	void clearFullGame();
public:
    Halite();
    Halite(unsigned short w, unsigned short h);
	bool input(GLFWwindow * window, std::string filename, unsigned short& width, unsigned short& height);
	void render(GLFWwindow * window, short& turnNumber, float zoom);
	std::map<unsigned char, hlt::Color> getColorCodes();
	~Halite();
};

#endif