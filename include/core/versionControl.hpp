//
// Created by Dextron12 on 26/09/2025.
//

#ifndef DEXIUM_CONFIG_HPP
#define DEXIUM_CONFIG_HPP

namespace Dexium::VERSION {
    inline int major = 0;
    inline int minor = 16;
    inline int patch = 1;
}

// CHANGELOG: v0.16.1-A
/*
 * Finished the SoundEffect system.
 * Tested SoundEffect & StreamAudio systems independently and together
 * Both systems work well and miniaudio handles threading internally

 For now Miniaudio library works well for Dexium's audio goals but lacks spatial audio support
 This isn't a big deal at the moment since we are only focusing on 2D right now.
 But when we move to 34D using miniaudio alone to deliver spatial audio might be challanging
 */

#endif //DEXIUM_CONFIG_HPP