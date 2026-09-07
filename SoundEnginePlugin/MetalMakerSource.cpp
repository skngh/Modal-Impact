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
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 1 is exclusive
    
    // init effects
    white_noise.Init();
    envelope.Init(static_cast<float>(sample_rate_));
    lpf.Init(static_cast<float>(sample_rate_));
    modal_bank.Init(static_cast<float>(sample_rate_));
    
    // grab params
    const float attack = m_pParams->RTPC.fAttack;
    const float decay = m_pParams->RTPC.fDecay;
    const float sustain = m_pParams->RTPC.fSustain;
    const float release = m_pParams->RTPC.fRelease;
    const float random_param = m_pParams->NonRTPC.fRandomness;
    const float transpose = m_pParams->RTPC.fTranspose;
    const float length_mult = m_pParams->RTPC.fLength;
    loop_ = m_pParams->RTPC.fLoop != 0.0f;
    gain_smoothed_ = utilities::DbToLin(m_pParams->RTPC.fGain);
    AkInt32 object_type = m_pParams->NonRTPC.fType;
 
    // convert semitones to transposition amount.
    // Scale gain by transposition (higher freq Q's are much louder and need to be scaled down)
    const float transpose_amount = std::pow(2.0f, transpose / 12.0f);
    const float transpose_gain_mult = 1.0f / transpose_amount;
    
    envelope.SetParams(attack, decay, sustain, release);
    lpf.SetCutoff(m_pParams->RTPC.fLPF);
    
    // set modal bank values
    for (int i = 0; i < kNumModes; ++i)
    {
        filters::BiquadParams filterParams;
        const auto& preset = kModalBanks[object_type];
        
        const float random_val = 2.0f * dis(gen) - 1.0f;
        
        // plus or minus % for randomness
        constexpr float kFreqJitter = 0.75f;
        constexpr float kGainJitter = 0.40f;
        constexpr float kT60Jitter  = 0.60f;
        
        const float t60_final = preset.filter_t60_[i] * (1.0f + kT60Jitter * random_val * random_param) * length_mult;
        
        filterParams.frequency_ = preset.filter_freqs_[i] * (1.0f + kFreqJitter * random_val * random_param) * transpose_amount;
        filterParams.gain_ = preset.filter_gain_[i] * (1.0f + kGainJitter * random_val * random_param) * transpose_gain_mult;
        filterParams.t60_ = t60_final;
        max_t60_ = max_t60_ > t60_final ? max_t60_ : t60_final; // for calculating duration
        
        modal_bank.SetParamsT60(filterParams, i);
    }
    
    // calculate duration + looping logic
    float duration = attack + decay + release + max_t60_;
    m_durationHandler.Setup(loop_ ? 0 : duration, loop_ ? 0 : in_pContext->GetNumLoops(), in_rFormat.uSampleRate);
    
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
    modal_bank.Reset();
    has_triggered_ = false;
    release_triggered_ = false;
    last_loop_value_ = false;
    
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
    m_durationHandler.ProduceBuffer(out_pBuffer);

    const AkUInt32 uNumChannels = out_pBuffer->NumChannels();
    
    if(!has_triggered_)
    {
        envelope.TriggerEnvelope(loop_);
        has_triggered_ = true;
    }
    
    if(last_loop_value_ != loop_ && !loop_ && !release_triggered_)
    {
        envelope.TriggerRelease();
        m_durationHandler.SetDuration(m_pParams->RTPC.fRelease + max_t60_);
        m_durationHandler.SetLooping(1);
        m_durationHandler.Reset();
        release_triggered_ = true;
    }
    
    UpdateRTPCParams();
    
    const AkUInt16 uNumFrames = out_pBuffer->uValidFrames;

    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)out_pBuffer->GetChannel(i);
        AkUInt16 uFramesProduced = 0;
        
        while (uFramesProduced < out_pBuffer->uValidFrames)
        {
            utilities::SmoothingOnePole(gain_smoothed_, gain_target_, 0.001f);
            
            float noise = lpf.Process(white_noise.Process()) * envelope.Process();
            float sig = modal_bank.Process(noise) * kPostGain * gain_smoothed_;
            
            *pBuf++ = sig;
            ++uFramesProduced;
        }
    }
    last_loop_value_ = loop_;
}

void MetalMakerSource::UpdateRTPCParams()
{
    lpf.SetCutoff(m_pParams->RTPC.fLPF);
    gain_target_ = utilities::DbToLin(m_pParams->RTPC.fGain);
    loop_ = m_pParams->RTPC.fLoop;
}

AkReal32 MetalMakerSource::GetDuration() const
{
    return m_durationHandler.GetDuration() * 1000.0f;
}
