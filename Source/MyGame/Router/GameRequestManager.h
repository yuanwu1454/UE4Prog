// GameRequestManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameRequestManager.generated.h"

// 请求结果枚举（全局通用）
UENUM(BlueprintType)
enum class EGameRequestResult : uint8
{
    Success,        // 请求成功
    Failed_RateLimit, // 请求频率超限
    Failed_Permission, // 权限不足
    Failed_RouterNotFound, // 路由不存在
    Failed_RequestNotFound, // 请求接口不存在
    Failed_ParamsInvalid, // 参数无效
    Failed_Other     // 其他错误
};

// 请求日志结构体（用于存储/导出日志）
USTRUCT(BlueprintType)
struct FRequestLog
{
    GENERATED_BODY()

    // 请求时间
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime RequestTime;

    // 玩家ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PlayerID;

    // 路由类型（如 Room/Match/Shop）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RouterType;

    // 请求类型（如 CreateRoom/JoinRoom/StartMatch）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RequestType;

    // 请求结果
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGameRequestResult Result;

    // 错误信息（失败时填充）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ErrorMsg;
};

// 请求委托（供外部监听请求结果）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(
    FOnRequestCompleted,
    const FString&, PlayerID,
    const FString&, RouterType,
    const FString&, RequestType,
    EGameRequestResult, Result,
    const FString&, ErrorMsg
);

UCLASS()
class MYGAME_API UGameRequestManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;

    // ============== 核心接口（对外暴露） ==============
    /**
     * 统一路由请求（所有客户端请求都通过此方法转发）
     * @param PC 发起请求的玩家控制器（用于身份认证/权限校验）
     * @param RouterType 路由类型（如 "Room"/"Match"/"Shop"）
     * @param RequestType 请求接口（如 "CreateRoom"/"JoinRoom"）
     * @param Params 请求参数（按接口要求传递，如 CreateRoom 需传 RoomName,MaxPlayers,GameModeClass）
     * @return 是否成功发起请求（仅判断格式/频率，不代表业务成功）
     */
    UFUNCTION(BlueprintCallable, Category = "GameRequest|Router")
    bool RouteRequest(
        ABasePlayerController* PC,
        const FString& RouterType,
        const FString& RequestType,
        const TArray<FString>& Params
    );

    /**
     * 导出请求日志（用于调试/运营分析）
     * @param FilePath 导出路径（如 "Save/Logs/RequestLog.csv"）
     */
    UFUNCTION(BlueprintCallable, Category = "GameRequest|Log")
    bool ExportRequestLog(const FString& FilePath);

    /**
     * 清空请求日志（释放内存）
     */
    UFUNCTION(BlueprintCallable, Category = "GameRequest|Log")
    void ClearRequestLog();

    // ============== 委托（供UI/其他模块监听） ==============
    UPROPERTY(BlueprintAssignable, Category = "GameRequest|Delegate")
    FOnRequestCompleted OnRequestCompleted;

protected:
    // ============== 生命周期 ==============
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    // ============== 内部核心逻辑 ==============
    /**
     * 校验请求频率（防止高频刷请求）
     * @param PlayerID 玩家唯一ID
     * @return 是否通过限流校验
     */
    bool CheckRequestRateLimit(const FString& PlayerID);

    /**
     * 校验请求参数（不同接口参数数量/格式不同）
     * @param RouterType 路由类型
     * @param RequestType 请求类型
     * @param Params 传入参数
     * @return 是否参数有效
     */
    bool ValidateRequestParams(const FString& RouterType, const FString& RequestType, const TArray<FString>& Params);

    /**
     * 执行具体请求（转发到对应Router）
     * @param PC 玩家控制器
     * @param RouterType 路由类型
     * @param RequestType 请求类型
     * @param Params 请求参数
     * @return 请求执行结果
     */
    EGameRequestResult ExecuteRequest(
        ABasePlayerController* PC,
        const FString& RouterType,
        const FString& RequestType,
        const TArray<FString>& Params
    );

    /**
     * 记录请求日志（统一存储）
     */
    void LogRequest(
        const FString& PlayerID,
        const FString& RouterType,
        const FString& RequestType,
        EGameRequestResult Result,
        const FString& ErrorMsg = ""
    );

    // ============== 配置参数（可在蓝图中调整） ==============
    /** 单个玩家最大请求频率（每秒最多请求数） */
    UPROPERTY(EditAnywhere, Category = "GameRequest|Config")
    float MaxRequestPerSecond = 5.0f;

    /** 请求日志最大存储数量（防止内存溢出） */
    UPROPERTY(EditAnywhere, Category = "GameRequest|Config")
    int32 MaxLogCount = 10000;

    // ============== 运行时数据 ==============
    /** 请求历史：Key=PlayerID, Value=请求时间列表（用于限流） */
    TMap<FString, TArray<FDateTime>> RequestHistory;

    /** 请求日志列表（存储所有请求记录） */
    TArray<FRequestLog> RequestLogs;

    /** 临界区（保证多线程下请求日志/历史的线程安全） */
    FCriticalSection RequestCriticalSection;
};