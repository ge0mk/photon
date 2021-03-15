#pragma once

#include <functional>

#include <math/matrix.hpp>
#include <math/vector.hpp>
#include <math/quaternion.hpp>

struct Style {
	struct Palette {
		math::vec4 text, background;
		math::vec4 primary, secondary;
		math::vec4 error, warning, info, success;
	} palette;

	struct PaletteModifiers {
		math::vec4 active, hovered, selected, disabled;
	} modifiers;

	float rounding;
	// corner rounding [0-1]
	int segments;
	// corner segments
	math::vec4 margin, border, padding;
	// size of (margin/border/padding) (left|top|right|bottom)
	// margin: outside of border, padding: inside border
	math::vec2 spacing, alignment, grid;
	// spacing: distance between children
	// alignment: (0|0) -> top left, (0|1) -> bottom left, (1|1) -> bottom right, (.5|.5) center
	// grid: number of items per row/column, 0 -> inf.

	Style(bool dark = true) {
		palette.text = dark ? math::vec4(1.0f, 1.0f, 1.0f, 1.0f) : math::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		palette.background = dark ? math::vec4(0.149f, 0.149f, 0.149f, 1.0f) : math::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		palette.primary = math::vec4(0.301f, 0.556f, 0.858f, 1.0f);
		palette.secondary = math::vec4(0.309f, 0.768f, 0.309f, 1.0f);
		palette.error = math::vec4(0.850f, 0.239f, 0.239f, 1.0f);
		palette.warning = math::vec4(0.937f, 0.741f, 0.223f, 1.0f);
		palette.info = math::vec4(0.317f, 0.654f, 0.803f, 1.0f);
		palette.success = math::vec4(0.494f, 0.850f, 0.270f, 1.0f);

		modifiers.active = dark ? math::vec4(1.2f, 1.2f, 1.2f, 1.0f) : math::vec4(0.8f, 0.8f, 0.8f, 1.0f);
		modifiers.hovered = dark ? math::vec4(1.1f, 1.1f, 1.1f, 1.0f) : math::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		modifiers.selected = dark ? math::vec4(1.0f, 1.0f, 5.0f, 1.0f) : math::vec4(0.5f, 0.5f, 1.0f, 1.0f);
		modifiers.disabled = dark ? math::vec4(1.0f, 1.0f, 1.0f, 0.5f) : math::vec4(0.5f, 0.5f, 0.5f, 1.0f);

		rounding = 0.5f;
		segments = 32;

		margin = math::vec4(1.0f, 1.0f, 1.0f, 1.0f) * 16;
		border = math::vec4(4.0f, 1.0f, 1.0f, 8.0f) * 4;
		padding = math::vec4(1.0f, 1.0f, 1.0f, 1.0f) * 4;

		spacing = math::vec2(5.0f, 5.0f);
		alignment = math::vec2(0.0f, 0.0f);
		grid = math::vec2(0.0f, 0.0f);
	}
};

struct Vertex {
	math::vec3 pos;
	math::vec4 color;
	math::vec2 uv;
};

class YeetGuiState {
public:
	void pushStyle(Style style) {
		styleStack.push_back(style);
	}
	Style getCurrentStyle() {
		if(styleStack.size() > 0) {
			return styleStack.back();
		}
		return defaultStyle;
	}
	void popStyle() {
		if(styleStack.size() > 0) {
			styleStack.pop_back();
		}
	}

	int depth = 0;
	math::vec2 cursor, content;
	Style defaultStyle = Style();
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

private:
	std::vector<Style> styleStack = {};
} state;

unsigned addVertex(Vertex v) {
	state.vertices.push_back(v);
	return state.vertices.size() - 1;
}

void addTriangle(unsigned a, unsigned b, unsigned c) {
	state.indices.push_back(a);
	state.indices.push_back(b);
	state.indices.push_back(c);
}

void addQuad(unsigned a, unsigned b, unsigned c, unsigned d) {
	addTriangle(a, b, c);
	addTriangle(c, d, a);
}

std::pair<std::vector<Vertex>, std::vector<unsigned>> frame(std::function<void()> callback) {
	state.depth = 0;
	state.cursor = math::vec2(0, 0);
	state.vertices = {};
	state.indices = {};
	callback();
	return {state.vertices, state.indices};
}

