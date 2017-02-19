#ifndef RAYTR_CORE_OCTREE_H
#define RAYTR_CORE_OCTREE_H
#include "triangle_mesh.h"
#include <vector>

namespace Raytr_Core
{


	//! octree acceleration structure
	class octree
	{
	private:
		//a structure for ordering intersections
		struct hitIndexT
		{
			float t;
			int index;

			hitIndexT() : t(BasicMath::cINFINITY) {}

			bool operator<(const hitIndexT& other)
			{
				return t < other.t;
			}
		};

		static inline void sort8_sorting_network_simple_swap(hitIndexT arr[8]);
		

	protected:
		octree* child[8];
		const triangle** data;
		size_t tCount;

	public:
		const bounding_volume_aabb boundingBox;

		explicit octree(const bounding_volume_aabb& boundingBox)
			: boundingBox(boundingBox)
		{
			//initialize all child nodes to nullptr
			child[0] = nullptr;
			child[1] = nullptr;
			child[2] = nullptr;
			child[3] = nullptr;
			child[4] = nullptr;
			child[5] = nullptr;
			child[6] = nullptr;
			child[7] = nullptr;
			//initialize the current node's data to nullptr
			data = nullptr;
			//initialize the triangle count for the current node to 0
			tCount = 0;
		}

		void buildOctree(const std::vector<const triangle*>& arr, int depth, size_t maxElementsCount)
		{
			tCount = arr.size();
			//recursion's end conditions:
			//if the depth is 0 or if the triangleCount at the node is lower than maxElementsCount
			//stop here - leaf node - record data
			if (depth == 0 || tCount <= maxElementsCount)
			{

				//if it is not 0
				if (tCount != 0)
				{
					//add the triangles pointers to the current node's data
					this->data = new const triangle*[tCount];
					for (int i = 0; i < tCount; ++i)
					{
						data[i] = arr[i];
					}
				}
				//end recursion
				return;
			}

			//construct the children
			child[0] = new octree(bounding_volume_aabb(boundingBox.min(), 0.5*(boundingBox.min() + boundingBox.max())));

			child[1] = new octree(bounding_volume_aabb(BasicMath::vec3(0.5*(boundingBox.min().x+boundingBox.max().x), boundingBox.min().y, boundingBox.min().z),
				BasicMath::vec3(boundingBox.max().x, 0.5*(boundingBox.min().y+boundingBox.max().y), 0.5*(boundingBox.min().z + boundingBox.max().z))));

			child[2] = new octree(bounding_volume_aabb(BasicMath::vec3(0.5*(boundingBox.min().x + boundingBox.max().x), 0.5*(boundingBox.min().y + boundingBox.max().y), boundingBox.min().z),
				BasicMath::vec3(boundingBox.max().x, boundingBox.max().y, 0.5*(boundingBox.min().z + boundingBox.max().z))));

			child[3] = new octree(bounding_volume_aabb(BasicMath::vec3(boundingBox.min().x, 0.5*(boundingBox.min().y + boundingBox.max().y), boundingBox.min().z),
				BasicMath::vec3(0.5*(boundingBox.min().x + boundingBox.max().x), boundingBox.max().y, 0.5*(boundingBox.min().z + boundingBox.max().z))));

			child[4] = new octree(bounding_volume_aabb(BasicMath::vec3(boundingBox.min().x, boundingBox.min().y, 0.5*(boundingBox.min().z + boundingBox.max().z)),
				BasicMath::vec3(0.5*(boundingBox.min().x + boundingBox.max().x), 0.5*(boundingBox.min().y + boundingBox.max().y), boundingBox.max().z)));

			child[5] = new octree(bounding_volume_aabb(BasicMath::vec3(0.5*(boundingBox.min().x + boundingBox.max().x), boundingBox.min().y, 0.5*(boundingBox.min().z + boundingBox.max().z)),
				BasicMath::vec3(boundingBox.max().x, 0.5*(boundingBox.min().y + boundingBox.max().y), boundingBox.max().z)));

			child[6] = new  octree(bounding_volume_aabb(0.5*(boundingBox.min() + boundingBox.max()), boundingBox.max()));

			child[7] = new octree(bounding_volume_aabb(BasicMath::vec3(boundingBox.min().x, 0.5*(boundingBox.min().y + boundingBox.max().y), 0.5*(boundingBox.min().z + boundingBox.max().z)),
				BasicMath::vec3(0.5*(boundingBox.min().x + boundingBox.max().x), boundingBox.max().y, boundingBox.max().z)));

			
			//for each child build a vector of the triangles that intersect the child's aabb
			for (int k = 0; k < 8; ++k)
			{
				std::vector<const triangle*> inside;
				//for each triangle decide whether to add it in the child's vector
				for (size_t i = 0; i < arr.size(); ++i)
				{
					//if the triangle aabb intersects the bounding child's aabb
					
					//is faster for intersections, but otherwise is much slower at tree bulding
					/*if (arr[i]->intersectsAABB(child[k]->boundingBox))
						inside.push_back(arr[i]);*/
					if (child[k]->boundingBox.intersectsAABB(arr[i]->boundingBox))
						inside.push_back(arr[i]);
					
				}
				//after having built the child's vector - pass it to the child and recursvely continue along the the tree
				
				/*
				//this does not work because, unless it's a pathological case
				//where the aabb is perfectly the same as multiple triangles' aabbs
				//(more precisely > maxElementsCount)
				//eventually it will be subdivided as finely as to exclude some of the triangles
				//Sure, an optimization is possible, but that would be through
				//either correct triangle-aabb intersection
				//or through another structure that would be useful if the tCount doesn't change
				//over some interval of depth
				if(inside.size()==tCount)
					child[k]->buildOctree(inside, depth - 1, tCount);
				else*/
					child[k]->buildOctree(inside, depth - 1, maxElementsCount);
			}
		}

