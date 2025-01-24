#pragma once
#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE(FTickProviderTick);

class FTickProvider : public FRunnable
{
public:
	FTickProvider(double FPS)
	{
		SetFPS(FPS);
		Thread = FRunnableThread::Create(this, TEXT("TickProvider"));
	}

	~FTickProvider()
	{
		bIsRunning = false;
		Thread->Kill();
		
		delete Thread;
		Thread = nullptr;
	}

	virtual uint32 Run() override
	{
		bIsRunning = true;
		while (bIsRunning)
		{
			double CurrentTime = GetCurrentTime();
			if (CurrentTime < NextFrameTime) continue;

			//FScopeLock Lock(&Mutex);
			Tick.Broadcast();
			NextFrameTime = GetNextUpdateTime(FPS, CurrentTime);
		}
		return 0;
	}

	virtual void Stop() override
	{
		bIsRunning = false;
	}

	void SetFPS(double value)
	{
		FScopeLock Lock(&Mutex);
		FPS = FMath::Clamp(value, 1, 120);
	}

	FTickProviderTick Tick;

private:
	FRunnableThread* Thread;
	FCriticalSection Mutex;
	
	bool bIsRunning = false;
	double NextFrameTime = 0;
	double FPS = 60.0;

	static double GetCurrentTime()
	{
		uint64 Cycle = FPlatformTime::Cycles64();
		double Delta = FPlatformTime::GetSecondsPerCycle64();
		return static_cast<double>(Cycle) * Delta;
	}

	static double GetNextUpdateTime(double FPS, double CurrentTime)
	{
		const double FrameTime = 1.0 / FPS;
		return CurrentTime + FrameTime;
	}
};