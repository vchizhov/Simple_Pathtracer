//TODO:
// add SIMD math library
//fix sort in octree

#include "tigr.h"

#pragma comment(lib,"d3d9.lib")

#include "threads_distribution.h"
#include "intensity_array.h"
#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "parallelogram.h"
#include "filter.h"
#include "triangle_cuboid.h"
#include "PlyLoader.h"
#include "octree.h"
#include "triangle_mesh_data.h"
#include "high_precision_timer.h"
#include "scene_loader.h"

using namespace BasicMath;
using namespace Raytr_Core;


class BackgroundColor
{
public:
	virtual vec3 value(const ray& r) const = 0;
};

class ConstantBackgroundColor : public BackgroundColor
{
public:
	virtual vec3 value(const ray& r) const
	{
		return vec3(0);
	}
};

class GradientBackgroundColor : public BackgroundColor
{
private:
	BasicMath::vec3 up, down;
public:
	GradientBackgroundColor(const BasicMath::vec3& down = vec3(1), const BasicMath::vec3& up = vec3(0.5, 0.7, 1.0))
		:up(up), down(down)
	{

	}

	virtual vec3 value(const ray& r) const
	{
		float t = 0.5f*(r.direction.y + 1.0f);
		return (1.0f - t)*down + t*up;
	}
};

struct RussianRoulette
{
	float minP, maxP;
	float mulFactor;

	RussianRoulette() :minP(0), maxP(1), mulFactor(2) {}
};

struct RenderOptions
{
	RenderOptions() : x(0), y(0), width(100), height(100), 
		numThreads(1), dxCoef(1), dyCoef(1), samples(1), shadowRaysCount(1), scatteredRaysPow(0), bounces(5), backgroundColor(nullptr)
	{}

	int x, y;							//!< upper left corner of the rectangle
	int width, height;					//!< size of the rectangle
	int numThreads;						//!< number of threads to run
	float dxCoef, dyCoef;				//!< coefficients by which to multiply the threads dsitribution rectangles
	int samples;						//!< numbers of initial samples per pixel
	int shadowRaysCount;				//!< number of shadow rays per iteration
	int scatteredRaysPow;				//!< a number k describing how many rays will be generated for indirect illumination: 2^(2*k)
	int bounces;						//!< maximum number of bounces allowe
	BackgroundColor* backgroundColor;	//!< background color
	RussianRoulette rrOptions;			//!< russian roulette parameters

};

