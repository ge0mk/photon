#include <gui.hpp>

void GuiSystem::beginFrame(float time, float dt) {
	drawList.clear();
	textRenderer.clear();

	depth = 0.0f;
}

void GuiSystem::endFrame() {
	drawList.update();
	textRenderer.update();
}

void GuiSystem::render(mat4 transform) {
	drawList.render();
	textRenderer.render(transform);
}

void GuiSystem::pushStyle(const GuiStyle &style) {
	styleStack.push_back(style);
}

GuiStyle& GuiSystem::currentStyle() {
	if(styleStack.size() > 0) {
		return styleStack.back();
	}
	return m_defaultStyle;
}

GuiStyle& GuiSystem::defaultStyle() {
	return m_defaultStyle;
}

void GuiSystem::popStyle() {
	if(styleStack.size() > 0) {
		styleStack.pop_back();
	}
}

vec2 GuiSystem::text(const std::string &text, vec2 pos) {
	auto obj = textRenderer.createObject(text, mat4().translate(vec3(pos.x, -pos.y, 0)), currentStyle().palette.text);
	vec2 size = 0;//textRenderer.calcSize(obj);
	return size;
}

void GuiSystem::rect(vec2 pos, vec2 size) {
	const GuiStyle &style = currentStyle();

	float radius = min(size.x, size.y) / 2 * style.rounding;
	float left = radius;
	float right = -radius;
	float top = radius;
	float bottom = -radius;

	vec2 tl = pos;
	vec2 tr = pos + vec2(size.x, 0);
	vec2 br = pos + vec2(size.x, size.y);
	vec2 bl = pos + vec2(0, size.y);
	vec2 itl = tl + vec2(left, top);
	vec2 itr = tr + vec2(right, top);
	vec2 ibr = br + vec2(right, bottom);
	vec2 ibl = bl + vec2(left, bottom);
	vec2 mtl = tl + vec2(left, 0);
	vec2 mlt = tl + vec2(0, top);
	vec2 mtr = tr + vec2(right, 0);
	vec2 mrt = tr + vec2(0, top);
	vec2 mbl = bl + vec2(left, 0);
	vec2 mlb = bl + vec2(0, bottom);
	vec2 mbr = br + vec2(right, 0);
	vec2 mrb = br + vec2(0, bottom);

	unsigned vitl = drawList.addVertex(Vertex(vec3(itl, depth), style.palette.background, vec2(0)));
	unsigned vitr = drawList.addVertex(Vertex(vec3(itr, depth), style.palette.background, vec2(0)));
	unsigned vibr = drawList.addVertex(Vertex(vec3(ibr, depth), style.palette.background, vec2(0)));
	unsigned vibl = drawList.addVertex(Vertex(vec3(ibl, depth), style.palette.background, vec2(0)));

	drawList.addQuad(vitl, vitr, vibr, vibl);

	unsigned vmtl = drawList.addVertex(Vertex(vec3(mtl, depth), style.palette.background, vec2(0)));
	unsigned vmtr = drawList.addVertex(Vertex(vec3(mtr, depth), style.palette.background, vec2(0)));
	unsigned vmlt = drawList.addVertex(Vertex(vec3(mlt, depth), style.palette.background, vec2(0)));
	unsigned vmrt = drawList.addVertex(Vertex(vec3(mrt, depth), style.palette.background, vec2(0)));
	unsigned vmbr = drawList.addVertex(Vertex(vec3(mbr, depth), style.palette.background, vec2(0)));
	unsigned vmbl = drawList.addVertex(Vertex(vec3(mbl, depth), style.palette.background, vec2(0)));
	unsigned vmrb = drawList.addVertex(Vertex(vec3(mrb, depth), style.palette.background, vec2(0)));
	unsigned vmlb = drawList.addVertex(Vertex(vec3(mlb, depth), style.palette.background, vec2(0)));

	drawList.addQuad(vitl, vmtl, vmtr, vitr);
	drawList.addQuad(vitr, vmrt, vmrb, vibr);
	drawList.addQuad(vibr, vmbr, vmbl, vibl);
	drawList.addQuad(vibl, vmlb, vmlt, vitl);

	//corners
	if(style.rounding > 0.0f) {
		for(unsigned i = 0; i < style.segments; i++) {
			float a1 = (float(i) / style.segments) * (3.141592654 / 2);
			float a2 = (float(i + 1) / style.segments) * (3.141592654 / 2);
			vec2 v0 = vec2(0, 0);
			vec2 v1 = vec2(sin(a1), cos(a1)) * radius;
			vec2 v2 = vec2(sin(a2), cos(a2)) * radius;

			vec2 ctl0 = itl + vec2(-v0.x,-v0.y);
			vec2 ctl1 = itl + vec2(-v1.x,-v1.y);
			vec2 ctl2 = itl + vec2(-v2.x,-v2.y);
			vec2 ctr0 = itr + vec2( v0.x,-v0.y);
			vec2 ctr1 = itr + vec2( v1.x,-v1.y);
			vec2 ctr2 = itr + vec2( v2.x,-v2.y);
			vec2 cbr0 = ibr + vec2( v0.x, v0.y);
			vec2 cbr1 = ibr + vec2( v1.x, v1.y);
			vec2 cbr2 = ibr + vec2( v2.x, v2.y);
			vec2 cbl0 = ibl + vec2(-v0.x, v0.y);
			vec2 cbl1 = ibl + vec2(-v1.x, v1.y);
			vec2 cbl2 = ibl + vec2(-v2.x, v2.y);

			unsigned vctl0 = drawList.addVertex(Vertex(vec3(ctl0, depth), style.palette.background, vec2(0)));
			unsigned vctl1 = drawList.addVertex(Vertex(vec3(ctl1, depth), style.palette.background, vec2(0)));
			unsigned vctl2 = drawList.addVertex(Vertex(vec3(ctl2, depth), style.palette.background, vec2(0)));
			unsigned vctr0 = drawList.addVertex(Vertex(vec3(ctr0, depth), style.palette.background, vec2(0)));
			unsigned vctr1 = drawList.addVertex(Vertex(vec3(ctr1, depth), style.palette.background, vec2(0)));
			unsigned vctr2 = drawList.addVertex(Vertex(vec3(ctr2, depth), style.palette.background, vec2(0)));
			unsigned vcbr0 = drawList.addVertex(Vertex(vec3(cbr0, depth), style.palette.background, vec2(0)));
			unsigned vcbr1 = drawList.addVertex(Vertex(vec3(cbr1, depth), style.palette.background, vec2(0)));
			unsigned vcbr2 = drawList.addVertex(Vertex(vec3(cbr2, depth), style.palette.background, vec2(0)));
			unsigned vcbl0 = drawList.addVertex(Vertex(vec3(cbl0, depth), style.palette.background, vec2(0)));
			unsigned vcbl1 = drawList.addVertex(Vertex(vec3(cbl1, depth), style.palette.background, vec2(0)));
			unsigned vcbl2 = drawList.addVertex(Vertex(vec3(cbl2, depth), style.palette.background, vec2(0)));

			drawList.addTriangle(vctl0, vctl1, vctl2);
			drawList.addTriangle(vctr0, vctr1, vctr2);
			drawList.addTriangle(vcbr0, vcbr1, vcbr2);
			drawList.addTriangle(vcbl0, vcbl1, vcbl2);
		}
	}
}

void GuiSystem::border(vec2 pos, vec2 size) {
	;
}
