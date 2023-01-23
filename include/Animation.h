//
// Created by richard on 22/01/23.
//

/*
 * Animation.h Created by Richard Buckley (C) 22/01/23
 */

/**
 * @file Animation.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 22/01/23
 * @brief 
 * @details
 */

#ifndef ROSE2_ANIMATION_H
#define ROSE2_ANIMATION_H

#include "Gadget.h"

#include <utility>

namespace rose {

    class Gadget;

    /**
     * @class Animation
     */
    class Animation {
    protected:
        std::weak_ptr<Gadget> mGadget;
        uint64_t mLastTick{0};

        AnimationProtocol::slot_type mAnimationSlot;

    public:
        Animation();
        Animation(const Animation&) = delete;
        Animation(Animation&&) = default;
        Animation& operator=(const Animation&) = delete;
        Animation& operator=(Animation&&) = default;

        virtual void animate(uint64_t ticks) = 0;

        virtual ~Animation() = default;

        virtual void initialize() = 0;

        void setGadget(const std::shared_ptr<Gadget>& gadget) { mGadget = gadget; }

        void connectSignal();

        void disconnectSignal();
    };

    enum class LightAnimationState {
        ON, OFF, PPS_ON, PPS_OFF, PPS_PAUSE
    };

    /**
     * @class LightAnimation
     */
    class LightAnimation : public Animation {
    protected:
        LightAnimationState mAnimationState{LightAnimationState::OFF};
        float mIntensity{0.f};
        float mRate{0.f};
        uint64_t mPause{250};

    public:
        LightAnimation() = default;
        LightAnimation(const LightAnimation&) = delete;
        LightAnimation(LightAnimation&&) = default;
        LightAnimation& operator=(const LightAnimation&) = delete;
        LightAnimation& operator=(LightAnimation&&) = default;

        ~LightAnimation() override = default;

        void initialize() override;

        void animate(uint64_t ticks) override;
    };

} // rose

#endif //ROSE2_ANIMATION_H
