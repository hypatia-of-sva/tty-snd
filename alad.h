/*
 *  alad - a glad-like header-only C/C++ OpenAL-loader by Hypatia of Sva. Look for the current version at: https://github.com/hypatia-of-sva/alad
 *
 *  Dependencies for compilation:
 *      - Kernel32.dll on Windows and -ldl on Unix for loading DLLs / shared libraries.
 *      - Current OpenAL header files; download here: https://github.com/kcat/openal-soft/tree/master/include
 * 
 *  Usage:
 *  
 *  Include this file once with
 *
 *          #define ALAD_IMPLEMENTATION
 *
 *  to define the functions and global function pointers, and include this file without this symbol defined in all other translation units.
 * 
 *  This header loads in all OpenAL symbols except the functions, by loading in the AL headers with AL_NO_PROTOTYPES and ALC_NO_PROTOTYPES defined.
 *  In order for this to work, you need up to date header files, download them from the master branch of openal-soft:
 *      https://github.com/kcat/openal-soft/tree/master/include
 *  (Currently to 1.24.2)
 *  alad can't currently check on whether or not the headers work properly, but compilation will fail due to undefined types.
 *  Keep that in mind if you have compilation issues and put those headers under <AL/alext.h> and <AL/efx-presets.h> (the others are included in alext.h).
 *
 *  There are three interfaces to initialize the API, the (recommended) simplified interface, the manual interface for explicit function loading and the legacy manual interface. 
 *
 *
 *
 *  Simplified Interface (generally recommended):
 *
 *  The simplified interface initializes the library first with
 *  
 *          aladLoadAL();
 *  
 *  which loads in all the core functions from the shared library. This should be enough to select a device and create a context. Once you've done that, call
 *
 *          aladUpdateAL();
 *
 *  which will load all extensions, except AL_EXT_direct_context, with the current context and its device. If an extension is not present, its functions will be NULL, check for that.
 *  The ALC functions are only valid for the current context's device (and technically the AL functions also for the context). If you change the device,
 *  call aladUpdateAL(); again. (If you make another context current, you can also do that, but most implementations of OpenAL will not differentiate
 *  between contexts on the same device for functionality, although it is allowed by the specification.)
 *
 *  Once you're done using OpenAL, terminate the library with
 *
 *          aladTerminate();
 *  
 *  this will unload the shared library.
 *  The library should be named
 *      - OpenAL32.dll / soft_oal.dll on Windows
 *      - libopenal.so.1 / libopenal.so on Linux/BSD
 *      - libopenal.1.dylib / libopenal.dylib on Mac OS.
 *  and should be on path for LoadLibraryA / dlopen.
 *
 *
 *
 *  Manual Interface (only recommended for working with AL_EXT_direct_context):
 *  
 *  In this Interface, you will directly be working with function pointers, and you will have to provide the loader yourself. There are three struct types:
 *  aladALFunctions, aladALCFunctions and aladDirectFunctions, which provide members for al and alc functions except AL_EXT_direct_context, and for the pointers of
 *  AL_EXT_direct_context respectively, where for the first two the common prexix al and alc is missing. The functions are loaded by the following utility functions:
 *  
 *         typedef void (*aladFunction) (void);
 *         typedef aladFunction (*aladLoader) (const char *name);
 *         void aladLoadALCoreMinimal(aladALFunctions* functions, aladLoader loader);
 *         void aladLoadALCoreRest(aladALFunctions* functions, aladLoader loader);
 *         void aladLoadEFX(aladALFunctions* functions, aladLoader loader);
 *         void aladLoadALExtensions(aladALFunctions* functions, aladLoader loader);
 *         void aladLoadALCCore(aladALFunctions* functions, aladLoader loader);
 *         void aladLoadALCExtensions(aladALFunctions* functions, aladLoader loader);
 *         void aladLoadDirectExtension(aladDirectFunctions* functions, aladLoader loader);
 * 
 *  which together load all function pointers except alGetProcAddress and alcGetProcAddress (as the GetProcAddress member of aladALFunctions and aladALCFunctions respectively).
 *  These functions, unlike those of the legacy interface, can be intermixed with those loaded by the simplified interface. This means that one option is to first use the 
 *  simplified interface to load the function pointers from the DLL, and then use these functions to optain explicit function pointers by functions like alcGetProcAddress2.
 *  The wrapping to get those functions to fir the aladLoader type, such as currying the device handle, is something that you will have to do yourself.
 * 
 *  You might also want to consider defining the macro ALAD_NO_SHORT_NAMES, this will then not define names such as alGetInteger. Instead, after the default intialization aladLoadAL();,
 *  you will have to call aladAL.GetInteger. However, this also means you can define these names yourself without the use of #undef.
 *  
 *
 *
 *  Legacy Manual interface (not recommended, description will not be updated):
 *
 *  The manual interface initializes the function pointers first with
 * 
 *          aladLoadALFromLoaderFunction(NULL);
 * 
 *  for default initialization, or with
 * 
 *          aladLoadALFromLoaderFunction(my_alGetProcAddress);
 * 
 *  where my_alGetProcAddress is a loader function of type LPALGETPROCADDRESS for custom initalization, to load all the function via alGetProcAddress.
 *
 *  Default initialization will pull in OpenAL32.dll / soft_oal.dll on Windows or libopenal.so.1 / libopenal.so on Unix respectively,
 *  and libopenal.1.dylib / libopenal.dylib on Mac OS (yet untested).
 *  Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code below in alad_open_ or provide your own function loader.
 *  The shared library will only be loaded once, you can call "aladLoadALFromLoaderFunction(NULL)" as often as you want
 *  to reload the pointers from the loaded shared library.
 *
 *  If you're unsure about loading with a function loader, intialize with
 *
 *          aladLoadALContextFree(AL_FALSE);
 *
 *  to load function pointers directly from the shared library. This only works for default initialization.
 *  This will however not load all function pointers, but only those necessary to create a context.
 *  Those are the core ALC functions and all Core AL functions not relating to buffers, listeners, sources,
 *  and also not alDopplerFactor, alDopplerVelocity, alSpeedOfSound and alDistanceModel.
 *  However, if you change the parameter to anything else (like AL_TRUE), all core AL and ALC functions will be loaded from the shared library.
 *
 *  If you want to use your own library, not available in default initialization, and want to load directly from it, you need to write a wrapper of type
 *  LPALGETPROCADDRESS of the form my_alGetProcAddress("[al-function]") = dlsym(my_lib, "[al-function]"). aladLoadALContextFree is not used here.
 *
 *  These aladLoad functions only initialize the Core API. To load the extensions you have to update the function pointers.
 *
 *  You can update those via the ALCcontext* context through
 *
 *          aladUpdateALPointers(context, AL_FALSE);
 *          aladUpdateALCPointersFromContext(context, AL_FALSE);
 *
 *  which will load all AL and ALC functions, including extensions, via alGetProcAddress for the specific context
 *  by switching the current context temporarily. It will just use the current context, if the parameter is NULL.
 *  If you replace AL_FALSE with anything else (AL_TRUE makes the most sense) this will only load the extensions,
 *  and the core function pointers will remain unchanged
 *
 *  Update ALC pointers to those loaded with a specific ALCdevice* device with
 * 
 *          aladUpdateALCPointersFromDevice(device, AL_FALSE);
 *
 *  If you want to remove this reference to the device, reload them to the nonspecific pointers by calling "aladUpdateALCPointersFromContext" again.
 *  or by calling "aladLoadALContextFree()" if you need them without reference to any context.
 *  Calling "aladLoadALFromLoaderFunction" again won't do anything different from "aladUpdateALCPointersFromContext",
 *  since both call alGetProcAddress and are therefore just dependent on driver state.
 *  The last parameter again makes sure to reload the core, change the value to disable that.
 * 
 *  Unload the library loaded with default initalization with
 * 
 *          aladTerminate();
 *
 *  This will not unload the function pointers or set them to NULL, but they will be probably invalid after unloading the shared library.
 * 
 *  
 *  alad will attempt to load all function pointers in all extensions as currently described in the openal-soft headers
 *  (repo version from 24.03.2025, commit https://github.com/kcat/openal-soft/commit/69c738b88b4e1d96a3f505ae6279bbfc4629abd3),
 *  the ones not available will be NULL, you have to check all functions you want to use for that. you have to check all functions you want to use for that.
 *  The ALC_INVALID_VALUE-Error-State of alcGetProcAddress is not being checked.
 *  If a functionality you expect to be present isn't working, you might want to check the spelling of the
 *          strings in the internal alad_load_alc_functions_ function.
 * 
 */


#pragma once

/* only include alad if the OpenAL headers are not used, because we include them again, and they may have been included with prototypes */
#if !(defined(ALAD_H)) && !(defined(AL_AL_H)) && !(defined(AL_ALC_H)) && !(defined(AL_ALEXT_H)) && !(defined(AL_EFX_H)) && !(defined(EFX_PRESETS_H))
#define ALAD_H

/* revision date */
#define ALAD_HEADER_REVISION 0x20250325

#ifndef __cplusplus
#ifndef nullptr
#define nullptr NULL
#endif
#endif

#ifdef __cplusplus
#define REINTERPRET_CAST(X,Y) reinterpret_cast<X>(Y)
#else
#define REINTERPRET_CAST(X,Y) ((X) Y)
#endif


