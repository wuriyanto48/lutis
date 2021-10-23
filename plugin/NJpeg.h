#ifndef HEADER_NJPEG_H
#define HEADER_NJPEG_H

#include <napi.h>
#include <jpeglib.h>
#include <vector>
#include <iostream>
#include "Type.h"
#include "Base.h"

namespace lutis 
{
    namespace njpeg
    {

        class NJpeg : public lutis::base::NBase
        {
            public:
                NJpeg(uint32_t width, uint32_t height, uint32_t channel)
                : lutis::base::NBase(width, height, channel)
                {
                    
                }

                ~NJpeg()
                {
                    if (data != nullptr)
                        delete[] data;
                }

                void Read(lutis::type::Byte** data)
                {
                    if (data != nullptr)
                        this->data = *data;
                }

                int ToBuffer(J_COLOR_SPACE color_space, lutis::type::Byte** out)
                {
                    jpeg_compress_struct cinfo;

                    jpeg_error_mgr jpeg_err;
                    cinfo.err = jpeg_std_error(&jpeg_err);
                    jpeg_err.error_exit = [](j_common_ptr cinfo){throw cinfo->err;};

                    try
                    {
                        jpeg_create_compress(&cinfo);
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
                    catch(jpeg_error_mgr* e)
                    {
                        jpeg_destroy_compress(&cinfo);
                        return -1;
                    }
                    
                }
            public:
                static NJpeg* FromBuffer(const Napi::Buffer<lutis::type::Byte>& input)
                {
                    lutis::type::Byte* raw_data = reinterpret_cast<lutis::type::Byte*>(input.Data());
                    const size_t size_data = input.Length();

                    jpeg_decompress_struct cinfo;

                    jpeg_error_mgr jpeg_err;
                    cinfo.err = jpeg_std_error(&jpeg_err);
                    jpeg_err.error_exit = [](j_common_ptr cinfo){throw cinfo->err;};
                     
                    try
                    {
                        jpeg_create_decompress(&cinfo);

                        jpeg_mem_src(&cinfo, raw_data, size_data);

                        int read_header_res = jpeg_read_header(&cinfo, TRUE);
                        printf("read_header_res: %d\n", read_header_res);

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
                        
                        uint32_t mem_size = sizeof(lutis::type::Byte) * width * height * channel;

                        out->length = size_data;
                        out->data = new lutis::type::Byte[mem_size];

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
                    catch(jpeg_error_mgr* e)
                    {
                        jpeg_destroy_decompress(&cinfo);
                        return nullptr;
                    }
                    
                }
        };
    }
}

#endif