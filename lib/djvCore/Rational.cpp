//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCore/Rational.h>

#include <djvCore/String.h>

namespace djv
{
    namespace Core
    {
        namespace Math
        {
            Rational::Rational()
            {}

            Rational::Rational(int num, int den) :
                _num(num),
                _den(den)
            {}

            float Rational::toFloat(const Rational& value)
            {
                return value._num / static_cast<float>(value._den);
            }

            Rational Rational::fromFloat(float value)
            {
                //! \todo Implement a proper floating-point to rational number conversion.
                //! Check-out: OpenEXR\IlmImf\ImfRational.h
                return Rational(static_cast<int>(round(value)), 1);
            }

            bool Rational::operator == (const Rational& other) const
            {
                return _num == other._num && _den == other._den;
            }

            bool Rational::operator != (const Rational& other) const
            {
                return !(*this == other);
            }

        } // namespace Math
    } // namespace Core

    std::ostream & operator << (std::ostream & is, const Core::Math::Rational& value)
    {
        is << value.getNum() << '/' << value.getDen();
        return is;
    }

    std::istream & operator >> (std::istream & os, Core::Math::Rational& value)
    {
        std::string s;
        os >> s;
        const auto split = Core::String::split(s, '/');
        if (2 == split.size())
        {
            value = Core::Math::Rational(std::stoi(split[0]), std::stoi(split[1]));
        }
        else
        {
            std::stringstream ss;
            ss << DJV_TEXT("Cannot parse the value") << " '" << s << "'.";
            throw std::invalid_argument(ss.str());
        }
        return os;
    }

} // namespace djv