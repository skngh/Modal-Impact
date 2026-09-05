/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#include "MetalMakerSource.h"
#include "../MetalMakerConfig.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreateMetalMakerSource(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, MetalMakerSource());
}

AK::IAkPluginParam* CreateMetalMakerSourceParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, MetalMakerSourceParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(MetalMakerSource, AkPluginTypeSource, MetalMakerConfig::CompanyID, MetalMakerConfig::PluginID)

MetalMakerSource::MetalMakerSource()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

MetalMakerSource::~MetalMakerSource()
{
}

AKRESULT MetalMakerSource::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSourcePluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (MetalMakerSourceParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;
    
    sample_rate_ = in_rFormat.uSampleRate;
    
    in_rFormat.channelConfig.SetStandard(AK_SPEAKER_SETUP_MONO);

    m_durationHandler.Setup(m_pParams->RTPC.fDuration, in_pContext->GetNumLoops(), in_rFormat.uSampleRate);
    
    white_noise.Init();
    

    envelope.Init(static_cast<float>(sample_rate_));
    
    envelope.SetParams(0.0f, 0.0f, 1.0f, 0.1f);

    lpf.Init(static_cast<float>(sample_rate_));
    
    lpf.SetCutoff(20000.0f);

    distortion.SetGain (0.2f);

    modal_bank.Init(static_cast<float>(sample_rate_));

    for (int i = 0; i < 10; ++i)
    {
        filters::BiquadParams filterParams;
        filterParams.frequency_ = filterFreqs[i] * 1.0f;
        filterParams.gain_ = filterGain[i];
        filterParams.t60_ = filterT60[i] * 1.0f;
        modal_bank.SetParamsT60(filterParams, i);
    }
    
    envelope.TriggerEnvelope();
    
    return AK_Success;
}

AKRESULT MetalMakerSource::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT MetalMakerSource::Reset()
{
    white_noise.Reset();
    envelope.Reset();
    lpf.Reset();
    
    return AK_Success;
}

AKRESULT MetalMakerSource::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeSource;
    out_rPluginInfo.bIsInPlace = true;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void MetalMakerSource::Execute(AkAudioBuffer* out_pBuffer)
{
    m_durationHandler.SetDuration(m_pParams->RTPC.fDuration);
//    utilities::ValueChanged(m_pParams->RTPC.fFrequency, last_frequency_, [&](float v) { bandpass.SetFreq(v); }, 1.0f);
//    utilities::ValueChanged(m_pParams->RTPC.fQ, last_q_, [&](float v) { bandpass.SetQ(v); }, 0.01f);
    m_durationHandler.ProduceBuffer(out_pBuffer);

    const AkUInt32 uNumChannels = out_pBuffer->NumChannels();

    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)out_pBuffer->GetChannel(i);

        AkUInt16 uFramesProduced = 0;
        
        while (uFramesProduced < out_pBuffer->uValidFrames)
        {
            float noise = white_noise.Process() * envelope.Process();
            
            float sig = distortion.Process(modal_bank.Process(noise));
            
            *pBuf++ = sig;
            ++uFramesProduced;
        }
    }
}

AkReal32 MetalMakerSource::GetDuration() const
{
    return m_durationHandler.GetDuration() * 1000.0f;
}
