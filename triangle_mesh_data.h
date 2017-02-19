#ifndef RAYTR_CORE_TRIANGLE_MESH_DATA_H
#define RAYTR_CORE_TRIANGLE_MESH_DATA_H
#include "aabb.h"
#include "triangle.h"
namespace Raytr_Core
{
	//! A class serving as a container and initializer of vertex and triangle buffers - to be used as an argument for triangle mesh classes
	class triangle_mesh_data
	{
	private:
		BasicMath::vec3			mPosition;		//!< x,y,z coordinates of the mesh
		BasicMath::vec3			mRotation;		//!< pitch,yaw,roll angles
		BasicMath::vec3			mScale;			//!< if you scale to 0 - can't return the vertices to the original size

		BasicMath::mat3			rotationMatrix; //!< the rotation matrix

		bounding_volume_aabb	boundingBox;	//!< the minimal axis aligned bounding box continaing all the vertices

		//! a function to scale,rotate and translate a vertex
		void					initialTransformVertexPosition(vertex& v) const
		{
			v.position = mPosition + rotationMatrix*BasicMath::mat3::scaling(mScale)*v.position;
		}

		//! a function to scale,rotate and translate a vertex's position and normal
		void					initialTransformVertexPositionAndNormal(vertex& v) const
		{
			v.position = mPosition + rotationMatrix*BasicMath::mat3::scaling(mScale)*v.position;
			v.normal = BasicMath::normalize(rotationMatrix*BasicMath::mat3::scaling(1.0f/mScale)*v.normal);
		}

		//! a function to update the bounding box with a new point
		void					initialAddVertexToBoundingBox(const vertex& v)
		{
			boundingBox.addPoint(v.position);
		}		

		vertex*					vertices;		//!< vertex buffer
		size_t					vCount;			//!< vertex count
		triangle*				triangles;		//!< triangles buffer
		size_t					tCount;			//! triangles count
	public:


		triangle_mesh_data(const BasicMath::vec3& position = BasicMath::vec3::zero,
			const BasicMath::vec3& rotation = BasicMath::vec3::zero,
			const BasicMath::vec3& scaling = BasicMath::vec3::one)
			: mPosition(position), mRotation(rotation), mScale(scaling), rotationMatrix(BasicMath::mat3::rotationXYZ(rotation)),
			vertices(nullptr), vCount(0), triangles(nullptr), tCount(0)
		{

		}

		//! creates a new vertex buffer with vCount number of elements (clears the buffer to 0)
		void initialCreateVertexBuffer(size_t vCount)
		{
			vertices = new vertex[vCount];
			memset(vertices, 0, vCount*cVERTEXBYTES);
		}

		//! creates a new triangle buffer with tCount number of elements
		void initialCreateTriangleBuffer(size_t tCount)
		{
			triangles = new triangle[tCount];
		}

		//! add a vertex to the vertex buffer when initializing, give only the position as argument
		void initialAddVertexFromPosition(const BasicMath::vec3& position)
		{
			vertices[vCount].position = position;
			initialTransformVertexPosition(vertices[vCount]);
			initialAddVertexToBoundingBox(vertices[vCount]);
			++vCount;
		}

		//! add a vertex and normal to the vertex buffer when initializing, give only the position as argument
		void initialAddVertexFromPositionAndNormal(const BasicMath::vec3& position, const BasicMath::vec3& normal)
		{
			vertices[vCount].position = position;
			vertices[vCount].normal = normal;
			initialTransformVertexPositionAndNormal(vertices[vCount]);
			initialAddVertexToBoundingBox(vertices[vCount]);
			++vCount;
		}

		//! add a vertex, normal and uv to the vertex buffer when initializing, give only the position as argument
		void initialAddVertex(const BasicMath::vec3& position, const BasicMath::vec3& normal, const BasicMath::vec2& uv)
		{
			vertices[vCount].position = position;
			vertices[vCount].normal = normal;
			vertices[vCount].uv = uv;
			initialTransformVertexPositionAndNormal(vertices[vCount]);
			initialAddVertexToBoundingBox(vertices[vCount]);
			++vCount;
		}


		//! updates the center and halfSize of the aabb
		void initialUpdateBoundingBox()
		{
			boundingBox.updateCenterAndHalfSize();
		}

		//! add a triangle to the triangle buffer when initializing, pass the triangle's vertices' indices as argument
		void initialAddTriangle(size_t i0, size_t i1, size_t i2)
		{
			//init triangle
			triangles[tCount] = triangle(&vertices[i0], &vertices[i1], &vertices[i2]);
			++tCount;
		}

