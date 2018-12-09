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

#include <djvUICore/System.h>

#include <djvUICore/FontSettings.h>
#include <djvUICore/SettingsSystem.h>
#include <djvUICore/StyleSettings.h>
#include <djvUICore/Style.h>

#include <djvAV/System.h>

namespace djv
{
    namespace UICore
    {
        struct System::Private
        {
            std::shared_ptr<FontSettings> fontSettings;
            std::shared_ptr<StyleSettings> styleSettings;
            std::shared_ptr<Style> style;
        };

        void System::_init(const std::shared_ptr<Core::Context> & context)
        {
            ISystem::_init("djv::UICore::System", context);
            AV::System::create(context);
            SettingsSystem::create(context);
            _p->fontSettings = FontSettings::create(context);
            _p->styleSettings = StyleSettings::create(context);
            _p->style = Style::create(context);
        }

        System::System() :
            _p(new Private)
        {}

        System::~System()
        {}

        std::shared_ptr<System> System::create(const std::shared_ptr<Core::Context> & context)
        {
            auto out = std::shared_ptr<System>(new System);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<FontSettings> System::getFontSettings() const
        {
            return _p->fontSettings;
        }

        const std::shared_ptr<StyleSettings> System::getStyleSettings() const
        {
            return _p->styleSettings;
        }

        const std::shared_ptr<Style> System::getStyle() const
        {
            return _p->style;
        }

    } // namespace UICore
} // namespace djv
