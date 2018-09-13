//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#pragma once

#include <djvUI/Core.h>

#include <djvCore/Util.h>

#include <QWidget>

#include <memory>

namespace djv
{
    namespace UI
    {
        class FloatObject;
        class UIContext;

        //! \class FloatEditSlider
        //!
        //! This class provides an floating point slider and edit widget.
        class FloatEditSlider : public QWidget
        {
            Q_OBJECT

                //! This property holds the value.
                Q_PROPERTY(
                    float  value
                    READ   value
                    WRITE  setValue
                    NOTIFY valueChanged)

                //! This property holds the default value.
                Q_PROPERTY(
                    float  defaultValue
                    READ   defaultValue
                    WRITE  setDefaultValue
                    NOTIFY defaultValueChanged)

                //! This property holds the minimum value.
                Q_PROPERTY(
                    float  min
                    READ   min
                    WRITE  setMin
                    NOTIFY minChanged)

                //! This property holds the maximum value.
                Q_PROPERTY(
                    float  max
                    READ   max
                    WRITE  setMax
                    NOTIFY maxChanged)

        public:
            explicit FloatEditSlider(UIContext *, QWidget * parent = nullptr);

            virtual ~FloatEditSlider();

            //! Get the value.
            float value() const;

            //! Get the default value.
            float defaultValue() const;

            //! Get whether a reset to default control is shown.
            bool hasResetToDefault() const;

            //! Get the minimum value.
            float min() const;

            //! Get the maximum value.
            float max() const;

            //! Get the small increment.
            float smallInc() const;

            //! Get the large increment.
            float largeInc() const;

            //! Get the edit floating-point object.
            FloatObject * editObject() const;

            //! Get the slider floating-point object.
            FloatObject * sliderObject() const;

        public Q_SLOTS:
            //! Set the value.
            void setValue(float);

            //! Set the default value.
            void setDefaultValue(float);

            //! Set whether a reset to default control is shown.
            void setResetToDefault(bool);

            //! Set the minimum value.
            void setMin(float);

            //! Set the maximum value.
            void setMax(float);

            //! Set the value range.
            void setRange(float min, float max);

            //! Set the value increment.
            void setInc(float smallInc, float largeInc);

        Q_SIGNALS:
            //! This signal is emitted when the value is changed.
            void valueChanged(float);

            //! This signal is emitted when the default value is changed
            void defaultValueChanged(float);

            //! This signal is emitted when the minimum value is changed.
            void minChanged(float);

            //! This signal is emitted when the maximum value is changed.
            void maxChanged(float);

            //! This signal is emitted when the value range is changed.
            void rangeChanged(float, float);

        private Q_SLOTS:
            void valueCallback();
            void sliderCallback(float);
            void defaultCallback();

            void widgetUpdate();

        private:
            DJV_PRIVATE_COPY(FloatEditSlider);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv