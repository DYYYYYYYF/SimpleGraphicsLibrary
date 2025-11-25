#include "FrameRateController.h"

FrameRateController::FrameRateController(int MaxFPS, int FixedFPS)
{
	SetMaxFPS(MaxFPS);
	SetFixedFPS(FixedFPS);

	FpsHistory_.fill(60.0f);
}

void FrameRateController::SetMaxFPS(int MaxFPS)
{
	MaxFPS_ = MaxFPS;
	TargetFrameTime_ = 1.0f / MaxFPS_;
}

void FrameRateController::SetFixedFPS(int FixedFPS)
{
	FixedFPS_ = FixedFPS;
	FixedDelta_ = 1.0f / FixedFPS_;
}

void FrameRateController::BeginFrame()
{
	FrameStart_ = Clock::now();
}

bool FrameRateController::ShouldRunFixedUpdate()
{
	Accumulator_ += DeltaTime_;

	if (Accumulator_ >= FixedDelta_)
	{
		Accumulator_ -= FixedDelta_;
		return true; // 正常逻辑帧
	}
	return false;
}

void FrameRateController::EndFrame()
{
	using namespace std::chrono;

	auto FrameEnd = Clock::now();
	duration<float> FrameDuration = FrameEnd - FrameStart_;
	DeltaTime_ = FrameDuration.count();

	// 若帧太快 -> sleep
	if (DeltaTime_ < TargetFrameTime_)
	{
		float sleepTime = TargetFrameTime_ - DeltaTime_;
		if (sleepTime > 0.0001f) // 超小 sleep 反而不准
			std::this_thread::sleep_for(duration<float>(sleepTime));

		auto AfterSleep = Clock::now();
		DeltaTime_ = duration<float>(AfterSleep - FrameStart_).count();
	}

	// 计算真实 FPS（滑动平均）
	float CurrentFPS = 1.0f / DeltaTime_;
	FpsHistory_[FpsIndex_] = CurrentFPS;
	FpsIndex_ = (FpsIndex_ + 1) % SMOOTH_SAMPLES;

	float Sum = 0.0f;
	for (float f : FpsHistory_)
		Sum += f;

	SmoothedFPS_ = Sum / SMOOTH_SAMPLES;
}