#include <gui.hpp>

void GuiStyleStack::pushStyle(const GuiStyle &style) {
	styles.push_back(style);
}

GuiStyle& GuiStyleStack::currentStyle() {
	if(styles.size() > 0) {
		return styles.back();
	}
	return m_defaultStyle;
}

GuiStyle& GuiStyleStack::defaultStyle() {
	return m_defaultStyle;
}

void GuiStyleStack::popStyle() {
	if(styles.size() > 0) {
		styles.pop_back();
	}
}

void GuiIO::onFramebufferResized(ivec2 size) {
	frameBufferSize = size;
}

GuiSystem::GuiSystem(freetype::Font &&font) : textRenderer(std::move(font)) {
	std::ifstream src("res/gui.glsl", std::ios::ate);
	std::string buffer(src.tellg(), '\0');
	src.seekg(src.beg);
	src.read(buffer.data(), buffer.size());
	prog = opengl::Program::load(buffer, opengl::Shader::VertexStage | opengl::Shader::FragmentStage);
	prog.use();
	prog.setUniform("sampler", 0);

	transformUBO.bindBase(0);
	transformUBO.setData(mat4());
}

void GuiSystem::beginFrame(float time, float dt) {
	mesh.clear();
	textRenderer.clear();
	depth = 0.0f;
}

void GuiSystem::endFrame() {
	mesh.update();
	textRenderer.update();
	//std::cout<<mesh.vertexCount<<" vertices, "<<mesh.indexCount<<" indices\n";
	//std::cout<<mesh.triangles<<" triangles, "<<mesh.quads<<" quads\n";
}

void GuiSystem::render(mat4 transform) {
	transform = transform.translate(vec3(-1, 1, 0)).scale(vec3(vec2(1.0f) / frameBufferSize * 2.0f, 1.0f));
	prog.use();
	transformUBO.bindBase(0);
	transformUBO.update(transform);
	mesh.render();

	textRenderer.render(transform);
}

void GuiSystem::rect(vec2 tl, vec2 br, vec4 color, vec2 uvtl, vec2 uvbr) {
	tl.y = -tl.y;
	br.y = -br.y;
	mesh.addQuad(
		Vertex(tl, uvtl, color),
		Vertex(vec2(br.x, tl.y), vec2(uvbr.x, uvtl.y), color),
		Vertex(br, uvbr, color),
		Vertex(vec2(tl.x, br.y), vec2(uvtl.x, uvbr.y), color)
	);
}

void GuiSystem::circle(vec2 center, float radius, float innerRadius, vec4 color, int segments) {
	circleSegment(center, radius, innerRadius, 0, 3.141592654f * 2.0f, color, segments);
}

