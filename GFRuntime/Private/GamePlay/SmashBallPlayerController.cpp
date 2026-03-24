// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/SmashBallPlayerController.h"

void ASmashBallPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ==================== 核心设置 ====================
	bShowMouseCursor = false;                    // 隐藏鼠标光标（砸球游戏最佳体验）
	bEnableClickEvents = true;                   // 保留点击事件（发射球用）
	bEnableMouseOverEvents = true;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	// 防止任何视角旋转（双保险）
	bEnableMouseOverEvents = true;  // 可选，根据你的Pawn设置
	
	// 编辑器下运行不隐藏
#if WITH_EDITOR
	bShowMouseCursor = true;
#endif
}

void ASmashBallPlayerController::SetUIMode()
{
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);
}

void ASmashBallPlayerController::SetGameMouseMode()
{
	bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
