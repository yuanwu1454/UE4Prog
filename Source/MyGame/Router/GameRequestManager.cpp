// GameRequestManager.cpp 核心片段（补充关键实现）
#include "GameRequestManager.h"

#include "Misc/FileHelper.h"
#include "MyGame/BasePlayerController.h"
#include "MyGame/ProjectNetworkUtils.h"
#include "MyGame/Router/Room/RoomRouter.h"

void UGameRequestManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 初始化配置（可从配置表加载）
    MaxRequestPerSecond = 5.0f;
    MaxLogCount = 10000;
}

bool UGameRequestManager::ShouldCreateSubsystem(UObject* Outer) const
{
    return UProjectNetworkUtils::IsPureClient(this);
}

bool UGameRequestManager::RouteRequest(ABasePlayerController* PC, const FString& RouterType, const FString& RequestType, const TArray<FString>& Params)
{
    // 1. 基础校验
    if (!PC)
    {
        LogRequest("", RouterType, RequestType, EGameRequestResult::Failed_Other, "PlayerController is null");
        OnRequestCompleted.Broadcast("", RouterType, RequestType, EGameRequestResult::Failed_Other, "PlayerController is null");
        return false;
    }

    
    FString PlayerID = FString::FromInt(PC->GetUniqueID());

    // 2. 频率限流校验
    if (!CheckRequestRateLimit(PlayerID))
    {
        LogRequest(PlayerID, RouterType, RequestType, EGameRequestResult::Failed_RateLimit, "Request rate limit exceeded");
        OnRequestCompleted.Broadcast(PlayerID, RouterType, RequestType, EGameRequestResult::Failed_RateLimit, "Request too frequent");
        return false;
    }

    // 3. 参数校验
    if (!ValidateRequestParams(RouterType, RequestType, Params))
    {
        LogRequest(PlayerID, RouterType, RequestType, EGameRequestResult::Failed_ParamsInvalid, "Invalid request params");
        OnRequestCompleted.Broadcast(PlayerID, RouterType, RequestType, EGameRequestResult::Failed_ParamsInvalid, "Invalid params");
        return false;
    }

    // 4. 执行请求
    EGameRequestResult Result = ExecuteRequest(PC, RouterType, RequestType, Params);
    FString ErrorMsg = Result == EGameRequestResult::Success ? "" : "Request execution failed";
    
    // 5. 记录日志 + 触发回调
    LogRequest(PlayerID, RouterType, RequestType, Result, ErrorMsg);
    OnRequestCompleted.Broadcast(PlayerID, RouterType, RequestType, Result, ErrorMsg);

    return Result == EGameRequestResult::Success;
}
/**
 * 导出请求日志到CSV文件（支持相对路径/绝对路径）
 * @param FilePath 导出路径（如 "Save/Logs/RequestLog.csv" 或绝对路径）
 * @return 是否导出成功
 */
bool UGameRequestManager::ExportRequestLog(const FString& FilePath)
{
    // 1. 空路径校验
    if (FilePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ExportRequestLog: FilePath is empty!"));
        return false;
    }

    // 2. 线程安全锁（防止导出时日志被修改）
    FScopeLock Lock(&RequestCriticalSection);

    // 3. 处理路径（转换为项目沙盒目录的绝对路径，避免权限问题）
    FString AbsoluteFilePath = FPaths::ProjectSavedDir() / FilePath;
    // 确保目录存在
    FString LogDirectory = FPaths::GetPath(AbsoluteFilePath);
    if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*LogDirectory))
    {
        UE_LOG(LogTemp, Error, TEXT("ExportRequestLog: Failed to create directory - %s"), *LogDirectory);
        return false;
    }

    // 4. 构建CSV内容（带表头）
    TArray<FString> CSVLines;
    // CSV表头（与FRequestLog字段对应）
    CSVLines.Add(TEXT("RequestTime,PlayerID,RouterType,RequestType,Result,ErrorMsg"));

    // 遍历日志转换为CSV行
    for (const FRequestLog& Log : RequestLogs)
    {
        // 格式化时间（UE的FDateTime转字符串）
        FString TimeStr = Log.RequestTime.ToString();
        // 转换枚举为可读字符串
        FString ResultStr;
        switch (Log.Result)
        {
            case EGameRequestResult::Success: ResultStr = TEXT("Success"); break;
            case EGameRequestResult::Failed_RateLimit: ResultStr = TEXT("Failed_RateLimit"); break;
            case EGameRequestResult::Failed_Permission: ResultStr = TEXT("Failed_Permission"); break;
            case EGameRequestResult::Failed_RouterNotFound: ResultStr = TEXT("Failed_RouterNotFound"); break;
            case EGameRequestResult::Failed_RequestNotFound: ResultStr = TEXT("Failed_RequestNotFound"); break;
            case EGameRequestResult::Failed_ParamsInvalid: ResultStr = TEXT("Failed_ParamsInvalid"); break;
            case EGameRequestResult::Failed_Other: ResultStr = TEXT("Failed_Other"); break;
            default: ResultStr = TEXT("Unknown"); break;
        }

        // 拼接CSV行（处理逗号转义，避免格式错误）
        FString Line = FString::Printf(
            TEXT("%s,%s,%s,%s,%s,%s"),
            *TimeStr,
            *Log.PlayerID.Replace(TEXT(","), TEXT("\\,")), // 转义逗号
            *Log.RouterType.Replace(TEXT(","), TEXT("\\,")),
            *Log.RequestType.Replace(TEXT(","), TEXT("\\,")),
            *ResultStr,
            *Log.ErrorMsg.Replace(TEXT(","), TEXT("\\,"))
        );
        CSVLines.Add(Line);
    }

    // 5. 将CSV内容写入文件
    FString CSVContent = FString::Join(CSVLines, TEXT("\n"));
    bool bWriteSuccess = FFileHelper::SaveStringToFile(
        CSVContent,
        *AbsoluteFilePath,
        FFileHelper::EEncodingOptions::AutoDetect,
        &IFileManager::Get(),
        FILEWRITE_NoFail | FILEWRITE_Append // 追加模式（避免覆盖已有日志）
    );

    // 6. 日志反馈
    if (bWriteSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("ExportRequestLog: Success! Log saved to - %s"), *AbsoluteFilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ExportRequestLog: Failed to write file - %s"), *AbsoluteFilePath);
    }

    return bWriteSuccess;
}