void GuiSystem::circleSegment(vec2 center, float radius, float innerRadius, float astart, float aend, vec4 color, int segments) {
	if(segments == 0) {
		segments = abs(aend - astart) / 3.141592654f * 5;
	}
	segments = abs(segments);
	float adist = abs(aend - astart) / segments;
	center.y = -center.y;

	if(innerRadius == 0.0f) {
		for(int i = 0; i < segments; i++) {
			float start = astart + float(i) * adist;
			float end = astart + float(i + 1) * adist;

			vec2 vtl = center + vec2(sin(start), cos(start)) * radius;
			vec2 vtr = center + vec2(sin(end), cos(end)) * radius;

			mesh.addTriangle(
				Vertex(vec3(vtl, depth), vec2(0), color),
				Vertex(vec3(vtr, depth), vec2(0), color),
				Vertex(vec3(center, depth), vec2(0), color)
			);
		}
	}
	else {
		for(int i = 0; i < segments; i++) {
			float start = astart + float(i) * adist;
			float end = astart + float(i + 1) * adist;

			vec2 vtl = center + vec2(sin(start), cos(start)) * radius;
			vec2 vtr = center + vec2(sin(end), cos(end)) * radius;
			vec2 vbl = center + vec2(sin(start), cos(start)) * innerRadius;
			vec2 vbr = center + vec2(sin(end), cos(end)) * innerRadius;

			mesh.addQuad(
				Vertex(vec3(vtl, depth), vec2(0), color),
				Vertex(vec3(vtr, depth), vec2(0), color),
				Vertex(vec3(vbr, depth), vec2(0), color),
				Vertex(vec3(vbl, depth), vec2(0), color)
			);
		}
	}
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

	unsigned vitl = mesh.addVertex(Vertex(vec3(itl, depth), vec2(0), style.palette.background));
	unsigned vitr = mesh.addVertex(Vertex(vec3(itr, depth), vec2(0), style.palette.background));
	unsigned vibr = mesh.addVertex(Vertex(vec3(ibr, depth), vec2(0), style.palette.background));
	unsigned vibl = mesh.addVertex(Vertex(vec3(ibl, depth), vec2(0), style.palette.background));

	mesh.addQuad(vitl, vitr, vibr, vibl);

	unsigned vmtl = mesh.addVertex(Vertex(vec3(mtl, depth), vec2(0), style.palette.background));
	unsigned vmtr = mesh.addVertex(Vertex(vec3(mtr, depth), vec2(0), style.palette.background));
	unsigned vmlt = mesh.addVertex(Vertex(vec3(mlt, depth), vec2(0), style.palette.background));
	unsigned vmrt = mesh.addVertex(Vertex(vec3(mrt, depth), vec2(0), style.palette.background));
	unsigned vmbr = mesh.addVertex(Vertex(vec3(mbr, depth), vec2(0), style.palette.background));
	unsigned vmbl = mesh.addVertex(Vertex(vec3(mbl, depth), vec2(0), style.palette.background));
	unsigned vmrb = mesh.addVertex(Vertex(vec3(mrb, depth), vec2(0), style.palette.background));
	unsigned vmlb = mesh.addVertex(Vertex(vec3(mlb, depth), vec2(0), style.palette.background));

	mesh.addQuad(vitl, vmtl, vmtr, vitr);
	mesh.addQuad(vitr, vmrt, vmrb, vibr);
	mesh.addQuad(vibr, vmbr, vmbl, vibl);
	mesh.addQuad(vibl, vmlb, vmlt, vitl);

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

			unsigned vctl0 = mesh.addVertex(Vertex(vec3(ctl0, depth), vec2(0), style.palette.background));
			unsigned vctl1 = mesh.addVertex(Vertex(vec3(ctl1, depth), vec2(0), style.palette.background));
			unsigned vctl2 = mesh.addVertex(Vertex(vec3(ctl2, depth), vec2(0), style.palette.background));
			unsigned vctr0 = mesh.addVertex(Vertex(vec3(ctr0, depth), vec2(0), style.palette.background));
			unsigned vctr1 = mesh.addVertex(Vertex(vec3(ctr1, depth), vec2(0), style.palette.background));
			unsigned vctr2 = mesh.addVertex(Vertex(vec3(ctr2, depth), vec2(0), style.palette.background));
			unsigned vcbr0 = mesh.addVertex(Vertex(vec3(cbr0, depth), vec2(0), style.palette.background));
			unsigned vcbr1 = mesh.addVertex(Vertex(vec3(cbr1, depth), vec2(0), style.palette.background));
			unsigned vcbr2 = mesh.addVertex(Vertex(vec3(cbr2, depth), vec2(0), style.palette.background));
			unsigned vcbl0 = mesh.addVertex(Vertex(vec3(cbl0, depth), vec2(0), style.palette.background));
			unsigned vcbl1 = mesh.addVertex(Vertex(vec3(cbl1, depth), vec2(0), style.palette.background));
			unsigned vcbl2 = mesh.addVertex(Vertex(vec3(cbl2, depth), vec2(0), style.palette.background));

			mesh.addTriangle(vctl0, vctl1, vctl2);
			mesh.addTriangle(vctr0, vctr1, vctr2);
			mesh.addTriangle(vcbr0, vcbr1, vcbr2);
			mesh.addTriangle(vcbl0, vcbl1, vcbl2);
		}
	}
}

