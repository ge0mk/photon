#pragma once

#include <string>
#include <vector>

#include <glfw/glfw3.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "pixel.hpp"

class Image {
public:
	Image() = default;
	Image(const Image &other) = default;
	Image(const std::string &filename);
	Image(math::ivec2 size, int channels);
	Image(const std::vector<uint8_t> &data, math::ivec2 size, int channels);

	void load(const std::string &filename);
	void save(const std::string &filename);

	Image& operator=(const Image &other) = default;

	uint8_t* data();
	const uint8_t* data() const;
	math::ivec2 size() const;
	int channels() const;
	int bitdepth() const;

	Pixel<uint8_t> operator[](const math::ivec2 &pos);

	GLFWimage getGLFWImage() const;

private:
	std::vector<uint8_t> m_data;
	math::ivec2 m_size;
	int m_channels, m_bitdepth = 8;
};

class HDRImage {
public:
	HDRImage() = default;
	HDRImage(const HDRImage &other) = default;
	HDRImage(const std::string &filename);
	HDRImage(math::ivec2 size, int channels);
	HDRImage(const std::vector<float> &data, math::ivec2 size);

	void load(const std::string &filename);
	void save(const std::string &filename);

	HDRImage& operator=(const HDRImage &other) = default;

	float* data();
	const float* data() const;
	math::ivec2 size() const;
	int channels() const;
	int bitdepth() const;

	Pixel<float> operator[](const math::ivec2 &pos);

private:
	std::vector<float> m_data;
	math::ivec2 m_size;
	int m_channels;
};