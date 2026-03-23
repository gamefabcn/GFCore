#include "GFEditor.h"

#include "AutoPackager.h"

#define LOCTEXT_NAMESPACE "FGFEditorModule"

void FGFEditorModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("GFEditor started"));
    
    // 初始化打包工具
    AutoPackagerInstance = MakeUnique<FAutoPackager>();
    AutoPackagerInstance->Initialize();
}

void FGFEditorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("GFEditor shutdown"));
    
    if (AutoPackagerInstance)
    {
        AutoPackagerInstance->Shutdown();
        AutoPackagerInstance.Reset();
    }
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FGFEditorModule, GFEditor)