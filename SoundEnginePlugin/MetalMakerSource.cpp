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

static AkUInt32 MixSeed(AkUInt32 x) // thanks stack overflow
{
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

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
    
    rng_state_ = MixSeed(in_pContext->GetVoiceInfo()->GetPlayingID());
    if (rng_state_ == 0)
        rng_state_ = 1;
    
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
 
    envelope.SetParams(attack, decay, sustain, release);
    lpf.SetCutoff(m_pParams->RTPC.fLPF);
    
    const auto& preset = kModalBanks[object_type];
    preset_post_gain_ = preset.post_gain_;
    // set modal bank values
    for (int i = 0; i < kNumModes; ++i)
    {
        const float random_val = NextRandom();
        
        // plus or minus % for randomness
        constexpr float kFreqJitter = 0.75f;
        constexpr float kGainJitter = 0.40f;
        constexpr float kT60Jitter  = 0.60f;
        
        const float t60_final = preset.filter_t60_[i] * (1.0f + kT60Jitter * random_val * random_param) * length_mult;
        
        base_filter_params_[i].frequency_ = preset.filter_freqs_[i] * (1.0f + kFreqJitter * random_val * random_param);
        base_filter_params_[i].gain_ = preset.filter_gain_[i] * (1.0f + kGainJitter * random_val * random_param);
        base_filter_params_[i].t60_ = t60_final;
        max_t60_ = max_t60_ > t60_final ? max_t60_ : t60_final; // for calculating duration
    }
    
    ApplyTranspose(transpose);
    
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
    elapsed_frames_ = 0;
    gain_smoothed_ = 0.0f;
    
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
    elapsed_frames_ += out_pBuffer->uValidFrames;

    const AkUInt32 uNumChannels = out_pBuffer->NumChannels();
    
    UpdateRTPCParams();
    
    if(!has_triggered_)
    {
        envelope.TriggerEnvelope(loop_);
        has_triggered_ = true;
    }
    
    if(last_loop_value_ != loop_ && !loop_ && !release_triggered_)
    {
        envelope.TriggerRelease();
        m_durationHandler.SetDuration(static_cast<AkReal32>(elapsed_frames_) / static_cast<AkReal32>(sample_rate_) + m_pParams->RTPC.fRelease + max_t60_);
        m_durationHandler.SetLooping(1);
        release_triggered_ = true;
    }

    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)out_pBuffer->GetChannel(i);
        AkUInt16 uFramesProduced = 0;
        
        while (uFramesProduced < out_pBuffer->uValidFrames)
        {
            utilities::SmoothingOnePole(gain_smoothed_, gain_target_, 0.001f);
            
            float noise = lpf.Process(white_noise.Process()) * envelope.Process();
            float sig = modal_bank.Process(noise) * kPostGain * gain_smoothed_ * preset_post_gain_;
            
            *pBuf++ = sig;
            ++uFramesProduced;
        }
    }
    last_loop_value_ = loop_;
}

float MetalMakerSource::NextRandom()
{
    rng_state_ ^= rng_state_ << 13;
    rng_state_ ^= rng_state_ >> 17;
    rng_state_ ^= rng_state_ << 5;
    return static_cast<float>(rng_state_) * (2.0f / 4294967296.0f) - 1.0f;
}

void MetalMakerSource::ApplyTranspose(float transpose)
{
    // convert semitones to transposition amount
    const float transpose_amount = std::pow(2.0f, transpose / 12.0f);
    // scale gain by transposition amount (it gets much louder when higher)
    const float transpose_gain_mult = 1.0f / transpose_amount;

    for (int i = 0; i < kNumModes; ++i)
    {
        filters::BiquadParams p = base_filter_params_[i];
        p.frequency_ *= transpose_amount;
        p.gain_ *= transpose_gain_mult;
        modal_bank.SetParamsT60(p, i);
    }
}

void MetalMakerSource::UpdateRTPCParams()
{
    auto& changes = m_pParams->m_paramChangeHandler;
    
    if(changes.HasChanged(PARAM_TRANSPOSE_ID))
    {
        ApplyTranspose(m_pParams->RTPC.fTranspose);
        changes.ResetParamChange(PARAM_TRANSPOSE_ID);
    }
    lpf.SetCutoff(m_pParams->RTPC.fLPF);
    gain_target_ = utilities::DbToLin(m_pParams->RTPC.fGain);
    loop_ = m_pParams->RTPC.fLoop;
}

AkReal32 MetalMakerSource::GetDuration() const
{
    return m_durationHandler.GetDuration() * 1000.0f;
}
