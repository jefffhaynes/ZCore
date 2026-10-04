#pragma once

#include <stdint.h>
#include <zephyr/drivers/display.h>
#include "Device.h"
#include "DataCache.h"
#include "Span.h"
#include "Span2D.h"
#include "ReturnCode.h"
#include "ErrorConverter.h"

#if !defined(CONFIG_DISPLAY)
#error "Display needs CONFIG_DISPLAY"
#endif

// TPixel is one whole pixel of the display's format: uint16_t or Rgb565 for RGB565, say.
class Display : public Device
{
public:
    constexpr Display(const struct device* device, display_pixel_format pixelFormat)
        : Device(device), _pixelFormat(pixelFormat)
    {
    }

    constexpr uint32_t GetWidth() const { return _width; }
    constexpr uint32_t GetHeight() const { return _height; }
    constexpr display_pixel_format GetPixelFormat() const { return _pixelFormat; }

    ReturnCode Initialize()
    {
        auto rc = FailIfNotReady();
        CHECK_RETURN_CODE(rc);

        struct display_capabilities capabilities = {};
        display_get_capabilities(GetDevice(), &capabilities);

        if((capabilities.supported_pixel_formats & _pixelFormat) == 0)
        {
            return ReturnCode::NotSupported;
        }

        if(capabilities.current_pixel_format != _pixelFormat)
        {
            auto err = display_set_pixel_format(GetDevice(), _pixelFormat);
            rc = ErrorConverter::Convert(err);
            CHECK_RETURN_CODE(rc);
        }

        _width = capabilities.x_resolution;
        _height = capabilities.y_resolution;

        return ReturnCode::Success;
    }

    ReturnCode TurnOn() const
    {
        auto err = display_blanking_off(GetDevice());

        // A display with no blanking control is always on.
        return err == -ENOSYS ? ReturnCode::Success : ErrorConverter::Convert(err);
    }

    ReturnCode TurnOff() const
    {
        auto err = display_blanking_on(GetDevice());
        return ErrorConverter::Convert(err);
    }

    // Clips to the screen. A full-screen buffer may be shown in place, so leave it unchanged
    // until another replaces it.
    template<typename TPixel>
    ReturnCode Write(uint32_t x, uint32_t y, Span2D<TPixel> pixels) const
    {
        auto rc = CheckPixel<TPixel>();
        CHECK_RETURN_CODE(rc);

        pixels = pixels.Slice(0, 0, x < _width ? _width - x : 0, y < _height ? _height - y : 0);

        if(pixels.IsEmpty())
        {
            return ReturnCode::Success;
        }

        if(pixels.GetStride() > UINT16_MAX)
        {
            return ReturnCode::InvalidArgument;
        }

        struct display_buffer_descriptor descriptor = {};
        descriptor.width = pixels.GetWidth();
        descriptor.height = pixels.GetHeight();
        descriptor.pitch = pixels.GetStride();
        descriptor.buf_size = ((pixels.GetHeight() - 1) * pixels.GetStride() + pixels.GetWidth()) * sizeof(TPixel);

        auto err = display_write(GetDevice(), x, y, &descriptor, pixels.GetData());
        return ErrorConverter::Convert(err);
    }

    template<typename TPixel>
    ReturnCode Write(Span2D<TPixel> pixels) const
    {
        return Write(0, 0, pixels);
    }

    // What's on screen, which can be the last full-screen buffer written. Flush what's drawn
    // here with DataCache: the controller reads memory, not the cache.
    template<typename TPixel>
    ReturnCode GetFrameBuffer(Span2D<TPixel>& frameBuffer) const
    {
        auto rc = CheckPixel<TPixel>();
        CHECK_RETURN_CODE(rc);

        auto* data = static_cast<TPixel*>(display_get_framebuffer(GetDevice()));

        if(data == nullptr)
        {
            return ReturnCode::NotSupported;
        }

        frameBuffer = Span2D<TPixel>(data, _width, _height);

        return ReturnCode::Success;
    }

    // Drawn straight into the frame buffer, so it needs a display that exposes one.
    template<typename TPixel>
    ReturnCode Fill(TPixel value) const
    {
        Span2D<TPixel> frameBuffer;
        auto rc = GetFrameBuffer(frameBuffer);
        CHECK_RETURN_CODE(rc);

        frameBuffer.Fill(value);

        return DataCache::Flush(Span<TPixel>(frameBuffer.GetData(), _width * _height));
    }

private:
    display_pixel_format _pixelFormat;
    uint32_t _width = 0;
    uint32_t _height = 0;

    template<typename TPixel>
    ReturnCode CheckPixel() const
    {
        if(_width == 0 || _height == 0)
        {
            return ReturnCode::InvalidState;
        }

        return sizeof(TPixel) * 8 == DISPLAY_BITS_PER_PIXEL(_pixelFormat) ? ReturnCode::Success
            : ReturnCode::InvalidArgument;
    }
};
