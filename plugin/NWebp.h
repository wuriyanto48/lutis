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

                int ToBuffer( lutis::type::Byte** out)
                {
                    WebPConfig webpconfig;
                    WebPPicture webpicture;
                    WebPMemoryWriter writer;

                    WebPMemoryWriterInit(&writer);
                    if (!WebPPictureInit(&webpicture) || !WebPConfigInit(&webpconfig)) return -1;

                    // Add additional tuning:
                    webpconfig.sns_strength = 90;
                    webpconfig.filter_sharpness = 6;
                    webpconfig.alpha_quality = 90;

                    if (!WebPConfigPreset(&webpconfig, WEBP_PRESET_DEFAULT, quality_factor))
                    {
                        FreeWebp(&webpicture, &writer);
                        return -1;
                    }

                    if (!WebPValidateConfig(&webpconfig))
                    {
                        FreeWebp(&webpicture, &writer);
                        return -1;
                    }

                    webpicture.width = width;
                    webpicture.height = height;

                    webpicture.use_argb = 1;

                    if (!WebPPictureAlloc(&webpicture))
                    {
                        FreeWebp(&webpicture, &writer);
                        return -1;
                    }

                    int import_ok = channel == 4 ? WebPPictureImportRGBA(&webpicture, data, stride) 
                        : WebPPictureImportRGB(&webpicture, data, stride);
                    if (!import_ok)
                    {
                        FreeWebp(&webpicture, &writer);
                        return -1;
                    }

                    webpicture.writer = WebPMemoryWrite;
                    webpicture.custom_ptr = &writer;

                    // encode
                    int encode_ok = WebPEncode(&webpconfig, &webpicture);
                    if (!encode_ok) 
                    {
                        FreeWebp(&webpicture, &writer);
                        return -1;
                    }

                    printf("encode writer.size: %lu\n", writer.size);
                    *out = writer.mem;

                    FreeWebp(&webpicture, &writer);

                    return 0;
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
                    
                    NWebp* nw = new NWebp(_width, _height, 4, decoder_conf.output.u.RGBA.stride, 60);

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
            
            private:
                void FreeWebp(WebPPicture* pic, WebPMemoryWriter* writer)
                {
                    WebPMemoryWriterClear(writer);
                    WebPPictureFree(pic);
                }
        };
    }
}

#endif
