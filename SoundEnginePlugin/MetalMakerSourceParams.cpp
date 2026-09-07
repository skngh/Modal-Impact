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

#include "MetalMakerSourceParams.h"

#include <AK/Tools/Common/AkBankReadHelpers.h>

MetalMakerSourceParams::MetalMakerSourceParams()
{
}

MetalMakerSourceParams::~MetalMakerSourceParams()
{
}

MetalMakerSourceParams::MetalMakerSourceParams(const MetalMakerSourceParams& in_rParams)
{
    RTPC = in_rParams.RTPC;
    NonRTPC = in_rParams.NonRTPC;
    m_paramChangeHandler.SetAllParamChanges();
}

AK::IAkPluginParam* MetalMakerSourceParams::Clone(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, MetalMakerSourceParams(*this));
}

AKRESULT MetalMakerSourceParams::Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    if (in_ulBlockSize == 0)
    {
        // Initialize default parameters here
        NonRTPC.fType = ObjectTypes::HOLLOW_BOTTLE;
        RTPC.fAttack = 0.0f;
        RTPC.fDecay = 0.0f;
        RTPC.fSustain = 1.0f;
        RTPC.fRelease = 0.1f;
        RTPC.fLPF = 20000.0f;
        NonRTPC.fRandomness = 0.0f;
        RTPC.fTranspose = 0.0f;
        RTPC.fLength = 1.0f;
        RTPC.fGain = -12.0f;
        RTPC.fLoop = false;
        m_paramChangeHandler.SetAllParamChanges();
        return AK_Success;
    }

    return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT MetalMakerSourceParams::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT MetalMakerSourceParams::SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    AKRESULT eResult = AK_Success;
    AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

    // Read bank data here
    NonRTPC.fType = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.fAttack = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fDecay = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fSustain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fRelease = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fLPF = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    NonRTPC.fRandomness = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fTranspose = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fLength = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fGain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fLoop = READBANKDATA(bool, pParamsBlock, in_ulBlockSize);
    CHECKBANKDATASIZE(in_ulBlockSize, eResult);
    m_paramChangeHandler.SetAllParamChanges();

    return eResult;
}

AKRESULT MetalMakerSourceParams::SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize)
{
    AKRESULT eResult = AK_Success;

    // Handle parameter change here
    switch (in_paramID)
    {
    case PARAM_TYPE_ID:
        NonRTPC.fType = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_TYPE_ID);
        break;
    case PARAM_ATTACK_ID:
        RTPC.fAttack = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_ATTACK_ID);
        break;
    case PARAM_DECAY_ID:
        RTPC.fDecay = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_DECAY_ID);
        break;
    case PARAM_SUSTAIN_ID:
        RTPC.fSustain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_SUSTAIN_ID);
        break;
    case PARAM_RELEASE_ID:
        RTPC.fRelease = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_RELEASE_ID);
        break;
    case PARAM_LPF_ID:
        RTPC.fLPF = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LPF_ID);
        break;
    case PARAM_RANDOMNESS_ID:
        NonRTPC.fRandomness = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_RANDOMNESS_ID);
        break;
    case PARAM_TRANSPOSE_ID:
        RTPC.fTranspose = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_TRANSPOSE_ID);
        break;
    case PARAM_LENGTH_ID:
        RTPC.fLength = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LENGTH_ID);
        break;
    case PARAM_GAIN_ID:
        RTPC.fGain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_GAIN_ID);
        break;
    case PARAM_LOOP_ID:
        RTPC.fLoop = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_LOOP_ID);
        break;
    default:
        eResult = AK_InvalidParameter;
        break;
    }

    return eResult;
}
