// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "GFConfigEngineSubsystem.generated.h"

/**
 * 全局配置引擎子系统，负责管理游戏的核心配置项，如输入设置、渲染设置等
 * 设计为EngineSubsystem，确保在整个引擎生命周期内可用，并且在所有游戏实例中共享
 * 具体配置项和逻辑可以根据项目需求进行扩展，例如：
 * - 输入配置：管理默认输入映射、敏感度等
 * - 渲染配置：管理全局渲染设置，如抗锯齿、阴影质量等
 * - 性能配置：管理全局性能相关设置，如帧率限制、资源加载策略等
 * 通过BlueprintCallable函数暴露核心配置项，方便在蓝图中进行调整和查询
 */
UCLASS()
class GFRUNTIME_API UGFConfigEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// ==================== 公共接口（Blueprint + C++ 都可调用） ====================
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	void LoadConfig();
	
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	void SaveConfig();
	
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	void SetTuioIp(FString ip);
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	FString GetTuioIp();
	
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	void SetTuioPort(int port);
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	int32 GetTuioPort();
	
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	float GetMusicVolume();
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	void SetMusicVolume(float volume);
	
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	float GetSFXVolume();
	UFUNCTION(BlueprintCallable, Category = "GFConfig")
	void SetSFXVolume(float volume);
	
	UFUNCTION(BlueprintPure, Category = "GFConfig")
	FString GetConfigFilePath() const;
private:
	// ==================== 配置字段（与方案一完全一致，方便迁移） ====================
	float MusicVolume = 1.0f;
	float SFXVolume = 1.0f;
	int TuioPort = 3333;
	FString TuioIp = "0.0.0.0";

	// ==================== .ini 配置（商业推荐） ====================
	const FString ConfigSection = TEXT("GFConfig");  // .ini 文件中的节名称
	const FString ConfigFilename = TEXT("GFConfig.ini");   // 最终生成在 Saved/Config/<Platform>/MyUserConfig.ini
	
};
