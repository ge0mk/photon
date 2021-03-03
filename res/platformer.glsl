#version 460

#if defined(VERTEX_SHADER)
	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec2 iUV;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 view, proj;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform, uvtransform;
	} obj;

	layout(location = 0) out vec3 oPos;
	layout(location = 1) out vec2 oUV;

	void main() {
		vec4 pos = vec4(iPos, 1.0f);
		pos = scene.proj * scene.view * obj.transform * pos;
		oPos = pos.xyz;
		gl_Position = pos;
		oUV = iUV;//(obj.uvtransform * vec4(iUV, 0.0f, 0.0f)).xy;
	}

#elif defined(FRAGMENT_SHADER)

	layout(std140, binding = 2) uniform Screeninfo {
		ivec2 res;
		float time, dt;
	};

	uniform sampler2D sampler;

	layout(location = 0) in vec4 iPos;
	layout(location = 1) in vec2 iUV;

	layout(location = 0) out vec4 fragColor;

	void main() {
		fragColor = texture(sampler, iUV);
	}

#endif