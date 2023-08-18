#ifndef _RIVEEXTENSION_THORVG_IMAGE_H_
#define _RIVEEXTENSION_THORVG_IMAGE_H_

#include <rive/renderer.hpp>
#include <vector>

#include "tvg_renderer/common.hpp"

class TvgRenderImage : public rive::RenderImage {
   protected:
    uint8_t *_image_data;
    std::vector<uint32_t> _bytes;

   public:
    TvgRenderImage();
    TvgRenderImage(uint8_t *data, uint32_t height, uint32_t width);

    const uint32_t *data() const;
    uint32_t *dataw() const;
    uint32_t size() const;
};

#endif