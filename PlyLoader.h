#ifndef RAYTR_CORE_PLY_LOADER_H
#define RAYTR_CORE_PLY_LOADER_H

#include "rply.h"
#include "triangle_mesh_data.h"

namespace Raytr_Core
{
	namespace PlyLoader
	{
		BasicMath::vec3 vpos;
		//callback functions to record the x,y,z coordinates
		static int vertexX_cb(p_ply_argument argument) {
			//read ply argument
			ply_get_argument_user_data(argument, NULL, NULL);
			//record x coordinate
			vpos.x = ply_get_argument_value(argument);
			return 1;
		}
		static int vertexY_cb(p_ply_argument argument) {
			//read ply argument
			ply_get_argument_user_data(argument, NULL, NULL);
			//record y coordinate
			vpos.y = ply_get_argument_value(argument);
			return 1;
		}
		static int vertexZ_cb(p_ply_argument argument) {
			//read ply argument
			ply_get_argument_user_data(argument, NULL, NULL);
			//record z coordinate
			vpos.z = ply_get_argument_value(argument);
			//add vertex to mesh data
			meshDataVector.back().initialAddVertexFromPosition(vpos);
			return 1;
		}


		size_t i0, i1, i2;
		//callback function to record the triangles
		static int face_cb(p_ply_argument argument) {
			long length, value_index;
			ply_get_argument_property(argument, NULL, &length, &value_index);
			switch (value_index) {
			case 0:
				i0 = ply_get_argument_value(argument);
				break;
			case 1:
				i1 = ply_get_argument_value(argument);
				break;
			case 2:
				i2 = ply_get_argument_value(argument);
				meshDataVector.back().initialAddTriangleAndAccumulateNormals(i0, i1, i2);
				break;
			default:
				break;
			}
			return 1;
		}

		//loads a triangles mesh from a .ply file, and scales, rotates and translates it, in that order
		int loadPlyMesh(const char* filename, BasicMath::vec3 pos = BasicMath::vec3::zero, BasicMath::vec3 rot = BasicMath::vec3::zero, BasicMath::vec3 size = BasicMath::vec3::one)
		{
			//open ply file
			p_ply ply = ply_open(filename, NULL, 0, NULL);
			//managed to open it?
			if (!ply)
			{
				std::cout << "PlyMeshLoader: Failed to open file: " << filename << "\n";
				return 0;
			}
			//is the header ok?
			if (!ply_read_header(ply))
			{
				std::cout << "PlyMeshLoader: File \"" << filename << "\" does not appear to be a correct .ply file.\n";
				return 0;
			}
			//number of vertices
			//for each x execute vertexX_cb
			size_t nvertices = ply_set_read_cb(ply, "vertex", "x", vertexX_cb, NULL, 0);
			//create a new triangle_data_mesh instance to hold the mesh being loaded
			meshDataVector.push_back(triangle_mesh_data(pos, rot, size));
			//allocate enough space for these vertices
			meshDataVector.back().initialCreateVertexBuffer(nvertices);
			//for each y execute vertexY_cb
			ply_set_read_cb(ply, "vertex", "y", vertexY_cb, NULL, NULL);
			//for each z execute vertexZ_cb
			ply_set_read_cb(ply, "vertex", "z", vertexZ_cb, NULL, NULL);
			//number of triangles
			//for each element of the list face execute face_cb
			size_t ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, NULL, 0);
			//allocatre enough space
			meshDataVector.back().initialCreateTriangleBuffer(ntriangles);
			//try reading the file
			std::cout << "PlyMeshLoader: Preparing to load model.\n";
			if (!ply_read(ply))
			{
				std::cout << "PlyMeshLoader: Failed reading ply file.\n";
				return 0;
			}
			//close the file and the ply interface
			ply_close(ply);

			//update bounding box and normalize normals
			meshDataVector.back().initialUpdateBoundingBox();
			meshDataVector.back().initialNormalizeNormals();
			std::cout << "PlyMeshLoader: Mesh loaded successfully.\n";
			return 1;
		}
	};
}

#endif
