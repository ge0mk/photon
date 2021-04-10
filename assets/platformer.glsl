#version 460

#if defined(VERTEX_SHADER)
	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec2 iUV;

	layout(std140, binding = 0) uniform CameraInfo {
		mat4 proj, view;
	} scene;

	layout(std140, binding = 1) uniform ObjectInfo {
		mat4 transform, uvtransform;
	} obj;

	layout(location = 0) out vec3 oPos;
	layout(location = 1) out vec2 oUV;

	void main() {
		vec4 pos = vec4(iPos, 1.0f);
		pos = scene.proj * scene.view * obj.transform * pos;
		oPos = (obj.transform * vec4(iPos, 1.0f)).xyz;
		gl_Position = pos;
		oUV = (obj.uvtransform * vec4(iUV, 0.0f, 1.0f)).xy;
	}

#elif defined(FRAGMENT_SHADER)

	layout(std140, binding = 2) uniform RenderInfo {
		vec4 tint;
		ivec2 res;
		float time, dt;
	};

	uniform sampler2D sampler;
	uniform sampler2D distanceMap;

	layout(location = 0) in vec3 iPos;
	layout(location = 1) in vec2 iUV;

	layout(location = 0) out vec4 fragColor;

	vec3 fetch(ivec2 tile) {
		ivec2 idx = tile + ivec2(128);
		idx.y = 320 - idx.y;
		return texelFetch(distanceMap, idx, 0).rgb;
	}

	vec3 fetch(vec2 tile) {
		vec2 idx = tile + vec2(128);
		idx.y = 320 - idx.y;
		return texture(distanceMap, idx / 320.0f).rgb;
	}

	float distance(vec2 tile) {
		vec3 d = fetch(tile);
		return d.r - d.g;
	}

	float distance2(vec2 tile) {
		return fetch(tile).g;
	}

	void main() {
		vec2 pixel = ivec2(iPos.xy);	// relative to center chunk bl
		vec2 tile = pixel / 8;
		ivec2 tileIndex = ivec2(tile) - ivec2(tile.x < 0 ? 1 : 0, tile.y < 0 ? 1 : 0);

		vec4 pixelColor = texture(sampler, iUV);
		pixelColor.rgb = mix(pixelColor.rgb, pixelColor.rgb * tint.rgb, tint.a);

		float surfaceDist = min(distance2(tile) + 1.0f / 255.0f, 1.0f);

		float depthShadow = 1.0f - min(surfaceDist * 32, 1.0f);
		float surfaceShadow = 1.0f;

		pixelColor.rgb *= depthShadow * surfaceShadow;
		fragColor = pixelColor;
	}

#endif
