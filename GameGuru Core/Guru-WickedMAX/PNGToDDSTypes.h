#pragma once
#include <string>

// Index 0: source file
// Index 1: destination file
struct MaterialToConvert
{
	std::string color[2];
	std::string normal[2];
	std::string ao;
	std::string roughness;
	std::string metalness;
	std::string surface[2];
};