#pragma once

#include <math/vector.hpp>

template<typename channel_t = uint8_t>
class Pixel {
public:
	Pixel(channel_t *data, int channels) : m_data(data), m_channels(channels) {}
	Pixel(const Pixel &other) = default;

	Pixel& operator=(const Pixel &other) = default;
	Pixel& operator=(const math::vec4 &color);
	Pixel& operator=(const math::vec3 &color);
	Pixel& operator=(const math::vec2 &color);
	Pixel& operator=(float color);

	operator math::vec4();
	operator math::vec3();
	operator math::vec2();
	operator float();

	math::vec4 rgba();
	math::vec3 rgb();
	float gray();
	float alpha();

private:
	channel_t *m_data;
	int m_channels;
};

template<typename channel_t>
Pixel<channel_t>& Pixel<channel_t>::operator=(const math::vec4 &color) {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4) {
			m_data[0] = color.r;
			m_data[1] = color.g;
			m_data[2] = color.b;
			m_data[3] = color.a;
		} else if(m_channels == 3) {
			m_data[0] = color.r;
			m_data[1] = color.g;
			m_data[2] = color.b;
		} else if(m_channels == 2) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f;
			m_data[1] = color.a;
		} else if(m_channels == 1) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f;
		}
	} else {
		if(m_channels == 4) {
			m_data[0] = color.r * 255;
			m_data[1] = color.g * 255;
			m_data[2] = color.b * 255;
			m_data[3] = color.a * 255;
		} else if(m_channels == 3) {
			m_data[0] = color.r * 255;
			m_data[1] = color.g * 255;
			m_data[2] = color.b * 255;
		} else if(m_channels == 2) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f * 255;
			m_data[1] = color.a * 255;
		} else if(m_channels == 1) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f * 255;
		}
	}
	return *this;
}

template<typename channel_t>
Pixel<channel_t>& Pixel<channel_t>::operator=(const math::vec3 &color) {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4) {
			m_data[0] = color.r;
			m_data[1] = color.g;
			m_data[2] = color.b;
			m_data[3] = 1.0f;
		} else if(m_channels == 3) {
			m_data[0] = color.r;
			m_data[1] = color.g;
			m_data[2] = color.b;
		} else if(m_channels == 2) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f;
			m_data[1] = 1.0f;
		} else if(m_channels == 1) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f;
		}
	} else {
		if(m_channels == 4) {
			m_data[0] = color.r * 255;
			m_data[1] = color.g * 255;
			m_data[2] = color.b * 255;
			m_data[3] = 255;
		} else if(m_channels == 3) {
			m_data[0] = color.r * 255;
			m_data[1] = color.g * 255;
			m_data[2] = color.b * 255;
		} else if(m_channels == 2) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f * 255;
			m_data[1] = 255;
		} else if(m_channels == 1) {
			m_data[0] = (color.r + color.g + color.b) / 3.0f * 255;
		}
	}
	return *this;
}

template<typename channel_t>
Pixel<channel_t>& Pixel<channel_t>::operator=(const math::vec2 &color) {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4) {
			m_data[0] = color.x;
			m_data[1] = color.x;
			m_data[2] = color.x;
			m_data[3] = color.y;
		} else if(m_channels == 3) {
			m_data[0] = color.x;
			m_data[1] = color.x;
			m_data[2] = color.x;
		} else if(m_channels == 2) {
			m_data[0] = color.x;
			m_data[1] = color.y;
		} else if(m_channels == 1) {
			m_data[0] = color.x;
		}
	} else {
		if(m_channels == 4) {
			m_data[0] = color.x * 255;
			m_data[1] = color.x * 255;
			m_data[2] = color.x * 255;
			m_data[3] = color.y * 255;
		} else if(m_channels == 3) {
			m_data[0] = color.x * 255;
			m_data[1] = color.x * 255;
			m_data[2] = color.x * 255;
		} else if(m_channels == 2) {
			m_data[0] = color.x * 255;
			m_data[1] = color.y * 255;
		} else if(m_channels == 1) {
			m_data[0] = color.x * 255;
		}
	}
	return *this;
}

