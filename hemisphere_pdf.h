#ifndef RAYTR_CORE_HEMISPHERE_PDF_H
#define RAYTR_CORE_HEMISPHERE_PDF_H
#include "object.h"

namespace Raytr_Core
{
	//! a strcture to hold the result
	struct pdf_info
	{
		BasicMath::vec3 generatedDirection;
		float pdf_value;
	};

	//! an abstract probability distribution class
	class pdf
	{
	public:
		virtual float value(const BasicMath::vec3& direction) const = 0;
		virtual BasicMath::vec3 generate() const = 0;
		virtual BasicMath::mat3 transform(const BasicMath::vec3& normal) const = 0;
		//! generate a sample, get it's pdf value and return the transformed sample
		virtual pdf_info sample(const BasicMath::vec3& normal) const = 0;
	};

	//! an object pdf class
	class object_pdf : public pdf
	{
	public:
		object* ptr;
		BasicMath::vec3 o;
		object_pdf(object* p, const BasicMath::vec3& origin) : ptr(p), o(origin) {}
		virtual float value(const BasicMath::vec3& direction) const
		{
			return ptr->pdf_value(o, direction);
		}
		virtual BasicMath::vec3 generate() const
		{
			return ptr->random(o);
		}
		virtual BasicMath::mat3 transform(const BasicMath::vec3& normal) const
		{
			return createCoordinateSystem(normalize(ptr->center-o));
		}

		virtual pdf_info sample(const BasicMath::vec3& normal) const
		{
			pdf_info result;
			result.generatedDirection = generate();
			result.pdf_value = value(result.generatedDirection);
			result.generatedDirection = transform(normal)*result.generatedDirection;
			return result;
		}
	};

	//! a pdf to mix 2 pdfs
	class mixture_pdf : public pdf
	{
	public:
		float mix;
		pdf* p[2];	
		mixture_pdf(pdf* p0, pdf* p1, float mix=0.5) { p[0] = p0; p[1] = p1; this->mix = mix; }
		virtual float value(const  BasicMath::vec3& direction) const
		{
			return mix * p[0]->value(direction) + (1-mix) * p[1]->value(direction);
		}
		virtual  BasicMath::vec3 generate() const
		{
			if (BasicMath::uniform_distribution(BasicMath::generator) < mix)
			{
				return p[0]->generate();
			}
			else
			{
				return p[1]->generate();
			}
		}

		virtual BasicMath::mat3 transform(const BasicMath::vec3& normal) const
		{
			return createCoordinateSystem(normal);
		}

		virtual pdf_info sample(const BasicMath::vec3& normal) const
		{
			pdf_info result;
			if (BasicMath::uniform_distribution(BasicMath::generator) < mix)
			{
				result.generatedDirection = p[0]->generate();
				result.pdf_value = 1/mix*p[0]->value(result.generatedDirection);
				result.generatedDirection = p[0]->transform(normal)*result.generatedDirection;
			}
			else
			{
				result.generatedDirection = p[1]->generate();
				result.pdf_value = 1/(1-mix)*p[1]->value(result.generatedDirection);
				result.generatedDirection = p[1]->transform(normal)*result.generatedDirection;
			}
			
			return result;
		}
		
	};

	//! uniform sampling
	class uniform_hemisphere_pdf : public pdf
	{
	public:
		virtual float value(const BasicMath::vec3& direction) const
		{
			return 1 / (2 * M_PI);
		}

		virtual BasicMath::vec3 generate() const
		{
			//generate 2 numbers from [0,1] with pdf(x) = 1
			float r1 = BasicMath::uniform_distribution(BasicMath::generator);
			float r2 = BasicMath::uniform_distribution(BasicMath::generator);
			//generate a vector in the hemisphere around the normal = [0,1,0]
			float sinTheta = sqrtf(1 - r2 * r2);
			float phi = 2 * float(M_PI) * r1;
			float x = sinTheta * cosf(phi);
			float z = sinTheta * sinf(phi);
			return BasicMath::vec3(x, r2, z);
		}

		virtual BasicMath::mat3 transform(const BasicMath::vec3& normal) const
		{
			return createCoordinateSystem(normal);
		}

		virtual pdf_info sample(const BasicMath::vec3& normal) const
		{
			pdf_info result;
			result.generatedDirection = generate();
			result.pdf_value = value(result.generatedDirection);
			result.generatedDirection = transform(normal)*result.generatedDirection;
			return result;
		}

	};

	//! cosine weighted sampling
	class cosine_weighted_hemisphere_pdf : public pdf
	{
	public:
		virtual float value(const BasicMath::vec3& direction) const
		{
			return BasicMath::dotProduct(direction, BasicMath::vec3(0,1,0)) / float(M_PI);
		}

		virtual BasicMath::vec3 generate() const
		{
			//generate 2 numbers from [0,1] with pdf(x) = 1
			float r1 = BasicMath::uniform_distribution(BasicMath::generator);
			float r2 = BasicMath::uniform_distribution(BasicMath::generator);
			//generate a vector in the hemisphere around the normal = [0,1,0]
			float sinTheta = sqrtf(1 - r2);
			float phi = 2 * float(M_PI) * r1;
			float x = sinTheta * cosf(phi);
			float z = sinTheta * sinf(phi);
			return BasicMath::vec3(x, sqrtf(r2), z);
		}

		virtual BasicMath::mat3 transform(const BasicMath::vec3& normal) const
		{
			return createCoordinateSystem(normal);
		}

		virtual pdf_info sample(const BasicMath::vec3& normal) const
		{
			pdf_info result;
			result.generatedDirection = generate();
			result.pdf_value = value(result.generatedDirection);
			result.generatedDirection = transform(normal)*result.generatedDirection;
			return result;
		}
	};

}
#endif
