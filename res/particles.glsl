#version 460

#if defined(VERTEX_SHADER)
	layout(location = 0) in vec4 iPos;
	layout(location = 1) in vec4 iUV;
	layout(location = 2) in vec4 world;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 proj, view;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform, uvtransform;
	} obj;

	out VS_OUT {
		vec2 pos;
		vec2 uvtl, uvbr;
		float rot, scale;
	} vs_out;

	void main() {
		vec4 pos = vec4(iPos.xy, 0.0f, 1.0f);
		pos = scene.proj * pos;
		gl_Position = pos;

		vs_out.pos = iPos.xy;
		vs_out.uvtl = (obj.uvtransform * vec4(iUV.xy, 0.0f, 1.0f)).xy;
		vs_out.uvbr = (obj.uvtransform * vec4(iUV.zw, 0.0f, 1.0f)).xy;
		vs_out.rot = world.z;
		vs_out.scale = world.w;
	}

#elif defined(GEOMETRY_SHADER)
	layout (points) in;
	layout (triangle_strip, max_vertices = 4) out;

	in VS_OUT {
		vec2 pos;
		vec2 uvtl, uvbr;
		float rot, scale;
	} gs_in[];

	layout(location = 0) out vec2 fUV;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 proj, view;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform, uvtransform;
	} obj;

	void vertex(vec2 pos, vec2 uv) {
		gl_Position = scene.proj * vec4(pos, 0.0f, 1.0f);
		fUV = uv;
		EmitVertex();
	}

	const vec2 tl = vec2(-1,  1);
	const vec2 tr = vec2( 1,  1);
	const vec2 br = vec2( 1, -1);
	const vec2 bl = vec2(-1, -1);

	void main() {
		vec2 pos = gs_in[0].pos;
		vec2 uvtl = gs_in[0].uvtl;
		vec2 uvbr = gs_in[0].uvbr;
		float rot = gs_in[0].rot;
		float scale = gs_in[0].scale;

		// todo: implement rotation
		vertex(pos + tl * scale, uvtl);
		vertex(pos + tr * scale, vec2(uvbr.x, uvtl.y));
		vertex(pos + br * scale, uvbr);
		vertex(pos + bl * scale, vec2(uvtl.x, uvbr.y));

		EndPrimitive();
	}

#elif defined(FRAGMENT_SHADER)
	uniform sampler2D sampler;

	layout(location = 0) in vec2 fUV;

	layout(location = 0) out vec4 fragColor;

	void main() {
		fragColor = texture(sampler, fUV);
	}

#endif