BasicMath::vec3 castRay(Raytr_Core::ray r, const Raytr_Core::scene& scn, const RenderOptions& options, vec3& directIllumination)
{
	intersection_info info;
	scattering_info sinfo;

	vec3 color(0);
	vec3 intensity(1);
	vec3 Le(0), Ld(0), Lr(0);

	float r1, r2;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//0) Does the ray hit anything?
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!scn.intersect(r, cEPSILON, cINFINITY, info))
	{
		//the ray does not hit the scene - return the background color
		Le += options.backgroundColor->value(r);
		directIllumination += Le;
		return color;
	}

	//add the emitted color of the intersected material
	Le += info.pObject->pMaterial->emitted(r, info);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2) If the material fully absorbs the ray and does not scatter other rays - just return the emitted color
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!info.pObject->pMaterial->scatter(r, info, sinfo))
	{
		directIllumination += Le;
		return color;
	}
	
	

	//for each object in the scene check whether it's an emitter
	int shadowRaysCount1 = 0;
	for (object* iter : scn.objects)
	{

		//if the object emits - sample it
		if (iter->pMaterial->emits)
		{
			//cast n shadow rays
			for (int j = 0; j < options.shadowRaysCount; ++j)
			{
				//generate a direction in the cosine lobe subtended by the sphere
				vec3 shadowRayDir1 = iter->random(info.position);
				//calulcate the pdf
				float shadowRayPdf1 = iter->pdf_value(info.position, shadowRayDir1);
				//transform the shadowray's direction to the coordinates we need
				shadowRayDir1 = createCoordinateSystem(normalize(iter->center - info.position))*shadowRayDir1;
				//the ray
				ray lightSampleRay1(info.position + info.normal*cEPSILON, shadowRayDir1);
				//find out the cos between this ray and the normal at the intersection point
				float cosLDN1 = dotProduct(info.normal, lightSampleRay1.direction);
				//if cos is 0 or negative continue to the next shadow ray
				if (cosLDN1 <= 0)
					continue;
				//if not, check whether it intersects anything before the light
				intersection_info tempInfo1;
				scn.intersect(lightSampleRay1, cEPSILON, cINFINITY, tempInfo1);

				//increment the counter for the shadow rays
				++shadowRaysCount1;

				//if it does intersect an object that's not the emitter before reaching it, continue on to the next shadow ray
				if (tempInfo1.pObject != iter)
				{
					continue;
				}
				else //if it does not - calculate the direct illumination
				{
					Ld += cosLDN1*tempInfo1.pObject->pMaterial->emitted(lightSampleRay1, tempInfo1)*info.pObject->pMaterial->brdf(r.direction, lightSampleRay1.direction, info) / shadowRayPdf1;
				}
			}
		}
	}

	if (shadowRaysCount1>0)
		Ld /= float(shadowRaysCount1);
	
	Le += Ld;
	directIllumination += Le;

	for (int bounce = 0; bounce < options.bounces; ++bounce)
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//1) The ray intersects an object:
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//add the emitted color if we're not sampling it as a light source:


		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//2) If the material fully absorbs the ray and does not scatter other rays - just return the emitted color
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (!info.pObject->pMaterial->scatter(r, info, sinfo))
		{
			return color;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//2.5) Direct illumination
		//////////////////////////////////////////////////////////////////////////////////////////////////////////


		//for each object in the scene check whether it's an emitter
		vec3 directIlluminationColor(0);
		int shadowRaysCount = 0;
		for (object* iter : scn.objects)
		{

			//if the object emits - sample it
			if (iter->pMaterial->emits)
			{
				//cast n shadow rays
				for (int j = 0; j < options.shadowRaysCount; ++j)
				{
					//generate a direction in the cosine lobe subtended by the sphere
					vec3 shadowRayDir = iter->random(info.position);
					//calulcate the pdf
					float shadowRayPdf = iter->pdf_value(info.position, shadowRayDir);
					//transform the shadowray's direction to the coordinates we need
					shadowRayDir = createCoordinateSystem(normalize(iter->center - info.position))*shadowRayDir;
					//the ray
					ray lightSampleRay(info.position + info.normal*cEPSILON, shadowRayDir);
					//find out the cos between this ray and the normal at the intersection point
					float cosLDN = dotProduct(info.normal, lightSampleRay.direction);
					//if cos is 0 or negative continue to the next shadow ray
					if (cosLDN <= 0)
						continue;
					//if not, check whether it intersects anything before the light
					intersection_info tempInfo;
					scn.intersect(lightSampleRay, cEPSILON, cINFINITY, tempInfo);

					//increment the counter for the shadow rays
					++shadowRaysCount;

					//if it does intersect an object that's not the emitter before reaching it, continue on to the next shadow ray
					if (tempInfo.pObject != iter)
					{
						continue;
					}
					else //if it does not - calculate the direct illumination
					{
						directIlluminationColor += tempInfo.pObject->pMaterial->emitted(lightSampleRay, tempInfo)*info.pObject->pMaterial->brdf(r.direction, lightSampleRay.direction, info)*cosLDN /
							shadowRayPdf;
					}
				}
			}
		}

		if (shadowRaysCount>0)
			color += (directIlluminationColor*intensity / float(shadowRaysCount));


		//Russian roulette - randomly terminate a path with a probability inversely proportional to the intensity
		float p = max(intensity);
		p = std::max(options.rrOptions.minP, std::min(options.rrOptions.maxP, options.rrOptions.mulFactor*p));
		if (p < uniform_distribution(generator))
		{
			return color;
		}
		//keep the estimator unbiased - scale the intensity
		intensity *= 1 / p;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//3) The ray is scattered
		//////////////////////////////////////////////////////////////////////////////////////////////////////////

		//generate uniformly distributed random numbers in [0,1]
		r1 = uniform_distribution(generator);
		r2 = uniform_distribution(generator);

		//generate a direction with a cosine weighted distribution:
		BasicMath::vec3 direction = cosineWeightedHemisphereSample(r1, r2);
		//calculate the pdf_value of this direction
		float pdf_value = BasicMath::cosineWeightedHemispherePdf(direction);
		//transorm the direction to the coordinate system of the intersection point
		direction = createCoordinateSystem(info.normal)*direction;

		//if the transformed direction's not in the upper hemisphere around the normal of the intersection point - has 0 contribution
		//no use continuing since intensity=0
		float cosDN = dotProduct(info.normal, direction);
		if (cosDN <= 0)
			return color;

		//scattered ray
		r = ray(info.position + info.normal*cEPSILON, direction);

		//accumulate the intensity
		intensity *= info.pObject->pMaterial->brdf(r.direction, direction, info)*cosDN / pdf_value;

		if (!scn.intersect(r, cEPSILON, cINFINITY, info)) //the ray does not hit the scene
		{
			color += options.backgroundColor->value(r)*intensity;
			return color;
		}

	}

	//ran out of bounces - return the last color
	return color;
}

