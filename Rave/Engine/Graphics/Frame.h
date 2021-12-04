#pragma once
#include "Engine/Graphics/SwapChain.h"
#include "Engine/Graphics/CommandBuffer.h"

namespace rv
{
	class Frame
	{
	public:
		Frame() = default;
		Frame(SwapChain& swap);

		ResultValue<u32> NextImage(const Semaphore* semaphore = nullptr, const Fence* fence = nullptr);
		void SetSwapChain(SwapChain& swap);

		ResultValue<u32> Start();
		Result Render(const CommandBuffer& drawCommand);
		Result RenderLast(const CommandBuffer& drawCommand);
		Result End();

		static Result Create(Frame& frame, const Device& device, SwapChain& swap);

		u64 timeout = std::numeric_limits<u64>::max();

	private:
		SwapChain* swap = nullptr;
		u32 image = 0;

		Semaphore imageAvailable;
		Semaphore renderFinished;
		Fence inFlight;
		bool first = false;
	};
}