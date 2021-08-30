#ifndef HEADER_TYPE_H
#define HEADER_TYPE_H

namespace lutis 
{
    namespace type 
    {
        typedef unsigned char Byte;

        struct Color { 
            double R;
            double G;
            double B;
        } c_rgb;

        struct InspectData {
            size_t sizeKB;
            size_t colorChannelSize;
            size_t totalArrayElement;
        };
    }
}

#endif