void copyArrayToBmp(const intensity_array& src, Tigr* dst, const ThreadsDistribution::rectangle& rect, int numSamples)
{
	int i;
	vec3 col;
	for (int y = rect.pos.y; y<rect.size.y + rect.pos.y; ++y)
	{
		for (int x = rect.pos.x; x < rect.size.x + rect.pos.x; ++x)
		{
			//write into buffer
			i = y*dst->w + x;
			//divide by the number of samples, gamma correct and clamp values in the 0-1 range
			col = BasicMath::clamp(pow(src(x, y) / numSamples, float(0.4545)), BasicMath::vec3::zero, BasicMath::vec3::one);
			dst->pix[i].r = 255 * col.x;
			dst->pix[i].g = 255 * col.y;
			dst->pix[i].b = 255 * col.z;
		}
	}
}

void render_thread(intensity_array& directIllumination, intensity_array& indirectIllumiantion, intensity_array& accumulatedIntensity, const Raytr_Core::camera& cam, const Raytr_Core::scene& scn, const ThreadsDistribution::rectangle& rect, const RenderOptions& options)
{
	ThreadsDistribution::point pos;
	float ndcX, ndcY;
	for (int y = rect.pos.y; y<rect.size.y + rect.pos.y; ++y)
	{
		for (int x = rect.pos.x; x < rect.size.x + rect.pos.x; ++x)
		{
			//map y from [0,height-1] to [1,-1]
			ndcY = 2 * float(y + BasicMath::uniform_distribution(BasicMath::generator)) / (1 - options.height) + 1;
			//map x from [0,width-1] to [-1,1]
			ndcX = 2 * float(x + BasicMath::uniform_distribution(BasicMath::generator)) / (options.width - 1) - 1;
			indirectIllumiantion(x, y) += castRay(cam.getRay(ndcX, ndcY), scn, options, directIllumination(x,y));
			accumulatedIntensity(x, y) = indirectIllumiantion(x, y) + directIllumination(x, y);
		}
	}
}

void render(Tigr* bmp, const Raytr_Core::camera& cam, const Raytr_Core::scene& scn, const RenderOptions& options)
{
	intensity_array directIllumination(options.width, options.height);
	intensity_array indirectIllumination(options.width, options.height);
	intensity_array accumulatedIntensity(options.width, options.height);

	int dx = options.dxCoef*float(options.width) / float(options.numThreads);
	int dy = options.dyCoef*float(options.height) / float(options.numThreads);
	std::vector<ThreadsDistribution::rectangles> rects = ThreadsDistribution::populateWithRectangles(
		ThreadsDistribution::rectangle(ThreadsDistribution::point(options.x, options.y), ThreadsDistribution::point(options.width, options.height)),
		ThreadsDistribution::point(dx, dy), options.numThreads);

	//find the biggest number of rectangles between the rectangle sets per thread
	int maxR = 0;
	for (int i = 0; i < options.numThreads; ++i)
	{
		if (maxR < rects[i].rects.size())
			maxR = rects[i].rects.size();
	}

	//dispatch a thread for each rectangle of its set
	//update after the threads finish their rectangles
	std::vector<std::thread> threads;
	for (int s = 1; s <= options.samples; ++s)
	{
		//keep track of how much time each frame takes
		HighPrecisionTimer sampleTime;
		sampleTime.StartCounter();
		for (int k = 0; k < maxR; ++k)
		{
			//do raytracing work
			for (int i = 0; i < options.numThreads; ++i)
			{
				if (rects[i].rects.size() > k)
				{
					threads.push_back(std::thread(render_thread, std::ref(directIllumination), std::ref(indirectIllumination),
						std::ref(accumulatedIntensity), std::ref(cam), std::ref(scn), rects[i].rects[k], options));
				}
			}
			for (int i = 0; i < options.numThreads; ++i)
			{
				if (rects[i].rects.size() > k)
				{
					threads[i].join();
				}
			}
			threads.clear();

			//copy  buffer to screen
			for (int i = 0; i < options.numThreads; ++i)
			{
				if (rects[i].rects.size() > k)
				{
					threads.push_back(std::thread(copyArrayToBmp, std::ref(accumulatedIntensity), bmp, rects[i].rects[k], s));
				}
			}

			for (int i = 0; i < options.numThreads; ++i)
			{
				if (rects[i].rects.size() > k)
				{
					threads[i].join();
				}
			}

			threads.clear();
			//check for escape->terminate early
			if (tigrKeyHeld(bmp, TK_ESCAPE))
			{
				return;
			}
			tigrUpdate(bmp);
		}
		std::cout << "Sample " << s << " time: " << sampleTime.GetCounter() << "\n";
	}
	//noise filter addition
	intensity_array filteredIntensity(options.width, options.height);
	median_filter filter;
	filter.filter_image(indirectIllumination, filteredIntensity);
	filteredIntensity += directIllumination;
	copyArrayToBmp(accumulatedIntensity, bmp, ThreadsDistribution::rectangle(0, 0, options.width, options.height), options.samples);
	tigrUpdate(bmp);
}

