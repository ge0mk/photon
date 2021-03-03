#version 460

#if defined(VERTEX_SHADER)

	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec4 iColor;
	layout(location = 2) in vec3 iNormal;
	layout(location = 3) in vec2 iUV;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 view, proj;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform, uvtransform;
	} obj;

	layout(location = 0) out vec3 oPos;
	layout(location = 1) out vec4 oColor;
	layout(location = 2) out vec3 oNormal;
	layout(location = 3) out vec2 oUV;

	void main() {
		vec4 pos = vec4(iPos, 1.0f);
		pos = scene.proj * scene.view * pos;
		oPos = pos.xyz;
		gl_Position = pos;

		oColor = iColor * iColor;
		oNormal = iNormal;
		oUV = (obj.uvtransform * vec4(iUV, 0.0f, 0.0f)).xy;
	}

#elif defined(FRAGMENT_SHADER)

	layout(std140, binding = 2) uniform Frameinfo {
		ivec2 res;
		float time, dt;
	} frame;

	layout(std140, binding = 3) uniform LightInfo {
		vec4 pos;
		vec3 dir;
		vec4 color;
	} lights[];

	uniform sampler2D sampler;

	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec4 iColor;
	layout(location = 2) in vec3 iNormal;
	layout(location = 3) in vec2 iUV;

	layout(location = 0) out vec4 fragColor;

	void main() {
		vec4 color = sqrt(iColor);
		fragColor = texture(sampler, iUV);
		vec2 center = gl_FragCoord.xy - frame.res / 2;
		center = abs(center / 3);
		center = pow(center, vec2(0.0001));
		float crosshair = clamp(center.x + center.y, 0.0, 2.0) / 2.0;
		if(crosshair < 1.0) {
			fragColor = vec4(vec3(0), 1);
		}
	}

#endif