#if defined(__cplusplus)
extern "C" {
#endif


#define AL_NO_PROTOTYPES
#define ALC_NO_PROTOTYPES
#include <AL/alext.h>
#include <AL/efx-presets.h>
/* currently it can't be checked whether or not the headers work properly. the old prototypes will be shadowed if present */

/* function definitions, referring to the new manual interface view of the simiplified and old manual interface */
#ifndef ALAD_NO_SHORT_NAMES
/* Core AL */
#define alDopplerFactor                 aladAL.DopplerFactor
#define alDopplerVelocity               aladAL.DopplerVelocity
#define alSpeedOfSound                  aladAL.SpeedOfSound
#define alDistanceModel                 aladAL.DistanceModel
#define alEnable                        aladAL.Enable
#define alDisable                       aladAL.Disable
#define alIsEnabled                     aladAL.IsEnabled
#define alGetString                     aladAL.GetString
#define alGetBooleanv                   aladAL.GetBooleanv
#define alGetIntegerv                   aladAL.GetIntegerv
#define alGetFloatv                     aladAL.GetFloatv
#define alGetDoublev                    aladAL.GetDoublev
#define alGetBoolean                    aladAL.GetBoolean
#define alGetInteger                    aladAL.GetInteger
#define alGetFloat                      aladAL.GetFloat
#define alGetDouble                     aladAL.GetDouble
#define alGetError                      aladAL.GetError
#define alIsExtensionPresent            aladAL.IsExtensionPresent
#define alGetProcAddress                aladAL.GetProcAddress
#define alGetEnumValue                  aladAL.GetEnumValue
#define alListenerf                     aladAL.Listenerf
#define alListener3f                    aladAL.Listener3f
#define alListenerfv                    aladAL.Listenerfv
#define alListeneri                     aladAL.Listeneri
#define alListener3i                    aladAL.Listener3i
#define alListeneriv                    aladAL.Listeneriv
#define alGetListenerf                  aladAL.GetListenerf
#define alGetListener3f                 aladAL.GetListener3f
#define alGetListenerfv                 aladAL.GetListenerfv
#define alGetListeneri                  aladAL.GetListeneri
#define alGetListener3i                 aladAL.GetListener3i
#define alGetListeneriv                 aladAL.GetListeneriv
#define alGenSources                    aladAL.GenSources
#define alDeleteSources                 aladAL.DeleteSources
#define alIsSource                      aladAL.IsSource
#define alSourcef                       aladAL.Sourcef
#define alSource3f                      aladAL.Source3f
#define alSourcefv                      aladAL.Sourcefv
#define alSourcei                       aladAL.Sourcei
#define alSource3i                      aladAL.Source3i
#define alSourceiv                      aladAL.Sourceiv
#define alGetSourcef                    aladAL.GetSourcef
#define alGetSource3f                   aladAL.GetSource3f
#define alGetSourcefv                   aladAL.GetSourcefv
#define alGetSourcei                    aladAL.GetSourcei
#define alGetSource3i                   aladAL.GetSource3i
#define alGetSourceiv                   aladAL.GetSourceiv
#define alSourcePlayv                   aladAL.SourcePlayv
#define alSourceStopv                   aladAL.SourceStopv
#define alSourceRewindv                 aladAL.SourceRewindv
#define alSourcePausev                  aladAL.SourcePausev
#define alSourcePlay                    aladAL.SourcePlay
#define alSourceStop                    aladAL.SourceStop
#define alSourceRewind                  aladAL.SourceRewind
#define alSourcePause                   aladAL.SourcePause
#define alSourceQueueBuffers            aladAL.SourceQueueBuffers
#define alSourceUnqueueBuffers          aladAL.SourceUnqueueBuffers
#define alGenBuffers                    aladAL.GenBuffers
#define alDeleteBuffers                 aladAL.DeleteBuffers
#define alIsBuffer                      aladAL.IsBuffer
#define alBufferData                    aladAL.BufferData
#define alBufferf                       aladAL.Bufferf
#define alBuffer3f                      aladAL.Buffer3f
#define alBufferfv                      aladAL.Bufferfv
#define alBufferi                       aladAL.Bufferi
#define alBuffer3i                      aladAL.Buffer3i
#define alBufferiv                      aladAL.Bufferiv
#define alGetBufferf                    aladAL.GetBufferf
#define alGetBuffer3f                   aladAL.GetBuffer3f
#define alGetBufferfv                   aladAL.GetBufferfv
#define alGetBufferi                    aladAL.GetBufferi
#define alGetBuffer3i                   aladAL.GetBuffer3i
#define alGetBufferiv                   aladAL.GetBufferiv

/* EFX */
#define alGenEffects                    aladAL.GenEffects
#define alDeleteEffects                 aladAL.DeleteEffects
#define alIsEffect                      aladAL.IsEffect
#define alEffecti                       aladAL.Effecti
#define alEffectiv                      aladAL.Effectiv
#define alEffectf                       aladAL.Effectf
#define alEffectfv                      aladAL.Effectfv
#define alGetEffecti                    aladAL.GetEffecti
#define alGetEffectiv                   aladAL.GetEffectiv
#define alGetEffectf                    aladAL.GetEffectf
#define alGetEffectfv                   aladAL.GetEffectfv
#define alGenFilters                    aladAL.GenFilters
#define alDeleteFilters                 aladAL.DeleteFilters
#define alIsFilter                      aladAL.IsFilter
#define alFilteri                       aladAL.Filteri
#define alFilteriv                      aladAL.Filteriv
#define alFilterf                       aladAL.Filterf
#define alFilterfv                      aladAL.Filterfv
#define alGetFilteri                    aladAL.GetFilteri
#define alGetFilteriv                   aladAL.GetFilteriv
#define alGetFilterf                    aladAL.GetFilterf
#define alGetFilterfv                   aladAL.GetFilterfv
#define alGenAuxiliaryEffectSlots       aladAL.GenAuxiliaryEffectSlots
#define alDeleteAuxiliaryEffectSlots    aladAL.DeleteAuxiliaryEffectSlots
#define alIsAuxiliaryEffectSlot         aladAL.IsAuxiliaryEffectSlot
#define alAuxiliaryEffectSloti          aladAL.AuxiliaryEffectSloti
#define alAuxiliaryEffectSlotiv         aladAL.AuxiliaryEffectSlotiv
#define alAuxiliaryEffectSlotf          aladAL.AuxiliaryEffectSlotf
#define alAuxiliaryEffectSlotfv         aladAL.AuxiliaryEffectSlotfv
#define alGetAuxiliaryEffectSloti       aladAL.GetAuxiliaryEffectSloti
#define alGetAuxiliaryEffectSlotiv      aladAL.GetAuxiliaryEffectSlotiv
#define alGetAuxiliaryEffectSlotf       aladAL.GetAuxiliaryEffectSlotf
#define alGetAuxiliaryEffectSlotfv      aladAL.GetAuxiliaryEffectSlotfv

/* AL extensions */
/* AL_EXT_STATIC_BUFFER */
#define alBufferDataStatic              aladAL.BufferDataStatic
/* AL_SOFT_buffer_sub_data */
#define alBufferSubDataSOFT             aladAL.BufferSubDataSOFT
/* AL_EXT_FOLDBACK */
#define alRequestFoldbackStart          aladAL.RequestFoldbackStart
#define alRequestFoldbackStop           aladAL.RequestFoldbackStop
/* AL_SOFT_buffer_samples */
#define alBufferSamplesSOFT             aladAL.BufferSamplesSOFT
#define alBufferSubSamplesSOFT          aladAL.BufferSubSamplesSOFT
#define alGetBufferSamplesSOFT          aladAL.GetBufferSamplesSOFT
#define alIsBufferFormatSupportedSOFT   aladAL.IsBufferFormatSupportedSOFT
/* AL_SOFT_source_latency */
#define alSourcedSOFT                   aladAL.SourcedSOFT
#define alSource3dSOFT                  aladAL.Source3dSOFT
#define alSourcedvSOFT                  aladAL.SourcedvSOFT
#define alGetSourcedSOFT                aladAL.GetSourcedSOFT
#define alGetSource3dSOFT               aladAL.GetSource3dSOFT
#define alGetSourcedvSOFT               aladAL.GetSourcedvSOFT
#define alSourcei64SOFT                 aladAL.Sourcei64SOFT
#define alSource3i64SOFT                aladAL.Source3i64SOFT
#define alSourcei64vSOFT                aladAL.Sourcei64vSOFT
#define alGetSourcei64SOFT              aladAL.GetSourcei64SOFT
#define alGetSource3i64SOFT             aladAL.GetSource3i64SOFT
#define alGetSourcei64vSOFT             aladAL.GetSourcei64vSOFT
/* AL_SOFT_deferred_updates */
#define alDeferUpdatesSOFT              aladAL.DeferUpdatesSOFT
#define alProcessUpdatesSOFT            aladAL.ProcessUpdatesSOFT
/* AL_SOFT_source_resampler */
#define alGetStringiSOFT                aladAL.GetStringiSOFT
/* AL_SOFT_events */
#define alEventControlSOFT              aladAL.EventControlSOFT
#define alEventCallbackSOFT             aladAL.EventCallbackSOFT
#define alGetPointerSOFT                aladAL.GetPointerSOFT
#define alGetPointervSOFT               aladAL.GetPointervSOFT
/* AL_SOFT_callback_buffer */
#define alBufferCallbackSOFT            aladAL.BufferCallbackSOFT
#define alGetBufferPtrSOFT              aladAL.GetBufferPtrSOFT
#define alGetBuffer3PtrSOFT             aladAL.GetBuffer3PtrSOFT
#define alGetBufferPtrvSOFT             aladAL.GetBufferPtrvSOFT
/* AL_SOFT_source_start_delay */
#define alSourcePlayAtTimeSOFT          aladAL.SourcePlayAtTimeSOFT
#define alSourcePlayAtTimevSOFT         aladAL.SourcePlayAtTimevSOFT
/* AL_EXT_debug */
#define alDebugMessageCallbackEXT       aladAL.DebugMessageCallbackEXT;
#define alDebugMessageInsertEXT         aladAL.DebugMessageInsertEXT;
#define alDebugMessageControlEXT        aladAL.DebugMessageControlEXT;
#define alPushDebugGroupEXT             aladAL.PushDebugGroupEXT;
#define alPopDebugGroupEXT              aladAL.PopDebugGroupEXT;
#define alGetDebugMessageLogEXT         aladAL.GetDebugMessageLogEXT;
#define alObjectLabelEXT                aladAL.ObjectLabelEXT;
#define alGetObjectLabelEXT             aladAL.GetObjectLabelEXT;
#define alGetPointerEXT                 aladAL.GetPointerEXT;
#define alGetPointervEXT                aladAL.GetPointervEXT;

/* Core ALC */
#define alcCreateContext                aladALC.CreateContext
#define alcMakeContextCurrent           aladALC.MakeContextCurrent
#define alcProcessContext               aladALC.ProcessContext
#define alcSuspendContext               aladALC.SuspendContext
#define alcDestroyContext               aladALC.DestroyContext
#define alcGetCurrentContext            aladALC.GetCurrentContext
#define alcGetContextsDevice            aladALC.GetContextsDevice
#define alcOpenDevice                   aladALC.OpenDevice
#define alcCloseDevice                  aladALC.CloseDevice
#define alcGetError                     aladALC.GetError
#define alcIsExtensionPresent           aladALC.IsExtensionPresent
#define alcGetProcAddress               aladALC.GetProcAddress
#define alcGetEnumValue                 aladALC.GetEnumValue
#define alcGetString                    aladALC.GetString
#define alcGetIntegerv                  aladALC.GetIntegerv
#define alcCaptureOpenDevice            aladALC.CaptureOpenDevice
#define alcCaptureCloseDevice           aladALC.CaptureCloseDevice
#define alcCaptureStart                 aladALC.CaptureStart
#define alcCaptureStop                  aladALC.CaptureStop
#define alcCaptureSamples               aladALC.CaptureSamples

/* ALC extensions */
/* ALC_EXT_thread_local_context */
#define alcSetThreadContext             aladALC.SetThreadContext
#define alcGetThreadContext             aladALC.GetThreadContext
/* ALC_SOFT_loopback */
#define alcLoopbackOpenDeviceSOFT       aladALC.LoopbackOpenDeviceSOFT
#define alcIsRenderFormatSupportedSOFT  aladALC.IsRenderFormatSupportedSOFT
#define alcRenderSamplesSOFT            aladALC.RenderSamplesSOFT
/* ALC_SOFT_pause_device */
#define alcDevicePauseSOFT              aladALC.DevicePauseSOFT
#define alcDeviceResumeSOFT             aladALC.DeviceResumeSOFT
/* ALC_SOFT_HRTF */
#define alcGetStringiSOFT               aladALC.GetStringiSOFT
#define alcResetDeviceSOFT              aladALC.ResetDeviceSOFT
/* ALC_SOFT_device_clock */
#define alcGetInteger64vSOFT            aladALC.GetInteger64vSOFT
/* ALC_SOFT_reopen_device */
#define alcReopenDeviceSOFT             aladALC.ReopenDeviceSOFT
/* ALC_SOFT_system_events */
#define alcEventIsSupportedSOFT         aladALC.EventIsSupportedSOFT;
#define alcEventControlSOFT             aladALC.EventControlSOFT;
#define alcEventCallbackSOFT            aladALC.EventCallbackSOFT;
#endif


/* Public Interface: */

/* simplified Interface */
extern void                             aladLoadAL();
extern void                             aladUpdateAL();
extern void                             aladTerminate();

/* old manual interface */
extern void                             aladLoadALContextFree (ALboolean loadAll);
extern void                             aladLoadALFromLoaderFunction (LPALGETPROCADDRESS inital_loader);
extern void                             aladUpdateALPointers (ALCcontext *context, ALboolean extensionsOnly);
extern void                             aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly);
extern void                             aladUpdateALCPointersFromDevice (ALCdevice *device, ALboolean extensionsOnly);

/* new manual interface */

typedef void (*aladFunction) (void);
typedef aladFunction (*aladLoader) (const char *name);

