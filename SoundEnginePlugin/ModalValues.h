//
//  ModalValues.h
//  MetalMakerSource
//
//  Created by Sam Knight on 9/4/26.
//

static constexpr int kNumModes = 15;

static constexpr float filterFreqs[kNumModes] {
    270.1f,
    810.6f,
    1119.0f,
    1362.0f,
    1681.0f,
    1883.0f,
    2314.0f,
    2455.0f,
    2635.0f,
    2686.0f,
    3605.0f,
    4875.0f,
    7035.0f,
    8353.0f,
    9980.0f,
};

static constexpr float filterT60[kNumModes] {
    4.5f,
    3.0f,
    1.4f,
    1.2f,
    2.0f,
    2.5f,
    1.1f,
    1.4f,
    1.5f,
    1.3f,
    1.2f,
    0.7f,
    0.5f,
    0.45f,
    0.1f,
};
static constexpr float filterGain[kNumModes] {
    1.0f,
    0.45f,
    0.03f,
    0.15f,
    0.13f,
    0.32f,
    0.05f,
    0.15f,
    0.13f,
    0.20f,
    0.14f,
    0.03f,
    0.01f,
    0.01f,
    0.007f,
};