void GuiSystem::border(vec2 pos, vec2 size) {
	const GuiStyle &style = currentStyle();

	float rounding = pow(1.0f - style.rounding, 16.0f);
	vec2 contentPos = pos + style.border.xy;
	vec2 contentSize = size - style.border.xy - style.border.zw;
	float innerRadius = min(contentSize.x, contentSize.y) / 2 * style.rounding;

	float left = innerRadius + style.border.x;
	float top = innerRadius + style.border.y;
	float right = -(innerRadius + style.border.z);
	float bottom = -(innerRadius + style.border.w);

	vec2 tl = pos;
	vec2 tr = pos + vec2(size.x, 0);
	vec2 br = pos + vec2(size.x, size.y);
	vec2 bl = pos + vec2(0, size.y);
	vec2 itl = tl + vec2(left, top);
	vec2 itr = tr + vec2(right, top);
	vec2 ibr = br + vec2(right, bottom);
	vec2 ibl = bl + vec2(left, bottom);

	vec2 mtl = itl + vec2(0, -innerRadius);
	vec2 mlt = itl + vec2(-innerRadius, 0);
	vec2 mrt = itr + vec2(innerRadius, 0);
	vec2 mtr = itr + vec2(0, -innerRadius);
	vec2 mbl = ibl + vec2(0, innerRadius);
	vec2 mlb = ibl + vec2(-innerRadius, 0);
	vec2 mrb = ibr + vec2(innerRadius, 0);
	vec2 mbr = ibr + vec2(0, innerRadius);

	vec2 otl = mix(itl + vec2(0, -(innerRadius + style.border.y)), tl, rounding);
	vec2 olt = mix(itl + vec2(-(innerRadius + style.border.x), 0), tl, rounding);
	vec2 ort = mix(itr + vec2((innerRadius + style.border.z), 0), tr, rounding);
	vec2 otr = mix(itr + vec2(0, -(innerRadius + style.border.y)), tr, rounding);
	vec2 obl = mix(ibl + vec2(0, (innerRadius + style.border.w)), bl, rounding);
	vec2 olb = mix(ibl + vec2(-(innerRadius + style.border.x), 0), bl, rounding);
	vec2 orb = mix(ibr + vec2((innerRadius + style.border.z), 0), br, rounding);
	vec2 obr = mix(ibr + vec2(0, (innerRadius + style.border.w)), br, rounding);

	mesh.addQuad(
		Vertex(vec3(mtl, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(mtr, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(otr, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(otl, depth + 0.5), style.palette.primary, vec2(0))
	);
	mesh.addQuad(
		Vertex(vec3(mlt, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(mlb, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(olb, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(olt, depth + 0.5), style.palette.primary, vec2(0))
	);
	mesh.addQuad(
		Vertex(vec3(mrt, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(mrb, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(orb, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(ort, depth + 0.5), style.palette.primary, vec2(0))
	);
	mesh.addQuad(
		Vertex(vec3(mbl, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(mbr, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(obr, depth + 0.5), style.palette.primary, vec2(0)),
		Vertex(vec3(obl, depth + 0.5), style.palette.primary, vec2(0))
	);

	//corners
	if(style.rounding > 0.0f) {
		for(unsigned i = 0; i < style.segments; i++) {
			float p1 = (float(i) / style.segments);
			float p2 = (float(i + 1) / style.segments);
			float a1 = p1 * (3.141592654 / 2);
			float a2 = p2 * (3.141592654 / 2);
			float outerRadiusl1 = innerRadius + mix(style.border.x, style.border.y, p1);
			float outerRadiusl2 = innerRadius + mix(style.border.x, style.border.y, p2);
			float outerRadiust1 = innerRadius + mix(style.border.z, style.border.y, p1);
			float outerRadiust2 = innerRadius + mix(style.border.z, style.border.y, p2);
			float outerRadiusr1 = innerRadius + mix(style.border.z, style.border.w, p1);
			float outerRadiusr2 = innerRadius + mix(style.border.z, style.border.w, p2);
			float outerRadiusb1 = innerRadius + mix(style.border.x, style.border.w, p1);
			float outerRadiusb2 = innerRadius + mix(style.border.x, style.border.w, p2);

			vec2 v = vec2(cos(a1), sin(a1));
			vec2 w = vec2(cos(a2), sin(a2));
			vec2 v0 = v * innerRadius;
			vec2 v1 = w * innerRadius;
			vec2 v2l = w * outerRadiusl2;
			vec2 v3l = v * outerRadiusl1;
			vec2 v2t = w * outerRadiust2;
			vec2 v3t = v * outerRadiust1;
			vec2 v2r = w * outerRadiusr2;
			vec2 v3r = v * outerRadiusr1;
			vec2 v2b = w * outerRadiusb2;
			vec2 v3b = v * outerRadiusb1;

			vec2 ctl0 = itl + vec2(-v0.x,-v0.y);
			vec2 ctl1 = itl + vec2(-v1.x,-v1.y);
			vec2 ctl2 = itl + mix(vec2(-v2l.x,-v2l.y), -vec2(left, top), rounding);
			vec2 ctl3 = itl + mix(vec2(-v3l.x,-v3l.y), -vec2(left, top), rounding);
			vec2 ctr0 = itr + vec2( v0.x,-v0.y);
			vec2 ctr1 = itr + vec2( v1.x,-v1.y);
			vec2 ctr2 = itr + mix(vec2( v2t.x,-v2t.y), -vec2(right, top), rounding);
			vec2 ctr3 = itr + mix(vec2( v3t.x,-v3t.y), -vec2(right, top), rounding);
			vec2 cbr0 = ibr + vec2( v0.x, v0.y);
			vec2 cbr1 = ibr + vec2( v1.x, v1.y);
			vec2 cbr2 = ibr + mix(vec2( v2r.x, v2r.y), -vec2(right, bottom), rounding);
			vec2 cbr3 = ibr + mix(vec2( v3r.x, v3r.y), -vec2(right, bottom), rounding);
			vec2 cbl0 = ibl + vec2(-v0.x, v0.y);
			vec2 cbl1 = ibl + vec2(-v1.x, v1.y);
			vec2 cbl2 = ibl + mix(vec2(-v2b.x, v2b.y), -vec2(left, bottom), rounding);
			vec2 cbl3 = ibl + mix(vec2(-v3b.x, v3b.y), -vec2(left, bottom), rounding);

			unsigned vctl0 = mesh.addVertex(Vertex(vec3(ctl0, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vctl1 = mesh.addVertex(Vertex(vec3(ctl1, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vctl2 = mesh.addVertex(Vertex(vec3(ctl2, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vctl3 = mesh.addVertex(Vertex(vec3(ctl3, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vctr0 = mesh.addVertex(Vertex(vec3(ctr0, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vctr1 = mesh.addVertex(Vertex(vec3(ctr1, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vctr2 = mesh.addVertex(Vertex(vec3(ctr2, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vctr3 = mesh.addVertex(Vertex(vec3(ctr3, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbr0 = mesh.addVertex(Vertex(vec3(cbr0, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbr1 = mesh.addVertex(Vertex(vec3(cbr1, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbr2 = mesh.addVertex(Vertex(vec3(cbr2, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbr3 = mesh.addVertex(Vertex(vec3(cbr3, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbl0 = mesh.addVertex(Vertex(vec3(cbl0, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbl1 = mesh.addVertex(Vertex(vec3(cbl1, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbl2 = mesh.addVertex(Vertex(vec3(cbl2, depth + 0.5), style.palette.primary, vec2(0)));
			unsigned vcbl3 = mesh.addVertex(Vertex(vec3(cbl3, depth + 0.5), style.palette.primary, vec2(0)));

			mesh.addQuad(vctl0, vctl1, vctl2, vctl3);
			mesh.addQuad(vctr0, vctr1, vctr2, vctr3);
			mesh.addQuad(vcbr0, vcbr1, vcbr2, vcbr3);
			mesh.addQuad(vcbl0, vcbl1, vcbl2, vcbl3);
		}
	}
}


vec2 GuiSystem::text(const std::string &text, vec2 pos) {
	auto obj = textRenderer.createObject(text, mat4().translate(vec3(pos.x, -pos.y, 0)), vec4(1));
	std::cout<<"rendering \""<<text<<"\" at "<<pos<<"\n";
	vec2 size = 0;//textRenderer.calcSize(obj);
	return size;
}