typedef struct aladALFunctions {
    /* Function Loader */
    LPALGETPROCADDRESS               GetProcAddress;
    /* Minimal Core AL */
    LPALENABLE                       Enable;
    LPALDISABLE                      Disable;
    LPALISENABLED                    IsEnabled;
    LPALGETSTRING                    GetString;
    LPALGETBOOLEANV                  GetBooleanv;
    LPALGETINTEGERV                  GetIntegerv;
    LPALGETFLOATV                    GetFloatv;
    LPALGETDOUBLEV                   GetDoublev;
    LPALGETBOOLEAN                   GetBoolean;
    LPALGETINTEGER                   GetInteger;
    LPALGETFLOAT                     GetFloat;
    LPALGETDOUBLE                    GetDouble;
    LPALGETERROR                     GetError;
    LPALISEXTENSIONPRESENT           IsExtensionPresent;
    LPALGETENUMVALUE                 GetEnumValue;
    /* Rest of Core AL */
    LPALDOPPLERFACTOR                DopplerFactor;
    LPALDOPPLERVELOCITY              DopplerVelocity;
    LPALSPEEDOFSOUND                 SpeedOfSound;
    LPALDISTANCEMODEL                DistanceModel;
    LPALLISTENERF                    Listenerf;
    LPALLISTENER3F                   Listener3f;
    LPALLISTENERFV                   Listenerfv;
    LPALLISTENERI                    Listeneri;
    LPALLISTENER3I                   Listener3i;
    LPALLISTENERIV                   Listeneriv;
    LPALGETLISTENERF                 GetListenerf;
    LPALGETLISTENER3F                GetListener3f;
    LPALGETLISTENERFV                GetListenerfv;
    LPALGETLISTENERI                 GetListeneri;
    LPALGETLISTENER3I                GetListener3i;
    LPALGETLISTENERIV                GetListeneriv;
    LPALGENSOURCES                   GenSources;
    LPALDELETESOURCES                DeleteSources;
    LPALISSOURCE                     IsSource;
    LPALSOURCEF                      Sourcef;
    LPALSOURCE3F                     Source3f;
    LPALSOURCEFV                     Sourcefv;
    LPALSOURCEI                      Sourcei;
    LPALSOURCE3I                     Source3i;
    LPALSOURCEIV                     Sourceiv;
    LPALGETSOURCEF                   GetSourcef;
    LPALGETSOURCE3F                  GetSource3f;
    LPALGETSOURCEFV                  GetSourcefv;
    LPALGETSOURCEI                   GetSourcei;
    LPALGETSOURCE3I                  GetSource3i;
    LPALGETSOURCEIV                  GetSourceiv;
    LPALSOURCEPLAYV                  SourcePlayv;
    LPALSOURCESTOPV                  SourceStopv;
    LPALSOURCEREWINDV                SourceRewindv;
    LPALSOURCEPAUSEV                 SourcePausev;
    LPALSOURCEPLAY                   SourcePlay;
    LPALSOURCESTOP                   SourceStop;
    LPALSOURCEREWIND                 SourceRewind;
    LPALSOURCEPAUSE                  SourcePause;
    LPALSOURCEQUEUEBUFFERS           SourceQueueBuffers;
    LPALSOURCEUNQUEUEBUFFERS         SourceUnqueueBuffers;
    LPALGENBUFFERS                   GenBuffers;
    LPALDELETEBUFFERS                DeleteBuffers;
    LPALISBUFFER                     IsBuffer;
    LPALBUFFERDATA                   BufferData;
    LPALBUFFERF                      Bufferf;
    LPALBUFFER3F                     Buffer3f;
    LPALBUFFERFV                     Bufferfv;
    LPALBUFFERI                      Bufferi;
    LPALBUFFER3I                     Buffer3i;
    LPALBUFFERIV                     Bufferiv;
    LPALGETBUFFERF                   GetBufferf;
    LPALGETBUFFER3F                  GetBuffer3f;
    LPALGETBUFFERFV                  GetBufferfv;
    LPALGETBUFFERI                   GetBufferi;
    LPALGETBUFFER3I                  GetBuffer3i;
    LPALGETBUFFERIV                  GetBufferiv;
    /* EFX */
    LPALGENEFFECTS                   GenEffects;
    LPALDELETEEFFECTS                DeleteEffects;
    LPALISEFFECT                     IsEffect;
    LPALEFFECTI                      Effecti;
    LPALEFFECTIV                     Effectiv;
    LPALEFFECTF                      Effectf;
    LPALEFFECTFV                     Effectfv;
    LPALGETEFFECTI                   GetEffecti;
    LPALGETEFFECTIV                  GetEffectiv;
    LPALGETEFFECTF                   GetEffectf;
    LPALGETEFFECTFV                  GetEffectfv;
    LPALGENFILTERS                   GenFilters;
    LPALDELETEFILTERS                DeleteFilters;
    LPALISFILTER                     IsFilter;
    LPALFILTERI                      Filteri;
    LPALFILTERIV                     Filteriv;
    LPALFILTERF                      Filterf;
    LPALFILTERFV                     Filterfv;
    LPALGETFILTERI                   GetFilteri;
    LPALGETFILTERIV                  GetFilteriv;
    LPALGETFILTERF                   GetFilterf;
    LPALGETFILTERFV                  GetFilterfv;
    LPALGENAUXILIARYEFFECTSLOTS      GenAuxiliaryEffectSlots;
    LPALDELETEAUXILIARYEFFECTSLOTS   DeleteAuxiliaryEffectSlots;
    LPALISAUXILIARYEFFECTSLOT        IsAuxiliaryEffectSlot;
    LPALAUXILIARYEFFECTSLOTI         AuxiliaryEffectSloti;
    LPALAUXILIARYEFFECTSLOTIV        AuxiliaryEffectSlotiv;
    LPALAUXILIARYEFFECTSLOTF         AuxiliaryEffectSlotf;
    LPALAUXILIARYEFFECTSLOTFV        AuxiliaryEffectSlotfv;
    LPALGETAUXILIARYEFFECTSLOTI      GetAuxiliaryEffectSloti;
    LPALGETAUXILIARYEFFECTSLOTIV     GetAuxiliaryEffectSlotiv;
    LPALGETAUXILIARYEFFECTSLOTF      GetAuxiliaryEffectSlotf;
    LPALGETAUXILIARYEFFECTSLOTFV     GetAuxiliaryEffectSlotfv;
    /* AL extensions */
    /* AL_EXT_STATIC_BUFFER */
    PFNALBUFFERDATASTATICPROC        BufferDataStatic;
    /* AL_SOFT_buffer_sub_data */
    PFNALBUFFERSUBDATASOFTPROC       BufferSubDataSOFT;
    /* AL_EXT_FOLDBACK */
    LPALREQUESTFOLDBACKSTART         RequestFoldbackStart;
    LPALREQUESTFOLDBACKSTOP          RequestFoldbackStop;
    /* AL_SOFT_buffer_samples */
    LPALBUFFERSAMPLESSOFT            BufferSamplesSOFT;
    LPALBUFFERSUBSAMPLESSOFT         BufferSubSamplesSOFT;
    LPALGETBUFFERSAMPLESSOFT         GetBufferSamplesSOFT;
    LPALISBUFFERFORMATSUPPORTEDSOFT  IsBufferFormatSupportedSOFT;
    /* AL_SOFT_source_latency */
    LPALSOURCEDSOFT                  SourcedSOFT;
    LPALSOURCE3DSOFT                 Source3dSOFT;
    LPALSOURCEDVSOFT                 SourcedvSOFT;
    LPALGETSOURCEDSOFT               GetSourcedSOFT;
    LPALGETSOURCE3DSOFT              GetSource3dSOFT;
    LPALGETSOURCEDVSOFT              GetSourcedvSOFT;
    LPALSOURCEI64SOFT                Sourcei64SOFT;
    LPALSOURCE3I64SOFT               Source3i64SOFT;
    LPALSOURCEI64VSOFT               Sourcei64vSOFT;
    LPALGETSOURCEI64SOFT             GetSourcei64SOFT;
    LPALGETSOURCE3I64SOFT            GetSource3i64SOFT;
    LPALGETSOURCEI64VSOFT            GetSourcei64vSOFT;
    /* AL_SOFT_deferred_updates */
    LPALDEFERUPDATESSOFT             DeferUpdatesSOFT;
    LPALPROCESSUPDATESSOFT           ProcessUpdatesSOFT;
    /* AL_SOFT_source_resampler */
    LPALGETSTRINGISOFT               GetStringiSOFT;
    /* AL_SOFT_events */
    LPALEVENTCONTROLSOFT             EventControlSOFT;
    LPALEVENTCALLBACKSOFT            EventCallbackSOFT;
    LPALGETPOINTERSOFT               GetPointerSOFT;
    LPALGETPOINTERVSOFT              GetPointervSOFT;
    /* AL_SOFT_callback_buffer */
    LPALBUFFERCALLBACKSOFT           BufferCallbackSOFT;
    LPALGETBUFFERPTRSOFT             GetBufferPtrSOFT;
    LPALGETBUFFER3PTRSOFT            GetBuffer3PtrSOFT;
    LPALGETBUFFERPTRVSOFT            GetBufferPtrvSOFT;
    /* AL_SOFT_source_start_delay */
    LPALSOURCEPLAYATTIMESOFT         SourcePlayAtTimeSOFT;
    LPALSOURCEPLAYATTIMEVSOFT        SourcePlayAtTimevSOFT;
    /* AL_EXT_debug */
    LPALDEBUGMESSAGECALLBACKEXT      DebugMessageCallbackEXT;
    LPALDEBUGMESSAGEINSERTEXT        DebugMessageInsertEXT;
    LPALDEBUGMESSAGECONTROLEXT       DebugMessageControlEXT;
    LPALPUSHDEBUGGROUPEXT            PushDebugGroupEXT;
    LPALPOPDEBUGGROUPEXT             PopDebugGroupEXT;
    LPALGETDEBUGMESSAGELOGEXT        GetDebugMessageLogEXT;
    LPALOBJECTLABELEXT               ObjectLabelEXT;
    LPALGETOBJECTLABELEXT            GetObjectLabelEXT;
    LPALGETPOINTEREXT                GetPointerEXT;
    LPALGETPOINTERVEXT               GetPointervEXT;
} aladALFunctions;
typedef struct aladALCFunctions {
    /* Function Loader */
    LPALCGETPROCADDRESS              GetProcAddress;
    /* Rest of Core ALC */
    LPALCCREATECONTEXT               CreateContext;
    LPALCMAKECONTEXTCURRENT          MakeContextCurrent;
    LPALCPROCESSCONTEXT              ProcessContext;
    LPALCSUSPENDCONTEXT              SuspendContext;
    LPALCDESTROYCONTEXT              DestroyContext;
    LPALCGETCURRENTCONTEXT           GetCurrentContext;
    LPALCGETCONTEXTSDEVICE           GetContextsDevice;
    LPALCOPENDEVICE                  OpenDevice;
    LPALCCLOSEDEVICE                 CloseDevice;
    LPALCGETERROR                    GetError;
    LPALCISEXTENSIONPRESENT          IsExtensionPresent;
    LPALCGETENUMVALUE                GetEnumValue;
    LPALCGETSTRING                   GetString;
    LPALCGETINTEGERV                 GetIntegerv;
    LPALCCAPTUREOPENDEVICE           CaptureOpenDevice;
    LPALCCAPTURECLOSEDEVICE          CaptureCloseDevice;
    LPALCCAPTURESTART                CaptureStart;
    LPALCCAPTURESTOP                 CaptureStop;
    LPALCCAPTURESAMPLES              CaptureSamples;
    /* ALC extensions */
    /* ALC_EXT_thread_local_context */
    PFNALCSETTHREADCONTEXTPROC       SetThreadContext;
    PFNALCGETTHREADCONTEXTPROC       GetThreadContext;
    /* ALC_SOFT_loopback */
    LPALCLOOPBACKOPENDEVICESOFT      LoopbackOpenDeviceSOFT;
    LPALCISRENDERFORMATSUPPORTEDSOFT IsRenderFormatSupportedSOFT;
    LPALCRENDERSAMPLESSOFT           RenderSamplesSOFT;
    /* ALC_SOFT_pause_device */
    LPALCDEVICEPAUSESOFT             DevicePauseSOFT;
    LPALCDEVICERESUMESOFT            DeviceResumeSOFT;
    /* ALC_SOFT_HRTF */
    LPALCGETSTRINGISOFT              GetStringiSOFT;
    LPALCRESETDEVICESOFT             ResetDeviceSOFT;
    /* ALC_SOFT_device_clock */
    LPALCGETINTEGER64VSOFT           GetInteger64vSOFT;
    /* ALC_SOFT_reopen_device */
    LPALCREOPENDEVICESOFT            ReopenDeviceSOFT;
    /* ALC_SOFT_system_events */
    LPALCEVENTISSUPPORTEDSOFT        EventIsSupportedSOFT;
    LPALCEVENTCONTROLSOFT            EventControlSOFT;
    LPALCEVENTCALLBACKSOFT           EventCallbackSOFT;
} aladALCFunctions;
typedef struct aladDirectFunctions {
    /* AL_EXT_direct_context */
    LPALCGETPROCADDRESS2                    alcGetProcAddress2;
    LPALENABLEDIRECT                        alEnableDirect;
    LPALDISABLEDIRECT                       alDisableDirect;
    LPALISENABLEDDIRECT                     alIsEnabledDirect;
    LPALDOPPLERFACTORDIRECT                 alDopplerFactorDirect;
    LPALSPEEDOFSOUNDDIRECT                  alSpeedOfSoundDirect;
    LPALDISTANCEMODELDIRECT                 alDistanceModelDirect;
    LPALGETSTRINGDIRECT                     alGetStringDirect;
    LPALGETBOOLEANVDIRECT                   alGetBooleanvDirect;
    LPALGETINTEGERVDIRECT                   alGetIntegervDirect;
    LPALGETFLOATVDIRECT                     alGetFloatvDirect;
    LPALGETDOUBLEVDIRECT                    alGetDoublevDirect;
    LPALGETBOOLEANDIRECT                    alGetBooleanDirect;
    LPALGETINTEGERDIRECT                    alGetIntegerDirect;
    LPALGETFLOATDIRECT                      alGetFloatDirect;
    LPALGETDOUBLEDIRECT                     alGetDoubleDirect;
    LPALGETERRORDIRECT                      alGetErrorDirect;
    LPALISEXTENSIONPRESENTDIRECT            alIsExtensionPresentDirect;
    LPALGETPROCADDRESSDIRECT                alGetProcAddressDirect;
    LPALGETENUMVALUEDIRECT                  alGetEnumValueDirect;
    LPALLISTENERFDIRECT                     alListenerfDirect;
    LPALLISTENER3FDIRECT                    alListener3fDirect;
    LPALLISTENERFVDIRECT                    alListenerfvDirect;
    LPALLISTENERIDIRECT                     alListeneriDirect;
    LPALLISTENER3IDIRECT                    alListener3iDirect;
    LPALLISTENERIVDIRECT                    alListenerivDirect;
    LPALGETLISTENERFDIRECT                  alGetListenerfDirect;
    LPALGETLISTENER3FDIRECT                 alGetListener3fDirect;
    LPALGETLISTENERFVDIRECT                 alGetListenerfvDirect;
    LPALGETLISTENERIDIRECT                  alGetListeneriDirect;
    LPALGETLISTENER3IDIRECT                 alGetListener3iDirect;
    LPALGETLISTENERIVDIRECT                 alGetListenerivDirect;
    LPALGENSOURCESDIRECT                    alGenSourcesDirect;
    LPALDELETESOURCESDIRECT                 alDeleteSourcesDirect;
    LPALISSOURCEDIRECT                      alIsSourceDirect;
    LPALSOURCEFDIRECT                       alSourcefDirect;
    LPALSOURCE3FDIRECT                      alSource3fDirect;
    LPALSOURCEFVDIRECT                      alSourcefvDirect;
    LPALSOURCEIDIRECT                       alSourceiDirect;
    LPALSOURCE3IDIRECT                      alSource3iDirect;
    LPALSOURCEIVDIRECT                      alSourceivDirect;
    LPALGETSOURCEFDIRECT                    alGetSourcefDirect;
    LPALGETSOURCE3FDIRECT                   alGetSource3fDirect;
    LPALGETSOURCEFVDIRECT                   alGetSourcefvDirect;
    LPALGETSOURCEIDIRECT                    alGetSourceiDirect;
    LPALGETSOURCE3IDIRECT                   alGetSource3iDirect;
    LPALGETSOURCEIVDIRECT                   alGetSourceivDirect;
    LPALSOURCEPLAYVDIRECT                   alSourcePlayDirect;
    LPALSOURCESTOPVDIRECT                   alSourceStopDirect;
    LPALSOURCEREWINDVDIRECT                 alSourceRewindvDirect;
    LPALSOURCEPAUSEVDIRECT                  alSourcePausevDirect;
    LPALSOURCEPLAYDIRECT                    alSourcePlayvDirect;
    LPALSOURCESTOPDIRECT                    alSourceStopvDirect;
    LPALSOURCEREWINDDIRECT                  alSourceRewindDirect;
    LPALSOURCEPAUSEDIRECT                   alSourcePauseDirect;
    LPALSOURCEQUEUEBUFFERSDIRECT            alSourceQueueBuffersDirect;
    LPALSOURCEUNQUEUEBUFFERSDIRECT          alSourceUnqueueBuffersDirect;
    LPALGENBUFFERSDIRECT                    alGenBuffersDirect;
    LPALDELETEBUFFERSDIRECT                 alDeleteBuffersDirect;
    LPALISBUFFERDIRECT                      alIsBufferDirect;
    LPALBUFFERDATADIRECT                    alBufferDataDirect;
    LPALBUFFERFDIRECT                       alBufferfDirect;
    LPALBUFFER3FDIRECT                      alBuffer3fDirect;
    LPALBUFFERFVDIRECT                      alBufferfvDirect;
    LPALBUFFERIDIRECT                       alBufferiDirect;
    LPALBUFFER3IDIRECT                      alBuffer3iDirect;
    LPALBUFFERIVDIRECT                      alBufferivDirect;
    LPALGETBUFFERFDIRECT                    alGetBufferfDirect;
    LPALGETBUFFER3FDIRECT                   alGetBuffer3fDirect;
    LPALGETBUFFERFVDIRECT                   alGetBufferfvDirect;
    LPALGETBUFFERIDIRECT                    alGetBufferiDirect;
    LPALGETBUFFER3IDIRECT                   alGetBuffer3iDirect;
    LPALGETBUFFERIVDIRECT                   alGetBufferivDirect;
        /* ALC_EXT_EFX */   
    LPALGENEFFECTSDIRECT                    alGenEffectsDirect;
    LPALDELETEEFFECTSDIRECT                 alDeleteEffectsDirect;
    LPALISEFFECTDIRECT                      alIsEffectDirect;
    LPALEFFECTIDIRECT                       alEffectiDirect;
    LPALEFFECTIVDIRECT                      alEffectivDirect;
    LPALEFFECTFDIRECT                       alEffectfDirect;
    LPALEFFECTFVDIRECT                      alEffectfvDirect;
    LPALGETEFFECTIDIRECT                    alGetEffectiDirect;
    LPALGETEFFECTIVDIRECT                   alGetEffectivDirect;
    LPALGETEFFECTFDIRECT                    alGetEffectfDirect;
    LPALGETEFFECTFVDIRECT                   alGetEffectfvDirect;
    LPALGENFILTERSDIRECT                    alGenFiltersDirect;
    LPALDELETEFILTERSDIRECT                 alDeleteFiltersDirect;
    LPALISFILTERDIRECT                      alIsFilterDirect;
    LPALFILTERIDIRECT                       alFilteriDirect;
    LPALFILTERIVDIRECT                      alFilterivDirect;
    LPALFILTERFDIRECT                       alFilterfDirect;
    LPALFILTERFVDIRECT                      alFilterfvDirect;
    LPALGETFILTERIDIRECT                    alGetFilteriDirect;
    LPALGETFILTERIVDIRECT                   alGetFilterivDirect;
    LPALGETFILTERFDIRECT                    alGetFilterfDirect;
    LPALGETFILTERFVDIRECT                   alGetFilterfvDirect;
    LPALGENAUXILIARYEFFECTSLOTSDIRECT       alGenAuxiliaryEffectSlotsDirect;
    LPALDELETEAUXILIARYEFFECTSLOTSDIRECT    alDeleteAuxiliaryEffectSlotsDirect;
    LPALISAUXILIARYEFFECTSLOTDIRECT         alIsAuxiliaryEffectSlotDirect;
    LPALAUXILIARYEFFECTSLOTIDIRECT          alAuxiliaryEffectSlotiDirect;
    LPALAUXILIARYEFFECTSLOTIVDIRECT         alAuxiliaryEffectSlotivDirect;
    LPALAUXILIARYEFFECTSLOTFDIRECT          alAuxiliaryEffectSlotfDirect;
    LPALAUXILIARYEFFECTSLOTFVDIRECT         alAuxiliaryEffectSlotfvDirect;
    LPALGETAUXILIARYEFFECTSLOTIDIRECT       alGetAuxiliaryEffectSlotiDirect;
    LPALGETAUXILIARYEFFECTSLOTIVDIRECT      alGetAuxiliaryEffectSlotivDirect;
    LPALGETAUXILIARYEFFECTSLOTFDIRECT       alGetAuxiliaryEffectSlotfDirect;
    LPALGETAUXILIARYEFFECTSLOTFVDIRECT      alGetAuxiliaryEffectSlotfvDirect;
        /* AL_EXT_BUFFER_DATA_STATIC */
    LPALBUFFERDATASTATICDIRECT              alBufferDataStaticDirect;
        /* AL_EXT_debug */
    LPALDEBUGMESSAGECALLBACKDIRECTEXT       alDebugMessageCallbackDirectEXT;
    LPALDEBUGMESSAGEINSERTDIRECTEXT         alDebugMessageInsertDirectEXT;
    LPALDEBUGMESSAGECONTROLDIRECTEXT        alDebugMessageControlDirectEXT;
    LPALPUSHDEBUGGROUPDIRECTEXT             alPushDebugGroupDirectEXT;
    LPALPOPDEBUGGROUPDIRECTEXT              alPopDebugGroupDirectEXT;
    LPALGETDEBUGMESSAGELOGDIRECTEXT         alGetDebugMessageLogDirectEXT;
    LPALOBJECTLABELDIRECTEXT                alObjectLabelDirectEXT;
    LPALGETOBJECTLABELDIRECTEXT             alGetObjectLabelDirectEXT;
    LPALGETPOINTERDIRECTEXT                 alGetPointerDirectEXT;
    LPALGETPOINTERVDIRECTEXT                alGetPointervDirectEXT;
        /* AL_EXT_FOLDBACK */
    LPALREQUESTFOLDBACKSTARTDIRECT          alRequestFoldbackStartDirect;
    LPALREQUESTFOLDBACKSTOPDIRECT           alRequestFoldbackStopDirect;
        /* AL_SOFT_buffer_sub_data */
    LPALBUFFERSUBDATADIRECTSOFT             alBufferSubDataDirectSOFT;
        /* AL_SOFT_source_latency */
    LPALSOURCEDDIRECTSOFT                   alSourcedDirectSOFT;
    LPALSOURCE3DDIRECTSOFT                  alSource3dDirectSOFT;
    LPALSOURCEDVDIRECTSOFT                  alSourcedvDirectSOFT;
    LPALGETSOURCEDDIRECTSOFT                alGetSourcedDirectSOFT;
    LPALGETSOURCE3DDIRECTSOFT               alGetSource3dDirectSOFT;
    LPALGETSOURCEDVDIRECTSOFT               alGetSourcedvDirectSOFT;
    LPALSOURCEI64DIRECTSOFT                 alSourcei64DirectSOFT;
    LPALSOURCE3I64DIRECTSOFT                alSource3i64DirectSOFT;
    LPALSOURCEI64VDIRECTSOFT                alSourcei64vDirectSOFT;
    LPALGETSOURCEI64DIRECTSOFT              alGetSourcei64DirectSOFT;
    LPALGETSOURCE3I64DIRECTSOFT             alGetSource3i64DirectSOFT;
    LPALGETSOURCEI64VDIRECTSOFT             alGetSourcei64vDirectSOFT;
        /* AL_SOFT_deferred_updates */
    LPALDEFERUPDATESDIRECTSOFT              alDeferUpdatesDirectSOFT;
    LPALPROCESSUPDATESDIRECTSOFT            alProcessUpdatesDirectSOFT;
        /* AL_SOFT_source_resampler */
    LPALGETSTRINGIDIRECTSOFT                alGetStringiDirectSOFT;
        /* AL_SOFT_events */
    LPALEVENTCONTROLDIRECTSOFT              alEventControlDirectSOFT;
    LPALEVENTCALLBACKDIRECTSOFT             alEventCallbackDirectSOFT;
    LPALGETPOINTERDIRECTSOFT                alGetPointerDirectSOFT;
    LPALGETPOINTERVDIRECTSOFT               alGetPointervDirectSOFT;
        /* AL_SOFT_callback_buffer */
    LPALBUFFERCALLBACKDIRECTSOFT            alBufferCallbackDirectSOFT;
    LPALGETBUFFERPTRDIRECTSOFT              alGetBufferPtrDirectSOFT;
    LPALGETBUFFER3PTRDIRECTSOFT             alGetBuffer3PtrDirectSOFT;
    LPALGETBUFFERPTRVDIRECTSOFT             alGetBufferPtrvDirectSOFT;
        /* AL_SOFT_source_start_delay */
    LPALSOURCEPLAYATTIMEDIRECTSOFT          alSourcePlayAtTimeDirectSOFT;
    LPALSOURCEPLAYATTIMEVDIRECTSOFT         alSourcePlayAtTimevDirectSOFT;
        /* EAX */
    LPEAXSETDIRECT                          EAXSetDirect;
    LPEAXGETDIRECT                          EAXGetDirect;
    LPEAXSETBUFFERMODEDIRECT                EAXSetBufferModeDirect;
    LPEAXGETBUFFERMODEDIRECT                EAXGetBufferModeDirect;
} aladDirectFunctions;

