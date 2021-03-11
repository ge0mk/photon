#include <opengl/texture.hpp>

namespace opengl {
	Texture::Texture(GLenum type) : type(type) {
		glGenTextures(1, &handle);
	}

	Texture::Texture(GLenum format, math::ivec2 size, GLenum atomic, GLenum type) : type(type), format(format), atomic(atomic), m_size(size) {
		switch(format) {
			case GL_LUMINANCE: channels = 1; break;
			case GL_LUMINANCE_ALPHA: channels = 2; break;
			case GL_RGB: channels = 3; break;
			case GL_RGBA: channels = 4; break;
		}
		internalFormat = calcInternalFormat(format, atomic);

		glGenTextures(1, &handle);
		bind();
		setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(type, 0, internalFormat, size.x, size.y, 0, format, atomic, nullptr);
		unbind();
	}

	Texture::Texture(const Image &image, GLenum type) : type(type) {				// 2D and other
		glGenTextures(1, &handle);
		load(image);
	}

	Texture::Texture(const std::vector<Image> &images, GLenum type) : type(type) {	// 3D and Cubemap
		glGenTextures(1, &handle);
		load(images);
	}

	Texture::Texture(const HDRImage &image, GLenum type) : type(type) {				// 2D and other
		glGenTextures(1, &handle);
		load(image);
	}

	Texture::Texture(const std::vector<HDRImage> &images, GLenum type) : type(type) {	// 3D and Cubemap
		glGenTextures(1, &handle);
		load(images);
	}

	Texture::Texture(Texture &&other) : handle(other.handle), type(other.type),
			format(other.format), m_size(other.m_size), channels(other.channels),
			atomic(other.atomic), internalFormat(other.internalFormat) {
		other.handle = 0;
	}

	Texture::~Texture() {
		if(handle) {
			glDeleteTextures(1, &handle);
		}
	}

	Texture& Texture::operator=(Texture &&other) {
		if(handle) {
			glDeleteTextures(1, &handle);
		}
		type = other.type;
		handle = other.handle;
		channels = other.channels;
		atomic = other.atomic;
		internalFormat = other.internalFormat;
		m_size = other.m_size;
		other.handle = 0;
		return *this;
	}

	void Texture::load(const Image &image) {
		channels = image.channels();
		m_size = image.size();

		switch(image.bitdepth()) {
			case  8: atomic = GL_UNSIGNED_BYTE; break;
			case 16: atomic = GL_UNSIGNED_SHORT; break;
			case 32: atomic = GL_UNSIGNED_INT; break;
		}
		switch(image.channels()) {
			case 1: format = GL_RED; break;
			case 2: format = GL_RG; break;
			case 3: format = GL_RGB; break;
			case 4: format = GL_RGBA; break;
		}
		internalFormat = calcInternalFormat(format, atomic);

		bind();
		setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
		setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
		setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(type, 0, internalFormat, image.size().x, image.size().y, 0, format, atomic, image.data());
		generateMipmap();
		unbind();
	}

	void Texture::load(const HDRImage &image) {
		channels = image.channels();
		atomic = GL_FLOAT;
		m_size = image.size();
		switch(image.channels()) {
			case 1: format = GL_LUMINANCE; break;
			case 2: format = GL_LUMINANCE_ALPHA; break;
			case 3: format = GL_RGB; break;
			case 4: format = GL_RGBA; break;
		}
		internalFormat = calcInternalFormat(format, atomic);

		bind();
		setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
		setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
		setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(type, 0, internalFormat, image.size().x, image.size().y, 0, format, atomic, image.data());
		generateMipmap();
		unbind();
	}

	void Texture::load(const std::vector<Image> &images) {
		atomic = GL_UNSIGNED_BYTE;
		if(type == Cubemap && images.size() == 6) {
			static const GLenum sides[6] = {
				GL_TEXTURE_CUBE_MAP_POSITIVE_X,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
			};

			channels = images[0].channels();
			m_size = images[0].size();
			switch(images[0].channels()) {
				case 1: format = GL_LUMINANCE; break;
				case 2: format = GL_LUMINANCE_ALPHA; break;
				case 3: format = GL_RGB; break;
				case 4: format = GL_RGBA; break;
			}
			internalFormat = calcInternalFormat(format, atomic);

			bind();
			for(unsigned i = 0; i < images.size(); i++) {
				const Image &image = images[i];
				if(m_size == image.size() && channels == image.channels()) {
					glTexImage2D(sides[i], 0, internalFormat, m_size.x, m_size.y, 0, format, atomic, image.data());
				}
			}
			setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			unbind();
		}
	}

