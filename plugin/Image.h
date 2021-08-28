#ifndef HEADER_IMAGE_H
#define HEADER_IMAGE_H

#include <napi.h>
#include "Type.h"

namespace lutis 
{
    namespace image 
    {
        int Inspect(const Napi::Buffer<lutis::type::Byte>& data)
        {
            // fprintf(stderr, "%lu\n", data.ByteLength() / sizeof(uint8_t));
            fprintf(stderr, "%lu\n", data.Length());
            return 0;
        }
    }
}

#endif