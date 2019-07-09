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

#include <djvViewApp/BackgroundImageWidget.h>

#include <djvViewApp/WindowSettings.h>

#include <djvUI/SettingsSystem.h>

#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/LogSystem.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct BackgroundImageWidget::Private
        {
            std::shared_ptr<AV::Image::Image> image;
            bool colorize = true;
            std::shared_ptr<AV::IO::IRead> read;
            std::shared_ptr<Time::Timer> timer;
            std::shared_ptr<ValueObserver<std::string> > imageObserver;
            std::shared_ptr<ValueObserver<bool> > colorizeObserver;
        };

        void BackgroundImageWidget::_init(Context * context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::BackgroundImageWidget");

            p.timer = Time::Timer::create(context);
            p.timer->setRepeating(true);

            auto weak = std::weak_ptr<BackgroundImageWidget>(std::dynamic_pointer_cast<BackgroundImageWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto windowSettings = settingsSystem->getSettingsT<WindowSettings>())
            {
                p.imageObserver = ValueObserver<std::string>::create(
                    windowSettings->observeBackgroundImage(),
                    [weak, context](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            try
                            {
                                auto io = context->getSystemT<AV::IO::System>();
                                widget->_p->read = io->read(value);
                                widget->_p->timer->start(
                                    Time::getMilliseconds(Time::TimerValue::Fast),
                                    [weak](float)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            std::shared_ptr<AV::Image::Image> image;
                                            {
                                                std::unique_lock<std::mutex> lock(widget->_p->read->getMutex());
                                                auto& queue = widget->_p->read->getVideoQueue();
                                                if (queue.hasFrames())
                                                {
                                                    image = queue.popFrame().image;
                                                }
                                            }
                                            if (image)
                                            {
                                                widget->_p->image = image;
                                                widget->_p->timer->stop();
                                                widget->_p->read.reset();
                                            }
                                        }
                                    });
                            }
                            catch (const std::exception& e)
                            {
                                widget->_p->image.reset();
                                auto logSystem = context->getSystemT<LogSystem>();
                                logSystem->log("djv::ViewApp::MainWindow", e.what());
                            }
                        }
                    });

                p.colorizeObserver = ValueObserver<bool>::create(
                    windowSettings->observeBackgroundImageColorize(),
                    [weak, context](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorize = value;
                            widget->_redraw();
                        }
                    });
            }
        }

        BackgroundImageWidget::BackgroundImageWidget() :
            _p(new Private)
        {}

        BackgroundImageWidget::~BackgroundImageWidget()
        {}

        std::shared_ptr<BackgroundImageWidget> BackgroundImageWidget::create(Context * context)
        {
            auto out = std::shared_ptr<BackgroundImageWidget>(new BackgroundImageWidget);
            out->_init(context);
            return out;
        }

        void BackgroundImageWidget::_paintEvent(Event::Paint & event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const auto& style = _getStyle();
                const auto& render = _getRender();
                const BBox2f& g = getGeometry();
                const AV::Image::Size& size = p.image->getSize();
                glm::vec2 pos(
                    floorf(g.w() / 2.f - size.w / 2.f),
                    floorf(g.h() / 2.f - size.h / 2.f));
                if (p.colorize)
                {
                    render->setFillColor(style->getColor(UI::ColorRole::Button));
                    render->drawFilledImage(p.image, pos);
                }
                else
                {
                    render->setFillColor(AV::Image::Color(1.f, 1.f, 1.f));
                    render->drawImage(p.image, pos);
                }
            }
        }


    } // namespace ViewApp
} // namespace djv
