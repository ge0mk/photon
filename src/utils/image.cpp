#include <utils/image.hpp>

Image::Image(const std::string &filename) {
	load(filename);
}

Image::Image(math::ivec2 size, int channels) : m_size(size), m_channels(channels) {
	m_data.resize(size.x * size.y * channels);
}

Image::Image(const std::vector<uint8_t> &data, math::ivec2 size, int channels) : m_data(data), m_size(size), m_channels(channels) {}

void Image::load(const std::string &filename) {
	uint8_t *data;
	if(stbi_is_16_bit(filename.c_str())) {
		m_bitdepth = 16;
		data = (uint8_t*)stbi_load_16(filename.c_str(), &m_size.x, &m_size.y, &m_channels, 0);
	}
	else {
		m_bitdepth = 8;
		data = (uint8_t*)stbi_load(filename.c_str(), &m_size.x, &m_size.y, &m_channels, 0);
	}

	size_t memSize = m_size.x * m_size.y * m_channels * (m_bitdepth / 8);
	m_data = std::vector<uint8_t>(data, data + memSize);
	stbi_image_free(data);
}

void Image::save(const std::string &filename) const {
	stbi_write_png(filename.c_str(), m_size.x, m_size.y, m_channels, m_data.data(), m_size.x * m_channels * m_bitdepth / 8);
}

uint8_t* Image::data() {
	return m_data.data();
}

const uint8_t* Image::data() const {
	return m_data.data();
}

math::ivec2 Image::size() const {
	return m_size;
}

int Image::channels() const {
	return m_channels;
}

int Image::bitdepth() const {
	return m_bitdepth;
}

Pixel<uint8_t> Image::operator[](const math::ivec2 &pos) {
	return Pixel<uint8_t>(&m_data[pos.y*m_size.x + pos.x], m_channels);
}

GLFWimage Image::getGLFWImage() const {
	GLFWimage img;
	img.width = m_size.x;
	img.height = m_size.y;
	img.pixels = (unsigned char*)m_data.data();
	return img;
}

HDRImage::HDRImage(const std::string &filename) {
	load(filename);
}

HDRImage::HDRImage(math::ivec2 size, int channels) : m_size(size), m_channels(channels) {
	m_data.resize(size.x * size.y * channels);
}

HDRImage::HDRImage(const std::vector<float> &data, math::ivec2 size) : m_data(data), m_size(size) {}

void HDRImage::load(const std::string &filename) {
	float *data = stbi_loadf(filename.c_str(), &m_size.x, &m_size.y, &m_channels, 0);
	size_t memSize = m_size.x * m_size.y * m_channels;
	m_data = std::vector<float>(data, data + memSize);
	stbi_image_free(data);
}

void HDRImage::save(const std::string &filename) const {
	stbi_write_hdr(filename.c_str(), m_size.x, m_size.y, m_channels, m_data.data());
}

float* HDRImage::data() {
	return m_data.data();
}

const float* HDRImage::data() const {
	return m_data.data();
}

math::ivec2 HDRImage::size() const {
	return m_size;
}

int HDRImage::channels() const {
	return m_channels;
}

int HDRImage::bitdepth() const {
	return 32;
}

Pixel<float> HDRImage::operator[](const math::ivec2 &pos) {
	return Pixel<float>(&m_data[pos.y*m_size.x + pos.x], m_channels);
}
