// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include "Kismet/GameplayStatics.h"


// ========== 重写Widget构造函数（打开UI时） ==========
void UMyUserWidget::NativeConstruct()
{
    // 先调用父类的NativeConstruct，保证Widget正常初始化
    Super::NativeConstruct();

    // 打开UI时，切换输入模式
    SwitchToUIMode();
}

// ========== 重写Widget销毁函数（关闭UI时） ==========
void UMyUserWidget::NativeDestruct()
{
    // 关闭UI时，恢复游戏输入模式
    RestoreToGameMode();

    // 后调用父类的NativeDestruct，保证Widget正常销毁
    Super::NativeDestruct();
}

// ========== 切换到UI输入模式 ==========
void UMyUserWidget::SwitchToUIMode()
{
    // 安全获取玩家控制器
    APlayerController* PC = GetSafePlayerController();
    if (!PC) return;

    // ========== 核心：设置输入模式 ==========
    // 方案1：允许同时操作「游戏」和「UI」（推荐，如边移动边操作背包）
    FInputModeGameAndUI InputModeGameAndUI;
    // 绑定当前UI为「焦点UI」，确保UI能接收输入（可选，提升UI交互稳定性）
    InputModeGameAndUI.SetWidgetToFocus(this->TakeWidget());
    // 禁止鼠标锁定到视口（可选，根据需求设置）
    InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputModeGameAndUI);

    // 方案2：仅允许操作「UI」（如暂停菜单、剧情对话，禁止玩家移动）
    // FInputModeUIOnly InputModeUIOnly;
    // InputModeUIOnly.SetWidgetToFocus(this->TakeWidget());
    // InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    // PC->SetInputMode(InputModeUIOnly);

    // 显示鼠标光标（UI操作必备）
    PC->bShowMouseCursor = true;
}

// ========== 恢复到游戏输入模式 ==========
void UMyUserWidget::RestoreToGameMode()
{
    // 安全获取玩家控制器
    APlayerController* PC = GetSafePlayerController();
    if (!PC) return;

    // ========== 核心：恢复仅游戏输入 ==========
    FInputModeGameOnly InputModeGameOnly;
    PC->SetInputMode(InputModeGameOnly);

    // 隐藏鼠标光标
    PC->bShowMouseCursor = false;
}

// ========== 安全获取玩家控制器（避坑关键） ==========
APlayerController* UMyUserWidget::GetSafePlayerController()
{
    // 方式1：通过GameplayStatics获取第一个玩家控制器（通用，推荐）
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    // 方式2：若Widget有持有PlayerController的引用，可直接返回（适合多玩家场景）
    // if (MyCachedPlayerController) return MyCachedPlayerController;

    return PC;
}