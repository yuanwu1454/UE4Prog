// Nothing ...

#pragma once

#include "CoreMinimal.h"
#include "CommonActor/ActorBase.h"
#include "WeaponBase.generated.h"

UCLASS()
class MYGAME_API AWeaponBase : public AActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