/**
 * 清空请求日志（线程安全，释放内存）
 */
void UGameRequestManager::ClearRequestLog()
{
    // 线程安全锁（防止并发读写）
    FScopeLock Lock(&RequestCriticalSection);

    // 清空日志列表和请求历史（请求历史也需要清理，避免内存泄漏）
    RequestLogs.Empty();
    RequestHistory.Empty();

    UE_LOG(LogTemp, Log, TEXT("ClearRequestLog: All request logs and history have been cleared!"));
}

/**
 * 子系统销毁（清理资源，防止内存泄漏）
 */
void UGameRequestManager::Deinitialize()
{
    Super::Deinitialize();

    // 销毁时自动清空日志（可选：也可先导出再清空）
    ClearRequestLog();

    // 清空委托（防止野指针）
    OnRequestCompleted.Clear();

    UE_LOG(LogTemp, Log, TEXT("GameRequestManager: Deinitialized successfully!"));
}

bool UGameRequestManager::CheckRequestRateLimit(const FString& PlayerID)
{
    FScopeLock Lock(&RequestCriticalSection); // 线程安全锁

    TArray<FDateTime>& History = RequestHistory.FindOrAdd(PlayerID);
    FDateTime Now = FDateTime::Now();

    // 清理1秒前的请求记录
    History.RemoveAll([Now](const FDateTime& Time) { return (Now - Time).GetTotalSeconds() > 1.0f; });

    // 检查当前请求数是否超限
    if (History.Num() >= MaxRequestPerSecond)
    {
        return false;
    }

    // 记录本次请求时间
    History.Add(Now);
    return true;
}

bool UGameRequestManager::ValidateRequestParams(const FString& RouterType, const FString& RequestType, const TArray<FString>& Params)
{
    // 按路由+请求类型校验参数数量
    if (RouterType == "Room")
    {
        if (RequestType == "CreateRoom" && Params.Num() != 3) // RoomName, MaxPlayers, GameModeClass
        {
            return false;
        }
        if (RequestType == "JoinRoom" && Params.Num() != 1) // RoomID
        {
            return false;
        }
    }
    else if (RouterType == "Match")
    {
        if (RequestType == "StartMatchmaking" && Params.Num() != 1) // MatchType
        {
            return false;
        }
    }

    // 校验参数非空
    for (const FString& Param : Params)
    {
        if (Param.IsEmpty())
        {
            return false;
        }
    }

    return true;
}

EGameRequestResult UGameRequestManager::ExecuteRequest(ABasePlayerController* PC, const FString& RouterType, const FString& RequestType, const TArray<FString>& Params)
{
    // 转发到对应Router执行
    if (RouterType == "Room")
    {
        URoomRouter* RoomRouter = PC->GetRoomRouter();
        if (!RoomRouter) return EGameRequestResult::Failed_RouterNotFound;

        if (RequestType == "CreateRoom")
        {
            RoomRouter->CreateRoom(Params[0], FCString::Atoi(*Params[1]), Params[2]);
        }
        else
        {
            return EGameRequestResult::Failed_RequestNotFound;
        }
    }
    else
    {
        return EGameRequestResult::Failed_RouterNotFound;
    }

    return EGameRequestResult::Success;
}

void UGameRequestManager::LogRequest(const FString& PlayerID, const FString& RouterType, const FString& RequestType, EGameRequestResult Result, const FString& ErrorMsg)
{
    FScopeLock Lock(&RequestCriticalSection);

    FRequestLog Log;
    Log.RequestTime = FDateTime::Now();
    Log.PlayerID = PlayerID;
    Log.RouterType = RouterType;
    Log.RequestType = RequestType;
    Log.Result = Result;
    Log.ErrorMsg = ErrorMsg;

    RequestLogs.Add(Log);

    // 限制日志数量，防止内存溢出
    if (RequestLogs.Num() > MaxLogCount)
    {
        RequestLogs.RemoveAt(0, RequestLogs.Num() - MaxLogCount);
    }
}