#ifndef HEADER_NWEBP_H
#define HEADER_NWEBP_H

#include <napi.h>
#include <webp/encode.h>
#include <webp/decode.h>
#include <webp/types.h>
#include <vector>
#include <iostream>
#include "Type.h"
#include "Base.h"

// Webp utility
namespace lutis 
{
    namespace nwebp
    {

        class NWebp : public lutis::base::NBase
        {
            public:
                NWebp(uint32_t width, uint32_t height, uint32_t channel, uint32_t stride, float quality_factor)
                : lutis::base::NBase(width, height, channel)
                {
                    this->stride = stride;
                    this->quality_factor = quality_factor;
                }

                ~NWebp()
                {
                    if (data != nullptr)
                    {
                        delete[] data;
                    }
                }

                uint32_t Stride() const
                {
                    return stride;
                }

                size_t OriginalLength() const
                {
                    return original_length;
                }

                WEBP_CSP_MODE ColorSpace() const
                {
                    return colorspace;
                }

            public:
                // simple formula to take each 3 flat pixel data and put it to struct
                // https://play.golang.org/p/CntT9L67Dxy
                const std::vector<lutis::type::Color>& PixelData()
                {
                    uint32_t channel = 4;
                    if (data == nullptr)
                        return pixel_data;

                    for (int h = 0; h != height; h++) 
                    {
                        for (int w = 0; w != width; w++)
                        {
                            struct lutis::type::Color c;
                            c.R = data[(h*width*channel)+(w*channel)+0];
                            c.G = data[(h*width*channel)+(w*channel)+1];
                            c.B = data[(h*width*channel)+(w*channel)+2];
                            c.A = data[(h*width*channel)+(w*channel)+3];
                            pixel_data.push_back(c);
                        }

                    }
                    return pixel_data;
                }

                // http://en.wikipedia.org/wiki/Luma_%28video%29
                // basic grayscale filter
                std::vector<lutis::type::Byte> ToGrayPixel()
                {
                    auto p_data = PixelData();
                    std::vector<lutis::type::Byte> flat_pixel;
                    
                    for (int i = 0; i < p_data.size(); i++)
                    {
                        auto c = p_data[i];

                        float red = (float) c.R * 0.299;
                        float green = (float) c.G * 0.587;
                        float blue = (float)c.B * 0.114;

                        // gray pixel
                        lutis::type::Byte gray = (lutis::type::Byte) (red + green + blue);

                        flat_pixel.push_back(gray);
                        flat_pixel.push_back(gray);
                        flat_pixel.push_back(gray);
                    }

                    return flat_pixel;
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
                    
                    NWebp* nw = new NWebp(_width, _height, 4, decoder_conf.output.u.RGBA.stride, 0);

                    nw->data = new lutis::type::Byte[decoder_conf.output.u.RGBA.size];
                    memcpy(nw->data, decoder_conf.output.u.RGBA.rgba, decoder_conf.output.u.RGBA.size);

                    nw->length = decoder_conf.output.u.RGBA.size;
                    nw->original_length = size_data;
                    nw->colorspace = decoder_conf.output.colorspace;
                    
                    WebPFreeDecBuffer(&decoder_conf.output);

                    return nw;
                }

            private:
                uint32_t stride;
                float quality_factor;
                WEBP_CSP_MODE colorspace;
                size_t original_length;
        };
    }
}

#endif
