#version 460

#if defined(VERTEX_SHADER)
	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec2 iUV;
	layout(location = 2) in vec4 iColor;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 proj, view;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform;
	} obj;

	layout(location = 0) out vec3 oPos;
	layout(location = 1) out vec2 oUV;
	layout(location = 2) out vec4 oColor;

	void main() {
		vec4 pos = vec4(iPos, 1.0f);
		pos = scene.proj * scene.view * obj.transform * pos;
		gl_Position = pos;
		oPos = pos.xyz;
		oUV = iUV;
		oColor = iColor;
	}

#elif defined(FRAGMENT_SHADER)

	uniform sampler2D sampler;

	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec2 iUV;
	layout(location = 2) in vec4 iColor;

	layout(location = 0) out vec4 fragColor;

	void main() {
		fragColor = vec4(iColor.rgb, texture(sampler, iUV).r * iColor.a);
	}

#endif