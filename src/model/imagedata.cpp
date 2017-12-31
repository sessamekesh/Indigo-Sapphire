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
			// Flip pixels
			//for (std::uint32_t row = 0u; row < tr.height / 2u; row++)
			//{
			//	for (std::uint32_t col = 0u; col < tr.width; col++)
			//	{
			//		for (std::uint32_t pixel = 0u; pixel < 4u; pixel++)
			//		{
			//			auto idx1 = (row * tr.width + col) * 4u + pixel;
			//			auto idx2 = ((tr.height - 1 - row) * tr.width + col) * 4u + pixel;
			//			auto tmp = tr.pixels[idx1];
			//			tr.pixels[idx1] = tr.pixels[idx2];
			//			tr.pixels[idx2] = tmp;
			//		}
			//	}
			//}
			return tr;
		}
	}
}