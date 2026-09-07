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

#ifndef MetalMakerSource_H
#define MetalMakerSource_H

#include <random>
#include "MetalMakerSourceParams.h"
#include <AK/Plugin/PluginServices/AkFXDurationHandler.h>
#include "../thirdparty/dsp/Source/sknightdsp.h"
#include "ModalValues.h"

using namespace sknight;

/// See https://www.audiokinetic.com/library/edge/?source=SDK&id=soundengine__plugins__source.html
/// for the documentation about source plug-ins
class MetalMakerSource
    : public AK::IAkSourcePlugin
{
public:
    MetalMakerSource();
    ~MetalMakerSource();

    /// Plug-in initialization.
    /// Prepares the plug-in for data processing, allocates memory and sets up the initial conditions.
    AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkSourcePluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat) override;

    /// Release the resources upon termination of the plug-in.
    AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// The reset action should perform any actions required to reinitialize the
    /// state of the plug-in to its original state (e.g. after Init() or on effect bypass).
    AKRESULT Reset() override;

    /// Plug-in information query mechanism used when the sound engine requires
    /// information about the plug-in to determine its behavior.
    AKRESULT GetPluginInfo(AkPluginInfo& out_rPluginInfo) override;

    /// Source plug-in DSP execution.
    void Execute(AkAudioBuffer* io_pBuffer) override;

    /// This method is called to determine the approximate duration (in ms) of the source.
    AkReal32 GetDuration() const override;

private:
    static constexpr float kPostGain = 0.5f; // separate from gain param

    /// Pull the per-buffer RTPC values. Transpose and Length are not handled
    /// here: they are latched in Init(), see the comment there.
    void UpdateRTPCParams();
    
    MetalMakerSourceParams* m_pParams;
    AK::IAkPluginMemAlloc* m_pAllocator;
    AK::IAkSourcePluginContext* m_pContext;
    AkFXDurationHandler m_durationHandler;
    
    AkUInt32 sample_rate_;
    AkReal32 last_frequency_ = -1.0f;
    AkReal32 last_q_ = -1.0f;
    AkReal32 gain_smoothed_ = 1.0f;
    AkReal32 gain_target_ = 1.0f;
    float max_t60_ = 0.0f;
    bool has_triggered_ = false;
    bool loop_ = false;
    bool last_loop_value_ = false;
    bool release_triggered_ = false;
    float preset_post_gain_ = 1.0f;
    
    utilities::ADSR envelope;
    generators::WhiteNoise white_noise;
    filters::ModalBank<kNumModes> modal_bank;
    filters::OnePole lpf;
};

#endif // MetalMakerSource_H
