#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <math/vector.hpp>
#include <spdlog/spdlog.h>
#include <utils/image.hpp>

#include <map>
#include <string>

namespace freetype {
	void init();
	void cleanup();

	struct Glyph {
		math::vec2 bearing, size;
		math::vec2 uv;
		int advanceX;
	};

	class Font {
	public:
		Font(const std::string &path);
		Font(Font &&other);
		~Font();

		Font& operator=(Font &&other);

		void setPixelSizes(size_t height, size_t width = 0);
		const Image& buildFontAtlas(unsigned numGlyphs = 256);
		const Image& getFontAtlas() const;

		Glyph& operator[](int glyph);
		const Glyph& operator[](int glyph) const;

	private:
		FT_Face handle;
		std::map<int, Glyph> glyphs;
		Image atlas;
	};
}