extern void aladLoadALCoreMinimal(aladALFunctions* functions, aladLoader loader);
extern void aladLoadALCoreRest(aladALFunctions* functions, aladLoader loader);
extern void aladLoadEFX(aladALFunctions* functions, aladLoader loader);
extern void aladLoadALExtensions(aladALFunctions* functions, aladLoader loader);
extern void aladLoadALCCore(aladALCFunctions* functions, aladLoader loader);
extern void aladLoadALCExtensions(aladALCFunctions* functions, aladLoader loader);
extern void aladLoadDirectExtension(aladDirectFunctions* functions, aladLoader loader);

/* global function pointers used by the other interfaces */
extern aladALFunctions aladAL;
extern aladALCFunctions aladALC;



#ifdef ALAD_IMPLEMENTATION

aladALFunctions aladAL = {0};
aladALCFunctions aladALC = {0};

/* new manual interface */

void aladLoadALCoreMinimal(aladALFunctions* functions, aladLoader loader) {
    functions[0].Enable             = REINTERPRET_CAST(LPALENABLE,              loader("alEnable"));
    functions[0].Disable            = REINTERPRET_CAST(LPALDISABLE,             loader("alDisable"));
    functions[0].IsEnabled          = REINTERPRET_CAST(LPALISENABLED,           loader("alIsEnabled"));
    functions[0].GetString          = REINTERPRET_CAST(LPALGETSTRING,           loader("alGetString"));
    functions[0].GetBooleanv        = REINTERPRET_CAST(LPALGETBOOLEANV,         loader("alGetBooleanv"));
    functions[0].GetIntegerv        = REINTERPRET_CAST(LPALGETINTEGERV,         loader("alGetIntegerv"));
    functions[0].GetFloatv          = REINTERPRET_CAST(LPALGETFLOATV,           loader("alGetFloatv"));
    functions[0].GetDoublev         = REINTERPRET_CAST(LPALGETDOUBLEV,          loader("alGetDoublev"));
    functions[0].GetBoolean         = REINTERPRET_CAST(LPALGETBOOLEAN,          loader("alGetBoolean"));
    functions[0].GetInteger         = REINTERPRET_CAST(LPALGETINTEGER,          loader("alGetInteger"));
    functions[0].GetFloat           = REINTERPRET_CAST(LPALGETFLOAT,            loader("alGetFloat"));
    functions[0].GetDouble          = REINTERPRET_CAST(LPALGETDOUBLE,           loader("alGetDouble"));
    functions[0].GetError           = REINTERPRET_CAST(LPALGETERROR,            loader("alGetError"));
    functions[0].IsExtensionPresent = REINTERPRET_CAST(LPALISEXTENSIONPRESENT,  loader("alIsExtensionPresent"));
    functions[0].GetEnumValue       = REINTERPRET_CAST(LPALGETENUMVALUE,        loader("alGetEnumValue"));        
}
void aladLoadALCoreRest(aladALFunctions* functions, aladLoader loader) {
    functions[0].DopplerFactor        = REINTERPRET_CAST(LPALDOPPLERFACTOR,         loader("alDopplerFactor"));
    functions[0].DopplerVelocity      = REINTERPRET_CAST(LPALDOPPLERVELOCITY,       loader("alDopplerVelocity"));
    functions[0].SpeedOfSound         = REINTERPRET_CAST(LPALSPEEDOFSOUND,          loader("alSpeedOfSound"));
    functions[0].DistanceModel        = REINTERPRET_CAST(LPALDISTANCEMODEL,         loader("alDistanceModel"));
    functions[0].Listenerf            = REINTERPRET_CAST(LPALLISTENERF,             loader("alListenerf"));
    functions[0].Listener3f           = REINTERPRET_CAST(LPALLISTENER3F,            loader("alListener3f"));
    functions[0].Listenerfv           = REINTERPRET_CAST(LPALLISTENERFV,            loader("alListenerfv"));
    functions[0].Listeneri            = REINTERPRET_CAST(LPALLISTENERI,             loader("alListeneri"));
    functions[0].Listener3i           = REINTERPRET_CAST(LPALLISTENER3I,            loader("alListener3i"));
    functions[0].Listeneriv           = REINTERPRET_CAST(LPALLISTENERIV,            loader("alListeneriv"));
    functions[0].GetListenerf         = REINTERPRET_CAST(LPALGETLISTENERF,          loader("alGetListenerf"));
    functions[0].GetListener3f        = REINTERPRET_CAST(LPALGETLISTENER3F,         loader("alGetListener3f"));
    functions[0].GetListenerfv        = REINTERPRET_CAST(LPALGETLISTENERFV,         loader("alGetListenerfv"));
    functions[0].GetListeneri         = REINTERPRET_CAST(LPALGETLISTENERI,          loader("alGetListeneri"));
    functions[0].GetListener3i        = REINTERPRET_CAST(LPALGETLISTENER3I,         loader("alGetListener3i"));
    functions[0].GetListeneriv        = REINTERPRET_CAST(LPALGETLISTENERIV,         loader("alGetListeneriv"));
    functions[0].GenSources           = REINTERPRET_CAST(LPALGENSOURCES,            loader("alGenSources"));
    functions[0].DeleteSources        = REINTERPRET_CAST(LPALDELETESOURCES,         loader("alDeleteSources"));
    functions[0].IsSource             = REINTERPRET_CAST(LPALISSOURCE,              loader("alIsSource"));
    functions[0].Sourcef              = REINTERPRET_CAST(LPALSOURCEF,               loader("alSourcef"));
    functions[0].Source3f             = REINTERPRET_CAST(LPALSOURCE3F,              loader("alSource3f"));
    functions[0].Sourcefv             = REINTERPRET_CAST(LPALSOURCEFV,              loader("alSourcefv"));
    functions[0].Sourcei              = REINTERPRET_CAST(LPALSOURCEI,               loader("alSourcei"));
    functions[0].Source3i             = REINTERPRET_CAST(LPALSOURCE3I,              loader("alSource3i"));
    functions[0].Sourceiv             = REINTERPRET_CAST(LPALSOURCEIV,              loader("alSourceiv"));
    functions[0].GetSourcef           = REINTERPRET_CAST(LPALGETSOURCEF,            loader("alGetSourcef"));
    functions[0].GetSource3f          = REINTERPRET_CAST(LPALGETSOURCE3F,           loader("alGetSource3f"));
    functions[0].GetSourcefv          = REINTERPRET_CAST(LPALGETSOURCEFV,           loader("alGetSourcefv"));
    functions[0].GetSourcei           = REINTERPRET_CAST(LPALGETSOURCEI,            loader("alGetSourcei"));
    functions[0].GetSource3i          = REINTERPRET_CAST(LPALGETSOURCE3I,           loader("alGetSource3i"));
    functions[0].GetSourceiv          = REINTERPRET_CAST(LPALGETSOURCEIV,           loader("alGetSourceiv"));
    functions[0].SourcePlayv          = REINTERPRET_CAST(LPALSOURCEPLAYV,           loader("alSourcePlayv"));
    functions[0].SourceStopv          = REINTERPRET_CAST(LPALSOURCESTOPV,           loader("alSourceStopv"));
    functions[0].SourceRewindv        = REINTERPRET_CAST(LPALSOURCEREWINDV,         loader("alSourceRewindv"));
    functions[0].SourcePausev         = REINTERPRET_CAST(LPALSOURCEPAUSEV,          loader("alSourcePausev"));
    functions[0].SourcePlay           = REINTERPRET_CAST(LPALSOURCEPLAY,            loader("alSourcePlay"));
    functions[0].SourceStop           = REINTERPRET_CAST(LPALSOURCESTOP,            loader("alSourceStop"));
    functions[0].SourceRewind         = REINTERPRET_CAST(LPALSOURCEREWIND,          loader("alSourceRewind"));
    functions[0].SourcePause          = REINTERPRET_CAST(LPALSOURCEPAUSE,           loader("alSourcePause"));
    functions[0].SourceQueueBuffers   = REINTERPRET_CAST(LPALSOURCEQUEUEBUFFERS,    loader("alSourceQueueBuffers"));
    functions[0].SourceUnqueueBuffers = REINTERPRET_CAST(LPALSOURCEUNQUEUEBUFFERS,  loader("alSourceUnqueueBuffers"));
    functions[0].GenBuffers           = REINTERPRET_CAST(LPALGENBUFFERS,            loader("alGenBuffers"));
    functions[0].DeleteBuffers        = REINTERPRET_CAST(LPALDELETEBUFFERS,         loader("alDeleteBuffers"));
    functions[0].IsBuffer             = REINTERPRET_CAST(LPALISBUFFER,              loader("alIsBuffer"));
    functions[0].BufferData           = REINTERPRET_CAST(LPALBUFFERDATA,            loader("alBufferData"));
    functions[0].Bufferf              = REINTERPRET_CAST(LPALBUFFERF,               loader("alBufferf"));
    functions[0].Buffer3f             = REINTERPRET_CAST(LPALBUFFER3F,              loader("alBuffer3f"));
    functions[0].Bufferfv             = REINTERPRET_CAST(LPALBUFFERFV,              loader("alBufferfv"));
    functions[0].Bufferi              = REINTERPRET_CAST(LPALBUFFERI,               loader("alBufferi"));
    functions[0].Buffer3i             = REINTERPRET_CAST(LPALBUFFER3I,              loader("alBuffer3i"));
    functions[0].Bufferiv             = REINTERPRET_CAST(LPALBUFFERIV,              loader("alBufferiv"));
    functions[0].GetBufferf           = REINTERPRET_CAST(LPALGETBUFFERF,            loader("alGetBufferf"));
    functions[0].GetBuffer3f          = REINTERPRET_CAST(LPALGETBUFFER3F,           loader("alGetBuffer3f"));
    functions[0].GetBufferfv          = REINTERPRET_CAST(LPALGETBUFFERFV,           loader("alGetBufferfv"));
    functions[0].GetBufferi           = REINTERPRET_CAST(LPALGETBUFFERI,            loader("alGetBufferi"));
    functions[0].GetBuffer3i          = REINTERPRET_CAST(LPALGETBUFFER3I,           loader("alGetBuffer3i"));
    functions[0].GetBufferiv          = REINTERPRET_CAST(LPALGETBUFFERIV,           loader("alGetBufferiv"));
}
void aladLoadEFX(aladALFunctions* functions, aladLoader loader) {
    functions[0].GenEffects                  = REINTERPRET_CAST(LPALGENEFFECTS,                     loader("alGenEffects"));
    functions[0].DeleteEffects               = REINTERPRET_CAST(LPALDELETEEFFECTS,                  loader("alDeleteEffects"));
    functions[0].IsEffect                    = REINTERPRET_CAST(LPALISEFFECT,                       loader("alIsEffect"));
    functions[0].Effecti                     = REINTERPRET_CAST(LPALEFFECTI,                        loader("alEffecti"));
    functions[0].Effectiv                    = REINTERPRET_CAST(LPALEFFECTIV,                       loader("alEffectiv"));
    functions[0].Effectf                     = REINTERPRET_CAST(LPALEFFECTF,                        loader("alEffectf"));
    functions[0].Effectfv                    = REINTERPRET_CAST(LPALEFFECTFV,                       loader("alEffectfv"));
    functions[0].GetEffecti                  = REINTERPRET_CAST(LPALGETEFFECTI,                     loader("alGetEffecti"));
    functions[0].GetEffectiv                 = REINTERPRET_CAST(LPALGETEFFECTIV,                    loader("alGetEffectiv"));
    functions[0].GetEffectf                  = REINTERPRET_CAST(LPALGETEFFECTF,                     loader("alGetEffectf"));
    functions[0].GetEffectfv                 = REINTERPRET_CAST(LPALGETEFFECTFV,                    loader("alGetEffectfv"));
    functions[0].GenFilters                  = REINTERPRET_CAST(LPALGENFILTERS,                     loader("alGenFilters"));
    functions[0].DeleteFilters               = REINTERPRET_CAST(LPALDELETEFILTERS,                  loader("alDeleteFilters"));
    functions[0].IsFilter                    = REINTERPRET_CAST(LPALISFILTER,                       loader("alIsFilter"));
    functions[0].Filteri                     = REINTERPRET_CAST(LPALFILTERI,                        loader("alFilteri"));
    functions[0].Filteriv                    = REINTERPRET_CAST(LPALFILTERIV,                       loader("alFilteriv"));
    functions[0].Filterf                     = REINTERPRET_CAST(LPALFILTERF,                        loader("alFilterf"));
    functions[0].Filterfv                    = REINTERPRET_CAST(LPALFILTERFV,                       loader("alFilterfv"));
    functions[0].GetFilteri                  = REINTERPRET_CAST(LPALGETFILTERI,                     loader("alGetFilteri"));
    functions[0].GetFilteriv                 = REINTERPRET_CAST(LPALGETFILTERIV,                    loader("alGetFilteriv"));
    functions[0].GetFilterf                  = REINTERPRET_CAST(LPALGETFILTERF,                     loader("alGetFilterf"));
    functions[0].GetFilterfv                 = REINTERPRET_CAST(LPALGETFILTERFV,                    loader("alGetFilterfv"));
    functions[0].GenAuxiliaryEffectSlots     = REINTERPRET_CAST(LPALGENAUXILIARYEFFECTSLOTS,        loader("alGenAuxiliaryEffectSlots"));
    functions[0].DeleteAuxiliaryEffectSlots  = REINTERPRET_CAST(LPALDELETEAUXILIARYEFFECTSLOTS,     loader("alDeleteAuxiliaryEffectSlots"));
    functions[0].IsAuxiliaryEffectSlot       = REINTERPRET_CAST(LPALISAUXILIARYEFFECTSLOT,          loader("alIsAuxiliaryEffectSlot"));
    functions[0].AuxiliaryEffectSloti        = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTI,           loader("alAuxiliaryEffectSloti"));
    functions[0].AuxiliaryEffectSlotiv       = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTIV,          loader("alAuxiliaryEffectSlotiv"));
    functions[0].AuxiliaryEffectSlotf        = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTF,           loader("alAuxiliaryEffectSlotf"));
    functions[0].AuxiliaryEffectSlotfv       = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTFV,          loader("alAuxiliaryEffectSlotfv"));
    functions[0].GetAuxiliaryEffectSloti     = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTI,        loader("alGetAuxiliaryEffectSloti"));
    functions[0].GetAuxiliaryEffectSlotiv    = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTIV,       loader("alGetAuxiliaryEffectSlotiv"));
    functions[0].GetAuxiliaryEffectSlotf     = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTF,        loader("alGetAuxiliaryEffectSlotf"));
    functions[0].GetAuxiliaryEffectSlotfv    = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTFV,       loader("alGetAuxiliaryEffectSlotfv"));
}
void aladLoadALExtensions(aladALFunctions* functions, aladLoader loader) {
    /* AL_EXT_STATIC_BUFFER */
    functions[0].BufferDataStatic            = REINTERPRET_CAST(PFNALBUFFERDATASTATICPROC,          loader("alBufferDataStatic"));
    /* AL_SOFT_buffer_sub_data */
    functions[0].BufferSubDataSOFT           = REINTERPRET_CAST(PFNALBUFFERSUBDATASOFTPROC,         loader("alBufferSubDataSOFT"));
    /* AL_EXT_FOLDBACK */
    functions[0].RequestFoldbackStart        = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTART,           loader("alRequestFoldbackStart"));
    functions[0].RequestFoldbackStop         = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTOP,            loader("alRequestFoldbackStop"));
    /* AL_SOFT_buffer_samples */
    functions[0].BufferSamplesSOFT           = REINTERPRET_CAST(LPALBUFFERSAMPLESSOFT,              loader("alBufferSamplesSOFT"));
    functions[0].BufferSubSamplesSOFT        = REINTERPRET_CAST(LPALBUFFERSUBSAMPLESSOFT,           loader("alBufferSubSamplesSOFT"));
    functions[0].GetBufferSamplesSOFT        = REINTERPRET_CAST(LPALGETBUFFERSAMPLESSOFT,           loader("alGetBufferSamplesSOFT"));
    functions[0].IsBufferFormatSupportedSOFT = REINTERPRET_CAST(LPALISBUFFERFORMATSUPPORTEDSOFT,    loader("alIsBufferFormatSupportedSOFT"));
    /* AL_SOFT_source_latency */
    functions[0].SourcedSOFT                 = REINTERPRET_CAST(LPALSOURCEDSOFT,                    loader("alSourcedSOFT"));
    functions[0].Source3dSOFT                = REINTERPRET_CAST(LPALSOURCE3DSOFT,                   loader("alSource3dSOFT"));
    functions[0].SourcedvSOFT                = REINTERPRET_CAST(LPALSOURCEDVSOFT,                   loader("alSourcedvSOFT"));
    functions[0].GetSourcedSOFT              = REINTERPRET_CAST(LPALGETSOURCEDSOFT,                 loader("alGetSourcedSOFT"));
    functions[0].GetSource3dSOFT             = REINTERPRET_CAST(LPALGETSOURCE3DSOFT,                loader("alGetSource3dSOFT"));
    functions[0].GetSourcedvSOFT             = REINTERPRET_CAST(LPALGETSOURCEDVSOFT,                loader("alGetSourcedvSOFT"));
    functions[0].Sourcei64SOFT               = REINTERPRET_CAST(LPALSOURCEI64SOFT,                  loader("alSourcei64SOFT"));
    functions[0].Source3i64SOFT              = REINTERPRET_CAST(LPALSOURCE3I64SOFT,                 loader("alSource3i64SOFT"));
    functions[0].Sourcei64vSOFT              = REINTERPRET_CAST(LPALSOURCEI64VSOFT,                 loader("alSourcei64vSOFT"));
    functions[0].GetSourcei64SOFT            = REINTERPRET_CAST(LPALGETSOURCEI64SOFT,               loader("alGetSourcei64SOFT"));
    functions[0].GetSource3i64SOFT           = REINTERPRET_CAST(LPALGETSOURCE3I64SOFT,              loader("alGetSource3i64SOFT"));
    functions[0].GetSourcei64vSOFT           = REINTERPRET_CAST(LPALGETSOURCEI64VSOFT,              loader("alGetSourcei64vSOFT"));
    /* AL_SOFT_deferred_updates */
    functions[0].DeferUpdatesSOFT            = REINTERPRET_CAST(LPALDEFERUPDATESSOFT,               loader("alDeferUpdatesSOFT"));
    functions[0].ProcessUpdatesSOFT          = REINTERPRET_CAST(LPALPROCESSUPDATESSOFT,             loader("alProcessUpdatesSOFT"));
    /* AL_SOFT_source_resampler */
    functions[0].GetStringiSOFT              = REINTERPRET_CAST(LPALGETSTRINGISOFT,                 loader("alGetStringiSOFT"));
    /* AL_SOFT_events */
    functions[0].EventControlSOFT            = REINTERPRET_CAST(LPALEVENTCONTROLSOFT,               loader("alEventControlSOFT"));
    functions[0].EventCallbackSOFT           = REINTERPRET_CAST(LPALEVENTCALLBACKSOFT,              loader("alEventCallbackSOFT"));
    functions[0].GetPointerSOFT              = REINTERPRET_CAST(LPALGETPOINTERSOFT,                 loader("alGetPointerSOFT"));
    functions[0].GetPointervSOFT             = REINTERPRET_CAST(LPALGETPOINTERVSOFT,                loader("alGetPointervSOFT"));
    /* AL_SOFT_callback_buffer */
    functions[0].BufferCallbackSOFT          = REINTERPRET_CAST(LPALBUFFERCALLBACKSOFT,             loader("alBufferCallbackSOFT"));
    functions[0].GetBufferPtrSOFT            = REINTERPRET_CAST(LPALGETBUFFERPTRSOFT,               loader("alGetBufferPtrSOFT"));
    functions[0].GetBuffer3PtrSOFT           = REINTERPRET_CAST(LPALGETBUFFER3PTRSOFT,              loader("alGetBuffer3PtrSOFT"));
    functions[0].GetBufferPtrvSOFT           = REINTERPRET_CAST(LPALGETBUFFERPTRVSOFT,              loader("alGetBufferPtrvSOFT"));
    /* AL_SOFT_source_start_delay */
    functions[0].SourcePlayAtTimeSOFT        = REINTERPRET_CAST(LPALSOURCEPLAYATTIMESOFT,           loader("alSourcePlayAtTimeSOFT"));
    functions[0].SourcePlayAtTimevSOFT       = REINTERPRET_CAST(LPALSOURCEPLAYATTIMEVSOFT,          loader("alSourcePlayAtTimevSOFT"));
    /* AL_EXT_debug */
    functions[0].DebugMessageCallbackEXT     = REINTERPRET_CAST(LPALDEBUGMESSAGECALLBACKEXT,        loader("alDebugMessageCallbackEXT"));
    functions[0].DebugMessageInsertEXT       = REINTERPRET_CAST(LPALDEBUGMESSAGEINSERTEXT,          loader("alDebugMessageInsertEXT"));
    functions[0].DebugMessageControlEXT      = REINTERPRET_CAST(LPALDEBUGMESSAGECONTROLEXT,         loader("alDebugMessageControlEXT"));
    functions[0].PushDebugGroupEXT           = REINTERPRET_CAST(LPALPUSHDEBUGGROUPEXT,              loader("alPushDebugGroupEXT"));
    functions[0].PopDebugGroupEXT            = REINTERPRET_CAST(LPALPOPDEBUGGROUPEXT,               loader("alPopDebugGroupEXT"));
    functions[0].GetDebugMessageLogEXT       = REINTERPRET_CAST(LPALGETDEBUGMESSAGELOGEXT,          loader("alGetDebugMessageLogEXT"));
    functions[0].ObjectLabelEXT              = REINTERPRET_CAST(LPALOBJECTLABELEXT,                 loader("alObjectLabelEXT"));
    functions[0].GetObjectLabelEXT           = REINTERPRET_CAST(LPALGETOBJECTLABELEXT,              loader("alGetObjectLabelEXT"));
    functions[0].GetPointerEXT               = REINTERPRET_CAST(LPALGETPOINTEREXT,                  loader("alGetPointerEXT"));
    functions[0].GetPointervEXT              = REINTERPRET_CAST(LPALGETPOINTERVEXT,                 loader("alGetPointervEXT"));
}
void aladLoadALCCore(aladALCFunctions* functions, aladLoader loader) {
    functions[0].CreateContext      = REINTERPRET_CAST(LPALCCREATECONTEXT,      loader("alcCreateContext"));
    functions[0].MakeContextCurrent = REINTERPRET_CAST(LPALCMAKECONTEXTCURRENT, loader("alcMakeContextCurrent"));
    functions[0].ProcessContext     = REINTERPRET_CAST(LPALCPROCESSCONTEXT,     loader("alcProcessContext"));
    functions[0].SuspendContext     = REINTERPRET_CAST(LPALCSUSPENDCONTEXT,     loader("alcSuspendContext"));
    functions[0].DestroyContext     = REINTERPRET_CAST(LPALCDESTROYCONTEXT,     loader("alcDestroyContext"));
    functions[0].GetCurrentContext  = REINTERPRET_CAST(LPALCGETCURRENTCONTEXT,  loader("alcGetCurrentContext"));
    functions[0].GetContextsDevice  = REINTERPRET_CAST(LPALCGETCONTEXTSDEVICE,  loader("alcGetContextsDevice"));
    functions[0].OpenDevice         = REINTERPRET_CAST(LPALCOPENDEVICE,         loader("alcOpenDevice"));
    functions[0].CloseDevice        = REINTERPRET_CAST(LPALCCLOSEDEVICE,        loader("alcCloseDevice"));
    functions[0].GetError           = REINTERPRET_CAST(LPALCGETERROR,           loader("alcGetError"));
    functions[0].IsExtensionPresent = REINTERPRET_CAST(LPALCISEXTENSIONPRESENT, loader("alcIsExtensionPresent"));
    functions[0].GetEnumValue       = REINTERPRET_CAST(LPALCGETENUMVALUE,       loader("alcGetEnumValue"));
    functions[0].GetString          = REINTERPRET_CAST(LPALCGETSTRING,          loader("alcGetString"));
    functions[0].GetIntegerv        = REINTERPRET_CAST(LPALCGETINTEGERV,        loader("alcGetIntegerv"));
    functions[0].CaptureOpenDevice  = REINTERPRET_CAST(LPALCCAPTUREOPENDEVICE,  loader("alcCaptureOpenDevice"));
    functions[0].CaptureCloseDevice = REINTERPRET_CAST(LPALCCAPTURECLOSEDEVICE, loader("alcCaptureCloseDevice"));
    functions[0].CaptureStart       = REINTERPRET_CAST(LPALCCAPTURESTART,       loader("alcCaptureStart"));
    functions[0].CaptureStop        = REINTERPRET_CAST(LPALCCAPTURESTOP,        loader("alcCaptureStop"));
    functions[0].CaptureSamples     = REINTERPRET_CAST(LPALCCAPTURESAMPLES,     loader("alcCaptureSamples"));
}
void aladLoadALCExtensions(aladALCFunctions* functions, aladLoader loader) {
    /* ALC_EXT_thread_local_context */
    functions[0].SetThreadContext            = REINTERPRET_CAST(PFNALCSETTHREADCONTEXTPROC,         loader("alcSetThreadContext"));
    functions[0].GetThreadContext            = REINTERPRET_CAST(PFNALCGETTHREADCONTEXTPROC,         loader("alcGetThreadContext"));
    /* ALC_SOFT_loopback */
    functions[0].LoopbackOpenDeviceSOFT      = REINTERPRET_CAST(LPALCLOOPBACKOPENDEVICESOFT,        loader("alcLoopbackOpenDeviceSOFT"));
    functions[0].IsRenderFormatSupportedSOFT = REINTERPRET_CAST(LPALCISRENDERFORMATSUPPORTEDSOFT,   loader("alcIsRenderFormatSupportedSOFT"));
    functions[0].RenderSamplesSOFT           = REINTERPRET_CAST(LPALCRENDERSAMPLESSOFT,             loader("alcRenderSamplesSOFT"));
    /* ALC_SOFT_pause_device */
    functions[0].DevicePauseSOFT             = REINTERPRET_CAST(LPALCDEVICEPAUSESOFT,               loader("alcDevicePauseSOFT"));
    functions[0].DeviceResumeSOFT            = REINTERPRET_CAST(LPALCDEVICERESUMESOFT,              loader("alcDeviceResumeSOFT"));
    /* ALC_SOFT_HRTF */
    functions[0].GetStringiSOFT              = REINTERPRET_CAST(LPALCGETSTRINGISOFT,                loader("alcGetStringiSOFT"));
    functions[0].ResetDeviceSOFT             = REINTERPRET_CAST(LPALCRESETDEVICESOFT,               loader("alcResetDeviceSOFT"));
    /* ALC_SOFT_device_clock */
    functions[0].GetInteger64vSOFT           = REINTERPRET_CAST(LPALCGETINTEGER64VSOFT,             loader("alcGetInteger64vSOFT"));
    /* ALC_SOFT_reopen_device */
    functions[0].ReopenDeviceSOFT            = REINTERPRET_CAST(LPALCREOPENDEVICESOFT,              loader("alcReopenDeviceSOFT"));
    /* ALC_SOFT_system_events */
    functions[0].EventIsSupportedSOFT        = REINTERPRET_CAST(LPALCEVENTISSUPPORTEDSOFT,          loader("alcEventIsSupportedSOFT"));
    functions[0].EventControlSOFT            = REINTERPRET_CAST(LPALCEVENTCONTROLSOFT,              loader("alcEventControlSOFT"));
    functions[0].EventCallbackSOFT           = REINTERPRET_CAST(LPALCEVENTCALLBACKSOFT,             loader("alcEventCallbackSOFT"));
}
void aladLoadDirectExtension(aladDirectFunctions* functions, aladLoader loader) {
    /* AL_EXT_direct_context */
    functions[0].alcGetProcAddress2                      = REINTERPRET_CAST(LPALCGETPROCADDRESS2                 ,loader("alcGetProcAddress2"));
    functions[0].alEnableDirect                          = REINTERPRET_CAST(LPALENABLEDIRECT                     ,loader("alEnableDirect"));
    functions[0].alDisableDirect                         = REINTERPRET_CAST(LPALDISABLEDIRECT                    ,loader("alDisableDirect"));
    functions[0].alIsEnabledDirect                       = REINTERPRET_CAST(LPALISENABLEDDIRECT                  ,loader("alIsEnabledDirect"));
    functions[0].alDopplerFactorDirect                   = REINTERPRET_CAST(LPALDOPPLERFACTORDIRECT              ,loader("alDopplerFactorDirect"));
    functions[0].alSpeedOfSoundDirect                    = REINTERPRET_CAST(LPALSPEEDOFSOUNDDIRECT               ,loader("alSpeedOfSoundDirect"));
    functions[0].alDistanceModelDirect                   = REINTERPRET_CAST(LPALDISTANCEMODELDIRECT              ,loader("alDistanceModelDirect"));
    functions[0].alGetStringDirect                       = REINTERPRET_CAST(LPALGETSTRINGDIRECT                  ,loader("alGetStringDirect"));
    functions[0].alGetBooleanvDirect                     = REINTERPRET_CAST(LPALGETBOOLEANVDIRECT                ,loader("alGetBooleanvDirect"));
    functions[0].alGetIntegervDirect                     = REINTERPRET_CAST(LPALGETINTEGERVDIRECT                ,loader("alGetIntegervDirect"));
    functions[0].alGetFloatvDirect                       = REINTERPRET_CAST(LPALGETFLOATVDIRECT                  ,loader("alGetFloatvDirect"));
    functions[0].alGetDoublevDirect                      = REINTERPRET_CAST(LPALGETDOUBLEVDIRECT                 ,loader("alGetDoublevDirect"));
    functions[0].alGetBooleanDirect                      = REINTERPRET_CAST(LPALGETBOOLEANDIRECT                 ,loader("alGetBooleanDirect"));
    functions[0].alGetIntegerDirect                      = REINTERPRET_CAST(LPALGETINTEGERDIRECT                 ,loader("alGetIntegerDirect"));
    functions[0].alGetFloatDirect                        = REINTERPRET_CAST(LPALGETFLOATDIRECT                   ,loader("alGetFloatDirect"));
    functions[0].alGetDoubleDirect                       = REINTERPRET_CAST(LPALGETDOUBLEDIRECT                  ,loader("alGetDoubleDirect"));
    functions[0].alGetErrorDirect                        = REINTERPRET_CAST(LPALGETERRORDIRECT                   ,loader("alGetErrorDirect"));
    functions[0].alIsExtensionPresentDirect              = REINTERPRET_CAST(LPALISEXTENSIONPRESENTDIRECT         ,loader("alIsExtensionPresentDirect"));
    functions[0].alGetProcAddressDirect                  = REINTERPRET_CAST(LPALGETPROCADDRESSDIRECT             ,loader("alGetProcAddressDirect"));
    functions[0].alGetEnumValueDirect                    = REINTERPRET_CAST(LPALGETENUMVALUEDIRECT               ,loader("alGetEnumValueDirect"));
    functions[0].alListenerfDirect                       = REINTERPRET_CAST(LPALLISTENERFDIRECT                  ,loader("alListenerfDirect"));
    functions[0].alListener3fDirect                      = REINTERPRET_CAST(LPALLISTENER3FDIRECT                 ,loader("alListener3fDirect"));
    functions[0].alListenerfvDirect                      = REINTERPRET_CAST(LPALLISTENERFVDIRECT                 ,loader("alListenerfvDirect"));
    functions[0].alListeneriDirect                       = REINTERPRET_CAST(LPALLISTENERIDIRECT                  ,loader("alListeneriDirect"));
    functions[0].alListener3iDirect                      = REINTERPRET_CAST(LPALLISTENER3IDIRECT                 ,loader("alListener3iDirect"));
    functions[0].alListenerivDirect                      = REINTERPRET_CAST(LPALLISTENERIVDIRECT                 ,loader("alListenerivDirect"));
    functions[0].alGetListenerfDirect                    = REINTERPRET_CAST(LPALGETLISTENERFDIRECT               ,loader("alGetListenerfDirect"));
    functions[0].alGetListener3fDirect                   = REINTERPRET_CAST(LPALGETLISTENER3FDIRECT              ,loader("alGetListener3fDirect"));
    functions[0].alGetListenerfvDirect                   = REINTERPRET_CAST(LPALGETLISTENERFVDIRECT              ,loader("alGetListenerfvDirect"));
    functions[0].alGetListeneriDirect                    = REINTERPRET_CAST(LPALGETLISTENERIDIRECT               ,loader("alGetListeneriDirect"));
    functions[0].alGetListener3iDirect                   = REINTERPRET_CAST(LPALGETLISTENER3IDIRECT              ,loader("alGetListener3iDirect"));
    functions[0].alGetListenerivDirect                   = REINTERPRET_CAST(LPALGETLISTENERIVDIRECT              ,loader("alGetListenerivDirect"));
    functions[0].alGenSourcesDirect                      = REINTERPRET_CAST(LPALGENSOURCESDIRECT                 ,loader("alGenSourcesDirect"));
    functions[0].alDeleteSourcesDirect                   = REINTERPRET_CAST(LPALDELETESOURCESDIRECT              ,loader("alDeleteSourcesDirect"));
    functions[0].alIsSourceDirect                        = REINTERPRET_CAST(LPALISSOURCEDIRECT                   ,loader("alIsSourceDirect"));
    functions[0].alSourcefDirect                         = REINTERPRET_CAST(LPALSOURCEFDIRECT                    ,loader("alSourcefDirect"));
    functions[0].alSource3fDirect                        = REINTERPRET_CAST(LPALSOURCE3FDIRECT                   ,loader("alSource3fDirect"));
    functions[0].alSourcefvDirect                        = REINTERPRET_CAST(LPALSOURCEFVDIRECT                   ,loader("alSourcefvDirect"));
    functions[0].alSourceiDirect                         = REINTERPRET_CAST(LPALSOURCEIDIRECT                    ,loader("alSourceiDirect"));
    functions[0].alSource3iDirect                        = REINTERPRET_CAST(LPALSOURCE3IDIRECT                   ,loader("alSource3iDirect"));
    functions[0].alSourceivDirect                        = REINTERPRET_CAST(LPALSOURCEIVDIRECT                   ,loader("alSourceivDirect"));
    functions[0].alSourceivDirect                        = REINTERPRET_CAST(LPALSOURCEIVDIRECT                   ,loader("alSourceivDirect"));
    functions[0].alGetSourcefDirect                      = REINTERPRET_CAST(LPALGETSOURCEFDIRECT                 ,loader("alGetSourcefDirect"));
    functions[0].alGetSource3fDirect                     = REINTERPRET_CAST(LPALGETSOURCE3FDIRECT                ,loader("alGetSource3fDirect"));
    functions[0].alGetSourcefvDirect                     = REINTERPRET_CAST(LPALGETSOURCEFVDIRECT                ,loader("alGetSourcefvDirect"));
    functions[0].alGetSourceiDirect                      = REINTERPRET_CAST(LPALGETSOURCEIDIRECT                 ,loader("alGetSourceiDirect"));
    functions[0].alGetSource3iDirect                     = REINTERPRET_CAST(LPALGETSOURCE3IDIRECT                ,loader("alGetSource3iDirect"));
    functions[0].alGetSourceivDirect                     = REINTERPRET_CAST(LPALGETSOURCEIVDIRECT                ,loader("alGetSourceivDirect"));
    functions[0].alSourcePlayDirect                      = REINTERPRET_CAST(LPALSOURCEPLAYVDIRECT                ,loader("alSourcePlayDirect"));
    functions[0].alSourceStopDirect                      = REINTERPRET_CAST(LPALSOURCESTOPVDIRECT                ,loader("alSourceStopDirect"));
    functions[0].alSourceRewindvDirect                   = REINTERPRET_CAST(LPALSOURCEREWINDVDIRECT              ,loader("alSourceRewindvDirect"));
    functions[0].alSourcePausevDirect                    = REINTERPRET_CAST(LPALSOURCEPAUSEVDIRECT               ,loader("alSourcePausevDirect"));
    functions[0].alSourcePlayvDirect                     = REINTERPRET_CAST(LPALSOURCEPLAYDIRECT                 ,loader("alSourcePlayvDirect"));
    functions[0].alSourceStopvDirect                     = REINTERPRET_CAST(LPALSOURCESTOPDIRECT                 ,loader("alSourceStopvDirect"));
    functions[0].alSourceRewindDirect                    = REINTERPRET_CAST(LPALSOURCEREWINDDIRECT               ,loader("alSourceRewindDirect"));
    functions[0].alSourcePauseDirect                     = REINTERPRET_CAST(LPALSOURCEPAUSEDIRECT                ,loader("alSourcePauseDirect"));
    functions[0].alSourceQueueBuffersDirect              = REINTERPRET_CAST(LPALSOURCEQUEUEBUFFERSDIRECT         ,loader("alSourceQueueBuffersDirect"));
    functions[0].alSourceUnqueueBuffersDirect            = REINTERPRET_CAST(LPALSOURCEUNQUEUEBUFFERSDIRECT       ,loader("alSourceUnqueueBuffersDirect"));
    functions[0].alGenBuffersDirect                      = REINTERPRET_CAST(LPALGENBUFFERSDIRECT                 ,loader("alGenBuffersDirect"));
    functions[0].alDeleteBuffersDirect                   = REINTERPRET_CAST(LPALDELETEBUFFERSDIRECT              ,loader("alDeleteBuffersDirect"));
    functions[0].alIsBufferDirect                        = REINTERPRET_CAST(LPALISBUFFERDIRECT                   ,loader("alIsBufferDirect"));
    functions[0].alBufferDataDirect                      = REINTERPRET_CAST(LPALBUFFERDATADIRECT                 ,loader("alBufferDataDirect"));
    functions[0].alBufferfDirect                         = REINTERPRET_CAST(LPALBUFFERFDIRECT                    ,loader("alBufferfDirect"));
    functions[0].alBuffer3fDirect                        = REINTERPRET_CAST(LPALBUFFER3FDIRECT                   ,loader("alBuffer3fDirect"));
    functions[0].alBufferfvDirect                        = REINTERPRET_CAST(LPALBUFFERFVDIRECT                   ,loader("alBufferfvDirect"));
    functions[0].alBufferiDirect                         = REINTERPRET_CAST(LPALBUFFERIDIRECT                    ,loader("alBufferiDirect"));
    functions[0].alBuffer3iDirect                        = REINTERPRET_CAST(LPALBUFFER3IDIRECT                   ,loader("alBuffer3iDirect"));
    functions[0].alBufferivDirect                        = REINTERPRET_CAST(LPALBUFFERIVDIRECT                   ,loader("alBufferivDirect"));
    functions[0].alGetBufferfDirect                      = REINTERPRET_CAST(LPALGETBUFFERFDIRECT                 ,loader("alGetBufferfDirect"));
    functions[0].alGetBuffer3fDirect                     = REINTERPRET_CAST(LPALGETBUFFER3FDIRECT                ,loader("alGetBuffer3fDirect"));
    functions[0].alGetBufferfvDirect                     = REINTERPRET_CAST(LPALGETBUFFERFVDIRECT                ,loader("alGetBufferfvDirect"));
    functions[0].alGetBufferiDirect                      = REINTERPRET_CAST(LPALGETBUFFERIDIRECT                 ,loader("alGetBufferiDirect"));
    functions[0].alGetBuffer3iDirect                     = REINTERPRET_CAST(LPALGETBUFFER3IDIRECT                ,loader("alGetBuffer3iDirect"));
    functions[0].alGetBufferivDirect                     = REINTERPRET_CAST(LPALGETBUFFERIVDIRECT                ,loader("alGetBufferivDirect"));
        /* ALC_EXT_EFX */   
    functions[0].alGenEffectsDirect                      = REINTERPRET_CAST(LPALGENEFFECTSDIRECT                 ,loader("alGenEffectsDirect"));
    functions[0].alDeleteEffectsDirect                   = REINTERPRET_CAST(LPALDELETEEFFECTSDIRECT              ,loader("alDeleteEffectsDirect"));
    functions[0].alIsEffectDirect                        = REINTERPRET_CAST(LPALISEFFECTDIRECT                   ,loader("alIsEffectDirect"));
    functions[0].alEffectiDirect                         = REINTERPRET_CAST(LPALEFFECTIDIRECT                    ,loader("alEffectiDirect"));
    functions[0].alEffectivDirect                        = REINTERPRET_CAST(LPALEFFECTIVDIRECT                   ,loader("alEffectivDirect"));
    functions[0].alEffectfDirect                         = REINTERPRET_CAST(LPALEFFECTFDIRECT                    ,loader("alEffectfDirect"));
    functions[0].alEffectfvDirect                        = REINTERPRET_CAST(LPALEFFECTFVDIRECT                   ,loader("alEffectfvDirect"));
    functions[0].alGetEffectiDirect                      = REINTERPRET_CAST(LPALGETEFFECTIDIRECT                 ,loader("alGetEffectiDirect"));
    functions[0].alGetEffectivDirect                     = REINTERPRET_CAST(LPALGETEFFECTIVDIRECT                ,loader("alGetEffectivDirect"));
    functions[0].alGetEffectfDirect                      = REINTERPRET_CAST(LPALGETEFFECTFDIRECT                 ,loader("alGetEffectfDirect"));
    functions[0].alGetEffectfvDirect                     = REINTERPRET_CAST(LPALGETEFFECTFVDIRECT                ,loader("alGetEffectfvDirect"));
    functions[0].alGenFiltersDirect                      = REINTERPRET_CAST(LPALGENFILTERSDIRECT                 ,loader("alGenFiltersDirect"));
    functions[0].alDeleteFiltersDirect                   = REINTERPRET_CAST(LPALDELETEFILTERSDIRECT              ,loader("alDeleteFiltersDirect"));
    functions[0].alIsFilterDirect                        = REINTERPRET_CAST(LPALISFILTERDIRECT                   ,loader("alIsFilterDirect"));
    functions[0].alFilteriDirect                         = REINTERPRET_CAST(LPALFILTERIDIRECT                    ,loader("alFilteriDirect"));
    functions[0].alFilterivDirect                        = REINTERPRET_CAST(LPALFILTERIVDIRECT                   ,loader("alFilterivDirect"));
    functions[0].alFilterfDirect                         = REINTERPRET_CAST(LPALFILTERFDIRECT                    ,loader("alFilterfDirect"));
    functions[0].alFilterfvDirect                        = REINTERPRET_CAST(LPALFILTERFVDIRECT                   ,loader("alFilterfvDirect"));
    functions[0].alGetFilteriDirect                      = REINTERPRET_CAST(LPALGETFILTERIDIRECT                 ,loader("alGetFilteriDirect"));
    functions[0].alGetFilterivDirect                     = REINTERPRET_CAST(LPALGETFILTERIVDIRECT                ,loader("alGetFilterivDirect"));
    functions[0].alGetFilterfDirect                      = REINTERPRET_CAST(LPALGETFILTERFDIRECT                 ,loader("alGetFilterfDirect"));
    functions[0].alGetFilterfvDirect                     = REINTERPRET_CAST(LPALGETFILTERFVDIRECT                ,loader("alGetFilterfvDirect"));
    functions[0].alGenAuxiliaryEffectSlotsDirect         = REINTERPRET_CAST(LPALGENAUXILIARYEFFECTSLOTSDIRECT    ,loader("alGenAuxiliaryEffectSlotsDirect"));
    functions[0].alDeleteAuxiliaryEffectSlotsDirect      = REINTERPRET_CAST(LPALDELETEAUXILIARYEFFECTSLOTSDIRECT ,loader("alDeleteAuxiliaryEffectSlotsDirect"));
    functions[0].alIsAuxiliaryEffectSlotDirect           = REINTERPRET_CAST(LPALISAUXILIARYEFFECTSLOTDIRECT      ,loader("alIsAuxiliaryEffectSlotDirect"));
    functions[0].alAuxiliaryEffectSlotiDirect            = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTIDIRECT       ,loader("alAuxiliaryEffectSlotiDirect"));
    functions[0].alAuxiliaryEffectSlotivDirect           = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTIVDIRECT      ,loader("alAuxiliaryEffectSlotivDirect"));
    functions[0].alAuxiliaryEffectSlotfDirect            = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTFDIRECT       ,loader("alAuxiliaryEffectSlotfDirect"));
    functions[0].alAuxiliaryEffectSlotfvDirect           = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTFVDIRECT      ,loader("alAuxiliaryEffectSlotfvDirect"));
    functions[0].alGetAuxiliaryEffectSlotiDirect         = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTIDIRECT    ,loader("alGetAuxiliaryEffectSlotiDirect"));
    functions[0].alGetAuxiliaryEffectSlotivDirect        = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTIVDIRECT   ,loader("alGetAuxiliaryEffectSlotivDirect"));
    functions[0].alGetAuxiliaryEffectSlotfDirect         = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTFDIRECT    ,loader("alGetAuxiliaryEffectSlotfDirect"));
    functions[0].alGetAuxiliaryEffectSlotfvDirect        = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTFVDIRECT   ,loader("alGetAuxiliaryEffectSlotfvDirect"));
        /* AL_EXT_BUFFER_DATA_STATIC */
    functions[0].alBufferDataStaticDirect                = REINTERPRET_CAST(LPALBUFFERDATASTATICDIRECT           ,loader("alBufferDataStaticDirect"));
        /* AL_EXT_debug */
    functions[0].alDebugMessageCallbackDirectEXT         = REINTERPRET_CAST(LPALDEBUGMESSAGECALLBACKDIRECTEXT    ,loader("alDebugMessageCallbackDirectEXT"));
    functions[0].alDebugMessageInsertDirectEXT           = REINTERPRET_CAST(LPALDEBUGMESSAGEINSERTDIRECTEXT      ,loader("alDebugMessageInsertDirectEXT"));
    functions[0].alDebugMessageControlDirectEXT          = REINTERPRET_CAST(LPALDEBUGMESSAGECONTROLDIRECTEXT     ,loader("alDebugMessageControlDirectEXT"));
    functions[0].alPushDebugGroupDirectEXT               = REINTERPRET_CAST(LPALPUSHDEBUGGROUPDIRECTEXT          ,loader("alPushDebugGroupDirectEXT"));
    functions[0].alPopDebugGroupDirectEXT                = REINTERPRET_CAST(LPALPOPDEBUGGROUPDIRECTEXT           ,loader("alPopDebugGroupDirectEXT"));
    functions[0].alGetDebugMessageLogDirectEXT           = REINTERPRET_CAST(LPALGETDEBUGMESSAGELOGDIRECTEXT      ,loader("alGetDebugMessageLogDirectEXT"));
    functions[0].alObjectLabelDirectEXT                  = REINTERPRET_CAST(LPALOBJECTLABELDIRECTEXT             ,loader("alObjectLabelDirectEXT"));
    functions[0].alGetObjectLabelDirectEXT               = REINTERPRET_CAST(LPALGETOBJECTLABELDIRECTEXT          ,loader("alGetObjectLabelDirectEXT"));
    functions[0].alGetPointerDirectEXT                   = REINTERPRET_CAST(LPALGETPOINTERDIRECTEXT              ,loader("alGetPointerDirectEXT"));
    functions[0].alGetPointervDirectEXT                  = REINTERPRET_CAST(LPALGETPOINTERVDIRECTEXT             ,loader("alGetPointervDirectEXT"));
        /* AL_EXT_FOLDBACK */
    functions[0].alRequestFoldbackStartDirect            = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTARTDIRECT       ,loader("alRequestFoldbackStartDirect"));
    functions[0].alRequestFoldbackStopDirect             = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTOPDIRECT        ,loader("alRequestFoldbackStopDirect"));
        /* AL_SOFT_buffer_sub_data */
    functions[0].alBufferSubDataDirectSOFT               = REINTERPRET_CAST(LPALBUFFERSUBDATADIRECTSOFT          ,loader("alBufferSubDataDirectSOFT"));
        /* AL_SOFT_source_latency */
    functions[0].alSourcedDirectSOFT                     = REINTERPRET_CAST(LPALSOURCEDDIRECTSOFT                ,loader("alSourcedDirectSOFT"));
    functions[0].alSource3dDirectSOFT                    = REINTERPRET_CAST(LPALSOURCE3DDIRECTSOFT               ,loader("alSource3dDirectSOFT"));
    functions[0].alSourcedvDirectSOFT                    = REINTERPRET_CAST(LPALSOURCEDVDIRECTSOFT               ,loader("alSourcedvDirectSOFT"));
    functions[0].alGetSourcedDirectSOFT                  = REINTERPRET_CAST(LPALGETSOURCEDDIRECTSOFT             ,loader("alGetSourcedDirectSOFT"));
    functions[0].alGetSource3dDirectSOFT                 = REINTERPRET_CAST(LPALGETSOURCE3DDIRECTSOFT            ,loader("alGetSource3dDirectSOFT"));
    functions[0].alGetSourcedvDirectSOFT                 = REINTERPRET_CAST(LPALGETSOURCEDVDIRECTSOFT            ,loader("alGetSourcedvDirectSOFT"));
    functions[0].alSourcei64DirectSOFT                   = REINTERPRET_CAST(LPALSOURCEI64DIRECTSOFT              ,loader("alSourcei64DirectSOFT"));
    functions[0].alSource3i64DirectSOFT                  = REINTERPRET_CAST(LPALSOURCE3I64DIRECTSOFT             ,loader("alSource3i64DirectSOFT"));
    functions[0].alSourcei64vDirectSOFT                  = REINTERPRET_CAST(LPALSOURCEI64VDIRECTSOFT             ,loader("alSourcei64vDirectSOFT"));
    functions[0].alGetSourcei64DirectSOFT                = REINTERPRET_CAST(LPALGETSOURCEI64DIRECTSOFT           ,loader("alGetSourcei64DirectSOFT"));
    functions[0].alGetSource3i64DirectSOFT               = REINTERPRET_CAST(LPALGETSOURCE3I64DIRECTSOFT          ,loader("alGetSource3i64DirectSOFT"));
    functions[0].alGetSourcei64vDirectSOFT               = REINTERPRET_CAST(LPALGETSOURCEI64VDIRECTSOFT          ,loader("alGetSourcei64vDirectSOFT"));
        /* AL_SOFT_deferred_updates */
    functions[0].alDeferUpdatesDirectSOFT                = REINTERPRET_CAST(LPALDEFERUPDATESDIRECTSOFT           ,loader("alDeferUpdatesDirectSOFT"));
    functions[0].alProcessUpdatesDirectSOFT              = REINTERPRET_CAST(LPALPROCESSUPDATESDIRECTSOFT         ,loader("alProcessUpdatesDirectSOFT"));
        /* AL_SOFT_source_resampler */
    functions[0].alGetStringiDirectSOFT                  = REINTERPRET_CAST(LPALGETSTRINGIDIRECTSOFT             ,loader("alGetStringiDirectSOFT"));
        /* AL_SOFT_events */
    functions[0].alEventControlDirectSOFT                = REINTERPRET_CAST(LPALEVENTCONTROLDIRECTSOFT           ,loader("alEventControlDirectSOFT"));
    functions[0].alEventCallbackDirectSOFT               = REINTERPRET_CAST(LPALEVENTCALLBACKDIRECTSOFT          ,loader("alEventCallbackDirectSOFT"));
    functions[0].alGetPointerDirectSOFT                  = REINTERPRET_CAST(LPALGETPOINTERDIRECTSOFT             ,loader("alGetPointerDirectSOFT"));
    functions[0].alGetPointervDirectSOFT                 = REINTERPRET_CAST(LPALGETPOINTERVDIRECTSOFT            ,loader("alGetPointervDirectSOFT"));
        /* AL_SOFT_callback_buffer */
    functions[0].alBufferCallbackDirectSOFT              = REINTERPRET_CAST(LPALBUFFERCALLBACKDIRECTSOFT         ,loader("alBufferCallbackDirectSOFT"));
    functions[0].alGetBufferPtrDirectSOFT                = REINTERPRET_CAST(LPALGETBUFFERPTRDIRECTSOFT           ,loader("alGetBufferPtrDirectSOFT"));
    functions[0].alGetBuffer3PtrDirectSOFT               = REINTERPRET_CAST(LPALGETBUFFER3PTRDIRECTSOFT          ,loader("alGetBuffer3PtrDirectSOFT"));
    functions[0].alGetBufferPtrvDirectSOFT               = REINTERPRET_CAST(LPALGETBUFFERPTRVDIRECTSOFT          ,loader("alGetBufferPtrvDirectSOFT"));
        /* AL_SOFT_source_start_delay */
    functions[0].alSourcePlayAtTimeDirectSOFT            = REINTERPRET_CAST(LPALSOURCEPLAYATTIMEDIRECTSOFT       ,loader("alSourcePlayAtTimeDirectSOFT"));
    functions[0].alSourcePlayAtTimevDirectSOFT           = REINTERPRET_CAST(LPALSOURCEPLAYATTIMEVDIRECTSOFT      ,loader("alSourcePlayAtTimevDirectSOFT"));
        /* EAX */
    functions[0].EAXSetDirect                            = REINTERPRET_CAST(LPEAXSETDIRECT                       ,loader("EAXSetDirect"));
    functions[0].EAXGetDirect                            = REINTERPRET_CAST(LPEAXGETDIRECT                       ,loader("EAXGetDirect"));
    functions[0].EAXSetBufferModeDirect                  = REINTERPRET_CAST(LPEAXSETBUFFERMODEDIRECT             ,loader("EAXSetBufferModeDirect"));
    functions[0].EAXGetBufferModeDirect                  = REINTERPRET_CAST(LPEAXGETBUFFERMODEDIRECT             ,loader("EAXGetBufferModeDirect"));
}


