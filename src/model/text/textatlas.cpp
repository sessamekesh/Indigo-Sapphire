#include <model/text/textatlas.h>

namespace model
{
	namespace text
	{
		TextAtlas::TextAtlas(
			std::uint32_t rows,
			std::uint32_t cols,
			std::string orderedGlyphs
		)
			: rows_(rows)
			, cols_(cols)
			, orderedGlyphs_(orderedGlyphs)
		{}

		bool TextAtlas::getGlyphRowCol(char glyph, std::uint32_t& o_row, std::uint32_t& o_col) const
		{
			std::uint32_t i = 0u;
			for (std::uint32_t row = 0u; row < rows_; row++)
			{
				for (std::uint32_t col = 0u; col < cols_; col++)
				{
					if (i >= orderedGlyphs_.size())
					{
						return false;
					}

					if (orderedGlyphs_[i] == glyph)
					{
						o_row = row;
						o_col = col;
						return true;
					}

					++i;
				}
			}

			return false;
		}

		std::uint32_t TextAtlas::rows() const
		{
			return rows_;
		}

		std::uint32_t TextAtlas::cols() const
		{
			return cols_;
		}
	}
}