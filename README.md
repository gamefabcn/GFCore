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
<img width="950" height="631" alt="image" src="https://github.com/user-attachments/assets/55d0b7b9-b8ed-4b2a-983b-3cbc1b48e715" />
3. 设置XXXEditor.Target.cs
   <img width="995" height="645" alt="image" src="https://github.com/user-attachments/assets/668a199b-6eff-45d2-a017-ce0b041e544a" />
