#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <set>
#include <cstdlib>
#include "vec3.h"

namespace SceneParser
{
	//the set of all literals in the scene file
	std::set<std::string> literals;
	struct meshInfo
	{
		std::string meshData;
		std::string material;
		meshInfo(){}
		meshInfo(const std::string& meshData, const std::string& material)
			: meshData(meshData), material(material) {}
	};
	//a vector to store the mesh descriptor and  material descriptor literals associated with a mesh
	std::vector<meshInfo> meshes;
	//a vector to store the mesh descriptor and  material descriptor literals associated with an octree mesh
	std::vector<meshInfo> octree_meshes;

	struct lightInfo
	{
		BasicMath::vec3 pos;
		float radius;
		std::string texture;
		lightInfo() {}
		lightInfo(float x, float y, float z, float rad, const std::string& str)
			:pos(x, y, z), radius(rad), texture(str) {}
	};
	//a vector to store the info about the lights
	std::vector<lightInfo> lights;

	//camera properties
	BasicMath::vec3 cameraPosition(0,0,0);
	BasicMath::vec3 cameraTarget(0,0,1);
	BasicMath::vec3 cameraUp(0, 1, 0);

	struct textureInfo
	{
		std::string filename;
		BasicMath::vec3 color;
		textureInfo() {}
		textureInfo(const std::string& filename, float r,float g, float b)
			:filename(filename), color(r,g,b)
		{

		}
	};
	//map between texture literal - textureInfo
	std::map<std::string, textureInfo> textureLiteralMap;

	struct meshDataInfo
	{
		std::string filename;
		BasicMath::vec3 position, rotation, scaling;
		meshDataInfo() {}
		meshDataInfo(const std::string& filename, float x, float y, float z, float rx, float ry, float rz, float sx, float sy, float sz)
			:filename(filename), position(x,y,z), rotation(rx,ry,rz), scaling(sx,sy,sz)
		{}
	};
	//map between a mesh data literal - meshDataInfo
	std::map<std::string, meshDataInfo> meshLiteralMap;

	struct constantTextureInfo
	{
		BasicMath::vec3 color;
		constantTextureInfo() {}
		constantTextureInfo(float r, float g, float b)
			:color(r, g, b) {}
	};
	//map between a constant texture literal - constantTextureInfo
	std::map<std::string, constantTextureInfo> constantTextureLiteralMap;

	//map between a lambertian literal - texture literal
	std::map<std::string, std::string> lambertianLiteralMap;

	//! creates a vector with all the words from the string (words are continuous blocks of symbols divided by ' ' and '\t')
	std::vector<std::string> getAllWords(const std::string& str)
	{
		std::vector<std::string> words;

		bool word = false;
		size_t index = 0;
		for (size_t i = 0; i < str.size(); ++i)
		{
			//are we currently reading a word?
			if (word)
			{
				if (str[i] == ' ' || str[i] == '\t')
				{
					words.push_back(str.substr(index, i - index));
					word = false;
					index = i;
				}
			}
			else
			{
				if (str[i] != ' ' && str[i] != '\t')
				{
					word = true;
					index = i;
				}
			}
		}

		//process the last case
		if (word && str.size() - index != 0)
			words.push_back(str.substr(index, str.size() - index));

		return words;
	}

	//! checks whether a filename can be considered valid for a texture
	bool wordIsTexture(const std::string& str)
	{
		if (str.size() < 7)
			return false;
		size_t last5 = str.size() - 5;
		if (str[0] != '"' || str.substr(last5, 5) != ".png\"")
			return false;
		return true;
	}

	//! checks whether a filename can be considered valid for a mesh
	bool wordIsMesh(const std::string& str)
	{
		if (str.size() < 7)
			return false;
		size_t last5 = str.size() - 5;
		if (str[0] != '"' || str.substr(last5, 5) != ".ply\"")
			return false;
		return true;
	}

	//checks whether a word is a literal (is not a keyword)
	bool wordIsLiteral(const std::string& str)
	{
		return (str != "Mesh" && str != "OctreeMesh" && str != "Light" && str != "Lambertian" && str != "Camera" && str != "Default");
	}

