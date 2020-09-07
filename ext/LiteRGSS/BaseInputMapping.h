#ifndef BaseInputMapping_H
#define BaseInputMapping_H

#include <algorithm>
#include <vector>
#include <array>

using MappingEntry = const char *;
using PhysicalKeyIndex = int32_t;
using VirtualKeyIndex = std::size_t;

template <std::size_t PhysicalKeyCount_, std::size_t VirtualKeyCount_>
struct BaseInputMapping {
	BaseInputMapping() = default;
	virtual ~BaseInputMapping() = default;
	BaseInputMapping(BaseInputMapping&&) = default;
	BaseInputMapping(const BaseInputMapping&) = delete;
	BaseInputMapping& operator=(const BaseInputMapping&) = delete;
	BaseInputMapping& operator=(BaseInputMapping&&) = delete;

	using PhysicalKeyIndex = ::PhysicalKeyIndex;
	using VirtualKeyIndex = ::VirtualKeyIndex;
	static const constexpr auto PhysicalKeyCount = PhysicalKeyCount_;
	static const constexpr auto VirtualKeyCount = VirtualKeyCount_;

	inline void keymap(VirtualKeyIndex virtualIndex, std::vector<PhysicalKeyIndex> keys) {
		for (const auto& physicalKey : keys) {
			keymap(virtualIndex, physicalKey);
		}
	}

	inline void keymap(VirtualKeyIndex virtualIndex, PhysicalKeyIndex physicalKey) {
		keyunmap(physicalKey);
		m_indexMap[physicalKey] = virtualIndex;
		m_reverseIndexMap[virtualIndex].push_back(physicalKey);
	}

	inline void keyset(VirtualKeyIndex virtualIndex, std::size_t index, PhysicalKeyIndex physicalKey) {
		keyunmap(physicalKey);
		m_indexMap[physicalKey] = virtualIndex;
		if (index < m_reverseIndexMap[virtualIndex].size()) {
			m_reverseIndexMap[virtualIndex][index] = physicalKey;
		}
	}

	VirtualKeyIndex lookup(PhysicalKeyIndex physicalKeyCode) const {
		if (physicalKeyCode < 0 || static_cast<std::size_t>(physicalKeyCode) >= PhysicalKeyCount) {
      return std::numeric_limits<VirtualKeyIndex>::max();
    }
    return m_indexMap[static_cast<std::size_t>(physicalKeyCode)];
	}

	inline void keyunmap(PhysicalKeyIndex physicalKey) {
		const auto virtualIndex = m_indexMap[physicalKey];
		if (virtualIndex < VirtualKeyCount) {
			auto& vec = m_reverseIndexMap[virtualIndex];
			vec.erase(std::remove(vec.begin(), vec.end(), physicalKey), vec.end());
		}
		m_indexMap[physicalKey] = std::numeric_limits<VirtualKeyIndex>::max();
	}

	inline void keyclear(VirtualKeyIndex virtualIndex) {
		auto& vec = m_reverseIndexMap[virtualIndex];
		for (const auto& physicalKey : vec) {
			m_indexMap[physicalKey] = std::numeric_limits<VirtualKeyIndex>::max();
		}
		vec.clear();
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(VirtualKeyIndex virtualIndex) const {
		return m_reverseIndexMap[virtualIndex];
	}

private:
	std::array<VirtualKeyIndex, PhysicalKeyCount> m_indexMap {};
	std::array<std::vector<PhysicalKeyIndex>, VirtualKeyCount> m_reverseIndexMap {};

};

#endif