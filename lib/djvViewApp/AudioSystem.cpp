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

#include <djvViewApp/AudioSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AudioSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void AudioSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::AudioSystem", context);

            DJV_PRIVATE_PTR();

            //! \todo Implement me!
            p.actions["IncreaseVolume"] = UI::Action::create();
            p.actions["IncreaseVolume"]->setEnabled(false);
            p.actions["DecreaseVolume"] = UI::Action::create();
            p.actions["DecreaseVolume"]->setEnabled(false);
            p.actions["Mute"] = UI::Action::create();
            p.actions["Mute"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Mute"]->setEnabled(false);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["IncreaseVolume"]);
            p.menu->addAction(p.actions["DecreaseVolume"]);
            p.menu->addAction(p.actions["Mute"]);

            auto weak = std::weak_ptr<AudioSystem>(std::dynamic_pointer_cast<AudioSystem>(shared_from_this()));
            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        AudioSystem::AudioSystem() :
            _p(new Private)
        {}

        AudioSystem::~AudioSystem()
        {}

        std::shared_ptr<AudioSystem> AudioSystem::create(Context * context)
        {
            auto out = std::shared_ptr<AudioSystem>(new AudioSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > AudioSystem::getActions()
        {
            return _p->actions;
        }

        MenuData AudioSystem::getMenu()
        {
            return
            {
                _p->menu,
                "G"
            };
        }

        void AudioSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.actions["IncreaseVolume"]->setText(_getText(DJV_TEXT("Increase Volume")));
            p.actions["IncreaseVolume"]->setTooltip(_getText(DJV_TEXT("Increase volume tooltip")));
            p.actions["DecreaseVolume"]->setText(_getText(DJV_TEXT("Decrease Volume")));
            p.actions["DecreaseVolume"]->setTooltip(_getText(DJV_TEXT("Decrease volume tooltip")));
            p.actions["Mute"]->setText(_getText(DJV_TEXT("Mute")));
            p.actions["Mute"]->setTooltip(_getText(DJV_TEXT("Mute tooltip")));

            p.menu->setText(_getText(DJV_TEXT("Audio")));
        }

    } // namespace ViewApp
} // namespace djv