/*  Function loading facilities: */

/* ISO C compatibility types for GCC warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]
   for alGetProcAddress, simply use aladLoader */
typedef aladFunction (ALC_APIENTRY *ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_) (ALCdevice *device, const ALCchar *funcname);
typedef aladFunction (ALC_APIENTRY *ALAD_ISO_C_COMPAT_dlsym_) (void *module, const char *name);

/* modelled after GLFW 3.3, see win32_module.c and posix_module.c specifically */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__)
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HMODULE alad_module_t_;
static alad_module_t_ alad_open_ (const char *path) {
        return LoadLibraryA (path);
}
static aladFunction alad_load_ (alad_module_t_ module, const char *name) {
        return REINTERPRET_CAST(aladFunction, GetProcAddress (module, name));
}
static void alad_close_ (alad_module_t_ module) {
        FreeLibrary (module);
}
#define alad_LIB_NAME_           "OpenAL32.dll"
#define alad_SECONDARY_LIB_NAME_ "soft_oal.dll"
#else /* Unix defaults otherwise */
#include <dlfcn.h>
typedef void *alad_module_t_;
static alad_module_t_ alad_open_ (const char *path) {
        return dlopen (path, RTLD_LAZY | RTLD_LOCAL);
}
static aladFunction alad_load_ (alad_module_t_ module, const char *name) {
        ALAD_ISO_C_COMPAT_dlsym_ compat_dlsym;
        compat_dlsym = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_dlsym_, dlsym);
        return REINTERPRET_CAST(aladFunction, compat_dlsym (module, name));
}
static void alad_close_ (alad_module_t_ module) {
        dlclose (module);
}

