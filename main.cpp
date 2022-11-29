#define CINTERFACE
#include <ole2.h>
#include <windows.h>
#include <detours.h>
#include <dsound.h>

//#pragma comment(linker, "/export:MyDetourFinishHelperProcess,@1,NONAME")

static int (WINAPI * TrueEntryPoint)(VOID) = NULL;
HRESULT (STDMETHODCALLTYPE *RealCreateSoundBuffer)(IDirectSound * This,
                                                    LPCDSBUFFERDESC pcDSBufferDesc,
                                                    LPDIRECTSOUNDBUFFER * ppDSBuffer,
                                                    LPUNKNOWN pUnkOuter) = NULL;

HRESULT MineCreateSoundBuffer(IDirectSound * This, LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER * ppDSBuffer, LPUNKNOWN pUnkOuter) {
    DSBUFFERDESC myBufferDesc;
    myBufferDesc = *pcDSBufferDesc;
    myBufferDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
    return RealCreateSoundBuffer(This, &myBufferDesc, ppDSBuffer, pUnkOuter);
}

int WINAPI MyEntryPoint(VOID) {
    LONG error;
    LPDIRECTSOUND dummyDirectSound = NULL;
    DirectSoundCreate(NULL, &dummyDirectSound, NULL);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    if(dummyDirectSound != NULL) {
        RealCreateSoundBuffer = dummyDirectSound->lpVtbl->CreateSoundBuffer;
        DetourAttach(&(PVOID&)RealCreateSoundBuffer, MineCreateSoundBuffer);
    }
    error = DetourTransactionCommit();
    if(dummyDirectSound != NULL) {
        dummyDirectSound->lpVtbl->Release(dummyDirectSound);
        dummyDirectSound = NULL;
    }
    return TrueEntryPoint();
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    LONG error;
    (void)hinst;
    (void)reserved;

    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    if (dwReason == DLL_PROCESS_ATTACH) {
        DetourRestoreAfterWith();

        // NB: DllMain can't call LoadLibrary, so we hook the app entry point.
        TrueEntryPoint = (int (WINAPI *)(VOID))DetourGetEntryPoint(NULL);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)TrueEntryPoint, MyEntryPoint);
        error = DetourTransactionCommit();
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        if (RealCreateSoundBuffer != NULL) {
            DetourDetach(&(PVOID&)RealCreateSoundBuffer, (PVOID)MineCreateSoundBuffer);
        }
        DetourDetach(&(PVOID&)TrueEntryPoint, MyEntryPoint);
        error = DetourTransactionCommit();
    }
    return TRUE;
}