void border(math::vec2 pos, math::vec2 size) {
	Style style = state.getCurrentStyle();
	float rounding = pow(1.0f - style.rounding, 16.0f);
	math::vec2 contentPos = pos + style.border.xy;
	math::vec2 contentSize = size - style.border.xy - style.border.zw;
	float innerRadius = math::min(contentSize.x, contentSize.y) / 2 * style.rounding;

	float left = innerRadius + style.border.x;
	float top = innerRadius + style.border.y;
	float right = -(innerRadius + style.border.z);
	float bottom = -(innerRadius + style.border.w);

	math::vec2 tl = pos;
	math::vec2 tr = pos + math::vec2(size.x, 0);
	math::vec2 br = pos + math::vec2(size.x, size.y);
	math::vec2 bl = pos + math::vec2(0, size.y);
	math::vec2 itl = tl + math::vec2(left, top);
	math::vec2 itr = tr + math::vec2(right, top);
	math::vec2 ibr = br + math::vec2(right, bottom);
	math::vec2 ibl = bl + math::vec2(left, bottom);

	math::vec2 mtl = itl + math::vec2(0, -innerRadius);
	math::vec2 mlt = itl + math::vec2(-innerRadius, 0);
	math::vec2 mrt = itr + math::vec2(innerRadius, 0);
	math::vec2 mtr = itr + math::vec2(0, -innerRadius);
	math::vec2 mbl = ibl + math::vec2(0, innerRadius);
	math::vec2 mlb = ibl + math::vec2(-innerRadius, 0);
	math::vec2 mrb = ibr + math::vec2(innerRadius, 0);
	math::vec2 mbr = ibr + math::vec2(0, innerRadius);

	math::vec2 otl = math::mix(itl + math::vec2(0, -(innerRadius + style.border.y)), tl, rounding);
	math::vec2 olt = math::mix(itl + math::vec2(-(innerRadius + style.border.x), 0), tl, rounding);
	math::vec2 ort = math::mix(itr + math::vec2((innerRadius + style.border.z), 0), tr, rounding);
	math::vec2 otr = math::mix(itr + math::vec2(0, -(innerRadius + style.border.y)), tr, rounding);
	math::vec2 obl = math::mix(ibl + math::vec2(0, (innerRadius + style.border.w)), bl, rounding);
	math::vec2 olb = math::mix(ibl + math::vec2(-(innerRadius + style.border.x), 0), bl, rounding);
	math::vec2 orb = math::mix(ibr + math::vec2((innerRadius + style.border.z), 0), br, rounding);
	math::vec2 obr = math::mix(ibr + math::vec2(0, (innerRadius + style.border.w)), br, rounding);

	unsigned vmtl = addVertex({math::vec3(mtl, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vmlt = addVertex({math::vec3(mlt, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vmtr = addVertex({math::vec3(mtr, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vmrt = addVertex({math::vec3(mrt, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vmbl = addVertex({math::vec3(mbl, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vmlb = addVertex({math::vec3(mlb, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vmbr = addVertex({math::vec3(mbr, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vmrb = addVertex({math::vec3(mrb, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned votl = addVertex({math::vec3(otl, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned volt = addVertex({math::vec3(olt, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned votr = addVertex({math::vec3(otr, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vort = addVertex({math::vec3(ort, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vobl = addVertex({math::vec3(obl, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned volb = addVertex({math::vec3(olb, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vobr = addVertex({math::vec3(obr, state.depth + 0.5), style.palette.primary, math::vec2(0)});
	unsigned vorb = addVertex({math::vec3(orb, state.depth + 0.5), style.palette.primary, math::vec2(0)});

	addQuad(vmtl, vmtr, votr, votl);
	addQuad(vmlt, vmlb, volb, volt);
	addQuad(vmrt, vmrb, vorb, vort);
	addQuad(vmbl, vmbr, vobr, vobl);

	//corners
	if(style.rounding > 0.0f) {
		for(unsigned i = 0; i < style.segments; i++) {
			float p1 = (float(i) / style.segments);
			float p2 = (float(i + 1) / style.segments);
			float a1 = p1 * (3.141592654 / 2);
			float a2 = p2 * (3.141592654 / 2);
			float outerRadiusl1 = innerRadius + math::mix(style.border.x, style.border.y, p1);
			float outerRadiusl2 = innerRadius + math::mix(style.border.x, style.border.y, p2);
			float outerRadiust1 = innerRadius + math::mix(style.border.z, style.border.y, p1);
			float outerRadiust2 = innerRadius + math::mix(style.border.z, style.border.y, p2);
			float outerRadiusr1 = innerRadius + math::mix(style.border.z, style.border.w, p1);
			float outerRadiusr2 = innerRadius + math::mix(style.border.z, style.border.w, p2);
			float outerRadiusb1 = innerRadius + math::mix(style.border.x, style.border.w, p1);
			float outerRadiusb2 = innerRadius + math::mix(style.border.x, style.border.w, p2);

			math::vec2 v = math::vec2(cos(a1), sin(a1));
			math::vec2 w = math::vec2(cos(a2), sin(a2));
			math::vec2 v0 = v * innerRadius;
			math::vec2 v1 = w * innerRadius;
			math::vec2 v2l = w * outerRadiusl2;
			math::vec2 v3l = v * outerRadiusl1;
			math::vec2 v2t = w * outerRadiust2;
			math::vec2 v3t = v * outerRadiust1;
			math::vec2 v2r = w * outerRadiusr2;
			math::vec2 v3r = v * outerRadiusr1;
			math::vec2 v2b = w * outerRadiusb2;
			math::vec2 v3b = v * outerRadiusb1;

			math::vec2 ctl0 = itl + math::vec2(-v0.x,-v0.y);
			math::vec2 ctl1 = itl + math::vec2(-v1.x,-v1.y);
			math::vec2 ctl2 = itl + mix(math::vec2(-v2l.x,-v2l.y), -math::vec2(left, top), rounding);
			math::vec2 ctl3 = itl + mix(math::vec2(-v3l.x,-v3l.y), -math::vec2(left, top), rounding);
			math::vec2 ctr0 = itr + math::vec2( v0.x,-v0.y);
			math::vec2 ctr1 = itr + math::vec2( v1.x,-v1.y);
			math::vec2 ctr2 = itr + mix(math::vec2( v2t.x,-v2t.y), -math::vec2(right, top), rounding);
			math::vec2 ctr3 = itr + mix(math::vec2( v3t.x,-v3t.y), -math::vec2(right, top), rounding);
			math::vec2 cbr0 = ibr + math::vec2( v0.x, v0.y);
			math::vec2 cbr1 = ibr + math::vec2( v1.x, v1.y);
			math::vec2 cbr2 = ibr + mix(math::vec2( v2r.x, v2r.y), -math::vec2(right, bottom), rounding);
			math::vec2 cbr3 = ibr + mix(math::vec2( v3r.x, v3r.y), -math::vec2(right, bottom), rounding);
			math::vec2 cbl0 = ibl + math::vec2(-v0.x, v0.y);
			math::vec2 cbl1 = ibl + math::vec2(-v1.x, v1.y);
			math::vec2 cbl2 = ibl + mix(math::vec2(-v2b.x, v2b.y), -math::vec2(left, bottom), rounding);
			math::vec2 cbl3 = ibl + mix(math::vec2(-v3b.x, v3b.y), -math::vec2(left, bottom), rounding);

			unsigned vctl0 = addVertex({math::vec3(ctl0, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vctl1 = addVertex({math::vec3(ctl1, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vctl2 = addVertex({math::vec3(ctl2, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vctl3 = addVertex({math::vec3(ctl3, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vctr0 = addVertex({math::vec3(ctr0, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vctr1 = addVertex({math::vec3(ctr1, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vctr2 = addVertex({math::vec3(ctr2, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vctr3 = addVertex({math::vec3(ctr3, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbr0 = addVertex({math::vec3(cbr0, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbr1 = addVertex({math::vec3(cbr1, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbr2 = addVertex({math::vec3(cbr2, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbr3 = addVertex({math::vec3(cbr3, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbl0 = addVertex({math::vec3(cbl0, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbl1 = addVertex({math::vec3(cbl1, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbl2 = addVertex({math::vec3(cbl2, state.depth + 0.5), style.palette.primary, math::vec2(0)});
			unsigned vcbl3 = addVertex({math::vec3(cbl3, state.depth + 0.5), style.palette.primary, math::vec2(0)});

			addQuad(vctl0, vctl1, vctl2, vctl3);
			addQuad(vctr0, vctr1, vctr2, vctr3);
			addQuad(vcbr0, vcbr1, vcbr2, vcbr3);
			addQuad(vcbl0, vcbl1, vcbl2, vcbl3);
		}
	}
}

void rect(math::vec2 pos, math::vec2 size) {
	Style style = state.getCurrentStyle();

	float radius = math::min(size.x, size.y) / 2 * style.rounding;
	float left = radius;
	float top = radius;
	float right = -radius;
	float bottom = -radius;

	math::vec2 tl = pos;
	math::vec2 tr = pos + math::vec2(size.x, 0);
	math::vec2 br = pos + math::vec2(size.x, size.y);
	math::vec2 bl = pos + math::vec2(0, size.y);
	math::vec2 itl = tl + math::vec2(left, top);
	math::vec2 itr = tr + math::vec2(right, top);
	math::vec2 ibr = br + math::vec2(right, bottom);
	math::vec2 ibl = bl + math::vec2(left, bottom);
	math::vec2 mtl = tl + math::vec2(left, 0);
	math::vec2 mlt = tl + math::vec2(0, top);
	math::vec2 mtr = tr + math::vec2(right, 0);
	math::vec2 mrt = tr + math::vec2(0, top);
	math::vec2 mbl = bl + math::vec2(left, 0);
	math::vec2 mlb = bl + math::vec2(0, bottom);
	math::vec2 mbr = br + math::vec2(right, 0);
	math::vec2 mrb = br + math::vec2(0, bottom);

	unsigned vitl = addVertex({math::vec3(itl, state.depth), style.palette.background, math::vec2(0)});
	unsigned vitr = addVertex({math::vec3(itr, state.depth), style.palette.background, math::vec2(0)});
	unsigned vibr = addVertex({math::vec3(ibr, state.depth), style.palette.background, math::vec2(0)});
	unsigned vibl = addVertex({math::vec3(ibl, state.depth), style.palette.background, math::vec2(0)});

	addQuad(vitl, vitr, vibr, vibl);

	unsigned vmtl = addVertex({math::vec3(mtl, state.depth), style.palette.background, math::vec2(0)});
	unsigned vmtr = addVertex({math::vec3(mtr, state.depth), style.palette.background, math::vec2(0)});
	unsigned vmlt = addVertex({math::vec3(mlt, state.depth), style.palette.background, math::vec2(0)});
	unsigned vmrt = addVertex({math::vec3(mrt, state.depth), style.palette.background, math::vec2(0)});
	unsigned vmbr = addVertex({math::vec3(mbr, state.depth), style.palette.background, math::vec2(0)});
	unsigned vmbl = addVertex({math::vec3(mbl, state.depth), style.palette.background, math::vec2(0)});
	unsigned vmrb = addVertex({math::vec3(mrb, state.depth), style.palette.background, math::vec2(0)});
	unsigned vmlb = addVertex({math::vec3(mlb, state.depth), style.palette.background, math::vec2(0)});

	addQuad(vitl, vmtl, vmtr, vitr);
	addQuad(vitr, vmrt, vmrb, vibr);
	addQuad(vibr, vmbr, vmbl, vibl);
	addQuad(vibl, vmlb, vmlt, vitl);

	//corners
	if(style.rounding > 0.0f) {
		for(unsigned i = 0; i < style.segments; i++) {
			float a1 = (float(i) / style.segments) * (3.141592654 / 2);
			float a2 = (float(i + 1) / style.segments) * (3.141592654 / 2);
			math::vec2 v0 = math::vec2(0, 0);
			math::vec2 v1 = math::vec2(sin(a1), cos(a1)) * radius;
			math::vec2 v2 = math::vec2(sin(a2), cos(a2)) * radius;

			math::vec2 ctl0 = itl + math::vec2(-v0.x,-v0.y);
			math::vec2 ctl1 = itl + math::vec2(-v1.x,-v1.y);
			math::vec2 ctl2 = itl + math::vec2(-v2.x,-v2.y);
			math::vec2 ctr0 = itr + math::vec2( v0.x,-v0.y);
			math::vec2 ctr1 = itr + math::vec2( v1.x,-v1.y);
			math::vec2 ctr2 = itr + math::vec2( v2.x,-v2.y);
			math::vec2 cbr0 = ibr + math::vec2( v0.x, v0.y);
			math::vec2 cbr1 = ibr + math::vec2( v1.x, v1.y);
			math::vec2 cbr2 = ibr + math::vec2( v2.x, v2.y);
			math::vec2 cbl0 = ibl + math::vec2(-v0.x, v0.y);
			math::vec2 cbl1 = ibl + math::vec2(-v1.x, v1.y);
			math::vec2 cbl2 = ibl + math::vec2(-v2.x, v2.y);

			unsigned vctl0 = addVertex({math::vec3(ctl0, state.depth), style.palette.background, math::vec2(0)});
			unsigned vctl1 = addVertex({math::vec3(ctl1, state.depth), style.palette.background, math::vec2(0)});
			unsigned vctl2 = addVertex({math::vec3(ctl2, state.depth), style.palette.background, math::vec2(0)});
			unsigned vctr0 = addVertex({math::vec3(ctr0, state.depth), style.palette.background, math::vec2(0)});
			unsigned vctr1 = addVertex({math::vec3(ctr1, state.depth), style.palette.background, math::vec2(0)});
			unsigned vctr2 = addVertex({math::vec3(ctr2, state.depth), style.palette.background, math::vec2(0)});
			unsigned vcbr0 = addVertex({math::vec3(cbr0, state.depth), style.palette.background, math::vec2(0)});
			unsigned vcbr1 = addVertex({math::vec3(cbr1, state.depth), style.palette.background, math::vec2(0)});
			unsigned vcbr2 = addVertex({math::vec3(cbr2, state.depth), style.palette.background, math::vec2(0)});
			unsigned vcbl0 = addVertex({math::vec3(cbl0, state.depth), style.palette.background, math::vec2(0)});
			unsigned vcbl1 = addVertex({math::vec3(cbl1, state.depth), style.palette.background, math::vec2(0)});
			unsigned vcbl2 = addVertex({math::vec3(cbl2, state.depth), style.palette.background, math::vec2(0)});

			addTriangle(vctl0, vctl1, vctl2);
			addTriangle(vctr0, vctr1, vctr2);
			addTriangle(vcbr0, vcbr1, vcbr2);
			addTriangle(vcbl0, vcbl1, vcbl2);
		}
	}
}

void widget(std::function<void()> content) {
	Style style = state.getCurrentStyle();
	math::vec2 pos = state.cursor;
	math::vec2 borderPos = pos + style.margin.xy;
	math::vec2 contentPos = borderPos + style.border.xy;

	math::vec2 childPos = contentPos + style.padding.xy;

	std::cout<<pos<<":"<<childPos<<"\n";
	state.cursor = childPos;
	state.depth++;
	content();
	state.depth--;

	math::vec2 size = state.cursor - childPos;
	std::cout<<size<<"\n";

	math::vec2 contentSize = size + style.padding.xy + style.padding.zw;
	math::vec2 borderSize = contentSize + style.border.xy + style.border.zw;
	math::vec2 outerSize = borderSize + style.margin.xy + style.margin.zw;
	state.cursor += outerSize;

	border(borderPos, borderSize);
	rect(contentPos, contentSize);
}

void widget(math::vec2 size, std::function<void()> content) {
	Style style = state.getCurrentStyle();
	math::vec2 pos = state.cursor;
	math::vec2 borderPos = pos + style.margin.xy;
	math::vec2 contentPos = borderPos + style.border.xy;

	math::vec2 childPos = contentPos + style.padding.xy;
	std::cout<<childPos<<"\n";
	state.cursor = childPos;
	state.depth++;
	content();
	state.depth--;

	//math::vec2 size = state.cursor - childPos;

	math::vec2 borderSize = size - style.margin.xy - style.margin.zw;
	math::vec2 contentSize = borderSize - style.border.xy - style.border.zw;
	math::vec2 childSize = contentSize - style.padding.xy - style.padding.zw;

	border(borderPos, borderSize);
	rect(contentPos, contentSize);
}