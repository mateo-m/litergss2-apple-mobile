#ifndef BaseInputMapping_H
#define BaseInputMapping_H

#include <algorithm>
#include <vector>
#include <array>

using MappingEntry = const char *;

template <std::size_t PhysicalKeyCount_, std::size_t VirtualKeyCount_>
struct BaseInputMapping {
	using PhysicalKeyIndex = int32_t;
	using VirtualKeyIndex = std::size_t;
	static const constexpr auto PhysicalKeyCount = PhysicalKeyCount_;
	static const constexpr auto VirtualKeyCount = VirtualKeyCount_;

	inline void keymap(VirtualKeyIndex virtualIndex, std::vector<PhysicalKeyIndex> keys) {
		for (const auto& physicalKey : keys) {
			keymap(virtualIndex, physicalKey);
		}
	}

	inline void keymap(VirtualKeyIndex virtualIndex, PhysicalKeyIndex physicalKey) {
		m_indexMap[physicalKey] = virtualIndex;
		m_reverseIndexMap[virtualIndex].push_back(physicalKey);
	}

	VirtualKeyIndex lookup(PhysicalKeyIndex physicalKeyCode) const {
		if (physicalKeyCode < 0 || static_cast<std::size_t>(physicalKeyCode) >= PhysicalKeyCount) {
      return std::numeric_limits<VirtualKeyIndex>::max();
    }
    return m_indexMap[static_cast<std::size_t>(physicalKeyCode)];
	}

	inline void keyunmap(PhysicalKeyIndex physicalKey) {
		const auto virtualIndex = m_indexMap[physicalKey];
		auto& vec = m_reverseIndexMap[virtualIndex];
		vec.erase(std::remove(vec.begin(), vec.end(), physicalKey), vec.end());
		m_indexMap[physicalKey] = std::numeric_limits<VirtualKeyIndex>::max();
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(VirtualKeyIndex virtualIndex) const {
		return m_reverseIndexMap[virtualIndex];
	}

private:
	std::array<VirtualKeyIndex, PhysicalKeyCount> m_indexMap {};
	std::array<std::vector<PhysicalKeyIndex>, VirtualKeyCount> m_reverseIndexMap {};

};

#endif