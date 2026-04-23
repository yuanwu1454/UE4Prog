// Nothing ...

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponentBase.h"
#include "KingAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UKingAbilitySystemComponent : public UAbilitySystemComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UKingAbilitySystemComponent();
};
