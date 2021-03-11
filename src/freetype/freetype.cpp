#include <freetype/freetype.hpp>

namespace freetype {
	FT_Library library;

	void init() {
		FT_Error error = FT_Init_FreeType(&library);
		if(error) {
			spdlog::error("couldn't initialize freetype!");
		}
	}

	void cleanup() {
		FT_Done_FreeType(library);
	}

	Font::Font(const std::string &path) {
		FT_Error error = FT_New_Face(library, path.c_str(), 0, &handle);
		if(error) {
			spdlog::error("couldn't load font {}!", path);
		}
	}

	Font::Font(Font &&other) : handle(other.handle), glyphs(std::move(other.glyphs)), atlas(std::move(other.atlas)) {
		other.handle = 0;
	}

	Font::~Font() {
		if(handle) {
			FT_Done_Face(handle);
		}
	}

	Font& Font::operator=(Font &&other) {
		if(handle) {
			FT_Done_Face(handle);
		}
		handle = other.handle;
		glyphs = std::move(other.glyphs);
		atlas = std::move(other.atlas);
		other.handle = 0;
		return *this;
	}

	void Font::setPixelSizes(size_t height, size_t width) {
		FT_Set_Pixel_Sizes(handle, width, height);
	}

	void Font::buildFontAtlas(int numGlyphs) {
		int max_dim = (1 + (handle->size->metrics.height >> 6)) * ceilf(sqrtf(numGlyphs));
		int tex_width = 1;
		while(tex_width < max_dim) tex_width <<= 1;
		int tex_height = tex_width;

		// render glyphs to atlas
		atlas = Image({tex_width, tex_height}, 1);
		char* pixels = (char*)atlas.data();
		int pen_x = 0, pen_y = 0;

		for(int i = 0; i < numGlyphs; i++){
			FT_Load_Char(handle, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
			FT_Bitmap* bmp = &handle->glyph->bitmap;

			if(pen_x + bmp->width >= tex_width){
				pen_x = 0;
				pen_y += ((handle->size->metrics.height >> 6) + 1);
			}

			for(int row = 0; row < bmp->rows; ++row){
				for(int col = 0; col < bmp->width; ++col){
					int x = pen_x + col;
					int y = pen_y + row;
					pixels[y * tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}

			// this is stuff you'd need when rendering individual glyphs out of the atlas

			glyphs[i].bearing.x = handle->glyph->bitmap_left;
			glyphs[i].bearing.y = handle->glyph->bitmap_top;
			glyphs[i].size.x = bmp->width;
			glyphs[i].size.y = bmp->rows;
			glyphs[i].uv.x = pen_x;
			glyphs[i].uv.y = pen_y;
			glyphs[i].advanceX = handle->glyph->advance.x >> 6;

			pen_x += bmp->width + 1;
		}
	}

	const Image& Font::getFontAtlas() const {
		return atlas;
	}

	Glyph& Font::operator[](int glyph) {
		return glyphs[glyph];
	}

	const Glyph& Font::operator[](int glyph) const {
		return glyphs.at(glyph);
	}
}