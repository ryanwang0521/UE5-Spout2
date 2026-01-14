#pragma once

#include "SpoutContext.h"
#include "CoreMinimal.h"
#include "SpoutSender.generated.h"

UCLASS(ClassGroup = (Spout), meta = (BlueprintSpawnableComponent))
class SPOUT2_API USpoutSender : public UActorComponent
{
	GENERATED_BODY()
	
public:
	USpoutSender();
	virtual void BeginDestroy() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "SpoutSender")
	bool Start(const FString& SenderName, UTextureRenderTarget2D* Source, int32 BufferCount = 2);

	UFUNCTION(BlueprintCallable, Category = "SpoutSender")
	void Stop();

private:
	TSharedPtr<SpoutSender> Sender;
	bool bIsInitialized = false;
};