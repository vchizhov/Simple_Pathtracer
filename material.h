#ifndef RAYTR_CORE_MATERIAL_H
#define RAYTR_CORE_MATERIAL_H

#include <math.h>
#include <vector>
#include "texture.h"
#include "object.h"
#include "hemisphere_pdf.h"
namespace Raytr_Core
{
	//! info structure for scattered rays
	struct scattering_info
	{
		BasicMath::vec3 attenuation;
		BasicMath::vec3 scattered_ray;
	};

	//! An abstract material class
	class material
	{
	public:
		bool emits;
		material(bool emits, pdf* pPdf) : emits(emits), pPdf(pPdf) {};
		virtual bool scatter(const ray& r, const intersection_info& info, scattering_info& sinfo) const = 0;
		virtual BasicMath::vec3 emitted(const ray& r, const intersection_info& info) const
		{
			return BasicMath::vec3(0, 0, 0);
		}

		virtual BasicMath::vec3 brdf(const BasicMath::vec3& r_in, const BasicMath::vec3& r_out, const intersection_info& info) const = 0;

		pdf* pPdf;
	};

	//! A lambertian material
	class lambertian_material : public material
	{
	public:
		lambertian_material(texture* diffuseTexture, pdf* pdf) : tex(diffuseTexture), material(false, pdf)
		{
		}

		virtual BasicMath::vec3 emitted(const ray& r, const intersection_info& info) const
		{
			return  BasicMath::vec3(0);
		}

		virtual bool scatter(const ray& r, const intersection_info& info, scattering_info& sinfo) const
		{
			return true;
		}

		virtual BasicMath::vec3 brdf(const BasicMath::vec3& r_in, const BasicMath::vec3& r_out, const intersection_info& info) const
		{
			return tex->value(info.uv, info.position).xyz()/float(M_PI);
		}

		texture* tex;
	};

	//! An emitter material
	class emitter_material : public material
	{
	public:
		emitter_material(texture* tex, pdf* pdf) :tex(tex), material(true, pdf)
		{
		}

		virtual BasicMath::vec3 emitted(const ray& r, const intersection_info& info) const
		{
			return tex->value(info.uv, info.position).xyz();
		}

		virtual bool scatter(const ray& r, const intersection_info& info, scattering_info& sinfo) const
		{
			return false;
		}

		virtual BasicMath::vec3 brdf(const BasicMath::vec3& r_in, const BasicMath::vec3& r_out, const intersection_info& info) const
		{
			return BasicMath::vec3(1);
		}

		texture* tex;
	};
}

#endif
