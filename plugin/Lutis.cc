#include <napi.h>
#include <stdio.h>
#include <vector>
#include "Core.h"
#include "Draw.h"
#include "Type.h"
#include "Filter.h"

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
        lutis::filter::Inspect(buf);

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
        lutis::filter::GaussianBlur(format, buf, out);

        // lutis::type::Byte dataOuts[buf.Length()];
        // for (size_t index = 0; index < buf.Length(); index++) {
        //     // printf("array[%lu] = %d\n", index, out[index]);
        //     dataOuts[index] = out[index];
        // }

        // // free memory
        // lutis::core::CleanUp(&out);

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out.data(), out.size());
    }

    static Napi::Value DrawCircle(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 4)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "second argument should be int").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "third argument should be int").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsNumber())
        {
            Napi::TypeError::New(env, "fourth argument should be int").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::String format = info[0].As<Napi::String>();
        uint32_t width = info[1].As<Napi::Number>().Uint32Value();
        uint32_t height = info[2].As<Napi::Number>().Uint32Value();
        double angle = info[3].As<Napi::Number>().DoubleValue();

        std::vector<lutis::type::Byte> out;

        lutis::draw::DrawElipse(format, width, height, angle, out);

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out.data(), out.size());

    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(Napi::String::New(env, "inspect"), Napi::Function::New(env, Inspect));
        exports.Set(Napi::String::New(env, "gaussianBlur"), Napi::Function::New(env, GaussianBlur));
        exports.Set(Napi::String::New(env, "drawCircle"), Napi::Function::New(env, DrawCircle));
        return exports;
    }

    NODE_API_MODULE(lutis, Init);
}