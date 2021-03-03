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
	layout(location = 4) out vec3 oLocalPos;

	void main() {
		vec4 pos = vec4(iPos, 1.0f);
		pos = scene.proj * scene.view * obj.transform * pos;
		oPos = pos.xyz;
		gl_Position = pos;
		oColor = iColor * iColor;
		oNormal = iNormal;
		oUV = (obj.uvtransform * vec4(iUV, 0.0f, 0.0f)).xy;
		oLocalPos = iPos;
	}

#elif defined(FRAGMENT_SHADER)

	layout(std140, binding = 2) uniform Screeninfo {
		ivec2 res;
		float time, dt;
		float gamma;
	};

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
	layout(location = 4) in vec3 iLocalPos;

	layout(location = 0) out vec4 fragColor;

	const vec2 invAtan = vec2(0.1591, 0.3183);


	vec4 reinhard(vec4 hdrColor, float gamma) {
		// reinhard tone mapping
		vec3 mapped = hdrColor.rgb / (hdrColor.rgb + vec3(1.0));
		// gamma correction
		mapped = pow(mapped, vec3(1.0 / gamma));
		return vec4(mapped, hdrColor.a);
	}

	vec2 SampleSphericalMap(vec3 v) {
		vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
		uv *= invAtan;
		uv += 0.5;
		//return uv;

		vec3 pos = normalize(v);
		return vec2(
			0.5 + atan(-pos.z, pos.x) / (2 * 3.141592654f),
			0.5 - asin(pos.y) / (3.141592654f)
		);
	}

	void main() {
		vec2 uv = SampleSphericalMap(normalize(iLocalPos));
		//fragColor = reinhard(texture(sampler, uv), gamma);
		if(gamma >= 0.0f) {
			fragColor = reinhard(texture(sampler, uv), gamma);
		}
		else {
			fragColor = texture(sampler, uv);
		}
	}

#endif