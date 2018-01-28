#pragma once

#include <vector>
#include <optional>

namespace model
{
	// R8G8B8A8
	struct ImageData
	{
		std::vector<unsigned char> pixels;
		unsigned int width;
		unsigned int height;
	};

	// R8
	struct GreyscaleImageData
	{
		std::vector<unsigned char> pixels;
		unsigned int width;
		unsigned int height;
	};

	std::optional<ImageData> readPNG(std::string fName);
	bool writePNG(const ImageData& image, std::string fName);
	void flip(ImageData& image);

	std::optional<GreyscaleImageData> readGreyscalePNG(std::string fName);
}