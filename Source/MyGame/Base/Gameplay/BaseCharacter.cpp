// Nothing ...


#include "BaseCharacter.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = false;
	// 适合：完全不需要每帧逻辑的 Actor（纯静态物体、道具、场景装饰）
	
	PrimaryActorTick.bStartWithTickEnabled = false;
	// 作用：游戏开始时，先把 Tick 关掉
	// SetActorTickEnabled(true);
	
	GetMesh()->bReceivesDecals = false;
	// bReceivesDecals 是控制图元（Primitive）是否接收贴花（Decal）渲染的总开关。
	// true（默认）：该模型表面可以显示贴花（弹孔、血迹、涂鸦、污渍等）
	// false：任何贴花投射到该模型上都会被忽略，完全不显示
}
