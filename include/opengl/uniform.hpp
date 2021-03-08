#pragma once

#include "buffer.hpp"

namespace opengl {
	template<typename T>
	class UniformBuffer : public Buffer<T> {
	public:
		UniformBuffer() : Buffer<T>(Buffer<T>::Uniform) {}
		UniformBuffer(const UniformBuffer<T> &other) : Buffer<T>(other) {}
		UniformBuffer(UniformBuffer<T> &&other) : Buffer<T>(std::forward(other)) {}

		UniformBuffer<T>& operator=(const UniformBuffer<T> &other) {
			Buffer<T>::operator=(other);
			return *this;
		}

		Buffer<T>& operator=(Buffer<T> &&other) {
			Buffer<T>::operator=(std::forward(other));
			return *this;
		}

		void initEmpty(size_t size = 1) {
			Buffer<T>::initEmpty(Buffer<T>::DynamicDraw, size);
		}

		void setData(const std::vector<T> &data) {
			Buffer<T>::setData(data, Buffer<T>::DynamicDraw);
		}

		void setData(const T &data) {
			Buffer<T>::setData(data, Buffer<T>::DynamicDraw);
		}
	};
}