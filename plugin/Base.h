#ifndef HEADER_BASE_H
#define HEADER_BASE_H

#include <napi.h>
#include <jpeglib.h>
#include <vector>
#include <iostream>
#include "Type.h"

namespace lutis 
{ 
namespace base 
{
    
class NBase
{
protected:
    uint32_t width;
    uint32_t height;
    uint32_t channel;
    size_t length;
    lutis::type::Byte* data;
    std::vector<lutis::type::Color> pixel_data;

public:
    NBase(uint32_t width, uint32_t height, uint32_t channel)
    {
        this->width = width;
        this->height = height;
        this->channel = channel;

        data = nullptr;
    }

    virtual ~NBase() = default;

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

    // simple formula to take each 3 flat pixel data and put it to struct
    // https://play.golang.org/p/CntT9L67Dxy
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

                if (channel == 4)
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
            if (channel == 4)
                flat_pixel.push_back((lutis::type::Byte) c.A);
        }

        return flat_pixel;
    }

    // c_real represent real number part number of the complex number
    // c_image represent imaginary part number of the complex number
    int GenerateJuliaSet(float c_real, float c_imag)
    {
        data = new lutis::type::Byte[width * height * channel];
        const int MAX_ITER = 1000;
        
        float scale_x = 3.0 / width;
        float scale_y = 3.0 / height;

        float escape_radius = 3.5;
        if (data == nullptr)
            return -1;
        // background
        for (int x = 0; x != width; x++) 
        {
            for (int y = 0; y != height; y++)
            {                
                data[(y*width*channel)+(x*channel)+0] = (uint8_t) 3;
                data[(y*width*channel)+(x*channel)+1] = (uint8_t) 248;
                data[(y*width*channel)+(x*channel)+2] = (uint8_t) 252;
                if (channel == 4)
                    data[(y*width*channel)+(x*channel)+3] = (uint8_t) 0xFF;

            }

        }
        // generate julia set
        for (int x = 0; x != width; x++) 
        {
            for (int y = 0; y != height; y++)
            {
                float zx = (float) x * scale_x - 1.5;
                float zy = (float) y * scale_y - 1.5;

                int i = 0;

                while ((zx*zx+zy*zy) < escape_radius && i < MAX_ITER)
                {
                    float x_temp = zx * zx - zy * zy;
                    zy = 2 * zx * zy + c_imag;
                    zx = x_temp + c_real;
                    i = i + 1;
                }
                
                data[(y*width*channel)+(x*channel)+0] = (uint8_t) i << 20;
                data[(y*width*channel)+(x*channel)+1] = (uint8_t) i << 21;
                data[(y*width*channel)+(x*channel)+2] = (uint8_t) i * 8;
                if (channel == 4)
                    data[(y*width*channel)+(x*channel)+3] = (uint8_t) 0xFF;

            }

        }

        return 0;
    }
};

} // base
} // lutis

#endif