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
        static int DecodeFromBuffer(const Napi::Buffer<lutis::type::Byte>& data, cv::Mat& dest)
        {
            const lutis::type::Byte* arrs = reinterpret_cast<lutis::type::Byte*>(data.Data());

            size_t length = data.Length();

            lutis::type::Byte* datas = new lutis::type::Byte[length];
            memcpy(datas, arrs, length);

            // decode from buffer to Mat
            cv::Mat rawImage(1, length, CV_8UC1, (void*)datas);
            dest = cv::imdecode(rawImage, cv::IMREAD_COLOR|cv::IMREAD_ANYDEPTH);

            delete[] datas;
            return 0;
        }

        static int Inspect(const Napi::Buffer<lutis::type::Byte>& data, lutis::type::InspectData& inspectDataOut)
        {
            cv::Mat decodedMat;
            int decodeResult = DecodeFromBuffer(data, decodedMat);
            if (decodeResult != 0)
                return 1;

            inspectDataOut.sizeKB = data.Length() / 1000;
            inspectDataOut.colorChannelSize = decodedMat.elemSize();
            inspectDataOut.totalArrayElement = decodedMat.total();
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