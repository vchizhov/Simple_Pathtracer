#ifndef RAYTR_CORE_TRIANGLE_MESH_H
#define RAYTR_CORE_TRIANGLE_MESH_H
#include "rds.h"
#include "material.h"
#include "triangle_mesh_data.h"

namespace Raytr_Core
{
	//! A triangle mesh class
	class triangle_mesh : public object
	{
	protected:
		const triangle_mesh_data& mesh_data;
		const bounding_volume* boundingVolume;
	public:
		
		triangle_mesh(const triangle_mesh_data& mesh_data, material* pMaterial, const bounding_volume* boundingVolume = nullptr)
			: mesh_data(mesh_data)
		{
			this->pMaterial = pMaterial;
			//every triangle mesh uses the bounding box of the mesh_data as a bounding box
			//if one is not explicitly passed as an argument
			if (boundingVolume == nullptr)
				this->boundingVolume = &mesh_data.getBoundingBox();
			else
				this->boundingVolume = boundingVolume;
		}

		bool virtual intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{
			float tminTemp=tmin, tmaxTemp=tmax;
			if (!boundingVolume->intersect(r, tminTemp, tmaxTemp))
				return false;

			float closestSoFar = tmax;
			//for each triangle check if the ray intersects it - return the closest one
			for (int i = 0; i< mesh_data.triangleCount(); ++i)
			{
				if (mesh_data.getTriangles()[i].intersect(r, tmin, closestSoFar, info))
					closestSoFar = info.t;
			}
			//hit anything
			if (closestSoFar!=tmax)
			{
				info.pObject = this;
				return true;
			}

			return false;
		}

		const BasicMath::vec3& getBoundingBoxMin() const
		{
			return mesh_data.getBoundingBox().min();
		}

		const BasicMath::vec3& getBoundingBoxMax() const
		{
			return mesh_data.getBoundingBox().max();
		}

		//nope - have to do it for each triangle
		virtual float pdf_value(const BasicMath::vec3& o, const BasicMath::vec3& v) const { return 0.0f; }
		//nope - have to do it for each triangle
		virtual BasicMath::vec3 random(const BasicMath::vec3& o) const { return BasicMath::vec3(0, 1, 0); }
		//nope - have to do it for each triangle
		virtual float pdf_value_area() const { return 0.0f; }
		//nope - have to do it for each triangle
		virtual BasicMath::vec3 random_area() const { return BasicMath::vec3(0, 1, 0); }
	};

}

#endif