	//! parses a line from a scene file
	bool parseLine(const std::string& str, size_t lineNumber)
	{
		//get all the words from the line
		std::vector<std::string> words(getAllWords(str));
		//if there are no words - just continue
		if (words.empty())
			return true;
		//if there's only one word - incorrect file
		if (words.size() == 1)
		{
			std::cout << "Syntax error at line: " << lineNumber << ". Incomplete statement.\n";
			return false;
		}

		//cases
		if (words[0] == "Default")
		{
			std::cout << "Syntax error at line: " << lineNumber << ". Default can only be used as an argument.\n";
			return false;
		}
		else if (words[0] == "Mesh")
		{
			//if a mesh needs to be created, it requires a triangle mesh descriptor, and a material descriptor as arguments:
			//Mesh meshDescriptorLiteral materialDescriptorLiteral

			//if the arguments are not 2 - syntax error
			if (words.size() != 3)
			{
				std::cout << "Syntax error at line: " << lineNumber << ".Mesh command accepts 2 arguments.\n";
				return false;
			}
			else
			{
				//if any of the arguments are acutally keywords - syntax error
				if (!wordIsLiteral(words[1]) || !wordIsLiteral(words[2]))
				{
					std::cout << "Syntax error at line: " << lineNumber << ".Mesh command has a keyword as an argument.\n";
					return false;
				}
				else
				{
					meshes.push_back(meshInfo(words[1], words[2]));
				}
			}
		}
		else if (words[0] == "OctreeMesh")
		{
			//if a mesh needs to be created, it requires a triangle mesh descriptor, and a material descriptor as arguments:
			//Mesh meshDescriptorLiteral materialDescriptorLiteral

			//if the arguments are not 2 - syntax error
			if (words.size() != 3)
			{
				std::cout << "Syntax error at line: " << lineNumber << ".OctreeMesh command accepts 2 arguments.\n";
				return false;
			}
			else
			{
				//if any of the arguments are acutally keywords - syntax error
				if (!wordIsLiteral(words[1]) || !wordIsLiteral(words[2]))
				{
					std::cout << "Syntax error at line: " << lineNumber << ".OctreeMesh command has a keyword as an argument.\n";
					return false;
				}
				else
				{
					octree_meshes.push_back(meshInfo(words[1], words[2]));
				}
			}
		}
		else if (words[0] == "Light")
		{
			//if the arguments are not 5 - syntax error
			if (words.size() != 6)
			{
				std::cout << "Syntax error at line: " << lineNumber << ".Light command accepts 5 arguments.\n";
				return false;
			}
			else
			{
				//if any of the arguments are acutally keywords - syntax error
				if (!wordIsLiteral(words[1]) || !wordIsLiteral(words[2]) || !wordIsLiteral(words[3]) || !wordIsLiteral(words[4]) || !wordIsLiteral(words[5]))
				{
					std::cout << "Syntax error at line: " << lineNumber << ".Light command has a keyword as an argument.\n";
					return false;
				}
				else
				{
					lights.push_back(lightInfo(atof(words[1].c_str()), atof(words[2].c_str()), atof(words[3].c_str()), atof(words[4].c_str()), words[5]));
				}
			}
		}
		else if (words[0] == "Camera")
		{
			//if the arguments are not 9 - syntax error
			if (words.size() != 10)
			{
				std::cout << "Syntax error at line: " << lineNumber << ".Camera command accepts 9 arguments.\n";
				return false;
			}
			else
			{
				//if any of the arguments are acutally keywords - syntax error
				if (!wordIsLiteral(words[1]) || !wordIsLiteral(words[2]) || !wordIsLiteral(words[3]) || !wordIsLiteral(words[4]) || !wordIsLiteral(words[5]) || !wordIsLiteral(words[6]) || !wordIsLiteral(words[7]) || !wordIsLiteral(words[8]) || !wordIsLiteral(words[9]))
				{
					std::cout << "Syntax error at line: " << lineNumber << ".Camera command has a keyword as an argument.\n";
					return false;
				}
				else
				{
					cameraPosition = BasicMath::vec3(atof(words[1].c_str()), atof(words[2].c_str()), atof(words[3].c_str()));
					cameraTarget = BasicMath::vec3(atof(words[4].c_str()), atof(words[5].c_str()), atof(words[6].c_str()));
					cameraUp = BasicMath::vec3(atof(words[7].c_str()), atof(words[8].c_str()), atof(words[9].c_str()));
				}
			}
		}
		else if(wordIsLiteral(words[0]))
		{
			//is this literal free
			if (literals.find(words[0]) != literals.end())
			{
				std::cout << "Syntax error at line: " << lineNumber << ".Literal '" << words[0] << "' redefinition\n";
				return false;
			}
			else
			{
				literals.insert(words[0]);
			}

			//is the literal pointing to a texture declaration
			if (wordIsTexture(words[1]))
			{
				if (words.size() != 5)
				{
					std::cout << "Syntax error at line: " << lineNumber << ".When trying to initialize a literal with a texture the format is: literal filename r g b\n";
					return false;
				}
				else
				{
					textureLiteralMap[words[0]] = textureInfo(words[1], atof(words[2].c_str()), atof(words[3].c_str()), atof(words[4].c_str()));
				}
			}//is the literal pointing to a mesh declaration
			else if (wordIsMesh(words[1]))
			{
				if (words.size() != 11)
				{
					std::cout << "Syntax error at line: " << lineNumber << ".When trying to initialize a literal with mesh data the format is: literal filename x y z rx ry rz sx sy sz\n";
					return false;
				}
				else
				{

					meshLiteralMap[words[0]] = meshDataInfo(words[1], atof(words[2].c_str()), atof(words[3].c_str()), atof(words[4].c_str()),
						atof(words[5].c_str()), atof(words[6].c_str()), atof(words[7].c_str()),
						atof(words[8].c_str()), atof(words[9].c_str()), atof(words[10].c_str()));

				}
			}
			else if (words[1] == "Lambertian")
			{
				//if the arguments are not 1 - syntax error
				if (words.size() != 3)
				{
					std::cout << "Syntax error at line: " << lineNumber << ".When trying to initialize a literal with a lambertian material the format is: literal Lambertian texture_literal\n";
					return false;
				}
				else
				{
					lambertianLiteralMap[words[0]] = words[2];
					
				}
			}
			//is it a constant texture declaration
			else if (words.size() == 4)
			{
				constantTextureLiteralMap[words[0]] = constantTextureInfo(atof(words[1].c_str()), atof(words[2].c_str()), atof(words[3].c_str()));
			}
			else
			{
				std::cout << "Syntax error at line: " << lineNumber << ".Literal '" << words[0] << "' declaration error.\n";
				return false;
			}
				
		}
		else
		{
			std::cout << "Something went wrong at line: " << lineNumber << "\n";
			return false;
		}
		return true;
	}

