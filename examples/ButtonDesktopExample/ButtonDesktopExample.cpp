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

#include <djvDesktop/Application.h>

#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto app = Desktop::Application::create(argc, argv);

        auto pushButton = UI::Button::Push::create(app.get());
        pushButton->setText("Push Button");
        pushButton->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
        pushButton->setClickedCallback(
            []
        {
            std::cout << "Clicked" << std::endl;
        });

        auto toggleButton = UI::Button::Push::create(app.get());
        toggleButton->setButtonType(UI::ButtonType::Toggle);
        toggleButton->setText("Toggle Button");
        toggleButton->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
        toggleButton->setCheckedCallback(
            [](bool value)
        {
            std::cout << value << std::endl;
        });

        auto layout = UI::Layout::Vertical::create(app.get());
        layout->setHAlign(UI::HAlign::Center);
        layout->setVAlign(UI::VAlign::Center);
        layout->addWidget(pushButton);
        layout->addWidget(toggleButton);

        auto window = UI::Window::create(app.get());
        window->addWidget(layout);
        window->show();

        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}