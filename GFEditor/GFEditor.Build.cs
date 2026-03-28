using UnrealBuildTool;

public class GFEditor : ModuleRules
{
    public GFEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // ====================== 公开依赖（Shipping 打包安全） ======================
        PublicDependencyModuleNames.AddRange(
            [
                "Core",
                "CoreUObject",
                "Engine",
                "GFRuntime"
            ]
        );

        // ====================== Editor-only 依赖（关键隔离！） ======================
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(
                [
                    "ToolMenus",
                    "InputCore",
                    "Slate",
                    "SlateCore",
                    "EditorStyle",
                    "UnrealEd",
                    "LevelEditor",
                    "AutomationController",
                    "AutomationTest",
                    "AssetTools",
                    "Projects",
                    "DesktopPlatform",
                    "EditorScriptingUtilities",
                    "UATHelper"
                ]
            );

            PublicDefinitions.Add("WITH_EDITOR=1");
        }
        
    }
}