	//! after parsing the file - checks the dependencies
	bool checkResultCorrectness()
	{
		//check lambertian materials - texture dependency
		for (auto iter : SceneParser::lambertianLiteralMap)
		{
			//does the texture which this material takes as an argument exist
			if (literals.find(iter.second) == literals.end())
			{
				std::cout << "SceneParser:: Couldn't find texture: " << iter.second << " used as an argument in Lambertian: " << iter.first << "\n";
				return false;
			}
		}
		
		//check lights - texture dependency
		for (auto iter : SceneParser::lights)
		{
			//does the texture which this material takes as an argument exist
			if (literals.find(iter.texture) == literals.end())
			{
				std::cout << "SceneParser:: Couldn't find texture: " << iter.texture << " used in Light.\n";
				return false;
			}
		}

		//check meshes - meshdata and material dependency
		for (auto iter : SceneParser::meshes)
		{
			//does the mesh data exist
			if (literals.find(iter.meshData) == literals.end())
			{
				std::cout << "SceneParser:: Couldn't find mesh data: " << iter.meshData << " used in Mesh.\n";
				return false;
			}

			//does the material exist
			if (literals.find(iter.material) == literals.end())
			{
				std::cout << "SceneParser:: Couldn't find material: " << iter.material << " used in Mesh.\n";
				return false;
			}
		}

		//check octree meshes - meshdata and material dependency
		for (auto iter : SceneParser::octree_meshes)
		{
			//does the mesh data exist
			if (literals.find(iter.meshData) == literals.end())
			{
				std::cout << "SceneParser:: Couldn't find mesh data: " << iter.meshData << " used in Mesh.\n";
				return false;
			}

			//does the material exist
			if (literals.find(iter.material) == literals.end())
			{
				std::cout << "SceneParser:: Couldn't find material: " << iter.material << " used in Mesh.\n";
				return false;
			}
		}

		return true;
	}

	//! try to parse a file
	bool parseFile(const char* filename)
	{
		std::ifstream file;
		file.open(filename);
		if (!file)
		{
			std::cout << "SceneParser: Failed to open file "<< filename << "\n";
			return false;
		}
		std::string readLine;
		size_t lineCount = 0;
		while (std::getline(file, readLine))
		{
			if (!parseLine(readLine, lineCount))
			{
				std::cout << "SceneParser: Something went wrong while parsing file: " << filename << ", exiting now.\n";
				file.close();
				return false;
			}
			++lineCount;
		}
		file.close();

		if (!checkResultCorrectness())
		{
			std::cout << "SceneParser: Unresolved dependenices found.\n";
			return false;
		}

		return true;
	}
}

#endif