template<typename channel_t>
Pixel<channel_t>& Pixel<channel_t>::operator=(float color) {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4) {
			m_data[0] = color;
			m_data[1] = color;
			m_data[2] = color;
			m_data[3] = 1.0f;
		} else if(m_channels == 3) {
			m_data[0] = color;
			m_data[1] = color;
			m_data[2] = color;
		} else if(m_channels == 2) {
			m_data[0] = color;
			m_data[1] = 1.0f;
		} else if(m_channels == 1) {
			m_data[0] = color;
		}
	} else {
		if(m_channels == 4) {
			m_data[0] = color * 255;
			m_data[1] = color * 255;
			m_data[2] = color * 255;
			m_data[3] = 255;
		} else if(m_channels == 3) {
			m_data[0] = color * 255;
			m_data[1] = color * 255;
			m_data[2] = color * 255;
		} else if(m_channels == 2) {
			m_data[0] = color * 255;
			m_data[1] = 255;
		} else if(m_channels == 1) {
			m_data[0] = color * 255;
		}
	}
	return *this;
}

template<typename channel_t>
Pixel<channel_t>::operator math::vec4() {
	return rgba();
}

template<typename channel_t>
Pixel<channel_t>::operator math::vec3() {
	return rgb();
}

template<typename channel_t>
Pixel<channel_t>::operator math::vec2() {
	return math::vec2(gray(), alpha());
}

template<typename channel_t>
Pixel<channel_t>::operator float() {
	return gray();
}

template<typename channel_t>
math::vec4 Pixel<channel_t>::rgba() {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4) {
			return math::vec4(
				m_data[0],
				m_data[1],
				m_data[2],
				m_data[3]
			);
		} else if(m_channels == 3) {
			return math::vec4(
				m_data[0],
				m_data[1],
				m_data[2],
				1.0f
			);
		} else if(m_channels == 2) {
			return math::vec4(
				m_data[0],
				m_data[0],
				m_data[0],
				m_data[1]
			);
		} else {
			return math::vec4(
				m_data[0],
				m_data[0],
				m_data[0],
				1.0f
			);
		}
	} else {
		if(m_channels == 4) {
			return math::vec4(
				float(m_data[0]) / 255.0f,
				float(m_data[1]) / 255.0f,
				float(m_data[2]) / 255.0f,
				float(m_data[3]) / 255.0f
			);
		} else if(m_channels == 3) {
			return math::vec4(
				float(m_data[0]) / 255.0f,
				float(m_data[1]) / 255.0f,
				float(m_data[2]) / 255.0f,
				1.0f
			);
		} else if(m_channels == 2) {
			return math::vec4(
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f,
				float(m_data[1]) / 255.0f
			);
		} else {
			return math::vec4(
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f,
				1.0f
			);
		}
	}
}

template<typename channel_t>
math::vec3 Pixel<channel_t>::rgb() {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4 || m_channels == 3) {
			return math::vec3(
				m_data[0],
				m_data[1],
				m_data[2]
			);
		} else if(m_channels == 2) {
			return math::vec3(
				m_data[0],
				m_data[0],
				m_data[0]
			);
		} else {
			return math::vec3(
				m_data[0],
				m_data[0],
				m_data[0]
			);
		}
	} else {
		if(m_channels == 4 || m_channels == 3) {
			return math::vec3(
				float(m_data[0]) / 255.0f,
				float(m_data[1]) / 255.0f,
				float(m_data[2]) / 255.0f
			);
		} else if(m_channels == 2) {
			return math::vec3(
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f
			);
		} else {
			return math::vec3(
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f,
				float(m_data[0]) / 255.0f
			);
		}
	}
}

template<typename channel_t>
float Pixel<channel_t>::gray() {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4 || m_channels == 3) {
			return (m_data[0] + m_data[1] + m_data[2]) / 3.0f;
		} else if(m_channels == 2) {
			return m_data[0];
		} else {
			return m_data[0];
		}
	} else {
		if(m_channels == 4 || m_channels == 3) {
			return (float(m_data[0]) / 255.0f + float(m_data[1]) / 255.0f + float(m_data[2]) / 255.0f) / 3.0f;
		} else if(m_channels == 2) {
			return float(m_data[0]) / 255.0f;
		} else {
			return float(m_data[0]) / 255.0f;
		}
	}
}

template<typename channel_t>
float Pixel<channel_t>::alpha() {
	if constexpr(std::is_floating_point<channel_t>()) {
		if(m_channels == 4) {
			return m_data[3];
		} else if(m_channels == 2) {
			return m_data[1];
		}
	} else {
		if(m_channels == 4) {
			return float(m_data[3]) / 255.0f;
		} else if(m_channels == 2) {
			return float(m_data[1]) / 255.0f;
		}
	}
	return 1.0f;
}