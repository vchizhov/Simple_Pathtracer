#ifndef RAYTR_CORE_TEXTURE_H
#define RAYTR_CORE_TEXTURE_H

#include "tigr.h"
#include "vec4.h"

namespace Raytr_Core
{
	//! An abstract texture class
	class texture
	{
	public:
		virtual BasicMath::vec4 value(const BasicMath::vec2& uv, const BasicMath::vec3& p) const = 0;
	};

	//! A texture with a constant color
	class constant_texture : public texture
	{
	public:
		constant_texture():color(BasicMath::vec4(0)) {}
		explicit constant_texture(const BasicMath::vec4& c) : color(c) {}
		virtual BasicMath::vec4 value(const BasicMath::vec2& uv, const BasicMath::vec3& p) const
		{
			return color;
		}

		BasicMath::vec4 color;
	};

	//! A checker texture
	class checker_texture : public texture
	{
	public:
		checker_texture(texture* even, texture* odd, int rows=10, int cols = 10) : even(even),odd(odd),rows(rows), cols(cols) {}
		virtual BasicMath::vec4 value(const BasicMath::vec2& uv, const BasicMath::vec3& p) const
		{
			int iu = int(floor(cols * uv.u)); int iv = int(floor(rows * uv.v));
			if (iu % 2 == iv % 2)
				return even->value(uv, p);
			else
				return odd->value(uv, p);
		}

		texture* odd;
		texture* even;
		int rows, cols;
	};

	//! A texture from an image
	class image_texture : public texture
	{
	public:
		image_texture(Tigr* image, const BasicMath::vec4& intensity = BasicMath::vec4::one)
			:intensity(intensity), image(image)
		{
		}
		virtual BasicMath::vec4 value(const BasicMath::vec2& uv, const BasicMath::vec3& p) const
		{
			int x = uv.u*(image->w - 1);
			int y = uv.v*(image->h - 1);
			int i = y*image->w + x;
			BasicMath::vec4 col(image->pix[i].r / float(255), image->pix[i].g / float(255), image->pix[i].b / float(255), image->pix[i].a / float(255));
			return col*intensity;
		}

		~image_texture()
		{
			tigrFree(image);
		}
		Tigr* image;
		BasicMath::vec4 intensity;
	};
}

#endif