void cornell_box_scene(scene** scn, camera** cam, float aspect)
{
	pdf* u_pdf = new cosine_weighted_hemisphere_pdf();

	texture* red = new constant_texture(vec4(0.65, 0.05, 0.05, 1));
	float s = 2;
	texture* diffuse_light = new constant_texture(vec4(s,s,s,1));
	texture* green = new constant_texture(vec4(0.12, 0.45, 0.15, 1));
	texture* white = new constant_texture(vec4(0.73, 0.73, 0.73, 1));
	texture* tex0 = new image_texture(tigrLoadImage("tex0.png"));
	texture* tex2 = new image_texture(tigrLoadImage("tex2.png"));
	texture* tex3 = new image_texture(tigrLoadImage("tex3.png"));
	texture* tex4 = new image_texture(tigrLoadImage("tex6.png"), vec4(3));
	texture* mytex = new image_texture(tigrLoadImage("mytex.png"), vec4(3));

	material* mred = new lambertian_material(red, u_pdf);
	material* mwhite = new lambertian_material(white, u_pdf);
	material* mgreen = new lambertian_material(green, u_pdf);
	material* mlight = new emitter_material(diffuse_light, u_pdf);
	

	material* msphere = new lambertian_material(tex0, u_pdf);
	material* mwall = new lambertian_material(tex2, u_pdf);
	material* mwall2 = new lambertian_material(tex3, u_pdf);
	material* mlight2 = new emitter_material(tex4, u_pdf);
	material* mmytex = new lambertian_material(mytex, u_pdf);
	int i = 0;
	
	
	if (!PlyLoader::loadPlyMesh("bun_zipper_res.ply", vec3(350, -80, 190), vec3(0,-M_PI*0.75,0), vec3(2000, 2000, 2000)))
	{
		std::cout << "Failed at mesh loading - will exit.\n";
		return;
	}
	triangle_mesh* meshPly = new triangle_mesh(meshDataVector.back(), mred);
	size_t maxElements = 40;
	int depth = meshDataVector.back().triangleCount() / (maxElements*log(8));
	std::cout << depth << "\n";
	triangle_octree_mesh* octreeMesh = new triangle_octree_mesh(meshDataVector.back(), mwhite,depth,maxElements);
	
	TrianglePrimitivesFactory& factory = TrianglePrimitivesFactory::init();
	factory.createTriangleMeshCuboid(vec3(378, 150, 190), vec3(M_PI/4, M_PI/4, 0), vec3(50, 150, 50));
	triangle_mesh* meshCuboid = new triangle_mesh(meshDataVector.back(), mwall);
	factory.createTriangleMeshSphericalCube(60, vec3(378, 150, 190),vec3(0,M_PI/4,0), vec3(1), 4,4);
	/*size_t maxElements = 40;
	int depth = meshDataVector.back().triangleCount() / (maxElements*log(8));
	std::cout << depth << "\n";
	triangle_mesh* meshSphericalCube = new triangle_octree_mesh(meshDataVector.back(), mwhite, depth, maxElements);*/

	*scn = new scene;
	(**scn)
		.addObject(new parallelogram(vec3(0, 0, 555), vec3(0, 0, -555), vec3(0, 555, 0), mgreen))			//left
		.addObject(new parallelogram(vec3(555, 0, 0), vec3(0, 0, 555), vec3(0, 555, 0), mred))			//right
		.addObject(new parallelogram(vec3(555, 0, 555), vec3(-555, 0, 0), vec3(0, 555, 0), mmytex))	//back
		.addObject(new parallelogram(vec3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), mwhite))		//top
		.addObject(new parallelogram(vec3(555, 0, 0), vec3(-555, 0, 0), vec3(0, 0, 555), mwhite))		//bottom
		//.addObject(meshPly)
		.addObject(octreeMesh)
		.addObject(new sphere(vec3(90, 290, 190), 90, mlight)); //light sphere
		//.addObject(new sphere(vec3(490, 490, 190), 90, mlight))  //light sphere
		//.addObject(new sphere(vec3(278, 278, 190), 90, mlight));  //light sphere
		//.addObject(new sphere(vec3(278, 278, 190), 90, mwhite))
		//.addObject(meshSphericalCube);
		//.addObject(new triangle_spherical_cube(vec3(300, 150, 190), 60, 5, 5, mred))
		
	//list[i++] = new translate( new rotate_y(new box(vec3(0,0,0), vec3(165,165,165), white), -18), vec3(130,0,65));
	//material* aluminium = new metal(vec3(0.8, 0.85, 0.88), 0.0);
	//list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));


	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0), aspect, vfov);

}

