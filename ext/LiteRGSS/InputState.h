#ifndef InputState_H
#define InputState_H

#include "GraphicsSingleton.h"
#include "InputMapping.h"
#include "KeyboardInputMapping.h"
#include "MouseInputMapping.h"

struct InputStateData {
	unsigned long count = 0;
	bool state = false;
};

template <class Mapping>
class InputState {
	static constexpr const auto VirtualKeyCount = PhysicalVirtualInputMapping<Mapping>::VirtualKeyCount;
	using VirtualKeyIndex = typename PhysicalVirtualInputMapping<Mapping>::VirtualKeyIndex;
	using PhysicalKeyIndex = typename PhysicalVirtualInputMapping<Mapping>::PhysicalKeyIndex;
public:
	void update(PhysicalKeyIndex physicalKeyCode, bool state) {
		const auto virtualIndex = m_physicalToVirtualMapping.Mapping::lookup(physicalKeyCode);
		if (virtualIndex < VirtualKeyCount && m_data[virtualIndex].state ^ state) {
			forceUpdate(virtualIndex, state);
		}
	}

	void forceUpdate(VirtualKeyIndex virtualIndex, bool state) {
		m_data[virtualIndex] = InputStateData { GraphicsSingleton::Get().frameCount(), state };
	}

	inline bool isPressed(VirtualKeyIndex virtualIndex) const {
		return (virtualIndex < VirtualKeyCount) ? m_data[virtualIndex].state : false;
	}

	inline bool isPressed(PhysicalKeyIndex physicalKeyCode) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyCode);
		return isPressed(virtualIndex);
	}

	inline bool isPressed(const std::string& physicalKeyName) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyName);
		return isPressed(virtualIndex);
	}

	inline bool isReleased(PhysicalKeyIndex physicalKeyCode) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyCode);
		return isReleasedVirtual(virtualIndex);
	}

	inline bool isReleased(const std::string& physicalKeyName) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyName);
		return isReleasedVirtual(virtualIndex);
	}

	inline bool isTriggered(PhysicalKeyIndex physicalKeyCode) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyCode);
		return isTriggeredVirtual(virtualIndex);
	}

	inline bool isTriggered(const std::string& physicalKeyName) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyName);
		return isTriggeredVirtual(virtualIndex);
	}

	inline bool repeat(const std::string& physicalKeyName) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyName);
		return repeatVirtual(virtualIndex);
	}

	inline bool repeat(PhysicalKeyIndex physicalKeyCode) const {
		const auto virtualIndex = m_physicalToVirtualMapping.lookup(physicalKeyCode);
		return repeatVirtual(virtualIndex);
	}

	void resetClocks() {
		for (VirtualKeyIndex i = 0; i < VirtualKeyCount; i++) {
			forceUpdate(i, false);
		}
	}

	const PhysicalVirtualInputMapping<Mapping>& mapping() {
		return m_physicalToVirtualMapping;
	}

private:
	bool repeatInput(VirtualKeyIndex virtualIndex) const {
		unsigned long count = (GraphicsSingleton::Get().frameCount() - m_data[virtualIndex].count);
		if (count > (GraphicsSingleton::Get().frameRate() / 2)) {
			count -= (GraphicsSingleton::Get().frameRate() / 2);
			if ((count % (GraphicsSingleton::Get().frameRate() / 6)) == 0) {
				return true;
			}
		}
		return false;
	}

	inline bool triggerRelease(VirtualKeyIndex index) const { return m_data[index].count == (GraphicsSingleton::Get().frameCount() - 1); }

	inline bool isReleasedVirtual(VirtualKeyIndex virtualIndex) const {
		if (isPressed(virtualIndex)) {
			return false;
		}
		if (triggerRelease(virtualIndex)) {
			return true;
		}
		return false;
	}

	inline bool isTriggeredVirtual(VirtualKeyIndex virtualIndex) const {
		if (!isPressed(virtualIndex)) {
			return false;
		}
		if (triggerRelease(virtualIndex)) {
			return true;
		}
		return false;
	}

	inline bool repeatVirtual(VirtualKeyIndex virtualIndex) const {
		if (!isPressed(virtualIndex)) {
			return false;
		}
		if (triggerRelease(virtualIndex)) {
			return true;
		}
		return repeatInput(virtualIndex);
	}

	PhysicalVirtualInputMapping<Mapping> m_physicalToVirtualMapping {};

	std::array<InputStateData, VirtualKeyCount> m_data {};
};

#endif