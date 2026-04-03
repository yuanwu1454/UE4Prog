// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAssetManager.h"

#include "MyPrimaryDataAsset.h"

UMyAssetManager::UMyAssetManager()
{
}

UMyAssetManager::UMyAssetManager(const FObjectInitializer& ObjectInitializer)
{
}

UMyAssetManager& UMyAssetManager::Get()
{
	UMyAssetManager* MyAssetManager = Cast<UMyAssetManager>(&UAssetManager::Get());
	return *MyAssetManager;
}


TSharedPtr<FStreamableHandle> UMyAssetManager::RequestSyncLoad(FSoftObjectPath& TargetToStream,
                                                               bool bManageActiveHandle, FString DebugName)
{
	TSharedPtr<FStreamableHandle> AssetHandle = StreamableManager.RequestSyncLoad(TargetToStream, bManageActiveHandle, DebugName);
	return AssetHandle;
}

void UMyAssetManager::TestStreamableManager()
{

// 	用法 1：异步加载单个资源（最常用，不卡顿）
// 适合：图标、模型、技能配置、物品数据
	// 软引用（你的变量）
	TSoftObjectPtr<UTexture2D> IconPath;

	// 异步加载
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
		IconPath.ToSoftObjectPath(),       // 要加载的资源
		FStreamableDelegate::CreateLambda([this, IconPath]() {
			// 加载完成回调
			UTexture2D* LoadedIcon = IconPath.Get();
			if (LoadedIcon)
			{
				UE_LOG(LogTemp, Log, TEXT("加载成功"));
			}
		}),
		FStreamableManager::DefaultAsyncLoadPriority // 优先级
	);

	// 保存 Handle，用来控制生命周期
// 	2. 那 FStreamableHandle 是干嘛用的？
// 它不是保证加载用的，而是控制加载用的：
// 暂停加载
// 取消加载
// 查询加载进度
// 等待加载（Wait）
// 判断是否加载中
// 手动释放（Release）
// 	3. 那什么时候必须存 Handle？
// 只有下面这些情况才需要存：
// 你要中途取消加载
// 你要检查是否正在加载
// 你要等待加载完成（同步等待）
// 你要手动控制资源释放时长
// 批量加载需要跟踪状态
	CurrentLoadHandle = Handle;

// 	用法 2：异步加载多个资源
// 适合：一次性加载一整套 UI、一套角色资源

	TSoftObjectPtr<UTexture2D> SkillDA;
	TSoftObjectPtr<UTexture2D> ModelPath;
	TSoftObjectPtr<UTexture2D> SoundPath;

	TArray<FSoftObjectPath> AssetsToLoad;
	AssetsToLoad.Add(SkillDA.ToSoftObjectPath());
	AssetsToLoad.Add(ModelPath.ToSoftObjectPath());
	AssetsToLoad.Add(SoundPath.ToSoftObjectPath());

	StreamableManager.RequestAsyncLoad(AssetsToLoad, 
		FStreamableDelegate::CreateLambda([this]() {
			UE_LOG(LogTemp, Log, TEXT("所有资源加载完成"));
		})
	);
//
// 	用法 3：同步加载（慎用！会卡主线程）
// 只适合编辑器、启动流程、小资源
	FSoftObjectPath ItemSoftPath;
	UMyPrimaryDataAsset* LoadedDA = StreamableManager.LoadSynchronous<UMyPrimaryDataAsset>(ItemSoftPath);


// 	用法 4：加载完卸载（释放内存）
// 业务开发最重要的一步！
	// 方法1：释放句柄（推荐）
	if (CurrentLoadHandle.IsValid())
	{
		CurrentLoadHandle->ReleaseHandle();
		CurrentLoadHandle.Reset();
	}

	// 方法2：直接让 manager 卸载
	StreamableManager.Unload(IconPath.ToSoftObjectPath());

	// 用法 5：组合句柄（等待多个加载全部完成）
	TSharedPtr<FStreamableHandle> Handle1;
	TSharedPtr<FStreamableHandle> Handle2;
	TArray<TSharedPtr<FStreamableHandle>> Handles;
    Handles.Add(Handle1);
    Handles.Add(Handle2);
    
    TSharedPtr<FStreamableHandle> CombinedHandle = StreamableManager.CreateCombinedHandle(Handles);
    
    CombinedHandle->BindCompleteDelegate(
        FStreamableDelegate::CreateLambda([](){
            UE_LOG(LogTemp, Log, TEXT("所有子加载都完成了！"));
        })
    );

	// 三、FStreamableHandle 关键方法（业务必记）
	// 是否加载完成
	Handle->HasLoadCompleted();

	// 是否还在加载中
	Handle->IsLoadingInProgress();

	// 是否有效（未取消、未释放）
	Handle->IsActive();

	// 等待加载完成（阻塞）
	Handle->WaitUntilComplete();

	// 拿到加载后的对象
	UObject* Asset = Handle->GetLoadedAsset();

	// 释放句柄 → 资源可能被GC（如果没有别人引用）
	Handle->ReleaseHandle();

	// 取消加载
	Handle->CancelHandle();