void bunny_scene(scene** scn, camera** cam, float aspect)
{
	pdf* u_pdf = new cosine_weighted_hemisphere_pdf();

	texture* red = new constant_texture(vec4(0.65, 0.05, 0.05, 1));
	float s = 10;
	texture* diffuse_light = new constant_texture(vec4(s, s, s, 1));
	texture* green = new constant_texture(vec4(0.12, 0.45, 0.15, 1));
	texture* white = new constant_texture(vec4(0.73, 0.73, 0.73, 1));
	texture* tex0 = new image_texture(tigrLoadImage("tex0.png"));
	texture* tex2 = new image_texture(tigrLoadImage("tex2.png"));
	texture* tex3 = new image_texture(tigrLoadImage("tex3.png"));
	texture* tex4 = new image_texture(tigrLoadImage("tex6.png"), vec4(3));

	material* mred = new lambertian_material(red, u_pdf);
	material* mwhite = new lambertian_material(white, u_pdf);
	material* mgreen = new lambertian_material(green, u_pdf);
	material* mlight = new emitter_material(diffuse_light, u_pdf);

	material* msphere = new lambertian_material(tex0, u_pdf);
	material* mwall = new lambertian_material(tex2, u_pdf);
	material* mwall2 = new lambertian_material(tex3, u_pdf);
	material* mlight2 = new emitter_material(tex4, u_pdf);
	int i = 0;

	if (!PlyLoader::loadPlyMesh("bun_zipper_res.ply", vec3(250, -50, 190), vec3(0, -M_PI, 0), vec3(2000, 2000, 2000)))
	{
		std::cout << "Failed at mesh loading - will exit.\n";
		return;
	}
	triangle_mesh* meshPly = new triangle_mesh(meshDataVector.back(), mwhite);
	size_t maxElements = 40;
	int depth = meshDataVector.back().triangleCount() / float(maxElements*log(8));
	depth = 10;
	std::cout << depth << "\n";
	triangle_octree_mesh* octreeMesh = new triangle_octree_mesh(meshDataVector.back(), mwhite, depth, maxElements);
	std::cout << "Octree built\n";
	*scn = new scene;
	(**scn)
		.addObject(new parallelogram(vec3(555, 0, 0), vec3(-555, 0, 0), vec3(0, 0, 555), mwhite))	//bottom
		.addObject(octreeMesh);
		//.addObject(new sphere(vec3(90, 190, 190), 90, mlight)); //light sphere


	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0), aspect, vfov);

}

