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

#include <djvViewLib/Application.h>

#include <djvViewLib/FileSystem.h>
#include <djvViewLib/HelpSystem.h>
#include <djvViewLib/ImageSystem.h>
#include <djvViewLib/ImageViewSystem.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/PlaybackSystem.h>
#include <djvViewLib/SettingsSystem.h>
#include <djvViewLib/ToolSystem.h>
#include <djvViewLib/WindowSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct Application::Private
        {
            std::vector<std::shared_ptr<ISystem> > systems;
            std::shared_ptr<MainWindow> mainWindow;
        };
        
        void Application::_init(int & argc, char ** argv)
        {
            Desktop::Application::_init(argc, argv);

            DJV_PRIVATE_PTR();
            p.systems.push_back(FileSystem::create(this));
            p.systems.push_back(WindowSystem::create(this));
            p.systems.push_back(ImageViewSystem::create(this));
            p.systems.push_back(ImageSystem::create(this));
            p.systems.push_back(PlaybackSystem::create(this));
            p.systems.push_back(ToolSystem::create(this));
            p.systems.push_back(HelpSystem::create(this));
            p.systems.push_back(SettingsSystem::create(this));

            p.mainWindow = MainWindow::create(this);
            p.mainWindow->show();
        }

        Application::Application() :
            _p(new Private)
        {}

        Application::~Application()
        {
            DJV_PRIVATE_PTR();
            while (p.systems.size())
            {
                auto system = p.systems.back();
                system->setParent(nullptr);
                p.systems.pop_back();
            }
        }

        std::shared_ptr<Application> Application::create(int & argc, char ** argv)
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(argc, argv);
            return out;
        }

    } // namespace ViewLib
} // namespace djv
