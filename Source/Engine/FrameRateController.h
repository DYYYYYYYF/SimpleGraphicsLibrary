#pragma once

#include <chrono>
#include <thread>
#include <array>

class FrameRateController
{
public:
	FrameRateController(int MaxFPS = 60, int FixedFPS = 60);

	void SetMaxFPS(int MaxFPS);
	void SetFixedFPS(int FixedFPS);

	void BeginFrame();
	bool ShouldRunFixedUpdate();   // 固定帧逻辑
	void EndFrame();               // 结束帧，同步帧率

	float GetDeltaTime() const { return DeltaTime_ * TimeScale_; }
	float GetUnscaledDeltaTime() const { return DeltaTime_; }

	float GetFixedDeltaTime() const { return FixedDelta_; }

	float GetFPS() const { return SmoothedFPS_; }
	void SetTimeScale(float scale) { TimeScale_ = scale; }

private:
	using Clock = std::chrono::high_resolution_clock;

	int MaxFPS_;
	float TargetFrameTime_;    // 渲染目标帧间隔

	int FixedFPS_;
	float FixedDelta_;

	Clock::time_point FrameStart_;

	float DeltaTime_ = 0.0f;
	float Accumulator_ = 0.0f;

	float SmoothedFPS_ = 0.0f;
	static const int SMOOTH_SAMPLES = 60;
	std::array<float, SMOOTH_SAMPLES> FpsHistory_;
	int FpsIndex_ = 0;

	float TimeScale_ = 1.0f;
};