void dragon_scene(scene** scn, camera** cam, float aspect)
{
	pdf* u_pdf = new cosine_weighted_hemisphere_pdf();

	texture* red = new constant_texture(vec4(0.65, 0.05, 0.05, 1));
	float s = 10;
	texture* diffuse_light = new constant_texture(vec4(s, s, s, 1));
	texture* green = new constant_texture(vec4(0.12, 0.45, 0.15, 1));
	texture* white = new constant_texture(vec4(0.73, 0.73, 0.73, 1));
	texture* tex0 = new image_texture(tigrLoadImage("tex0.png"));
	texture* tex2 = new image_texture(tigrLoadImage("tex2.png"));
	texture* tex3 = new image_texture(tigrLoadImage("tex3.png"));
	texture* tex4 = new image_texture(tigrLoadImage("tex6.png"), vec4(3));

	material* mred = new lambertian_material(red, u_pdf);
	material* mwhite = new lambertian_material(white, u_pdf);
	material* mgreen = new lambertian_material(green, u_pdf);
	material* mlight = new emitter_material(diffuse_light, u_pdf);

	material* msphere = new lambertian_material(tex0, u_pdf);
	material* mwall = new lambertian_material(tex2, u_pdf);
	material* mwall2 = new lambertian_material(tex3, u_pdf);
	material* mlight2 = new emitter_material(tex4, u_pdf);
	int i = 0;

	if (!PlyLoader::loadPlyMesh("dragon_vrip_res.ply", vec3(250, -50, 190), vec3(M_PI*0.1, -M_PI*0.8, 0), vec3(2000, 2000, 2000)))
	{
		std::cout << "Failed at mesh loading - will exit.\n";
		return;
	}
	triangle_mesh* meshPly = new triangle_mesh(meshDataVector.back(), mwhite);
	size_t maxElements = 40;
	int depth = meshDataVector.back().triangleCount() / float(maxElements*log(8));
	std::cout << depth << "\n";
	triangle_octree_mesh* octreeMesh = new triangle_octree_mesh(meshDataVector.back(), mred, depth, maxElements);
	std::cout << "Octree built\n";
	*scn = new scene;
	(**scn)
		.addObject(new parallelogram(vec3(555, 0, 0), vec3(-555, 0, 0), vec3(0, 0, 555), mwhite))		//bottom
		.addObject(octreeMesh)
	.addObject(new sphere(vec3(190, 390, 50), 30, mlight)); //light sphere


	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0), aspect, vfov);

}

void happy_scene(scene** scn, camera** cam, float aspect)
{
	pdf* u_pdf = new cosine_weighted_hemisphere_pdf();

	texture* red = new constant_texture(vec4(0.65, 0.05, 0.05, 1));
	float s = 10;
	texture* diffuse_light = new constant_texture(vec4(s, s, s, 1));
	texture* green = new constant_texture(vec4(0.12, 0.45, 0.15, 1));
	texture* white = new constant_texture(vec4(0.73, 0.73, 0.73, 1));
	texture* tex0 = new image_texture(tigrLoadImage("tex0.png"));
	texture* tex2 = new image_texture(tigrLoadImage("tex2.png"));
	texture* tex3 = new image_texture(tigrLoadImage("tex3.png"));
	texture* tex4 = new image_texture(tigrLoadImage("tex6.png"), vec4(3));

	material* mred = new lambertian_material(red, u_pdf);
	material* mwhite = new lambertian_material(white, u_pdf);
	material* mgreen = new lambertian_material(green, u_pdf);
	material* mlight = new emitter_material(diffuse_light, u_pdf);

	material* msphere = new lambertian_material(tex0, u_pdf);
	material* mwall = new lambertian_material(tex2, u_pdf);
	material* mwall2 = new lambertian_material(tex3, u_pdf);
	material* mlight2 = new emitter_material(tex4, u_pdf);
	int i = 0;

	if (!PlyLoader::loadPlyMesh("happy_vrip.ply", vec3(250, -50, 190), vec3(0, -M_PI, 0), vec3(2000, 2000, 2000)))
	{
		std::cout << "Failed at mesh loading - will exit.\n";
		return;
	}
	triangle_mesh* meshPly = new triangle_mesh(meshDataVector.back(), mwhite);
	size_t maxElements = 40;
	int depth = meshDataVector.back().triangleCount() / float(maxElements*log(8));
	std::cout << "Building octree with depth: " << depth << "\n";
	HighPrecisionTimer octreeBuildTimer;
	octreeBuildTimer.StartCounter();
	triangle_octree_mesh* octreeMesh = new triangle_octree_mesh(meshDataVector.back(), mred, depth, maxElements);
	std::cout << "Octree built in " << octreeBuildTimer.GetCounter() << "s.\n";
	*scn = new scene;
	(**scn)
		.addObject(new parallelogram(vec3(555, 0, 0), vec3(-555, 0, 0), vec3(0, 0, 555), mwhite))		//bottom
		//.addObject(new sphere(vec3(190, 390, 50), 30, mlight)); //light sphere
		.addObject(octreeMesh);

	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0), aspect, vfov);

}

