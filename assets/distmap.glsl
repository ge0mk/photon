#version 460

#if defined(COMPUTE_SHADER)
	layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

	layout(binding = 0) uniform usampler2D src;
	layout(rgba8ui, binding = 1) uniform uimage2D dst;

	uvec4 get(ivec2 pixel) {
		return texelFetch(src, pixel, 0);
	}

	void main() {
		ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
		uvec4 color = get(pixel);

		uint distToAir = 255, distToTile = 255;
		if(color.a == 255) {
			distToTile = 0;
		}
		else {
			distToAir = 0;
		}
		for(int ty = 0; ty < 320; ty++) {
			for(int tx = 0; tx < 320; tx++) {
				if(tx == pixel.x && ty == pixel.y) {
					continue;
				}
				uvec4 other = get(ivec2(tx, ty));
				if(other.a == color.a) {
					continue;
				}
				float d = clamp(distance(vec2(pixel), vec2(tx, ty)), 0.0f, 255.0f);
				if(other.a == 255) {
					distToTile = min(distToTile, uint(round(d)));
				}
				else {
					distToAir = min(distToAir, uint(round(d)));
				}
			}
		}
		color.r = distToTile;
		color.g = distToAir;

		imageStore(dst, pixel, color);
	}
#endif