		bool intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{
			float closestSoFar = tmax;
			if(child[0]!=nullptr) //if it's not a leaf node - check children
			{
				//for each child node - check whether the ray intersects its bounding box
				int intersectionCount = 0;
				hitIndexT intersectionInfo[8];
				for (int i = 0; i < 8; ++i)
				{
					float tminTemp = tmin, tmaxTemp = closestSoFar;
					//if there's an intersection:
					if (boundingBox.intersect(r, tminTemp, tmaxTemp))
					{
						//record intersections with node, and the distance along the ray (t) for these itnersections
						intersectionInfo[intersectionCount].index = i;
						intersectionInfo[intersectionCount].t = tminTemp;
						++intersectionCount;
					}
					
				}
				//order the intersections by closest to furthest
				//this fucks up things big time - need better
				std::sort(intersectionInfo, intersectionInfo + intersectionCount);
				//sort8_sorting_network_simple_swap(intersectionInfo);
				//for each intersected aabb check whether the ray intersects any geometry inside
				//if it does, return since that's the closest
				for (int i = 0; i < intersectionCount; ++i)
				{
					if (child[intersectionInfo[i].index]->intersect(r, tmin, closestSoFar, info))
					{
						//need to update info.pObject in the metod calling this one
						return true;
					}
				}
			}
			else//if it's a leaf node - find the intersections with the triangles
			{
				for (size_t i = 0; i < tCount; ++i)
				{
					if (data[i]->intersect(r, tmin, closestSoFar, info))
						closestSoFar = info.t;
				}
			}

			//hit anything from the current node's geometry?
			if (closestSoFar != tmax)
				//need to update info.pObject in the metod calling this one
				return true;

			return false;
		}

		~octree()
		{
			if(data!=nullptr)
				delete[] data;
		}
	};

	inline void octree::sort8_sorting_network_simple_swap(hitIndexT* arr)
	{
#define min(x, y) (x<y?x:y)
#define max(x, y) (x<y?y:x) 
#define SWAP(x,y) { const hitIndexT a = min(arr[x], arr[y]);const hitIndexT b = max(arr[x], arr[y]);arr[x] = a; arr[y] = b; }
		SWAP(0, 1); SWAP(2, 3); SWAP(4, 5); SWAP(6, 7);
		SWAP(0, 2); SWAP(1, 3); SWAP(4, 6); SWAP(5, 7);
		SWAP(1, 2); SWAP(5, 6); SWAP(0, 4); SWAP(3, 7);
		SWAP(1, 5); SWAP(2, 6);
		SWAP(1, 4); SWAP(3, 6);
		SWAP(2, 4); SWAP(3, 5);
		SWAP(3, 4);
#undef SWAP
#undef min
#undef max
		}

	//! triangle mesh using an octree structure for faster intersections
	class triangle_octree_mesh : public triangle_mesh
	{
	private:
		octree root;

		//!helper function to build the tree
		void build(int depth, int maxElementsCount)
		{
			//create the vector for the input of the octree building
			std::vector<const triangle*> inside(mesh_data.triangleCount());

			for (size_t i = 0; i < mesh_data.triangleCount(); ++i)
			{
				inside[i] = &mesh_data.getTriangles()[i];
			}
			root.buildOctree(inside, depth, maxElementsCount);
		}

	public:
		triangle_octree_mesh(const triangle_mesh_data& mesh_data, material* pMaterial, int depth, size_t maxElementsCount, const bounding_volume* boundingVolume=&cDefaultboundingVolume)
			: triangle_mesh(mesh_data, pMaterial, boundingVolume), root(mesh_data.getBoundingBox())
		{
			//build the tree
			build(depth, maxElementsCount);
		}

		bool virtual intersect(const ray& r, float tmin, float tmax, intersection_info& info) const
		{
			//check against the bounding volume
			//if a custom one is not explicitly passed to the construct bounding_volume_none is used
			float tminTemp = tmin, tmaxTemp = tmax;
			if (!boundingVolume->intersect(r, tminTemp, tmaxTemp))
				return false;

			//check against the root's aabb
			tminTemp = tmin, tmaxTemp = tmax;
			if (!root.boundingBox.intersect(r, tminTemp, tmaxTemp))
				return false;

			//if the ray does not intersect any of the geometry in the aabb - then there's no intersection
			if (!root.intersect(r, tmin, tmax, info))
				return false;

			//does intersect - update the pObject pointer
			info.pObject = this;
			return true;
		}

	};
}

#endif