void lucy_scene(scene** scn, camera** cam, float aspect)
{
	pdf* u_pdf = new cosine_weighted_hemisphere_pdf();

	texture* red = new constant_texture(vec4(0.65, 0.05, 0.05, 1));
	float s = 10;
	texture* diffuse_light = new constant_texture(vec4(s, s, s, 1));
	texture* green = new constant_texture(vec4(0.12, 0.45, 0.15, 1));
	texture* white = new constant_texture(vec4(0.73, 0.73, 0.73, 1));
	texture* tex0 = new image_texture(tigrLoadImage("tex0.png"));
	texture* tex2 = new image_texture(tigrLoadImage("tex2.png"));
	texture* tex3 = new image_texture(tigrLoadImage("tex3.png"));
	texture* tex4 = new image_texture(tigrLoadImage("tex6.png"), vec4(3));

	material* mred = new lambertian_material(red, u_pdf);
	material* mwhite = new lambertian_material(white, u_pdf);
	material* mgreen = new lambertian_material(green, u_pdf);
	material* mlight = new emitter_material(diffuse_light, u_pdf);

	material* msphere = new lambertian_material(tex0, u_pdf);
	material* mwall = new lambertian_material(tex2, u_pdf);
	material* mwall2 = new lambertian_material(tex3, u_pdf);
	material* mlight2 = new emitter_material(tex4, u_pdf);
	int i = 0;

	if (!PlyLoader::loadPlyMesh("lucy.ply", vec3(250, -50, 190), vec3(-M_PI/2, 0, 0), vec3(2000, 2000, 2000)))
	{
		std::cout << "Failed at mesh loading - will exit.\n";
		return;
	}
	triangle_mesh* meshPly = new triangle_mesh(meshDataVector.back(), mwhite);
	size_t maxElements = 100;
	int depth = meshDataVector.back().triangleCount() / float(maxElements*log(8));
	std::cout << "Building octree with depth: " << depth << "\n";
	HighPrecisionTimer octreeBuildTimer;
	octreeBuildTimer.StartCounter();
	triangle_octree_mesh* octreeMesh = new triangle_octree_mesh(meshDataVector.back(), mred, depth, maxElements);
	std::cout << "Octree built in " << octreeBuildTimer.GetCounter() << "\n";
	*scn = new scene;
	(**scn)
		.addObject(new parallelogram((octreeMesh->getBoundingBoxMin()+ octreeMesh->getBoundingBoxMax())/2 -
			vec3(0, 2*(octreeMesh->getBoundingBoxMin() + octreeMesh->getBoundingBoxMax()).y,0) -
			vec3(1.5 * (octreeMesh->getBoundingBoxMax().x - octreeMesh->getBoundingBoxMin().x),
			(octreeMesh->getBoundingBoxMax().y - octreeMesh->getBoundingBoxMin().y)*0.005,
				1.5 * (octreeMesh->getBoundingBoxMax().z - octreeMesh->getBoundingBoxMin().z)),
			vec3(0, 0, 3 * (octreeMesh->getBoundingBoxMax().z - octreeMesh->getBoundingBoxMin().z)),
			vec3(3 * (octreeMesh->getBoundingBoxMax().x - octreeMesh->getBoundingBoxMin().x), 0, 0), mwhite))	//bottom
																										//.addObject(new sphere(vec3(190, 390, 50), 30, mlight)); //light sphere
		.addObject(octreeMesh);

	std::cout << "Bounding box min: " << octreeMesh->getBoundingBoxMin() << "\nBounding box max: " << octreeMesh->getBoundingBoxMax() << "\n";
	vec3 lookfrom(vec3((octreeMesh->getBoundingBoxMin().x + octreeMesh->getBoundingBoxMax().x) / 2, octreeMesh->getBoundingBoxMax().y, 6*octreeMesh->getBoundingBoxMin().z-5*octreeMesh->getBoundingBoxMax().z));
	vec3 lookat((octreeMesh->getBoundingBoxMin()+ octreeMesh->getBoundingBoxMax())/2);
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0), aspect, vfov);

}

