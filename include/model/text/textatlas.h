#pragma once

#include <cstdint>
#include <string>

namespace model
{
	namespace text
	{
		class TextAtlas
		{
		public:
			TextAtlas(
				std::uint32_t rows,
				std::uint32_t cols,
				std::string orderedGlyphs
			);
			~TextAtlas() = default;
			TextAtlas(const TextAtlas&) = default;

			bool getGlyphRowCol(char glyph, std::uint32_t& o_row, std::uint32_t& o_col) const;
			std::uint32_t rows() const ;
			std::uint32_t cols() const;

		protected:
			std::uint32_t rows_;
			std::uint32_t cols_;
			std::string orderedGlyphs_;
		};
	}
}