		//! add a triangle to the triangle buffer when initializing, pass the triangle's vertices' indices as argument
		void initialAddTriangleAndAccumulateNormals(size_t i0, size_t i1, size_t i2)
		{
			//init triangle
			triangles[tCount] = triangle(&vertices[i0], &vertices[i1], &vertices[i2]);
			//update normals
			vertices[i0].normal += triangles[tCount].nnormal;
			vertices[i1].normal += triangles[tCount].nnormal;
			vertices[i2].normal += triangles[tCount].nnormal;
			++tCount;
		}

		//! normalizes the vertices' normals
		void initialNormalizeNormals()
		{
			//fix the normals
			for (int i = 0; i < vCount; ++i)
			{
				vertices[i].normal = BasicMath::normalize(vertices[i].normal);
			}
		}

		size_t vertexCount() const
		{
			return vCount;
		}

		//returns the triangles buffer
		const triangle* getTriangles() const
		{
			return triangles;
		}

		//returns the triangles count
		size_t triangleCount() const
		{
			return tCount;
		}

		//returns the axis aligned bounding box
		const bounding_volume_aabb& getBoundingBox() const
		{
			return boundingBox;
		}

		//! translates all of the vertices of the mesh (relative position)
		void translate(const BasicMath::vec3& translation)
		{
			mPosition += translation;
			//translate each vertex
			for (size_t i = 0; i < vCount; ++i)
			{
				vertices[i].position += translation;
			}
		}

		//! positions the mesh's origin at position (translates all the vertices) (absolute position)
		void position(const BasicMath::vec3& position)
		{
			translate(position - this->mPosition);
		}

		//! rotates all of the vertices of the mesh around the origin (absolute rotation)
		void rotate(const BasicMath::vec3& rotation)
		{

			//multiply by the new rotation
			rotationMatrix = BasicMath::mat3::rotationXYZ(rotation)*BasicMath::transpose(rotationMatrix);
			this->mRotation = rotation;
			//rotate each vertex
			for (size_t i = 0; i < vCount; ++i)
			{
				vertices[i].position = mPosition + rotationMatrix*(vertices[i].position - mPosition);
			}
		}

		//! rotates all of the vertices of the mesh around the origin (relative rotation)
		void turn(const BasicMath::vec3& rotation)
		{
			rotate(this->mRotation + rotation);
		}

		//! scales all of the vertices of the mesh (absolute scaling)
		void scaling(const BasicMath::vec3& scale)
		{
			//v' = p+R*S*v -> v = S^(-1)*R^(-1)*(v'-p)
			//v'' = p + R*S'*v = p + R*S'*S^(-1)*R^(-1)*(v'-p) -> M = R*S'*S^(-1)*R^(-1)
			BasicMath::mat3 transformationMatrix = rotationMatrix*BasicMath::mat3::scaling(scale)
				*BasicMath::mat3::scaling(1.0f / this->mScale)*BasicMath::transpose(rotationMatrix);
			this->mScale = scale;
			for (size_t i = 0; i < vCount; ++i)
			{
				vertices[i].position = mPosition + transformationMatrix*(vertices[i].position - mPosition);
			}
		}

		//! scales all of the vertices of the mesh relative to the previous scale
		void scale(const BasicMath::vec3& scaling)
		{
			this->scaling(this->mScale * scaling);
		}

		//! scales, rotates and positions a mesh in that order (absolute)
		void scalingRotationPosition(const BasicMath::vec3& position,
			const BasicMath::vec3& rotation = BasicMath::vec3::zero,
			const BasicMath::vec3& scaling = BasicMath::vec3::one)
		{
			//v' = p+R*S*v -> v = S^(-1)*R^(-1)*(v'-p)
			//v'' = p' + R'*S'*v = p + R'*S'*S^(-1)*R^(-1)*(v'-p) -> M = R'*S'*S^(-1)*R^(-1)
			BasicMath::mat3 transformationMatrix = BasicMath::mat3::rotationXYZ(rotation)*BasicMath::mat3::scaling(scaling)
				*BasicMath::mat3::scaling(1.0f / this->mScale)*BasicMath::transpose(rotationMatrix);
			this->mScale = scaling;
			for (size_t i = 0; i < vCount; ++i)
			{
				vertices[i].position = position + transformationMatrix*(vertices[i].position - mPosition);
			}
		}

		//! scales, rotates and positions a mesh in that order (relative)
		void scaleTurnTranslate(const BasicMath::vec3& translation,
			const BasicMath::vec3& rotation = BasicMath::vec3::zero,
			const BasicMath::vec3& scaling = BasicMath::vec3::one)
		{
			scalingRotationPosition(this->mPosition + translation, this->mRotation + rotation, this->mScale*scaling);
		}

	};

	std::vector<triangle_mesh_data> meshDataVector;
}

#endif
