#version 460

#if defined(VERTEX_SHADER)
	layout(location = 0) in vec4 iPos;
	layout(location = 1) in vec4 iUV;
	layout(location = 2) in vec4 world;
	layout(location = 3) in vec2 scale;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 proj, view;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform, uvtransform;
	} obj;

	out VS_OUT {
		vec2 pos, scale;
		vec2 uvtl, uvbr;
		float rot;
	} vs_out;

	void main() {
		vec4 pos = vec4(iPos.xy, 0.0f, 1.0f);
		gl_Position = scene.proj * scene.view * obj.transform * pos;

		vs_out.pos = iPos.xy;
		vs_out.uvtl = iUV.xy;
		vs_out.uvbr = iUV.zw;
		vs_out.rot = world.z;
		vs_out.scale = scale;
	}

#elif defined(GEOMETRY_SHADER)
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;

	in VS_OUT {
		vec2 pos, scale;
		vec2 uvtl, uvbr;
		float rot;
	} gs_in[];

	layout(location = 0) out vec2 fUV;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 proj, view;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform, uvtransform;
	} obj;

	mat2 rotationZ(float angle) {
		return mat2(
			cos(angle),	-sin(angle),
			sin(angle),	 cos(angle)
		);
	}

	void vertex(vec2 pos, vec2 uv) {
		gl_Position = scene.proj * scene.view * obj.transform * vec4(pos, 0.0f, 1.0f);
		fUV = uv;
		EmitVertex();
	}

	const vec2 tl = vec2(-1,  1);
	const vec2 tr = vec2( 1,  1);
	const vec2 br = vec2( 1, -1);
	const vec2 bl = vec2(-1, -1);

	void main() {
		vec2 pos = gs_in[0].pos;
		vec2 scale = gs_in[0].scale;
		vec2 uvtl = gs_in[0].uvtl;
		vec2 uvbr = gs_in[0].uvbr;
		float rot = gs_in[0].rot;

		mat2 transform = rotationZ(rot);

		vertex(pos + transform * (tr * scale), vec2(uvbr.x, uvtl.y));
		vertex(pos + transform * (br * scale), uvbr);
		vertex(pos + transform * (tl * scale), uvtl);
		vertex(pos + transform * (bl * scale), vec2(uvtl.x, uvbr.y));

		EndPrimitive();
	}

#elif defined(FRAGMENT_SHADER)
	uniform sampler2D sampler;

	layout(location = 0) in vec2 fUV;

	layout(location = 0) out vec4 fragColor;

	void main() {
		fragColor = texture(sampler, fUV);
		fragColor = vec4(0.2,0.5,0.8,1);
	}

#endif