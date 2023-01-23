//
// Created by richard on 22/01/23.
//

/*
 * Animation.cpp Created by Richard Buckley (C) 22/01/23
 */

/**
 * @file Animation.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 22/01/23
 */

#include "Application.h"
#include "Animation.h"

namespace rose {
    Animation::Animation() {
        mAnimationSlot = AnimationProtocol::createSlot();
        mAnimationSlot->receiver = [this](uint64_t ticks) {this->animate(ticks); };
    }

    void Animation::connectSignal() {
        if (!mGadget.expired() && mGadget.lock()->isInitialized()) {
            mGadget.lock()->getApplicationPtr()->animationSignal.connect(mAnimationSlot);
        }
    }

    void Animation::disconnectSignal() {
        if (!mGadget.expired()) {
            mGadget.lock()->getApplicationPtr()->animationSignal.disconnect(mAnimationSlot);
        }
    }

    void LightAnimation::animate(uint64_t ticks) {
        auto dTicks = ticks - mLastTick;
        auto delta = static_cast<float>(dTicks) * (mRate / 1000.f);
        if (mAnimationState != LightAnimationState::PPS_PAUSE)
            mLastTick = ticks;

        switch (mAnimationState) {
            case LightAnimationState::OFF:
                if (mIntensity > 0.f) {
                    mIntensity = fmax(mIntensity - delta, 0.f);
                } else {
                    disconnectSignal();
                }
                break;
            case LightAnimationState::ON:
                if (mIntensity < 1.f) {
                    mIntensity = fmin(mIntensity + delta, 1.f);
                } else {
                    disconnectSignal();
                }
                break;
            case LightAnimationState::PPS_OFF:
                if (mIntensity > 0.f) {
                    mIntensity = fmax(mIntensity - delta, 0.f);
                } else {
                    mAnimationState = LightAnimationState::PPS_PAUSE;
                }
                break;
            case LightAnimationState::PPS_ON:
                if (mIntensity < 1.f) {
                    mIntensity = fmin(mIntensity + delta, 1.f);
                } else {
                    mAnimationState = LightAnimationState::PPS_PAUSE;
                }
                break;
            case LightAnimationState::PPS_PAUSE:
                if (dTicks >= mPause) {
                    mAnimationState = mIntensity > 0.f ? LightAnimationState::PPS_OFF : LightAnimationState::PPS_ON;
                    mLastTick = ticks;
                }
                break;
        }

        if (!mGadget.expired()) {
            auto gadget = mGadget.lock();

            if (gadget->getVisualMetrics().animateBackground) {
                gadget->getVisualMetrics().animateBackground = color::DarkRed.color();
                gadget->getVisualMetrics().animateBackground[Color::ALPHA] = mIntensity;
            }

            if (auto exposed = gadget->getExposedRectangle(); exposed)
                gadget->getWindow()->expose(exposed);
        }
    }

    void LightAnimation::initialize() {
        mAnimationState = LightAnimationState::PPS_ON;
        mRate = 10.f;
        mIntensity = 0.f;
        mPause = 125;
        if (!mGadget.expired()) {
            mGadget.lock()->getVisualMetrics().animateBackground = color::TransparentDarkRed.color();
            switch (mAnimationState) {
                case LightAnimationState::OFF:
                    break;
                case LightAnimationState::ON:
                case LightAnimationState::PPS_OFF:
                case LightAnimationState::PPS_ON:
                case LightAnimationState::PPS_PAUSE:
                    connectSignal();
                    break;
            }
        }
    }
} // rose