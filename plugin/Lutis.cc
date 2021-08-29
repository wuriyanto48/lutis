#include <napi.h>
#include <stdio.h>
#include <vector>
#include "Core.h"
#include "Type.h"

namespace lutis
{

    static Napi::Value Inspect(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsBuffer())
        {
            Napi::TypeError::New(env, "argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::Buffer<lutis::type::Byte> buf = info[0].As<Napi::Buffer<lutis::type::Byte>>();
        lutis::core::Inspect(buf);

        return info.Env().Undefined();
    }

    static Napi::Value GaussianBlur(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsBuffer())
        {
            Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::String format = info[0].As<Napi::String>();
        Napi::Buffer<lutis::type::Byte> buf = info[1].As<Napi::Buffer<lutis::type::Byte>>();
        std::vector<lutis::type::Byte> out;
        lutis::core::GaussianBlur(format, buf, out);

        // lutis::type::Byte dataOuts[buf.Length()];
        // for (size_t index = 0; index < buf.Length(); index++) {
        //     // printf("array[%lu] = %d\n", index, out[index]);
        //     dataOuts[index] = out[index];
        // }

        // // free memory
        // lutis::core::CleanUp(&out);

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out.data(), out.size());
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(Napi::String::New(env, "inspect"), Napi::Function::New(env, Inspect));
        exports.Set(Napi::String::New(env, "gaussianBlur"), Napi::Function::New(env, GaussianBlur));
        return exports;
    }

    NODE_API_MODULE(lutis, Init);
}