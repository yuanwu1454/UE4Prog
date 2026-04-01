// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MySingleTableManagerBase.generated.h"

/**
 * 
 */
UCLASS()

class UMySingleTableManagerBase : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	static UMySingleTableManagerBase* Get(UClass* Cls);

	void Init();
protected:
	virtual void OnInit(){};
private:
	bool bIsClientDataInit=false;

};