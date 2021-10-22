#ifndef HEADER_NWEBP_H
#define HEADER_NWEBP_H

#include <napi.h>
#include <webp/encode.h>
#include <webp/decode.h>
#include <webp/types.h>
#include <vector>
#include <iostream>
#include "Type.h"

// Webp utility
namespace lutis 
{
    namespace nwebp
    {

        class NWebp
        {
            public:
                NWebp(uint32_t width, uint32_t height, uint32_t stride, float quality_factor)
                {
                    this->width = width;
                    this->height = height;
                    this->stride = stride;
                    this->quality_factor = quality_factor;

                    data = nullptr;
                }

                ~NWebp()
                {
                    if (data != nullptr)
                    {
                        delete[] data;
                    }
                }

                lutis::type::Byte* Data() 
                {
                    return data;
                }

                uint32_t Width() const
                {
                    return width;
                }

                uint32_t Height() const
                {
                    return height;
                }

                uint32_t Stride() const
                {
                    return stride;
                }

                size_t Length() const
                {
                    return length;
                }

                WEBP_CSP_MODE ColorSpace() const
                {
                    return colorspace;
                }

            public:
                static NWebp* FromBuffer(const Napi::Buffer<lutis::type::Byte>& input)
                {
                    lutis::type::Byte* raw_data = reinterpret_cast<lutis::type::Byte*>(input.Data());
                    const size_t size_data = input.Length();

                    int _width;
                    int _height;

                    int info_res = WebPGetInfo(raw_data, size_data, &_width, &_height);
                    if (!info_res)
                        return nullptr;
                    
                    printf("size_data %zu\n", size_data);
                    printf("webp width %u\n", _width);
                    printf("webp height %u\n", _height);

                    WebPDecoderConfig decoder_conf;
                    if (!WebPInitDecoderConfig(&decoder_conf))
                        return nullptr;
                    
                    if (WebPGetFeatures(raw_data, size_data, &decoder_conf.input) != VP8_STATUS_OK)
                        return nullptr;
                    
                    // Specify the desired output colorspace:
                    decoder_conf.output.colorspace = MODE_RGBA;

                    if (WebPDecode(raw_data, size_data, &decoder_conf) != VP8_STATUS_OK)
                        return nullptr;
                    
                    NWebp* nw = new NWebp(_width, _height, decoder_conf.output.u.RGBA.stride, 0);

                    printf("webp size %zu\n", decoder_conf.output.u.RGBA.size);
                    nw->data = new lutis::type::Byte[decoder_conf.output.u.RGBA.size];
                    memcpy(nw->data, decoder_conf.output.u.RGBA.rgba, decoder_conf.output.u.RGBA.size);

                    nw->length = decoder_conf.output.u.RGBA.size;
                    nw->colorspace = decoder_conf.output.colorspace;
                    WebPFreeDecBuffer(&decoder_conf.output);

                    return nw;
                }

            private:
                uint32_t width;
                uint32_t height;
                uint32_t stride;
                float quality_factor;
                WEBP_CSP_MODE colorspace;
                size_t length;
                lutis::type::Byte* data;
        };
    }
}

#endif
