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

#include <djvUI/Widget.h>

#include <djvUI/Action.h>
#include <djvUI/IconSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Tooltip.h>
#include <djvUI/UISystem.h>
#include <djvUI/Window.h>

#include <djvAV/FontSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/IEventSystem.h>
#include <djvCore/Math.h>

#include <algorithm>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo [2.0 S] Should this be configurable?
            const float tooltipTimeout   =  .5f;
            const float tooltipHideDelta = 1.f;

            size_t globalWidgetCount = 0;

        } // namespace

        float Widget::_updateTime      = 0.f;
        bool  Widget::_tooltipsEnabled = true;
        bool  Widget::_resizeRequest   = true;
        bool  Widget::_redrawRequest   = true;

        void Widget::_init(Core::Context * context)
        {
            IObject::_init(context);
            
            setClassName("djv::UI::Widget");

            /*context->log("djv::UI::Widget", "Widget::Widget");
            context->log("djv::UI::Widget", String::Format("widget count = %%1").arg(globalWidgetCount));*/
            ++globalWidgetCount;

            _fontSystem = context->getSystemT<AV::Font::System>();
            _render = context->getSystemT<AV::Render::Render2D>();
            _uiSystem = context->getSystemT<UISystem>();
            _iconSystem = context->getSystemT<IconSystem>();
            _style = _uiSystem->getStyle();
        }

        Widget::~Widget()
        {
            --globalWidgetCount;
            /*context->log("djv::UI::Widget", "Widget::~Widget");
            context->log("djv::UI::Widget", String::Format("widget count = %%1").arg(globalWidgetCount));*/
        }
        
        std::shared_ptr<Widget> Widget::create(Context * context)
        {
            //! \bug It would be prefereable to use std::make_shared() here, but how can we do that
            //! with protected contructors?
            auto out = std::shared_ptr<Widget>(new Widget);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Window> Widget::getWindow() const
        {
            return getParentRecursiveT<Window>();
        }

        void Widget::setVisible(bool value)
        {
            if (value == _visible)
                return;
            _visible = value;
            _visibleInit = value;
            _resize();
        }

        void Widget::setOpacity(float value)
        {
            if (value == _opacity)
                return;
            _opacity = value;
            _resize();
        }

        void Widget::setGeometry(const BBox2f & value)
        {
            if (value == _geometry)
                return;
            _geometry = value;
            _resize();
        }

        void Widget::setMargin(const Layout::Margin & value)
        {
            if (value == _margin)
                return;
            _margin = value;
            _resize();
        }

        void Widget::setHAlign(HAlign value)
        {
            if (value == _hAlign)
                return;
            _hAlign = value;
            _resize();
        }

        void Widget::setVAlign(VAlign value)
        {
            if (value == _vAlign)
                return;
            _vAlign = value;
            _resize();
        }

        BBox2f Widget::getAlign(const BBox2f & value, const glm::vec2 & minimumSize, HAlign hAlign, VAlign vAlign)
        {
            float x = 0.f;
            float y = 0.f;
            float w = 0.f;
            float h = 0.f;
            switch (hAlign)
            {
            case HAlign::Center:
                w = minimumSize.x;
                x = value.min.x + value.w() / 2.f - minimumSize.x / 2.f;
                break;
            case HAlign::Left:
                x = value.min.x;
                w = minimumSize.x;
                break;
            case HAlign::Right:
                w = minimumSize.x;
                x = value.min.x + value.w() - minimumSize.x;
                break;
            case HAlign::Fill:
                x = value.min.x;
                w = value.w();
                break;
            default: break;
            }
            switch (vAlign)
            {
            case VAlign::Center:
                h = minimumSize.y;
                y = value.min.y + value.h() / 2.f - minimumSize.y / 2.f;
                break;
            case VAlign::Top:
                y = value.min.y;
                h = minimumSize.y;
                break;
            case VAlign::Bottom:
                h = minimumSize.y;
                y = value.min.y + value.h() - minimumSize.y;
                break;
            case VAlign::Fill:
                y = value.min.y;
                h = value.h();
                break;
            default: break;
            }
            return BBox2f(floorf(x), floorf(y), ceilf(w), ceilf(h));
        }

        void Widget::setBackgroundRole(ColorRole value)
        {
            if (value == _backgroundRole)
                return;
            _backgroundRole = value;
            _redraw();
        }

        void Widget::setShadowOverlay(const std::vector<Side>& value)
        {
            if (value == _shadowOverlay)
                return;
            _shadowOverlay = value;
            _redraw();
        }

        void Widget::setPointerEnabled(bool value)
        {
            _pointerEnabled = value;
        }

        bool Widget::hasTextFocus() const
        {
            bool out = false;
            auto eventSystem = getContext()->getSystemT<Event::IEventSystem>();
            out = eventSystem->getTextFocus().lock() == shared_from_this();
            return out;
        }

        void Widget::takeTextFocus()
        {
            auto eventSystem = getContext()->getSystemT<Event::IEventSystem>();
            eventSystem->setTextFocus(shared_from_this());
        }

        void Widget::releaseTextFocus()
        {
            auto eventSystem = getContext()->getSystemT<Event::IEventSystem>();
            if (eventSystem->getTextFocus().lock() == shared_from_this())
            {
                eventSystem->setTextFocus(nullptr);
            }
        }

        void Widget::addAction(const std::shared_ptr<Action> & action)
        {
            _actions.push_back(action);
        }

        void Widget::removeAction(const std::shared_ptr<Action> & action)
        {
            const auto i = std::find(_actions.begin(), _actions.end(), action);
            if (i != _actions.end())
            {
                _actions.erase(i);
            }
        }

        void Widget::clearActions()
        {
            _actions.clear();
        }

        void Widget::setTooltip(const std::string & value)
        {
            _tooltipText = value;
        }

        void Widget::setTooltipsEnabled(bool value)
        {
            _tooltipsEnabled = value;
        }

        size_t Widget::getGlobalWidgetCount()
        {
            return globalWidgetCount;
        }

        void Widget::moveToFront()
        {
            IObject::moveToFront();
            if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
            {
                auto object = std::dynamic_pointer_cast<Widget>(shared_from_this());
                auto& siblings = parent->_childWidgets;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.push_back(object);
            }
        }

        void Widget::moveToBack()
        {
            IObject::moveToBack();
            if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
            {
                auto object = std::dynamic_pointer_cast<Widget>(shared_from_this());
                auto& siblings = parent->_childWidgets;
                const auto i = std::find(siblings.begin(), siblings.end(), object);
                if (i != siblings.end())
                {
                    siblings.erase(i);
                }
                siblings.insert(siblings.begin(), object);
            }
        }

        bool Widget::event(Event::IEvent & event)
        {
            bool out = IObject::event(event);
            if (!out)
            {
                switch (event.getEventType())
                {
                case Event::Type::ParentChanged:
                {
                    auto & parentChangedEvent = static_cast<Event::ParentChanged &>(event);
                    _clipped = parentChangedEvent.getNewParent() ? true : false;
                    _clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
                    _redraw();
                    break;
                }
                case Event::Type::ChildAdded:
                {
                    auto& childAddedEvent = static_cast<Event::ChildAdded&>(event);
                    if (auto widget = std::dynamic_pointer_cast<Widget>(childAddedEvent.getChild()))
                    {
                        const auto i = std::find(_childWidgets.begin(), _childWidgets.end(), widget);
                        if (i != _childWidgets.end())
                        {
                            _childWidgets.erase(i);
                        }
                        _childWidgets.push_back(widget);
                    }
                    _resize();
                    break;
                }
                case Event::Type::ChildRemoved:
                {
                    auto& childRemovedEvent = static_cast<Event::ChildRemoved&>(event);
                    if (auto widget = std::dynamic_pointer_cast<Widget>(childRemovedEvent.getChild()))
                    {
                        const auto i = std::find(_childWidgets.begin(), _childWidgets.end(), widget);
                        if (i != _childWidgets.end())
                        {
                            _childWidgets.erase(i);
                        }
                    }
                    _resize();
                    break;
                }
                case Event::Type::ChildOrder:
                case Event::Type::Locale:
                    _resize();
                    break;
                case Event::Type::Update:
                {
                    auto & updateEvent = static_cast<Event::Update &>(event);
                    _updateTime = updateEvent.getTime();

                    for (auto & i : _pointerToTooltips)
                    {
                        const auto j = _pointerHover.find(i.first);
                        if (_tooltipsEnabled &&
                            (_updateTime - i.second.timer) > tooltipTimeout &&
                            !i.second.tooltip &&
                            j != _pointerHover.end())
                        {
                            for (
                                auto widget = std::dynamic_pointer_cast<Widget>(shared_from_this());
                                widget;
                                widget = std::dynamic_pointer_cast<Widget>(widget->getParent().lock()))
                            {
                                if (auto tooltipWidget = widget->_createTooltip(j->second))
                                {
                                    if (auto window = getWindow())
                                    {
                                        i.second.tooltip = Tooltip::create(window, j->second, tooltipWidget, getContext());
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
                case Event::Type::Style:
                {
                    _styleEvent(static_cast<Event::Style &>(event));
                    _resize();
                    break;
                }
                case Event::Type::PreLayout:
                    _visibleInit = false;
                    _preLayoutEvent(static_cast<Event::PreLayout &>(event));
                    break;
                case Event::Type::Layout:
                    _layoutEvent(static_cast<Event::Layout &>(event));
                    break;
                case Event::Type::Clip:
                {
                    auto& clipEvent = static_cast<Event::Clip &>(event);
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsVisible = parent->_visible && parent->_parentsVisible;
                        _clipped =
                            !clipEvent.getClipRect().isValid() ||
                            !_visible ||
                            !parent->_visible ||
                            !parent->_parentsVisible;
                        _clipRect = clipEvent.getClipRect();
                    }
                    else
                    {
                        _parentsVisible = true;
                        _clipped = false;
                        _clipRect = BBox2f(0.f, 0.f, 0.f, 0.f);
                    }
                    if (_clipped)
                    {
                        for (auto& i : _pointerToTooltips)
                        {
                            i.second.tooltip = nullptr;
                            i.second.timer   = _updateTime;
                        }
                    }
                    _clipEvent(clipEvent);
                    break;
                }
                case Event::Type::Paint:
                {
                    if (auto parent = std::dynamic_pointer_cast<Widget>(getParent().lock()))
                    {
                        _parentsOpacity = parent->_opacity * parent->_parentsOpacity;
                    }
                    else
                    {
                        _parentsOpacity = 1.f;
                    }
                    if (!_visibleInit)
                    {
                        _render->setColorMult(!isEnabled(true) ? .65f : 1.f);
                        _render->setAlphaMult(getOpacity(true));
                        _paintEvent(static_cast<Event::Paint &>(event));
                    }
                    break;
                }
                case Event::Type::PaintOverlay:
                {
                    if (!_visibleInit)
                    {
                        _paintOverlayEvent(static_cast<Event::PaintOverlay&>(event));
                    }
                    break;
                }
                case Event::Type::PointerEnter:
                {
                    auto & pointerEvent = static_cast<Event::PointerEnter &>(event);
                    const auto & info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    _pointerHover[id] = info.projectedPos;
                    _pointerToTooltips[id] = TooltipData();
                    _pointerToTooltips[id].timer = _updateTime;
                    _pointerEnterEvent(static_cast<Event::PointerEnter &>(event));
                    break;
                }
                case Event::Type::PointerLeave:
                {
                    auto & pointerEvent = static_cast<Event::PointerLeave &>(event);
                    const auto id = pointerEvent.getPointerInfo().id;
                    const auto i = _pointerHover.find(id);
                    if (i != _pointerHover.end())
                    {
                        _pointerHover.erase(i);
                    }
                    const auto j = _pointerToTooltips.find(id);
                    if (j != _pointerToTooltips.end())
                    {
                        _pointerToTooltips.erase(j);
                    }
                    _pointerLeaveEvent(static_cast<Event::PointerLeave &>(event));
                    break;
                }
                case Event::Type::PointerMove:
                {
                    auto & pointerEvent = static_cast<Event::PointerMove &>(event);
                    const auto & info = pointerEvent.getPointerInfo();
                    const auto id = info.id;
                    const auto i = _pointerToTooltips.find(id);
                    if (i != _pointerToTooltips.end())
                    {
                        const auto delta = info.projectedPos - _pointerHover[id];
                        const float l = glm::length(delta);
                        if (l > tooltipHideDelta)
                        {
                            i->second.tooltip = nullptr;
                            i->second.timer   = _updateTime;
                        }
                    }
                    _pointerHover[id] = info.projectedPos;
                    _pointerMoveEvent(static_cast<Event::PointerMove &>(event));
                    break;
                }
                case Event::Type::ButtonPress:
                    _buttonPressEvent(static_cast<Event::ButtonPress &>(event));
                    break;
                case Event::Type::ButtonRelease:
                    _buttonReleaseEvent(static_cast<Event::ButtonRelease &>(event));
                    break;
                case Event::Type::Scroll:
                    _scrollEvent(static_cast<Event::Scroll &>(event));
                    break;
                case Event::Type::Drop:
                    _dropEvent(static_cast<Event::Drop &>(event));
                    break;
                case Event::Type::KeyPress:
                    _keyPressEvent(static_cast<Event::KeyPress &>(event));
                    break;
                case Event::Type::KeyRelease:
                    _keyReleaseEvent(static_cast<Event::KeyRelease &>(event));
                    break;
                case Event::Type::TextFocus:
                    _textFocusEvent(static_cast<Event::TextFocus &>(event));
                    break;
                case Event::Type::TextFocusLost:
                    _textFocusLostEvent(static_cast<Event::TextFocusLost &>(event));
                    break;
                case Event::Type::Text:
                    _textEvent(static_cast<Event::Text &>(event));
                    break;
                default: break;
                }
                out = event.isAccepted();
            }
            return out;
        }

        void Widget::_paintEvent(Event::Paint& event)
        {
            if (_backgroundRole != ColorRole::None)
            {
                _render->setFillColor(_style->getColor(_backgroundRole));
                _render->drawRect(getGeometry());
            }
        }

        void Widget::_paintOverlayEvent(Event::PaintOverlay& event)
        {
            auto style = _getStyle();
            const float ss = style->getMetric(MetricsRole::Shadow);
            const BBox2f& g = getGeometry();
            _render->setFillColor(_style->getColor(ColorRole::Shadow));
            /*if (_shadowOverlay.size())
            {
                _render->drawShadow(BBox2f(g.min.x + ss * 10, g.min.y + ss * 10, g.w(), g.h()), ss);
            }*/
            for (const auto& i : _shadowOverlay)
            {
                switch (i)
                {
                case Side::Left:
                    _render->drawShadow(BBox2f(g.min.x, g.min.y, ss, g.h()), AV::Side::Right);
                    break;
                case Side::Right:
                    _render->drawShadow(BBox2f(g.max.x - ss, g.min.y, ss, g.h()), AV::Side::Left);
                    break;
                case Side::Top:
                    _render->drawShadow(BBox2f(g.min.x, g.min.y, g.w(), ss), AV::Side::Bottom);
                    break;
                case Side::Bottom:
                    _render->drawShadow(BBox2f(g.min.x, g.max.y - ss, g.w(), ss), AV::Side::Top);
                    break;
                default: break;
                }
            }
        }

        void Widget::_pointerEnterEvent(Event::PointerEnter & event)
        {
            if (_pointerEnabled && !event.isRejected())
            {
                event.accept();
            }
        }

        void Widget::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_pointerMoveEvent(Event::PointerMove & event)
        {
            if (_pointerEnabled)
            {
                event.accept();
            }
        }

        void Widget::_keyPressEvent(Event::KeyPress & event)
        {
            if (isEnabled(true))
            {
                // Find the shortcuts.
                std::vector<std::shared_ptr<Shortcut> > shortcuts;
                for (const auto & i : _actions)
                {
                    if (i->observeEnabled()->get())
                    {
                        for (auto j : i->observeShortcuts()->get())
                        {
                            shortcuts.push_back(j);
                        }
                    }
                }

                // Sort the actions so that we test those with keyboard modifiers first.
                std::sort(shortcuts.begin(), shortcuts.end(),
                    [](const std::shared_ptr<Shortcut> & a, const std::shared_ptr<Shortcut> & b) -> bool
                {
                    return a->observeShortcutModifiers()->get() > b->observeShortcutModifiers()->get();
                });

                for (const auto & i : shortcuts)
                {
                    const int key = i->observeShortcutKey()->get();
                    const int modifiers = i->observeShortcutModifiers()->get();
                    if ((key == event.getKey() && event.getKeyModifiers() & modifiers) ||
                        (key == event.getKey() && modifiers == 0 && event.getKeyModifiers() == 0))
                    {
                        event.accept();
                        i->doCallback();
                        break;
                    }
                }
            }
        }

        void Widget::_setMinimumSize(const glm::vec2 & value)
        {
            if (value == _minimumSize)
                return;
            _minimumSize = value;
            _resize();
        }

        std::string Widget::_getTooltipText() const
        {
            std::string out;
            for (const auto & action : _actions)
            {
                const auto & actionTooltip = action->observeTooltip()->get();
                if (!actionTooltip.empty())
                {
                    out = actionTooltip;
                    break;
                }
            }
            if (!_tooltipText.empty())
            {
                out = _tooltipText;
            }
            return out;
        }

        std::shared_ptr<Widget> Widget::_createTooltipDefault(const std::string & text)
        {
            auto context = getContext();
            auto textBlock = TextBlock::create(text, context);
            textBlock->setTextColorRole(ColorRole::TooltipForeground);
            textBlock->setBackgroundRole(ColorRole::TooltipBackground);
            textBlock->setMargin(MetricsRole::Margin);
            return textBlock;
        }

        std::shared_ptr<Widget> Widget::_createTooltip(const glm::vec2 &)
        {
            std::shared_ptr<Widget> out;
            if (!_tooltipText.empty())
            {
                out = _createTooltipDefault(_getTooltipText());
            }
            return out;
        }

    } // namespace UI
} // namespace djv
