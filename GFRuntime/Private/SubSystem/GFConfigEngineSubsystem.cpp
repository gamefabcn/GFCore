// Fill out your copyright notice in the Description page of Project Settings.

#include "SubSystem/GFConfigEngineSubsystem.h"

void UGFConfigEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadConfig(); // 启动时加载
	UE_LOG(LogTemp, Log, TEXT("GFConfigEngineSubsystem 初始化完成，已加载 %s.ini 配置"), *ConfigFilename);
}

void UGFConfigEngineSubsystem::Deinitialize()
{
	SaveConfig();
	Super::Deinitialize();
}

void UGFConfigEngineSubsystem::LoadConfig()
{
	GConfig->GetFloat(*ConfigSection, TEXT("MusicVolume"), MusicVolume, ConfigFilename);
	GConfig->GetFloat(*ConfigSection, TEXT("SFXVolume"), SFXVolume, ConfigFilename);
	GConfig->GetString(*ConfigSection, TEXT("TuioIp"), TuioIp, ConfigFilename);
	GConfig->GetInt(*ConfigSection, TEXT("TuioPort"), TuioPort, ConfigFilename);
}

void UGFConfigEngineSubsystem::SaveConfig()
{
	// 写入配置（自动创建文件）
	GConfig->SetFloat(*ConfigSection, TEXT("MusicVolume"), MusicVolume, ConfigFilename);
	GConfig->SetFloat(*ConfigSection, TEXT("SFXVolume"), SFXVolume, ConfigFilename);
	GConfig->SetString(*ConfigSection, TEXT("TuioIp"), *TuioIp, ConfigFilename);
	GConfig->SetInt(*ConfigSection, TEXT("TuioPort"), TuioPort, ConfigFilename);

	// 立即落盘（商业必备，防止崩溃丢失）
	GConfig->Flush(false, ConfigFilename);

	UE_LOG(LogTemp, Log, TEXT("%s.ini 配置保存成功 → %s"), *ConfigFilename, *GetConfigFilePath());
}

void UGFConfigEngineSubsystem::SetTuioIp(FString ip)
{
	TuioIp = ip;
}

FString UGFConfigEngineSubsystem::GetTuioIp()
{
	return TuioIp;
}

void UGFConfigEngineSubsystem::SetTuioPort(int32 port)
{
	TuioPort = port;
}

int UGFConfigEngineSubsystem::GetTuioPort()
{
	return TuioPort;
}

float UGFConfigEngineSubsystem::GetMusicVolume()
{
	return FMath::Clamp(MusicVolume, 0.0f, 1.0f);
}

void UGFConfigEngineSubsystem::SetMusicVolume(float volume)
{
	MusicVolume = FMath::Clamp(volume, 0.0f, 1.0f);
}

float UGFConfigEngineSubsystem::GetSFXVolume()
{
	return FMath::Clamp(SFXVolume, 0.0f, 1.0f);
}

void UGFConfigEngineSubsystem::SetSFXVolume(float volume)
{
	SFXVolume = FMath::Clamp(volume, 0.0f, 1.0f);
}

FString UGFConfigEngineSubsystem::GetConfigFilePath() const
{
	// 返回实际保存路径（调试用）
	return FPaths::ProjectSavedDir() / TEXT("Config/") / ConfigFilename;
}
