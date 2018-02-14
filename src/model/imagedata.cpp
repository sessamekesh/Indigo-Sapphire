#include <model/imagedata.h>
#include <lodepng.h>
#include <iostream>

namespace model
{
	std::optional<ImageData> readPNG(std::string fName)
	{
		ImageData tr = {};

		unsigned int error = lodepng::decode(tr.pixels, tr.width, tr.height, fName);
		if (error)
		{
			std::cerr << "Image decoding error - " << error << ": " << lodepng_error_text(error) << std::endl;
			return {};
		}
		else
		{
			return tr;
		}
	}

	bool writePNG(const ImageData& image, std::string fName)
	{
		std::vector<std::uint8_t> data;
		unsigned int error = lodepng::encode(data, image.pixels, image.width, image.height);
		if (error)
		{
			std::cerr << "Image encoding error - " << error << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}
		error = lodepng::save_file(data, fName);
		if (error)
		{
			std::cerr << "Image saving error - " << error << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}

		return true;
	}

	void flip(ImageData& image)
	{
		for (std::uint32_t row = 0u; row < image.height / 2u; row++)
		{
			for (std::uint32_t col = 0u; col < image.width; col++)
			{
				for (std::uint32_t pixel = 0u; pixel < 4u; pixel++)
				{
					auto firstIdx = (row * image.width + col) * 4u + pixel;
					auto secondIdx = ((image.height - row - 1u) * image.width + col) * 4u + pixel;

					auto tmp = image.pixels[firstIdx];
					image.pixels[firstIdx] = image.pixels[secondIdx];
					image.pixels[secondIdx] = tmp;

				}
			}
		}
	}

	std::optional<GreyscaleImageData> readGreyscalePNG(std::string fName)
	{
		GreyscaleImageData tr = {};

		unsigned int error = lodepng::decode(tr.pixels, tr.width, tr.height, fName, LCT_GREY, 8u);
		if (error)
		{
			std::cerr << "Image decoding error - " << error << ": " << lodepng_error_text(error) << std::endl;
			return {};
		}
		else
		{
			return tr;
		}
	}

	bool writePNG(const GreyscaleImageData& image, std::string fName)
	{
		std::vector<std::uint8_t> data;
		unsigned int error = lodepng::encode(data, image.pixels, image.width, image.height, LCT_GREY);
		if (error)
		{
			std::cerr << "Image encoding error - " << error << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}
		error = lodepng::save_file(data, fName);
		if (error)
		{
			std::cerr << "Image saving error - " << error << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}

		return true;
	}
}