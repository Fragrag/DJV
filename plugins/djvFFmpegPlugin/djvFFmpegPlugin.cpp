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

//! \file djvFFmpegPlugin.cpp

#include <djvFFmpegPlugin.h>

#include <djvFFmpegLoad.h>
#include <djvFFmpegSave.h>
#include <djvFFmpegWidget.h>

#include <djvAssert.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvFileIoUtil.h>
#include <djvMath.h>

#include <QApplication>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvFFmpegPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvFFmpegPlugin::Options
//------------------------------------------------------------------------------

djvFFmpegPlugin::Options::Options() :
    codec  (H264),
    quality(HIGH)
{}

//------------------------------------------------------------------------------
// djvFFmpegPlugin
//------------------------------------------------------------------------------

const QString djvFFmpegPlugin::staticName = "FFmpeg";

QStringList djvFFmpegPlugin::avCodecLabels()
{
    QStringList out;
    
    const AVCodecDescriptor * desc = 0;
    
    while ((desc = avcodec_descriptor_next(desc)))
    {
        if (AVMEDIA_TYPE_VIDEO == desc->type)
        {
            out += desc->name;
        }
    }

    out.sort();
    
    return out;
}

QStringList djvFFmpegPlugin::avCodecTextLabels()
{
    QStringList out;

    const AVCodecDescriptor * desc = 0;
    
    while ((desc = avcodec_descriptor_next(desc)))
    {
        if (AVMEDIA_TYPE_VIDEO == desc->type)
        {
            out += desc->long_name;
        }
    }

    out.sort();

    return out;
}

const QStringList & djvFFmpegPlugin::codecLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFFmpegPlugin", "H264") <<
        qApp->translate("djvFFmpegPlugin", "MPEG4");

    DJV_ASSERT(data.count() == CODEC_COUNT);

    return data;
}

const QStringList & djvFFmpegPlugin::qualityLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFFmpegPlugin", "Low") <<
        qApp->translate("djvFFmpegPlugin", "Medium") <<
        qApp->translate("djvFFmpegPlugin", "High");

    DJV_ASSERT(data.count() == QUALITY_COUNT);

    return data;
}

const QStringList & djvFFmpegPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFFmpegPlugin", "Codec") <<
        qApp->translate("djvFFmpegPlugin", "Quality");

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

namespace
{

void avLogCallback(void * ptr, int level, const char * fmt, va_list vl)
{
    if (level > av_log_get_level())
        return;

    //! \todo Add multi-thread safe logging.

    /*char s [djvStringUtil::cStringLength];
    
    SNPRINTF(s, djvStringUtil::cStringLength, fmt, vl);
    
    DJV_LOG("djvFFmpegPlugin", s);*/
}

} // namespace

void djvFFmpegPlugin::initPlugin() throw (djvError)
{
    //DJV_DEBUG("djvFFmpegPlugin::initPlugin");
    
    //av_log_set_callback(avLogCallback);
    
    av_register_all();

    //DJV_DEBUG_PRINT("codecs = " << avCodecLabels());
}

djvPlugin * djvFFmpegPlugin::copyPlugin() const
{
    djvFFmpegPlugin * plugin = new djvFFmpegPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvFFmpegPlugin::pluginName() const
{
    return staticName;
}

QStringList djvFFmpegPlugin::extensions() const
{
    return QStringList() <<
        ".avi"  <<
        ".dv"   <<
        ".gif"  <<
        ".flv"  <<
        ".mkv"  <<
        ".mov"  <<
        ".mpg"  <<
        ".mpeg" <<
        ".mp4"  <<
        ".m4v";
}

bool djvFFmpegPlugin::isSequence() const
{
    return false;
}

QStringList djvFFmpegPlugin::option(const QString & in) const
{
    const QStringList & list = options();

    QStringList out;

    if (0 == in.compare(list[OPTIONS_CODEC], Qt::CaseInsensitive))
    {
        out << _options.codec;
    }
    else if (0 == in.compare(list[OPTIONS_QUALITY], Qt::CaseInsensitive))
    {
        out << _options.quality;
    }

    return out;
}

bool djvFFmpegPlugin::setOption(const QString & in, QStringList & data)
{
    const QStringList & list = options();

    try
    {
        if (0 == in.compare(list[OPTIONS_CODEC], Qt::CaseInsensitive))
        {
            CODEC codec;
            
            data >> codec;
            
            if (codec != _options.codec)
            {
                _options.codec = codec;
                
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(list[OPTIONS_QUALITY], Qt::CaseInsensitive))
        {
            QUALITY quality;
            
            data >> quality;
            
            if (quality != _options.quality)
            {
                _options.quality = quality;
                
                Q_EMIT optionChanged(in);
            }
        }
    }
    catch (QString)
    {
        return false;
    }

    return true;
}

QStringList djvFFmpegPlugin::options() const
{
    return optionsLabels();
}

void djvFFmpegPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if (qApp->translate("djvFFmpegPlugin", "-ffmpeg_codec") == arg)
            {
                in >> _options.codec;
            }
            else if (qApp->translate("djvFFmpegPlugin", "-ffmpeg_quality") == arg)
            {
                in >> _options.quality;
            }
            else
            {
                tmp << arg;
            }
        }
    }
    catch (const QString &)
    {
        throw arg;
    }

    in = tmp;
}

QString djvFFmpegPlugin::commandLineHelp() const
{
    return qApp->translate("djvFFmpegPlugin",
"\n"
"FFmpeg Options\n"
"\n"
"    -ffmpeg_codec (value)\n"
"        Set the codec used when saving FFmpeg movies. Options = %1. "
"Default = %2.\n"
"    -ffmpeg_quality (value)\n"
"        Set the quality used when saving FFmpeg movies. Options = %3. "
"Default = %4.\n"
    ).
    arg(codecLabels().join(", ")).
    arg(djvStringUtil::label(_options.codec).join(", ")).
    arg(qualityLabels().join(", ")).
    arg(djvStringUtil::label(_options.quality).join(", "));
}

djvImageLoad * djvFFmpegPlugin::createLoad() const
{
    return new djvFFmpegLoad;
}

djvImageSave * djvFFmpegPlugin::createSave() const
{
    return new djvFFmpegSave(_options);
}

djvAbstractPrefsWidget * djvFFmpegPlugin::createWidget()
{
    return new djvFFmpegWidget(this);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvFFmpegPlugin::CODEC,
    djvFFmpegPlugin::codecLabels())
_DJV_STRING_OPERATOR_LABEL(djvFFmpegPlugin::QUALITY,
    djvFFmpegPlugin::qualityLabels())