	void Texture::load(const std::vector<HDRImage> &images) {
		atomic = GL_FLOAT;
		if(type == Cubemap && images.size() == 6) {
			static const GLenum sides[6] = {
				GL_TEXTURE_CUBE_MAP_POSITIVE_X,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
			};

			channels = images[0].channels();
			m_size = images[0].size();
			switch(images[0].channels()) {
				case 1: format = GL_LUMINANCE; break;
				case 2: format = GL_LUMINANCE_ALPHA; break;
				case 3: format = GL_RGB; break;
				case 4: format = GL_RGBA; break;
			}
			internalFormat = calcInternalFormat(format, atomic);

			bind();
			for(unsigned i = 0; i < images.size(); i++) {
				const HDRImage &image = images[i];
				if(m_size == image.size() && channels == image.channels()) {
					glTexImage2D(sides[i], 0, internalFormat, m_size.x, m_size.y, 0, format, atomic, image.data());
				}
			}
			setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			unbind();
		}
	}

	Image Texture::toImage() {
		if(type == T2D) {
			Image target(m_size, channels);
			bind();
			glGetTexImage(type, 0, format, GL_UNSIGNED_BYTE, target.data());
			unbind();
			return target;
		}
		return Image(math::ivec2(0, 0), 1);
	}

	HDRImage Texture::toHDRImage() {
		if(type == T2D) {
			HDRImage target(m_size, channels);
			bind();
			glGetTexImage(type, 0, format, GL_FLOAT, target.data());
			unbind();
			return target;
		}
		return HDRImage(math::ivec2(0, 0), 1);
	}

	void Texture::bind() {
		glBindTexture(type, handle);
	}

	void Texture::unbind() {
		glBindTexture(type, 0);
	}

	void Texture::generateMipmap() {
		glGenerateMipmap(type);
	}

	void Texture::activate(GLenum unit) {
		glActiveTexture(unit);
	}

	void Texture::setParameter(GLenum pname, int value) {
		glTexParameteri(type, pname, value);
	}

	void Texture::setParameter(GLenum pname, float value) {
		glTexParameterf(type, pname, value);
	}

	void Texture::resize(math::ivec2 size) {
		this->m_size = size;
		bind();
		glTexImage2D(type, 0, internalFormat, size.x, size.y, 0, format, atomic, nullptr);
		unbind();
	}

	GLenum Texture::getType() const {
		return type;
	}

	GLenum Texture::getFormat() const {
		return format;
	}

	GLuint Texture::getHandle() const {
		return handle;
	}

	math::ivec2 Texture::size() const {
		return m_size;
	}

	int Texture::getChannels() const {
		return channels;
	}

	GLint Texture::calcInternalFormat(GLenum format, GLenum atomic) {
		if(format == GL_RED && atomic == GL_FLOAT)
			return GL_R32F;
		if(format == GL_RED && atomic == GL_HALF_FLOAT)
			return GL_R16F;
		if(format == GL_RED && atomic == GL_UNSIGNED_BYTE)
			return GL_R8;
		if(format == GL_RED && atomic == GL_UNSIGNED_SHORT)
			return GL_R16;
		if(format == GL_RG && atomic == GL_FLOAT)
			return GL_RG32F;
		if(format == GL_RG && atomic == GL_HALF_FLOAT)
			return GL_RG16F;
		if(format == GL_RG && atomic == GL_UNSIGNED_BYTE)
			return GL_RG8;
		if(format == GL_RG && atomic == GL_UNSIGNED_SHORT)
			return GL_RG16;
		if(format == GL_RGB && atomic == GL_FLOAT)
			return GL_RGB32F;
		if(format == GL_RGB && atomic == GL_HALF_FLOAT)
			return GL_RGB16F;
		if(format == GL_RGB && atomic == GL_UNSIGNED_BYTE)
			return GL_RGB8;
		if(format == GL_RGB && atomic == GL_UNSIGNED_SHORT)
			return GL_RGB16;
		if(format == GL_RGBA && atomic == GL_FLOAT)
			return GL_RGBA32F;
		if(format == GL_RGBA && atomic == GL_HALF_FLOAT)
			return GL_RGBA16F;
		if(format == GL_RGBA && atomic == GL_UNSIGNED_BYTE)
			return GL_RGBA8;
		if(format == GL_RGBA && atomic == GL_UNSIGNED_SHORT)
			return GL_RGBA16;
		return format;
	}
}
