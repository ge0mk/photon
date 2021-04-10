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
		if (color.a == 255) {
			distToTile = 0;
		}
		else {
			distToAir = 0;
		}

		bool done = false;
		for (int radius = 1; radius < 255 && !done; radius++) {
			for (int dy = -radius; dy <= radius; dy++) {
				if (pixel.y + dy < 320 || pixel.y + dy >= 0) {
					if (abs(dy) < radius) { // sides
						if (pixel.x - radius >= 0) { // left
							uvec4 other = get(pixel + ivec2(-radius, dy));
							if(other.a != color.a) {
								float d = clamp(length(vec2(-radius, dy)), 0.0f, 255.0f);
								if(other.a == 255) {
									distToTile = min(distToTile, uint(round(d)));
								}
								else {
									distToAir = min(distToAir, uint(round(d)));
								}
								done = true;
							}
						}
						if (pixel.x + radius < 320) { // right
							uvec4 other = get(pixel + ivec2(radius, dy));
							if(other.a != color.a) {
								float d = clamp(length(vec2(radius, dy)), 0.0f, 255.0f);
								if(other.a == 255) {
									distToTile = min(distToTile, uint(round(d)));
								}
								else {
									distToAir = min(distToAir, uint(round(d)));
								}
								done = true;
							}
						}
					}
					else { // top / bottom
						for(int dx = -radius; dx <= radius; dx++) {
							if(pixel.x + dx < 320 || pixel.x + dx >= 0) {
								uvec4 other = get(pixel + ivec2(dx, dy));
								if(other.a != color.a) {
									float d = clamp(length(vec2(dx, dy)), 0.0f, 255.0f);
									if(other.a == 255) {
										distToTile = min(distToTile, uint(round(d)));
									}
									else {
										distToAir = min(distToAir, uint(round(d)));
									}
									done = true;
								}
							}
						}
					}
				}
			}
		}

		color.r = distToTile;
		color.g = distToAir;

		imageStore(dst, pixel, color);
	}
#endif