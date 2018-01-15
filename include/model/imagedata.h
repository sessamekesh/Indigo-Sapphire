#pragma once

#include <vector>
#include <optional>

namespace model
{
	struct ImageData
	{
		std::vector<unsigned char> pixels;
		unsigned int width;
		unsigned int height;
	};

	std::optional<ImageData> readPNG(std::string fName);
	bool writePNG(const ImageData& image, std::string fName);
	void flip(ImageData& image);
}