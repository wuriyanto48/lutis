#ifndef HEADER_NMAGICK_H
#define HEADER_NMAGICK_H

#include <napi.h>
#include <Magick++.h>
#include <vector>
#include <string>
#include <iostream>
#include "Type.h"

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

        const static std::unordered_map<std::string, int> string_to_format_case
        ({
            {"JPEG", 1},
            {"jpeg", 2},
            {"JPG", 3},
            {"jpg", 4},
            {"PNG", 5},
            {"png", 6},
            {"BMP", 7},
            {"bmp", 8}
        });

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

        NMagickFormat GetFormat(const std::string& format_str)
        {
            switch (string_to_format_case.at(format_str))
            {
            case 1:
            case 2:
            case 3:
            case 4:
                return JPEG;
            case 5:
            case 6:
                return PNG;
            case 7:
            case 8:
                return BMP;
            default:
                return JPEG;
            }
        }

        // https://www.imagemagick.org/Magick++/Documentation.html
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

            int Rotate(double angle, lutis::type::Byte** out_buffer, size_t* out_size)
            {
                try 
                {
                    image.rotate(angle);
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

            int DrawText(const std::string& text, const std::string& font, double text_size, uint32_t text_width, 
                uint32_t text_height, const lutis::type::Vector2& pos, 
                const lutis::type::Color& text_color, 
                lutis::type::Byte** out_buffer, size_t* out_size)
            {
                // operations
                try 
                {
                    // std::vector<Magick::Drawable> textDrawList;
                    // // set the text font: the font is specified via a string representing
                    // // a fully qualified X font name (wildcards '*' are allowed)
                    // textDrawList.push_back(Magick::DrawableFont("-misc-fixed-medium-o-semicondensed—13-*-*-*-c-60-iso8859-1"));
                    // // set the text to be drawn at specified position: x=101, y=50 this case
                    // textDrawList.push_back( Magick::DrawableText(100, 100, text));
                    // // set the text color (the fill color must be set to transparent)
                    // textDrawList.push_back( Magick::DrawableStrokeColor(Magick::Color("black")));
                    // textDrawList.push_back( Magick::DrawableFillColor(Magick::Color("red")));

                    // image.draw(textDrawList);

                    // // Set draw options 
                    // image.strokeColor("red"); // Outline color 
                    
                    image.fillColor(Magick::ColorRGB(text_color.R, text_color.G, text_color.B)); // Fill color 
                    image.fontFamily(font);
                    image.fontPointsize(text_size);
                    Magick::Geometry place = Magick::Geometry(text_width, text_height, pos.x, pos.y);
                    image.annotate(text, place);
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