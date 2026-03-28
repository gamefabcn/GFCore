using UnrealBuildTool;

public class GFRuntime : ModuleRules
{
    public GFRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            [
                "Core", 
            ]
        );

        PrivateDependencyModuleNames.AddRange(
            [
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "UMG",
                "TUIO", // 不用的项目可以移除
                "EnhancedInput", // 不用的项目可以移除
            ]
        );
    }
}