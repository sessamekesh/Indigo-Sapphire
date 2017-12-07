#include <util/io.h>
#include <fstream>

namespace util
{
	std::future<std::string> readFileAsync(const char* path)
	{
		return std::async(std::launch::async, [path]()->std::string {
			std::ifstream fin(path, std::ios::in);
			if (!fin)
			{
				return "";
			}
			fin.seekg(0u, std::ios::end);
			const size_t fileSize = fin.tellg();
			fin.seekg(0u, std::ios::beg);

			std::string VSText;
			VSText.reserve(fileSize);
			VSText.assign(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());

			return VSText;
		});
	}
}