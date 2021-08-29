#ifndef HEADER_IMAGE_H
#define HEADER_IMAGE_H

#include <napi.h>
#include "Type.h"
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace lutis 
{
    namespace core 
    {
        const int MAX_KERNEL_LENGTH = 31;

        int Inspect(const Napi::Buffer<lutis::type::Byte>& data)
        {
            // fprintf(stderr, "%lu\n", data.ByteLength() / sizeof(uint8_t));
            fprintf(stderr, "Size in Bytes: %lu\n", data.Length());

            cv::Mat im(853, 853, CV_8UC1, *data.Data());
            fprintf(stderr, "Is image empty: %u\n", im.empty());
            return 0;
        }

        int GaussianBlur(const std::string& format, const Napi::Buffer<lutis::type::Byte>& data, std::vector<lutis::type::Byte>& out)
        {
            printf("performing GaussianBlur\n");
            lutis::type::Byte* arrs = reinterpret_cast<lutis::type::Byte*>(data.Data());

            lutis::type::Byte datas[data.Length()];
            for (size_t index = 0; index < data.Length(); index++) 
            {
                datas[index] = arrs[index];
            }

            // decode from buffer to Mat
            cv::Mat rawImage(1, sizeof(datas), CV_8UC1, (void*)datas);
            cv::Mat decodedMat = cv::imdecode(rawImage, cv::IMREAD_COLOR|cv::IMREAD_ANYDEPTH);
            
            cv::Mat dst = decodedMat.clone();

            for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
                cv::GaussianBlur(decodedMat, dst, cv::Size( i, i ), 0, 0 );
            
            // cv::imwrite("dst.jpg", dst);
            // printf("dst.total(): %lu\n", dst.total());
            // printf("dst.elemSize(): %lu\n", dst.elemSize());
            printf("size dst.data %lu\n", sizeof(dst.data));

            for (size_t index = 0; index < data.Length(); index++) {
                printf("array[%lu] = %d\n", index, dst.data[index]);
            }

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

        void CleanUp(lutis::type::Byte** out)
        {
            lutis::type::Byte* imBytes = *out;
            delete[] imBytes;
        }
    }
}

#endif