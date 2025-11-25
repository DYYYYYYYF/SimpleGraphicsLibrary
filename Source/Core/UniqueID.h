#pragma once

#include "CoreModuleAPI.h"
#include <cstdint>
#include <atomic>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <functional>

#ifndef INVALID_ID
#define INVALID_ID 0xFFFF
#endif

// UUID风格的唯一ID生成器
class UUID {
public:
	using IDType = uint64_t;

	// ID结构: [32位时间戳][16位进程ID][16位序列号]
	static IDType Generate() {
		auto now = std::chrono::system_clock::now();
		auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
			now.time_since_epoch()).count();

		uint64_t timeComponent = (static_cast<uint64_t>(timestamp) & 0xFFFFFFFF) << 32;
		uint64_t processComponent = (GetProcessID() & 0xFFFF) << 16;
		uint64_t sequence = GetCounter().fetch_add(1, std::memory_order_relaxed) & 0xFFFF;

		return timeComponent | processComponent | sequence;
	}

	// 转换为十六进制字符串
	static std::string ToHexString(IDType id) {
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::setw(16) << id;
		return ss.str();
	}

	// 从十六进制字符串解析
	static IDType FromHexString(const std::string& hex) {
		std::stringstream ss;
		ss << std::hex << hex;
		IDType id;
		ss >> id;
		return id;
	}

	// 解析时间戳部分（秒）
	static uint32_t GetTimestamp(IDType id) {
		return static_cast<uint32_t>(id >> 32);
	}

	// 解析进程ID部分
	static uint16_t GetProcessID(IDType id) {
		return static_cast<uint16_t>((id >> 16) & 0xFFFF);
	}

	// 解析序列号部分
	static uint16_t GetSequence(IDType id) {
		return static_cast<uint16_t>(id & 0xFFFF);
	}

	// 详细信息字符串
	static std::string ToString(IDType id) {
		std::stringstream ss;
		ss << "ID:" << ToHexString(id)
			<< " [Timestamp:" << GetTimestamp(id)
			<< " ProcessID:" << GetProcessID(id)
			<< " Sequence:" << GetSequence(id) << "]";
		return ss.str();
	}

	// 重置计数器（谨慎使用）
	static void ResetCounter() {
		GetCounter().store(1, std::memory_order_relaxed);
	}

	static constexpr IDType InvalidID = 0;

private:
	// 使用函数内静态变量避免静态初始化顺序问题
	static std::atomic<uint16_t>& GetCounter() {
		static std::atomic<uint16_t> counter{ 1 };
		return counter;
	}

	static uint16_t GetProcessID() {
		static const uint16_t processID = InitProcessID();
		return processID;
	}

	static uint16_t InitProcessID() {
		return static_cast<uint16_t>(
			std::hash<std::thread::id>{}(std::this_thread::get_id()) & 0xFFFF
			);
	}
};