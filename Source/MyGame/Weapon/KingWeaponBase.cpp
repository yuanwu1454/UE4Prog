// Nothing ...


#include "KingWeaponBase.h"

#include "Components/BoxComponent.h"


// Sets default values
AKingWeaponBase::AKingWeaponBase()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	// 启用伤害时，才进行启用
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}