/* there are also libopenal.so.1.[X].[Y] and libopenal.1.[X].[Y].dylib respectively, but it would be difficult to look all of those up */
#if defined(__APPLE__)
/* not tested myself; the only references I could find are https://github.com/ToweOPrO/sadsad and https://pastebin.com/MEmh3ZFr, which is at least tenuous */
#define alad_LIB_NAME_           "libopenal.1.dylib"
#define alad_SECONDARY_LIB_NAME_ "libopenal.dylib"
#else
#define alad_LIB_NAME_           "libopenal.so.1"
#define alad_SECONDARY_LIB_NAME_ "libopenal.so"
#endif

#endif /* _WIN32 */

/* this being nullptr also signals that the library is not loaded */
static alad_module_t_ alad_module_ = nullptr;
aladFunction alad_load_global_ (const char* name) {
    return alad_load_ (alad_module_, name);
}
ALCdevice * aladBakedDevice_;
aladFunction alad_load_alc_with_baked_device_ (const char* name) {
    return ((ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_) aladALC.GetProcAddress) (aladBakedDevice_, name);
}
void alad_load_lib_(void) {
    if(alad_module_ != nullptr) return;
    alad_module_ = alad_open_ (alad_LIB_NAME_);
    if (alad_module_ == nullptr) {
        alad_module_ = alad_open_ (alad_SECONDARY_LIB_NAME_);
    }
    if (alad_module_ == nullptr) return;
}

