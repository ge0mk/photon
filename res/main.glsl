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
	layout(location = 4) out vec3 oFragPos;

	void main() {
		vec4 pos = vec4(iPos, 1.0f);
		pos = scene.proj * scene.view * obj.transform * pos;
		oPos = pos.xyz;
		gl_Position = pos;
		oColor = iColor * iColor;
		oNormal = vec4(obj.transform * vec4(iNormal, 0.0f)).xyz;
		oUV = (obj.uvtransform * vec4(iUV, 0.0f, 0.0f)).xy;
		oFragPos = vec4(obj.transform * vec4(iPos, 1.0f)).xyz;
	}

#elif defined(FRAGMENT_SHADER)

	layout(std140, binding = 2) uniform Screeninfo {
		ivec2 res;
		float time, dt;
		float gamma, ambient;
	};

	layout(std140, binding = 3) uniform LightInfo {
		vec4 pos;
		vec4 dir;
		vec4 color;
	} lights[5];

	uniform sampler2D sampler;

	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec4 iColor;
	layout(location = 2) in vec3 iNormal;
	layout(location = 3) in vec2 iUV;
	layout(location = 4) in vec3 iFragPos;

	layout(location = 0) out vec4 fragColor;

	vec4 reinhard(vec4 hdrColor, float gamma) {
		// reinhard tone mapping
		vec3 mapped = hdrColor.rgb / (hdrColor.rgb + vec3(1.0));
		// gamma correction
		mapped = pow(mapped, vec3(1.0 / gamma));
		return vec4(mapped, hdrColor.a);
	}

	void main() {
		vec3 diffuse;
		vec4 objectColor;

		if(gamma >= 0.0f) {
			objectColor = reinhard(texture(sampler, iUV), gamma);
		}
		else {
			objectColor = texture(sampler, iUV);
		}

		objectColor = mix(objectColor, vec4(iColor.rgb, 1.0), iColor.a);

		vec3 norm = normalize(iNormal);
		for(uint i = 0; i < lights.length; i++) {
			vec3 lightDir = normalize(lights[i].pos.xyz - iFragPos);
			float diff = max(dot(norm, lightDir), 0.0);
			diffuse += diff * lights[i].color.rgb;
		}

		fragColor = vec4((ambient + diffuse) * objectColor.rgb, objectColor.a);
	}

#endif