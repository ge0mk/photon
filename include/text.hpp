#pragma once

#include <memory>
#include <string>
#include <vector>
#include <fstream>

#include <math/matrix.hpp>
#include <math/vector.hpp>

#include <opengl/buffer.hpp>
#include <opengl/mesh.hpp>
#include <opengl/program.hpp>
#include <opengl/texture.hpp>
#include <opengl/vao.hpp>

#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>

#include <utils/image.hpp>

using namespace math;

class Font {
public:
	struct VMetrics {
		int ascent, descent;
		int lineGap;
	};

	struct HMetrics {
		int advanceWidth, leftSideBearing;
	};

	struct BoundingBox {
		ivec2 bl, tr;
	};

	struct Glyph {
		int unicode, index;
		float advanceX, scale;
		vec2 bl, tr;
		vec2 uvtl, uvbr;
	};

	struct GlyphRange {
		int startCodepoint, range;
	};

	Font(const std::string &path);
	Font(const Font &other) = default;
	Font& operator=(const Font &other) = default;

	int findGlyphIndex(int unicode);
	float scaleForPixelHeight(float pixels);
	float scaleForMappingEmToPixels(float pixels);

	VMetrics getFontVMetrics();
	VMetrics getFontVMetricsOS2();
	BoundingBox getFontBoundingBox();

	HMetrics getCodepointHMetrics(int codepoint);
	int getCodepointKernAdvance(int c1, int c2);
	BoundingBox getCodepointBoundingBox(int codepoint);

	HMetrics getGlyphHMetrics(int glyph);
	int getGlyphKernAdvance(int g1, int g2);
	BoundingBox getGlyphBoundingBox(int glyph);

	Glyph getGlyph(int codepoint, float scale);
	Glyph getGlyph(int index);

	Image& generateBitmap(float height, GlyphRange range);
	Image generateSDF(float height, int padding, GlyphRange range);

private:
	friend class TextRenderer;

	stbtt_fontinfo font;
	std::vector<Glyph> glyphs;
	Image texture;
};

class TextObject {
public:
	TextObject(const std::string &text, mat4 transform, vec4 color);

private:
	friend class TextRenderer;

	std::string text;
	mat4 transform;
	vec4 color;
};

class TextRenderer {
public:
	using Vertex = opengl::Vertex<vec3, vec2, vec4>;
	using Mesh = opengl::Mesh<vec3, vec2, vec4>;

	TextRenderer(const Font &font);

	std::shared_ptr<TextObject> createObject(const std::string &text, mat4 transform, vec4 color);
	void removeObject(const std::shared_ptr<TextObject> &object);

	void update();
	void render();

private:
	Font font;
	std::vector<std::shared_ptr<TextObject>> objects;

	opengl::Program prog;
	opengl::Texture texture;
	Mesh mesh;
};