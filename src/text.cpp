#include <text.hpp>

Font::Font(const std::string &path) {
	std::ifstream src(path, std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());

	if(stbtt_InitFont(&font, (const unsigned char*)buffer.c_str(), 0) == 0) {
		throw std::runtime_error("couldn't load font '" + path + "'!");
	}
}

int Font::findGlyphIndex(int unicode) {
	return stbtt_FindGlyphIndex(&font, unicode);
}

float Font::scaleForPixelHeight(float pixels) {
	return stbtt_ScaleForPixelHeight(&font, pixels);
}

float Font::scaleForMappingEmToPixels(float pixels) {
	return stbtt_ScaleForMappingEmToPixels(&font, pixels);
}

Font::VMetrics Font::getFontVMetrics() {
	VMetrics metrics;
	stbtt_GetFontVMetrics(&font, &metrics.ascent, &metrics.descent, &metrics.lineGap);
	return metrics;
}

Font::VMetrics Font::getFontVMetricsOS2() {
	VMetrics metrics;
	stbtt_GetFontVMetricsOS2(&font, &metrics.ascent, &metrics.descent, &metrics.lineGap);
	return metrics;
}

Font::BoundingBox Font::getFontBoundingBox() {
	BoundingBox box;
	stbtt_GetFontBoundingBox(&font, &box.bl.x, &box.bl.y, &box.tr.x, &box.tr.y);
	return box;
}

Font::HMetrics Font::getCodepointHMetrics(int codepoint) {
	HMetrics metrics;
	stbtt_GetCodepointHMetrics(&font, codepoint, &metrics.advanceWidth, &metrics.leftSideBearing);
	return metrics;
}

int Font::getCodepointKernAdvance(int c1, int c2) {
	return stbtt_GetCodepointKernAdvance(&font, c1, c2);
}

Font::BoundingBox Font::getCodepointBoundingBox(int codepoint) {
	BoundingBox box;
	stbtt_GetCodepointBox(&font, codepoint, &box.bl.x, &box.bl.y, &box.tr.x, &box.tr.y);
	return box;
}

Font::HMetrics Font::getGlyphHMetrics(int glyph) {
	HMetrics metrics;
	stbtt_GetGlyphHMetrics(&font, glyph, &metrics.advanceWidth, &metrics.leftSideBearing);
	return metrics;}

int Font::getGlyphKernAdvance(int g1, int g2) {
	return stbtt_GetGlyphKernAdvance(&font, g1, g2);
}

Font::BoundingBox Font::getGlyphBoundingBox(int glyph) {
	BoundingBox box;
	stbtt_GetGlyphBox(&font, glyph, &box.bl.x, &box.bl.y, &box.tr.x, &box.tr.y);
	return box;
}

Font::Glyph Font::getGlyph(int codepoint, float scale) {
	int index = findGlyphIndex(codepoint);
	HMetrics hm = getCodepointHMetrics(codepoint);
	BoundingBox box = getCodepointBoundingBox(codepoint);

	return {
		codepoint, index,
		hm.advanceWidth * scale, scale,
		vec2(box.bl) * scale, vec2(box.tr) * scale
	};
}

Font::Glyph Font::getGlyph(int index) {
	return glyphs[index];
}

