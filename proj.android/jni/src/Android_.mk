LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := main

#SDK_ROOT points to folder with SDL and oxygine-framework
LOCAL_SRC_FILES := ../../../../oxygine//SDL/src/main/android/SDL_android_main.c

MAIN := ../../../src
FLOW := ../../../src/flow/flow
SOUND := ../../../src/sound/sound
LOCAL_SRC_FILES += \
$(SRC)/example.cpp \
$(SRC)/main.cpp \
$(SRC)/App.cpp \
$(SRC)/CustomTween.cpp \
$(SRC)/Func.cpp \
$(SRC)/Resource.cpp \
$(FLOW)/flow.cpp \
$(FLOW)/Scene.cpp \
$(FLOW)/Transition.cpp \
$(SOUND)/MemoryStream.cpp \
$(SOUND)/OggStream.cpp \
$(SOUND)/ResSound.cpp \
$(SOUND)/SoundHandle.cpp \
$(SOUND)/SoundInstance.cpp \
$(SOUND)/SoundPlayer.cpp \
$(SOUND)/SoundSystem.cpp \
$(SOUND)/WavStream.cpp \
$(SOUND)/oal/SoundHandleOAL.cpp \
$(SOUND)/oal/SoundOAL.cpp \
$(SOUND)/oal/SoundSystemOAL.cpp \
$(SOUND)/oal/StaticSoundHandleOAL.cpp \
$(SOUND)/oal/StaticStreamOAL.cpp \
$(SOUND)/oal/StreamingSoundHandleOAL.cpp \
$(SOUND)/null/SoundHandleNull.cpp \
$(SOUND)/null/SoundNull.cpp \
$(SOUND)/null/SoundSystemNull.cpp

LOCAL_STATIC_LIBRARIES := oxygine-framework_static libogg_static tremor
LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_SHARED_LIBRARY)


#import from NDK_MODULE_PATH defined in build.cmd
$(call import-module, oxygine-framework)
