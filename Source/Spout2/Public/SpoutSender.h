#pragma once

#include "SpoutContext.h"
#include "CoreMinimal.h"
#include "TickProvider.h"
#include "SpoutSender.generated.h"

UCLASS(ClassGroup = (Spout), meta = (BlueprintSpawnableComponent))
class SPOUT2_API USpoutSender : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "SpoutSender")
	bool Start(const FString& SenderName, UTextureRenderTarget2D* Source, const double FrameRate = 60.0);

	UFUNCTION(BlueprintCallable, Category = "SpoutSender")
	void Stop();

	UFUNCTION(BlueprintCallable, Category = "SpoutSender")
	void ChangeFrameRate(const double FrameRate = 60.0);

private:
	TSharedPtr<SpoutSender> Sender;
	FTickProvider* TickProvider;
	bool bIsInitialized = false;

	void TickThread() const;
};