#ifndef RAYTR_CORE_VERTEX_H
#define RAYTR_CORE_VERTEX_H
#include "vec3.h"
namespace Raytr_Core
{
	//! A vertex class to be used in a triangle
	struct vertex
	{
		BasicMath::vec3 position;
		BasicMath::vec3 normal;
		BasicMath::vec2 uv;

		vertex() {}

		vertex(const BasicMath::vec3& position, const BasicMath::vec3& normal, const BasicMath::vec2& uv)
			:position(position), normal(normal), uv(uv)
		{}
	};

	//! vertex size in bytes
	const size_t cVERTEXBYTES = sizeof(vertex);
}
#endif

