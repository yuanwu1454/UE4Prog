#pragma once

#include "CoreMinimal.h"
#include "Misc/DateTime.h"
#include "Logging/LogMacros.h"

// 定义自定义日志类别
DECLARE_LOG_CATEGORY_EXTERN(LogMultiplayer, Log, All);

/**
 * 多人游戏日志工具类（带客户端唯一标识）
 */
class MYGAME_API FMultiplayerLogHelper
{
public:
	// 日志级别枚举
	enum class ELogLevel
	{
		Log,    // 普通日志
		Warning,// 警告
		Error,  // 错误
		Fatal   // 致命错误
	};

	/**
	 * 输出带时间戳、网络身份（含客户端ID）的日志
	 * @param LogContent 日志内容
	 * @param Level 日志级别
	 * @param WorldContextObject 世界上下文对象
	 * @param PlayerContext 玩家上下文（如PlayerController/PlayerState，可选，用于精准定位玩家）
	 */
	static void LogWithIdentity(const FString& LogContent, ELogLevel Level = ELogLevel::Log, 
								UObject* WorldContextObject = nullptr, UObject* PlayerContext = nullptr);

private:
	// 获取格式化的时间戳（YYYY-MM-DD HH:MM:SS.ms）
	static FString GetFormattedTimestamp();

	// 获取带唯一标识的网络身份描述（如[主机/Host]、[客户端/Client_01]）
	static FString GetNetworkIdentityWithID(UObject* WorldContextObject, UObject* PlayerContext);

	// 辅助函数：获取玩家唯一ID
	static FString GetPlayerUniqueID(UObject* PlayerContext);

	// 辅助函数：获取客户端网络标识（IP+端口）
	static FString GetClientNetworkInfo(UObject* WorldContextObject);
};

// 简化调用的宏定义（新增PlayerContext参数）
#define MULTI_LOG(Content, WorldContext, PlayerContext) FMultiplayerLogHelper::LogWithIdentity(Content, FMultiplayerLogHelper::ELogLevel::Log, WorldContext, PlayerContext)
#define MULTI_LOG_WARNING(Content, WorldContext, PlayerContext) FMultiplayerLogHelper::LogWithIdentity(Content, FMultiplayerLogHelper::ELogLevel::Warning, WorldContext, PlayerContext)
#define MULTI_LOG_ERROR(Content, WorldContext, PlayerContext) FMultiplayerLogHelper::LogWithIdentity(Content, FMultiplayerLogHelper::ELogLevel::Error, WorldContext, PlayerContext)