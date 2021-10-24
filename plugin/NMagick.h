#ifndef HEADER_NMAGICK_H
#define HEADER_NMAGICK_H

#include <napi.h>
#include <Magick++.h>
#include <vector>
#include <string>
#include <iostream>
#include "Type.h"
#include "Base.h"

namespace lutis 
{
    namespace nmagick
    {

        // https://imagemagick.org/script/formats.php
        enum NMagickFormat
        {
            JPEG = 1,
            PNG = 2,
            BMP = 3
        };

        std::string GetFormatStr(NMagickFormat format)
        {
            switch (format)
            {
            case JPEG:
                return "JPEG";
            case PNG:
                return "PNG";
            case BMP:
                return "BMP";
            default:
                return "JPEG";
            }
        }


        class NMagick
        {
            public:
            NMagick(NMagickFormat format)
            {
                // create empty image
                image = Magick::Image();
                this->format = format;
            }

            NMagick(NMagickFormat format, const Magick::Blob& blob)
            {
                image = Magick::Image(blob);
                this->blob = blob;
                this->format = format;
            }

            ~NMagick()
            {

            }

            Magick::Image Image() const
            {
                return image;
            }

            Magick::Blob Blob() const
            {
                return blob;
            }

            size_t Width() const
            {
                return image.columns();
            }

            size_t Height() const
            {
                return image.rows();
            }

            int Resize(double width, double height, double factor, lutis::type::Byte** out_buffer, size_t* out_size)
            {
                try 
                {
                    Magick::Geometry size(height*factor, width*factor);
                    image.zoom(size);
                } catch(Magick::Error& err)
                {
                    return -1;
                }

                Magick::Blob blob_out;
                image.magick(GetFormatStr(format));
                image.write(&blob_out);

                *out_buffer = new lutis::type::Byte[blob_out.length()];
                *out_size = blob_out.length();
                memcpy(*out_buffer, blob_out.data(), blob_out.length());

                return 0;

            }

            public:
            static NMagick* FromBuffer(const Napi::Buffer<lutis::type::Byte>& input, NMagickFormat format)
            {
                lutis::type::Byte* raw_data = reinterpret_cast<lutis::type::Byte*>(input.Data());
                const size_t size_data = input.Length();

                // decode from raw buffer to Magick Blob
                Magick::Blob _blob(raw_data, size_data);
                Magick::Image _image;

                try
                {
                    _image.read(_blob);
                }
                catch(const Magick::Error& e)
                {
                    return nullptr;
                }

                NMagick* out = new NMagick(format);
                out->image = _image;
                out->blob = _blob;

                return out;
                
            }

            private:
            Magick::Image image;
            Magick::Blob blob;
            NMagickFormat format;
        };
    }
}

#endif