// 	四、业务开发者必须注意的 8 个关键点（避坑）
// 1. 句柄（Handle）必须保存！
// 如果你不保存 TSharedPtr<FStreamableHandle>，它会立刻被销毁 → 资源加载完立刻被卸载
// → 表现为：资源一闪而过、显示为缺省紫黑贴图
// 解决方案：把 Handle 存为类成员变量。

// 2. 软引用 vs 硬引用
// 动态加载必须用：
// TSoftObjectPtr<T>
// FSoftObjectPath
// 不要用 TObjectPtr / UObject* 做动态加载，会导致打包丢失资源

// 3. ReleaseHandle () 才会真正释放内存
// 加载 = 引用 + 1
// ReleaseHandle() = 引用 - 1
// 引用为 0 时 UE 才会 GC 卸载资源

// 4. 异步加载不能立即 Get ()
// 错误：
// cpp
// 运行
// StreamableManager.RequestAsyncLoad(Path);
// 	UObject* Asset = SoftPtr.Get(); // 空！！
// 	正确：必须在回调里使用

	// 	5. 不要频繁同步加载大资源
// 	LoadSynchronous 会卡住主线程，造成卡顿、掉帧
// 	只能用在启动、过场、小文本配置

	// 	6. 重复加载同一个资源是安全的
// 	UE 内部会去重，不会重复加载
// 	多个 Handle 都会指向同一份内存

	// 	7. 地图切换 / 游戏结束必须释放 Handle
// 	否则会造成内存泄漏，资源常驻内存

	// 	8. 打包问题
// 	软引用的资源必须在：
// 	Project Settings → Packaging → Asset Manager 中被扫描
// 	或者加入 PrimaryAssetTypes to Scan
// 	否则打包后找不到资源

	// 	五、和 UPrimaryDataAsset 的关系
// 	你之前问的 UPrimaryDataAsset 底层就是用：
// 	FStreamableManager 加载
// 	FStreamableHandle 持有内存
// 	蓝图节点 Async Load Primary Asset → 底层就是 RequestAsyncLoad
}

void UMyAssetManager::Test()
{
	// 1. 扫描与注册 PrimaryAsset（建立资源目录）
// 	ScanPathsForPrimaryAssets()
// ScanPrimaryAssetTypesFromConfig()
// GetPrimaryAssetIdList()
// GetPrimaryAssetDataList()
	// 2. 异步加载 / 卸载 PrimaryAsset（内存控制）
// 	LoadPrimaryAsset()
// LoadPrimaryAssets()
// UnloadPrimaryAsset()
// ChangeBundleStateForPrimaryAssets()
	// 3. Asset Bundle 分组加载（按功能加载资源）

// 	ChangeBundleStateForPrimaryAssets()
// GetAssetBundleEntry()
// InitializeAssetBundlesFromMetadata()
	// 4. 打包 / 烘焙 / 分包 / 热更规则（打包策略）
// 	ModifyCook()
// GetPackageChunkIds()
// ShouldCookForPlatform()
// UpdateManagementDatabase()

	// UAssetManager& AssetManager = UAssetManager::Get();
	// FPrimaryAssetId AssetId("ItemData", "DA_Sword");
	//
	// LoadPrimaryAsset(AssetId, [],()
	// {
	// 	// 加载完成
	// });
	//
	// TArray<FPrimaryAssetId> Ids;
	// AssetManager.GetPrimaryAssetIdList("ItemData", Ids);
	//
	// AssetManager.LoadPrimaryAssets(Ids);
}