/* simplified Interface */
void aladLoadAL () {
    alad_load_lib_();
    aladAL.GetProcAddress = REINTERPRET_CAST(LPALGETPROCADDRESS           , alad_load_global_("alGetProcAddress"));
    aladLoadALCoreMinimal(&aladAL, alad_load_global_);
    aladLoadALCoreRest(&aladAL, alad_load_global_);
    aladALC.GetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS, alad_load_global_("alcGetProcAddress"));
    aladLoadALCCore(&aladALC, alad_load_global_);
}
void aladUpdateAL () {
    aladLoadEFX(&aladAL, (aladLoader) aladAL.GetProcAddress);
    aladLoadALExtensions(&aladAL, (aladLoader) aladAL.GetProcAddress);
    aladBakedDevice_ = aladALC.GetContextsDevice(aladALC.GetCurrentContext());
    if(aladALC.GetProcAddress != nullptr) aladLoadALCExtensions(&aladALC, alad_load_alc_with_baked_device_);
}
void aladTerminate () {
    if (alad_module_ != nullptr) alad_close_ (alad_module_);
    alad_module_ = nullptr;
}

/* old manual interface */
void aladLoadALContextFree (ALboolean loadAll) {
    alad_load_lib_();
    aladAL.GetProcAddress = REINTERPRET_CAST(LPALGETPROCADDRESS           , alad_load_global_("alGetProcAddress"));
    aladLoadALCoreMinimal(&aladAL, alad_load_global_);
    if (loadAll != AL_FALSE) {
        aladLoadALCoreRest(&aladAL, alad_load_global_);
    }
    aladALC.GetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS, alad_load_global_("alcGetProcAddress"));
    aladLoadALCCore(&aladALC, alad_load_global_);
}
void aladLoadALFromLoaderFunction (LPALGETPROCADDRESS inital_loader) {
    if (inital_loader != nullptr) {
        aladAL.GetProcAddress = inital_loader;
    } else if (alGetProcAddress == nullptr) {
        alad_load_lib_();
        if(alad_module_ == nullptr) {
            aladAL.GetProcAddress = nullptr;
            return;
        }
        else aladAL.GetProcAddress = REINTERPRET_CAST(LPALGETPROCADDRESS           ,  alad_load_global_("alGetProcAddress"));
    }
    aladLoadALCoreMinimal(&aladAL, (aladLoader) aladAL.GetProcAddress);
    aladLoadALCoreRest(&aladAL, (aladLoader) aladAL.GetProcAddress);
    if(aladALC.GetProcAddress == nullptr) aladALC.GetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS,  ((aladLoader) aladAL.GetProcAddress)("alcGetProcAddress"));
    aladLoadALCCore(&aladALC, (aladLoader) aladAL.GetProcAddress);
}
void aladUpdateALPointers (ALCcontext *context, ALboolean extensionsOnly) {
    ALCcontext *oldContext = nullptr;
    if(context != nullptr) {
        oldContext = aladALC.GetCurrentContext();
        aladALC.MakeContextCurrent(context);
    }
    if (extensionsOnly == AL_FALSE) {
        aladLoadALCoreMinimal(&aladAL, (aladLoader) aladAL.GetProcAddress);
        aladLoadALCoreRest(&aladAL, (aladLoader) aladAL.GetProcAddress);
    }
    aladLoadEFX(&aladAL, (aladLoader) aladAL.GetProcAddress);
    aladLoadALExtensions(&aladAL, (aladLoader) aladAL.GetProcAddress);
    if(context != nullptr) {
        aladALC.MakeContextCurrent(oldContext);
    }
}
void aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly) {
    ALCcontext *oldContext = nullptr;
    if(context != nullptr) {
        oldContext = aladALC.GetCurrentContext();
        aladALC.MakeContextCurrent(context);
    }
    if (extensionsOnly == AL_FALSE) {
        if(aladALC.GetProcAddress == nullptr) aladALC.GetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS, ((aladLoader) aladAL.GetProcAddress)("alcGetProcAddress"));
        aladLoadALCCore(&aladALC, (aladLoader) aladAL.GetProcAddress);
    }
    aladLoadALCExtensions(&aladALC, (aladLoader) aladAL.GetProcAddress);
    
    if(context != nullptr) {
        aladALC.MakeContextCurrent(oldContext);
    }
}
void aladUpdateALCPointersFromDevice (ALCdevice *device, ALboolean extensionsOnly) {
    if(aladALC.GetProcAddress == nullptr) return;
    aladBakedDevice_ = device;
    if (extensionsOnly == AL_FALSE) {
        aladLoadALCCore(&aladALC, alad_load_alc_with_baked_device_);
    }
    aladLoadALCExtensions(&aladALC, alad_load_alc_with_baked_device_);
}



#endif        /* ALAD_IMPLEMENTATION */

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif        /* ALAD_H */
