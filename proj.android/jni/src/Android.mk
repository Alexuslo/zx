LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := tremor
LOCAL_SRC_FILES := ../../../src/dependencies/openal-android/obj/local/$(TARGET_ARCH_ABI)/libtremor.a
include $(PREBUILT_STATIC_LIBRARY) 

include $(CLEAR_VARS)
LOCAL_MODULE := ogg
LOCAL_SRC_FILES := ../../../src/dependencies/openal-android/obj/local/$(TARGET_ARCH_ABI)/libogg.a
include $(PREBUILT_STATIC_LIBRARY) 

include $(CLEAR_VARS)
LOCAL_MODULE := openal
LOCAL_SRC_FILES := ../../../src/dependencies/openal-android/obj/local/$(TARGET_ARCH_ABI)/libopenal.a 
include $(PREBUILT_STATIC_LIBRARY) 



include $(CLEAR_VARS)

LOCAL_MODULE := main

#SDK_ROOT points to folder with SDL and oxygine-framework
LOCAL_SRC_FILES := ../../../../oxygine//SDL/src/main/android/SDL_android_main.c

M_SRC := ../../../src
F_SRC := ../../../src/flow/flow
S_SRC := ../../../src/sound/sound
L_SRC := ../../../src/libs
D_SRC := ../../../src/Device
C_SRC := ../../../src/Cpu

LOCAL_SRC_FILES += \
$(M_SRC)/App.cpp \
$(M_SRC)/Button.cpp \
$(M_SRC)/CustomTween.cpp \
$(M_SRC)/Func.cpp \
$(M_SRC)/main.cpp \
$(M_SRC)/Resource.cpp \
$(M_SRC)/Scene.cpp \
$(F_SRC)/flow.cpp \
$(F_SRC)/Stage.cpp \
$(F_SRC)/Transition.cpp \
$(S_SRC)/MemoryStream.cpp \
$(S_SRC)/OggStream.cpp \
$(S_SRC)/ResSound.cpp \
$(S_SRC)/SoundHandle.cpp \
$(S_SRC)/SoundInstance.cpp \
$(S_SRC)/SoundPlayer.cpp \
$(S_SRC)/SoundSystem.cpp \
$(S_SRC)/WavStream.cpp \
$(S_SRC)/oal/SoundHandleOAL.cpp \
$(S_SRC)/oal/SoundOAL.cpp \
$(S_SRC)/oal/SoundSystemOAL.cpp \
$(S_SRC)/oal/StaticSoundHandleOAL.cpp \
$(S_SRC)/oal/StaticStreamOAL.cpp \
$(S_SRC)/oal/StreamingSoundHandleOAL.cpp \
$(S_SRC)/null/SoundHandleNull.cpp \
$(S_SRC)/null/SoundNull.cpp \
$(S_SRC)/null/SoundSystemNull.cpp \
$(D_SRC)/Graphics.cpp \
$(D_SRC)/Io.cpp \
$(D_SRC)/Memory.cpp \
$(D_SRC)/SnapShot.cpp \
$(C_SRC)/Cpu.cpp

LOCAL_C_INCLUDES := 	$(L_SRC) \
			$(L_SRC)/OpenAL \
			$(L_SRC)/OpenAL/Alc \
			$(L_SRC)/OpenAL/include \
			$(L_SRC)/OpenAL/include/AL \
			$(L_SRC)/OpenAL/OpenAl32/Include 

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_EXPORT_CFLAGS += $(LOCAL_CFLAGS)
LOCAL_WHOLE_STATIC_LIBRARIES := ogg openal tremor
LOCAL_EXPORT_LDLIBS := -lOpenSLES -ldl -llog
LOCAL_LDLIBS := -lOpenSLES

LOCAL_STATIC_LIBRARIES := oxygine-framework_static
LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_SHARED_LIBRARY)


#import from NDK_MODULE_PATH defined in build.cmd
$(call import-module, oxygine-framework)