Image& Font::generateBitmap(float height, GlyphRange range) {
	glyphs.clear();
	auto b = getFontBoundingBox();
	const vec2 scale = scaleForPixelHeight(height);
	const ivec2 glyphSize = height;//vec2(b.tr - b.bl) * scale;

	std::cout<<b.bl<<":"<<b.tr<<glyphSize<<scale<<"\n";
	const int columns = 16;
	const int rows = range.range / columns + (range.range % columns != 0);
	const int stride = columns * glyphSize.x * sizeof(unsigned char);
	ivec2 size = ivec2(columns, rows) * ceil(glyphSize);
	std::cout<<columns<<"|"<<rows<<":"<<size<<"\n";
	texture = Image(size, 1);
	unsigned char *data = texture.data();

	for(int i = 0; i < range.range; i++) {
		int codepoint = range.startCodepoint + i;
		Glyph g = getGlyph(codepoint, scale.y);
		const int x = (i % columns) * glyphSize.x, x2 = x + glyphSize.x;
		const int y = (i / columns) * glyphSize.y, y2 = y + glyphSize.y;
		vec2 uvtl = vec2(x, y) / vec2(size);
		vec2 uvbr = vec2(x2, y2) / vec2(size);
		g.uvbr = uvbr;
		g.uvtl = uvtl;
		unsigned char *localData = &data[y * stride + x];
		stbtt_MakeCodepointBitmap(&font, localData, glyphSize.x, glyphSize.y, stride, scale.x, scale.y, codepoint);
		std::cout<<"'"<<char(g.unicode)<<"':"<<g.advanceX<<": "<<g.bl<<": "<<g.tr<<"\n";
		glyphs.push_back(g);
	}

	return texture;
}

Image Font::generateSDF(float scale, int padding, GlyphRange range) {
	;
}

TextObject::TextObject(const std::string &text, mat4 transform, vec4 color)
: text(text), transform(transform), color(color) {}

TextRenderer::TextRenderer(const Font &font) : font(font) {
	std::ifstream src("res/text.glsl", std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());
	prog = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::FragmentStage);

	texture.load(this->font.generateBitmap(16, {' ', 96}));
}

std::shared_ptr<TextObject> TextRenderer::createObject(const std::string &text, mat4 transform, vec4 color) {
	objects.push_back(std::shared_ptr<TextObject>(new TextObject(text, transform, color)));
	return objects.back();
}

void TextRenderer::removeObject(const std::shared_ptr<TextObject> &object) {
	;
}

void TextRenderer::update() {
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	float scale = font.scaleForPixelHeight(16);

	for(const auto &object : objects) {
		std::string text = object->text;

		auto quad = [&](vec2 bl, vec2 tr, vec2 uvbl, vec2 uvtr) {
			unsigned baseIndex = vertices.size();
			vec2 tl = vec2(bl.x, tr.y);
			vec2 br = vec2(tr.x, bl.y);
			vec2 uvtl = vec2(uvbl.x, uvtr.y);
			vec2 uvbr = vec2(uvtr.x, uvbl.y);

			vertices.push_back(Vertex{bl, uvbl, object->color});
			vertices.push_back(Vertex{tl, uvtl, object->color});
			vertices.push_back(Vertex{tr, uvtr, object->color});
			vertices.push_back(Vertex{br, uvbr, object->color});

			indices.push_back(baseIndex + 0);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
			indices.push_back(baseIndex + 0);
		};

		int x = 0, y = 0;
		for(unsigned i = 0; i < text.length(); i++) {
			std::cout<<int(text[i])<<":"<<font.glyphs.size()<<"\n";
			Font::Glyph glyph = font.getGlyph(text[i] - ' ');

			float x1 = i + glyph.bl.x * scale;
			float x2 = i + glyph.tr.x * scale;
			float y1 = 0 + glyph.bl.y * scale;
			float y2 = 0 + glyph.tr.y * scale;

			float uvx1 = i + glyph.uvtl.x * scale;
			float uvx2 = i + glyph.uvbr.x * scale;
			float uvy1 = 0 + glyph.uvbr.y * scale;
			float uvy2 = 0 + glyph.uvtl.y * scale;

			quad(vec2(x1, y1), vec2(x2, y2), vec2(uvx1, uvy1), vec2(uvx2, uvy2));

			x += glyph.advanceX * scale;
		}
	}

	mesh.setData({vertices, indices});
}

void TextRenderer::render() {
	prog.use();
	texture.bind();
	texture.activate();
	mesh.drawElements();
}
