//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/Speed.h>

#include <djvCore/String.h>

#include <algorithm>
#include <cmath>

namespace djv
{
    namespace AV
    {
        namespace
        {
            Speed::FPS _globalSpeed = Speed::getDefaultSpeed();

        } // namespace

        Speed::Speed()
        {
            _set(_globalSpeed);
        }

        Speed::Speed(int scale, int duration) :
            _scale(scale),
            _duration(duration)
        {}

        Speed::Speed(FPS in)
        {
            _set(in);
        }

        float Speed::speedToFloat(const Speed & speed)
        {
            return speed._scale / static_cast<float>(speed._duration);
        }

        Speed Speed::floatToSpeed(float value)
        {
            //! \todo Implement a proper floating-point to rational number conversion.
            //! Check-out: OpenEXR\IlmImf\ImfRational.h
            for (size_t i = 0; i < static_cast<size_t>(FPS::Count); ++i)
            {
                const FPS fps = static_cast<FPS>(i);
                if (abs(value - speedToFloat(fps)) < .001f)
                {
                    return fps;
                }
            }
            return Speed(static_cast<int>(round(value)));
        }

        Speed::FPS Speed::getDefaultSpeed()
        {
            return FPS::_24;
        }

        Speed::FPS Speed::getGlobalSpeed()
        {
            return _globalSpeed;
        }

        void Speed::setGlobalSpeed(FPS fps)
        {
            _globalSpeed = fps;
        }

        bool Speed::operator == (const Speed & other) const
        {
            return _scale == other._scale && _duration == other._duration;
        }

        bool Speed::operator != (const Speed & other) const
        {
            return !(*this == other);
        }

        void Speed::_set(FPS fps)
        {
            static const int scale[] =
            {
                1,
                3,
                6,
                12,
                15,
                16,
                18,
                24000,
                24,
                25,
                30000,
                30,
                50,
                60000,
                60,
                120
            };
            DJV_ASSERT(static_cast<size_t>(FPS::Count) == sizeof(scale) / sizeof(scale[0]));
            static const int duration[] =
            {
                1,
                1,
                1,
                1,
                1,
                1,
                1,
                1001,
                1,
                1,
                1001,
                1,
                1,
                1001,
                1,
                1
            };
            DJV_ASSERT(static_cast<size_t>(FPS::Count) == sizeof(duration) / sizeof(duration[0]));
            _scale = scale[static_cast<size_t>(fps)];
            _duration = duration[static_cast<size_t>(fps)];
        }

    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Speed,
        FPS,
        "1",
        "3",
        "6",
        "12",
        "15",
        "16",
        "18",
        "23.976",
        "24",
        "25",
        "29.97",
        "30",
        "50",
        "59.94",
        "60",
        "120");

    std::ostream& operator << (std::ostream & is, const AV::Speed & value)
    {
        is << value.getScale() << '/' << value.getDuration();
        return is;
    }

    std::istream& operator >> (std::istream & os, AV::Speed & value)
    {
        std::string s;
        os >> s;
        const auto split = Core::String::split(s, '/');
        if (2 == split.size())
        {
            value = AV::Speed(std::stoi(split[0]), std::stoi(split[1]));
        }
        else
        {
            std::stringstream ss;
            ss << DJV_TEXT("Cannot parse: ") << s;
            throw std::invalid_argument(ss.str());
        }
        return os;
    }

} // namespace djv