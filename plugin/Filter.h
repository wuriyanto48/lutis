#ifndef HEADER_FILTER_H
#define HEADER_FILTER_H

#include <napi.h>
#include "Core.h"
#include "Type.h"
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace lutis 
{
    namespace filter 
    {
        static const int MAX_KERNEL_LENGTH = 31;

        static int GaussianBlur(const std::string& format, 
            const Napi::Buffer<lutis::type::Byte>& data, std::vector<lutis::type::Byte>& out)
        {
            printf("performing GaussianBlur\n");
            cv::Mat decodedMat;
            int decodeResult = lutis::core::DecodeFromBuffer(data, decodedMat);
            if (decodeResult != 0)
                return 1;
            
            cv::Mat dst = decodedMat.clone();

            for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
                cv::GaussianBlur(decodedMat, dst, cv::Size( i, i ), 0, 0 );
            
            // cv::imwrite("dst.jpg", dst);

            // int size = dst.total() * dst.elemSize();
            // *out = new lutis::type::Byte[size];
            // memcpy(*out, dst.data, size * sizeof(lutis::type::Byte));

            // *out = new lutis::type::Byte[data.Length()];
            // memcpy(*out, datas, data.Length() * sizeof(lutis::type::Byte));

            // encode Mat to memory buffer
            // opencv supported format https://docs.opencv.org/3.4/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56
            cv::imencode(format, dst, out);

            return 0;
        }
    }
}

#endif