void xyz_dragon_scene(scene** scn, camera** cam, float aspect)
{
	pdf* u_pdf = new cosine_weighted_hemisphere_pdf();

	texture* red = new constant_texture(vec4(0.65, 0.05, 0.05, 1));
	float s = 10;
	texture* diffuse_light = new constant_texture(vec4(s, s, s, 1));
	texture* green = new constant_texture(vec4(0.12, 0.45, 0.15, 1));
	texture* white = new constant_texture(vec4(0.73, 0.73, 0.73, 1));
	texture* tex0 = new image_texture(tigrLoadImage("tex0.png"));
	texture* tex2 = new image_texture(tigrLoadImage("tex2.png"));
	texture* tex3 = new image_texture(tigrLoadImage("tex3.png"));
	texture* tex4 = new image_texture(tigrLoadImage("tex6.png"), vec4(3));

	material* mred = new lambertian_material(red, u_pdf);
	material* mwhite = new lambertian_material(white, u_pdf);
	material* mgreen = new lambertian_material(green, u_pdf);
	material* mlight = new emitter_material(diffuse_light, u_pdf);

	material* msphere = new lambertian_material(tex0, u_pdf);
	material* mwall = new lambertian_material(tex2, u_pdf);
	material* mwall2 = new lambertian_material(tex3, u_pdf);
	material* mlight2 = new emitter_material(tex4, u_pdf);
	int i = 0;

	if (!PlyLoader::loadPlyMesh("xyzrgb_dragon.ply", vec3(250, 130, 190), vec3(0, 0, 0), vec3(3, 3, 3)))
	{
		std::cout << "Failed at mesh loading - will exit.\n";
		return;
	}
	triangle_mesh* meshPly = new triangle_mesh(meshDataVector.back(), mwhite);
	size_t maxElements = 40;
	int depth = meshDataVector.back().triangleCount() / float(maxElements*log(8));
	std::cout << "Building octree with depth: " << depth << "\n";
	HighPrecisionTimer octreeBuildTimer;
	octreeBuildTimer.StartCounter();
	triangle_octree_mesh* octreeMesh = new triangle_octree_mesh(meshDataVector.back(), mred, depth, maxElements);
	std::cout << "Octree built in " << octreeBuildTimer.GetCounter() <<"\n";
	*scn = new scene;
	(**scn)
		.addObject(new parallelogram(vec3(555, 0, 0), vec3(-555, 0, 0), vec3(0, 0, 555), mwhite))		//bottom
																										//.addObject(new sphere(vec3(190, 390, 50), 30, mlight)); //light sphere
		.addObject(octreeMesh);

	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float vfov = 40.0;
	*cam = new camera(lookfrom, lookat, vec3(0, 1, 0), aspect, vfov);

}

int main(int argc, char *argv[])
{

	RenderOptions options;
	options.x = 0;
	options.y = 0;
	options.width = 800;
	options.height = 800;
	options.numThreads = 8;
	options.dxCoef = 8;
	options.dyCoef = 1;
	options.samples = 50;
	options.shadowRaysCount = 0;
	options.scatteredRaysPow = 1;
	options.bounces = 10;
	options.backgroundColor = new GradientBackgroundColor(vec3(0), vec3(1));
	options.rrOptions.mulFactor = 10;



	Tigr *screen = tigrWindow(options.width,options.height, "Raytracer", 0);
	tigrClear(screen, tigrRGB(0,0,0));
	tigrUpdate(screen);


	camera* cam;
	scene* scn;
	//cornell_box_scene(&scn, &cam, float(options.width) / float(options.height));
	/*xyz_dragon_scene(&scn, &cam, float(options.width) / float(options.height));*/
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float vfov = 40.0;
	cam = new camera(lookfrom, lookat, vec3(0, 1, 0), float(options.width) / float(options.height), vfov);
	if (!SceneLoader::loadScene("testScene.txt", &scn))
	{
		tigrFree(screen);
		return 1;
	}

	HighPrecisionTimer globalTime;
	globalTime.StartCounter();
	render(screen, *cam, *scn, options);
	std::cout << "Rendering time: " << globalTime.GetCounter() << "\n";
	while (!tigrClosed(screen))
	{
		if (tigrKeyDown(screen, TK_SPACE))
		{
			tigrSaveImage("output_image.png", screen);
			break;
		}
		tigrUpdate(screen);
	}
	tigrFree(screen);
	delete scn;
	delete cam;
	return 0;
}