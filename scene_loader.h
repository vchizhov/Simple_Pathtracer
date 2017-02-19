#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H
#include "scene_parser.h"
#include "PlyLoader.h"
#include "scene.h"
#include "sphere.h"
#include "octree.h"
#include "high_precision_timer.h"
namespace SceneLoader
{
	std::map<std::string, Raytr_Core::texture*> textureMap;
	std::map<std::string, Raytr_Core::triangle_mesh_data*> meshDataMap;
	std::map<std::string, Raytr_Core::material*> materialMap;

	//! loads the textures from disk
	bool loadTextures()
	{
		Tigr* temp = NULL;
		for (auto iter : SceneParser::textureLiteralMap)
		{
			//try to load the image from disk
			temp = tigrLoadImage(iter.second.filename.substr(1, iter.second.filename.size() - 2).c_str());
			if (temp == NULL)
			{
				std::cout << "SceneLoader:: Couldn't load texture: " << iter.second.filename << "\n";
				return false;
			}
			else
			{
				textureMap[iter.first] = new Raytr_Core::image_texture(temp, BasicMath::vec4(iter.second.color, 1));
			}
		}
		return true;
	}

	//!loads the meshes from disk
	bool loadMeshData()
	{
		for (auto iter : SceneParser::meshLiteralMap)
		{
			//try to load the mesh from disk
			std::cout << iter.second.filename.c_str() << "\n";
			if (!Raytr_Core::PlyLoader::loadPlyMesh(iter.second.filename.substr(1, iter.second.filename.size() - 2).c_str(), iter.second.position, iter.second.rotation, iter.second.scaling))
			{
				std::cout << "SceneLoader:: Couldn't load ply mesh: " << iter.second.filename << "\n";
				return false;
			}
			else
			{
				meshDataMap[iter.first] = &Raytr_Core::meshDataVector.back();
			}
		}
		return true;
	}

	//!creates the constant textures
	void createConstantTextures()
	{
		for (auto iter : SceneParser::constantTextureLiteralMap)
		{
			textureMap[iter.first] = new Raytr_Core::constant_texture(BasicMath::vec4(iter.second.color, 1));
		}
	}

	//!creates the lambertian materials
	void createLambertianMaterials()
	{
		for (auto iter : SceneParser::lambertianLiteralMap)
		{
			materialMap[iter.first] = new Raytr_Core::lambertian_material(textureMap[iter.second], nullptr);
		}
	}

	//! creates lights
	void createLights(Raytr_Core::scene& scn)
	{
		for (auto iter : SceneParser::lights)
		{
			scn.addObject(new Raytr_Core::sphere(iter.pos, iter.radius, new Raytr_Core::emitter_material(textureMap[iter.texture],nullptr)));
		}
	}

	//! creates meshes
	void createMeshes(Raytr_Core::scene& scn)
	{
		for (auto iter : SceneParser::meshes)
		{
			scn.addObject(new Raytr_Core::triangle_mesh(*meshDataMap[iter.meshData], materialMap[iter.material]));
		}
	}

	//! creates octree meshes
	void createOctreeMeshes(Raytr_Core::scene& scn)
	{
		size_t maxElements = 40;
		for (auto iter : SceneParser::octree_meshes)
		{
			size_t depth = meshDataMap[iter.meshData]->triangleCount() / (maxElements*log(8));
			std::cout << "Building octree of " << iter.meshData << " with depth: " << depth << "\n";
			HighPrecisionTimer octreeBuildTimer;
			octreeBuildTimer.StartCounter();
			scn.addObject(new Raytr_Core::triangle_octree_mesh(*meshDataMap[iter.meshData], materialMap[iter.material], depth, maxElements));
			std::cout << "Octree built in " << octreeBuildTimer.GetCounter() << "\n";
		}
	}

	bool loadScene(const char* filename, Raytr_Core::scene** scn)
	{
		*scn = new Raytr_Core::scene;
		if (!SceneParser::parseFile(filename))
		{
			return false;
		}
		if (!loadTextures())
		{
			std::cout << "SceneLoader:: Failed at loading texture from disk.\n:";
			return false;
		}
		if (!loadMeshData())
		{
			std::cout << "SceneLoader:: Failed at loading mesh data from disk.\n:";
			return false;
		}
		createConstantTextures();
		createLambertianMaterials();
		createLights(**scn);
		createMeshes(**scn);
		createOctreeMeshes(**scn);
		return true;
	}
}

#endif
