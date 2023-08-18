#include "image.h"

TvgRenderImage::TvgRenderImage() {
    m_Width = 1;
    m_Height = 1;
}

TvgRenderImage::TvgRenderImage(uint8_t *data, uint32_t height, uint32_t width) {
    m_Width = width;
    m_Height = height;
    _image_data = data;

    _bytes.resize(size() / 4);
    for (int i = 0; i < size() / 4; i++) {
        _bytes[i] = RGBA(data[i], data[i + 1], data[i + 2], data[i + 3]).color;
    }
}

const uint32_t *TvgRenderImage::data() const {
    return _bytes.data();
}

uint32_t *TvgRenderImage::dataw() const {
    return const_cast<uint32_t *>(data());
}

uint32_t TvgRenderImage::size() const {
    return m_Width * m_Height;
}