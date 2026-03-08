// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyPlayerCharacter.h"
#include "MyGameMode.h"
// 绑定输入（PlayerController 初始化时自动调用）
void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 绑定按键 Q 到 OnTriggerQPressed（仅客户端生效）
	InputComponent->BindAction("TriggerQ", IE_Pressed, this, &AMyPlayerController::OnTriggerQPressed);
}

// 按键 Q 触发逻辑（客户端执行）
void AMyPlayerController::OnTriggerQPressed()
{
	// 仅客户端处理输入（服务器不处理按键）
	if (!IsLocalController()) return;

	// 获取当前操控的角色
	AMyPlayerCharacter* MyCharacter = GetControlledCharacter();
	if (!MyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("未找到可操控的角色"));
		return;
	}

	MyCharacter->UpdateMoveSpeed(10.f);
	
	UE_LOG(LogTemp, Log, TEXT("客户端按键 Q"));
}

// 安全获取操控的角色（加空检）
AMyPlayerCharacter* AMyPlayerController::GetControlledCharacter()
{
	return Cast<AMyPlayerCharacter>(GetPawn());
}

// 客户端入口：触发加入Session
void AMyPlayerController::Client_RequestJoinSpecificSession(const FString& TargetSessionName)
{
    if (HasAuthority()) return; // 确保只在客户端执行
    Server_GetSessionInfo(TargetSessionName); // 调用服务器RPC
}

// 服务器RPC：调用GameMode的FindGameSessions，获取Session信息
void AMyPlayerController::Server_GetSessionInfo_Implementation(const FString& TargetSessionName)
{
    // 服务器端：获取GameMode（此时GetAuthGameMode有效）
    AMyGameMode* GM = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM)
    {
        UE_LOG(LogPlayerController, Error, TEXT("服务器端GameMode为空，无法查询Session"));
        return;
    }

    // 调用GameMode的FindGameSessions，获取服务器侧的Session列表
    TArray<FOnlineSessionSearchResult> FoundSessions = GM->FindGameSessions();
    
    // 筛选出客户端要找的指定Session
    FOnlineSessionSearchResult TargetSession;
    for (const auto& Session : FoundSessions)
    {
        if (Session.Session.SessionName.ToString() == TargetSessionName)
        {
            TargetSession = Session;
            break;
        }
    }

    // 把Session信息返回给客户端
    Client_OnSessionInfoReceived(TargetSession);
}

// 客户端回调：拿到Session信息后，本地执行加入
void AMyPlayerController::Client_OnSessionInfoReceived_Implementation(const FOnlineSessionSearchResult& SessionResult)
{
    if (!SessionResult.IsValid())
    {
        UE_LOG(LogPlayerController, Error, TEXT("客户端未获取到有效Session信息"));
        return;
    }

    // 客户端本地执行加入Session（核心：这一步必须在客户端做，不能在GameMode做）
    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer) return;

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid()) return;

    // 绑定加入完成回调，切地图
    SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
    SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &AMyPlayerController::OnJoinSessionComplete)
    );

    // 客户端发起加入Session请求
    FName SessionName = FName(*SessionResult.Session.ToString());
    SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionName, SessionResult);
}

// 客户端加入Session完成：切地图
void AMyPlayerController::OnJoinSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (!bWasSuccessful) return;

    FString ConnectURL;
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (SessionInterface->GetResolvedConnectString(SessionName, ConnectURL))
    {
        UGameplayStatics::OpenLevel(GetWorld(), FName(*ConnectURL), true);
    }
}