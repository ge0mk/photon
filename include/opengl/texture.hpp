#pragma once

#include <glad/glad.h>
#include <utils/image.hpp>

namespace opengl {
	class Texture {
	public:
		enum Dimension {
			T2D = GL_TEXTURE_2D,
			T3D = GL_TEXTURE_3D,
			Cubemap = GL_TEXTURE_CUBE_MAP
		};

		Texture(GLenum type = T2D);
		Texture(GLenum format, math::ivec2 size, GLenum atomic = GL_UNSIGNED_BYTE, GLenum type = T2D);
		Texture(const Image &image, GLenum type = T2D);				// 2D and other
		Texture(const std::vector<Image> &images, GLenum type);		// 3D and Cubemap
		Texture(const HDRImage &image, GLenum type = T2D);			// 2D and other
		Texture(const std::vector<HDRImage> &images, GLenum type);	// 3D and Cubemap
		Texture(Texture &&other);
		~Texture();

		Texture& operator=(Texture &&other);

		void load(const Image &image);
		void load(const Image &image, GLenum internalFormat, GLenum format);
		void load(const std::vector<Image> &images);
		void load(const HDRImage &image);
		void load(const std::vector<HDRImage> &images);

		Image toImage();
		HDRImage toHDRImage();

		void bind();
		void unbind();
		void generateMipmap();
		void activate(GLenum unit = GL_TEXTURE0);

		void setParameter(GLenum pname, int value);
		void setParameter(GLenum pname, float value);

		void resize(math::ivec2 size);

		GLenum getType() const;
		GLenum getFormat() const;
		GLuint getHandle() const;
		math::ivec2 size() const;
		int getChannels() const;

		static GLint calcInternalFormat(GLenum format, GLenum atomic);

	private:
		GLuint handle;
		GLenum type = T2D, format = GL_RGBA, atomic = GL_UNSIGNED_BYTE;
		GLint internalFormat;
		math::ivec2 m_size;
		int channels;
	};
}