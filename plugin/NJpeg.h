#ifndef HEADER_NJPEG_H
#define HEADER_NJPEG_H

#include <napi.h>
#include <jpeglib.h>
#include <vector>
#include <iostream>
#include "Type.h"

namespace lutis 
{
    namespace njpeg
    {

        class NJpeg
        {
            public:
                NJpeg(uint32_t width, uint32_t height, uint32_t channel)
                {
                    this->width = width;
                    this->height = height;
                    this->channel = channel;

                    data = nullptr;
                    uint32_t mem_size = sizeof(lutis::type::Byte) * width * height * channel;
                    data = new lutis::type::Byte[mem_size];
                }

                ~NJpeg()
                {
                    if (data != nullptr)
                        delete[] data;
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

                uint32_t Channel() const
                {
                    return channel;
                }

                size_t Length() const
                {
                    return length;
                }

                void Read(lutis::type::Byte** data)
                {
                    if (data != nullptr)
                        this->data = *data;
                }

                int ToBuffer(J_COLOR_SPACE color_space, lutis::type::Byte** out)
                {
                    jpeg_compress_struct cinfo;
                    jpeg_create_compress(&cinfo);

                    jpeg_error_mgr jpeg_err;
                    cinfo.err = jpeg_std_error(&jpeg_err);

                    size_t out_length;

                    /* set to in memory buffer */
                    jpeg_mem_dest(&cinfo, out, &out_length);

                    cinfo.image_width      = width;
                    cinfo.image_height     = height;
                    cinfo.input_components = channel;
                    cinfo.in_color_space   = color_space;
                    jpeg_set_defaults(&cinfo);

                    jpeg_start_compress(&cinfo, TRUE);

                    uint8_t *row = data;
                    const uint32_t stride = width * channel;
                    for (int y = 0; y < height; y++) {
                        jpeg_write_scanlines(&cinfo, &row, 1);
                        row += stride;
                    }

                    jpeg_finish_compress(&cinfo);
                    jpeg_destroy_compress(&cinfo);

                    length = out_length;

                    return 0;
                }

                const std::vector<lutis::type::Color>& PixelData()
                {
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
                static NJpeg* FromBuffer(const Napi::Buffer<lutis::type::Byte>& input)
                {
                    lutis::type::Byte* raw_data = reinterpret_cast<lutis::type::Byte*>(input.Data());
                    const size_t size_data = input.Length();

                    jpeg_decompress_struct cinfo;
                    jpeg_create_decompress(&cinfo);

                    jpeg_error_mgr jpeg_err;
                    cinfo.err = jpeg_std_error(&jpeg_err);

                    jpeg_mem_src(&cinfo, raw_data, size_data);

                    jpeg_read_header(&cinfo, TRUE);

                    jpeg_start_decompress(&cinfo);

                    uint32_t width = cinfo.image_width;
                    uint32_t height = cinfo.image_height;
                    uint32_t channel = cinfo.output_components;

                    printf("width %u\n", width);
                    printf("height %u\n", height);
                    printf("channel %u\n", channel);

                    // uint32_t mem_size = sizeof(lutis::type::Byte) * width * height * channel;

                    // *out = new lutis::type::Byte[mem_size];

                    NJpeg* out = new NJpeg(width, height, channel);
                    out->length = size_data;

                    const uint32_t row_stride = width * channel;
                    
                    lutis::type::Byte* row = out->Data();
                    for (int i = 0; i < height; i++)
                    {
                        jpeg_read_scanlines(&cinfo, &row, 1);
                        row += row_stride;
                    }

                    jpeg_finish_decompress(&cinfo);
                    jpeg_destroy_decompress(&cinfo);

                    return out;
                }
            private:
                uint32_t width;
                uint32_t height;
                uint32_t channel;
                size_t length;
                lutis::type::Byte* data;
                std::vector<lutis::type::Color> pixel_data;
        };
    }
}

#endif