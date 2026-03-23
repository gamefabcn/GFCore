# GFCore
ue5.7.4 以上使用。

请给主项目添加依赖：
1. XXX.uproject Modules 添加
   ```
   	{
			"Name": "GFRuntime",
			"Type": "Runtime",
			"LoadingPhase": "Default"
		},
		{
			"Name": "GFEditor",
			"Type": "Editor",
			"LoadingPhase": "Default"
		}
   ```
2. 项目XXX.Target.cs
```
public class XXXTarget : TargetRules
{
	public XXXTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("XXX");
		RegisterModulesCreatedByRider(); // 重点
	}
	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "GFRuntime" });
	}
}
```
3. 设置XXXEditor.Target.cs
```
public class XXXEditorTarget : TargetRules
{
	public XXXEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("XXX");
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "GFRuntime", "GFEditor" });
	}
}

```
