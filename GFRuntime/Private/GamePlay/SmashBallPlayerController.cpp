// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/SmashBallPlayerController.h"

#include "TuioContainerStruct.h"
#include "TuioReceiverComponent.h"

ASmashBallPlayerController::ASmashBallPlayerController()
{
	TuioReceiver = CreateDefaultSubobject<UTUIOReceiverComponent>(TEXT("TuioReceiver"));
}

void ASmashBallPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (TuioReceiver)
	{
		TuioReceiver->OnAddCursor.AddDynamic(this, &ASmashBallPlayerController::OnTuioCursorAdded);
		UE_LOG(LogTemp, Log, TEXT("TUIO 组件已集成至 PlayerController"));
	}

	// 禁止鼠标/摇杆控制视角转动
	SetIgnoreLookInput(true); 
	// 如果角色移动也一起禁掉
	SetIgnoreMoveInput(true);
	
#if WITH_EDITOR
	// 直接在这里控制捕获行为，防止被覆盖
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
#endif
}

void ASmashBallPlayerController::OnDebugLeftMousePressed()
{
	// 1. 获取屏幕鼠标位置
	float MouseX, MouseY;
	if (!GetMousePosition(MouseX, MouseY)) return;

	FVector2D ScreenPos(MouseX, MouseY);
	Click(ScreenPos);
	
}
void ASmashBallPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
#if WITH_EDITOR
	if (InputComponent)
	{
		// 直接绑定，bConsumeInput 设为 false 保证不影响游戏原本逻辑
		FInputKeyBinding& B = InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ASmashBallPlayerController::OnDebugLeftMousePressed);
		B.bConsumeInput = false;
	}
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

void ASmashBallPlayerController::OnTuioCursorAdded(const FTuioContainerStruct Data)
{
	int32 SizeX, SizeY;
	GetViewportSize(SizeX, SizeY);
	FVector2D ScreenPos(Data.Position.X * SizeX, Data.Position.Y * SizeY);
	Click(ScreenPos);
}

void ASmashBallPlayerController::Click(FVector2D screenPos)
{
	FHitResult HitResult;
	if (GetHitResultAtScreenPosition(screenPos, ECC_Visibility, false, HitResult))
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitActor->GetName());
			OnBroadcastHit.Broadcast(screenPos, HitActor);
		}
	}
	else
	{
		OnBroadcastHit.Broadcast(screenPos, nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Nothing Hit"));
	}
}

