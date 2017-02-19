#ifndef RAYTR_CORE_TRIANGLE_CUBOID_H
#define RAYTR_CORE_TRIANGLE_CUBOID_H

#include "triangle_mesh_data.h"

namespace Raytr_Core
{
	//! a factory to create various geometric primitives
	class TrianglePrimitivesFactory
	{
	private:
		//! cuboid
		void generateCuboidBuffers(triangle_mesh_data& mesh_data) const
		{
			BasicMath::vec3 v[8] = {
				BasicMath::vec3(-1, -1, -1),
				BasicMath::vec3(1, -1, -1),
				BasicMath::vec3(1, -1, 1),
				BasicMath::vec3(-1, -1, 1),
				BasicMath::vec3(-1, 1, -1),
				BasicMath::vec3(1, 1, -1),
				BasicMath::vec3(1, 1, 1),
				BasicMath::vec3(-1, 1, 1)
			};

			BasicMath::vec3 n[6] = {
				BasicMath::vec3(0,0,-1),
				BasicMath::vec3(1,0,0),
				BasicMath::vec3(0,0,1),
				BasicMath::vec3(-1,0,0),
				BasicMath::vec3(0,-1,0),
				BasicMath::vec3(0,1,0),
			};

			meshDataVector.back().initialCreateVertexBuffer(24);
			meshDataVector.back().initialCreateTriangleBuffer(24);
			//4 sides
			for (int i = 0; i < 4; ++i)
			{
				int i1 = (i + 1) % 4;
				mesh_data.initialAddVertex(v[i], n[i], BasicMath::vec2(0, 0));
				mesh_data.initialAddVertex(v[i1], n[i], BasicMath::vec2(1, 0));
				mesh_data.initialAddVertex(v[i1 + 4], n[i], BasicMath::vec2(1, 1));
				mesh_data.initialAddVertex(v[i + 4], n[i], BasicMath::vec2(0, 1));
			}
			//bottom and top
			mesh_data.initialAddVertex(v[1], n[4], BasicMath::vec2(1, 0));
			mesh_data.initialAddVertex(v[0], n[4], BasicMath::vec2(0, 0));
			mesh_data.initialAddVertex(v[3], n[4], BasicMath::vec2(0, 1));
			mesh_data.initialAddVertex(v[2], n[4], BasicMath::vec2(1, 1));
			
			mesh_data.initialAddVertex(v[4], n[5], BasicMath::vec2(0, 0));
			mesh_data.initialAddVertex(v[5], n[5], BasicMath::vec2(1, 0));
			mesh_data.initialAddVertex(v[6], n[5], BasicMath::vec2(1, 1));
			mesh_data.initialAddVertex(v[7], n[5], BasicMath::vec2(0, 1));

			//generate triangles
			for (int i = 0; i < 6; ++i)
			{
				//0,3,1
				mesh_data.initialAddTriangle(4 * i, 4 * i + 3, 4 * i + 1);
				//0,2,1
				mesh_data.initialAddTriangle(4 * i + 1, 4 * i + 3, 4 * i + 2);
			}

		}

		//! sperical cube helper function
		void generateSubdividedSphericalFace(triangle_mesh_data& mesh_data, const BasicMath::vec3& p0, const BasicMath::vec3& e1, const BasicMath::vec3& e2, float radius, size_t segments, size_t slices) const
		{
			float dx = 1.0f / float(segments); float dy = 1.0f / float(slices);
			BasicMath::vec3 u, v;
			size_t vOffset = mesh_data.vertexCount();
			for (int j = 0; j <= slices; ++j)
			{
				v = j*dy*e2;
				for (int i = 0; i <= segments; ++i)
				{
					u = i*dx*e1;
					u = BasicMath::normalize(p0 + u + v);
					mesh_data.initialAddVertex(radius*u, u, BasicMath::vec2(i/float(segments),j/float(slices)));
				}
			}

			for (int j = 0; j < slices; ++j)
			{
				int indexY = vOffset + j*(segments + 1);
				for (int i = 0; i < segments; ++i)
				{
					//0,3,1
					mesh_data.initialAddTriangle(indexY + i, indexY + segments + 1 + i, indexY + i + 1);
					//1,3,2
					mesh_data.initialAddTriangle(indexY + i + 1, indexY + segments + 1 + i, indexY + segments + 2 + i);
				}
			}
		}

		//! spherical cube
		void generateSphericalCubeBuffers(triangle_mesh_data& mesh_data, float radius, size_t segments, size_t slices)
		{
			meshDataVector.back().initialCreateVertexBuffer(6 * (segments + 1) * (slices + 1));
			meshDataVector.back().initialCreateTriangleBuffer(12 * slices* segments);
			//generate vertices
			BasicMath::vec3 v[18] = {
				//front
				BasicMath::vec3(-0.5, -0.5, -0.5),
				BasicMath::vec3(1, 0, 0),
				BasicMath::vec3(0, 1, 0),
				//right
				BasicMath::vec3(0.5, -0.5, -0.5),
				BasicMath::vec3(0, 0, 1),
				BasicMath::vec3(0, 1, 0),
				//back
				BasicMath::vec3(0.5, -0.5, 0.5),
				BasicMath::vec3(-1, 0, 0),
				BasicMath::vec3(0, 1, 0),
				//left
				BasicMath::vec3(-0.5, -0.5, 0.5),
				BasicMath::vec3(0, 0, -1),
				BasicMath::vec3(0, 1, 0),
				//bottom
				BasicMath::vec3(0.5, -0.5, -0.5),
				BasicMath::vec3(-1, 0, 0),
				BasicMath::vec3(0, 0, 1),
				//top
				BasicMath::vec3(-0.5, 0.5, -0.5),
				BasicMath::vec3(1, 0, 0),
				BasicMath::vec3(0, 0, 1)
			};

			for (int i = 0; i < 6; ++i)
			{
				generateSubdividedSphericalFace(mesh_data, v[3 * i], v[3 * i + 1], v[3 * i + 2], radius, segments, slices);
			}

		}


		TrianglePrimitivesFactory() {}

	public:
		static TrianglePrimitivesFactory& init()
		{
			static TrianglePrimitivesFactory singleton;
			return singleton;
		}

		triangle_mesh_data& createTriangleMeshCuboid(const BasicMath::vec3& position = BasicMath::vec3::zero,
			const BasicMath::vec3& rotation = BasicMath::vec3::zero,
			const BasicMath::vec3& scaling = BasicMath::vec3::one)
		{
			meshDataVector.push_back(triangle_mesh_data(position, rotation, scaling));
			
			generateCuboidBuffers(meshDataVector.back());

			return meshDataVector.back();
		}

		triangle_mesh_data& createTriangleMeshSphericalCube(float radius , const BasicMath::vec3& position = BasicMath::vec3::zero,
			const BasicMath::vec3& rotation = BasicMath::vec3::zero,
			const BasicMath::vec3& scaling = BasicMath::vec3::one, int slices = 8, int segments = 8)
		{
			meshDataVector.push_back(triangle_mesh_data(position, rotation, scaling));

			generateSphericalCubeBuffers(meshDataVector.back(), radius, segments, slices);

			return meshDataVector.back();
		}
	};

}

#endif