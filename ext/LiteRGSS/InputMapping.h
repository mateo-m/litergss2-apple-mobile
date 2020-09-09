#ifndef InputMapping_H
#define InputMapping_H

#include <iostream>

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <array>

using MappingEntry = const char *;
using PhysicalKeyIndex = int32_t;
using VirtualKeyIndex = std::size_t;

template <class Mapping>
struct InputMapping {
	InputMapping() {
		for (std::size_t i = 0; i < static_cast<std::size_t>(Mapping::VirtualKeys::KeyCount); i++) {
			m_namedMap.emplace(Mapping::VirtualKeyNames[i], i );
			std::string lowerVKeyName = Mapping::VirtualKeyNames[i];
			std::transform(lowerVKeyName.begin(), lowerVKeyName.end(), lowerVKeyName.begin(), [](unsigned char c){ return std::tolower(c); });
			if (Mapping::VirtualKeyNamesAliases.count(lowerVKeyName) > 0) {
				m_namedMap.emplace(lowerVKeyName, i );
			}
		}
		Mapping::fill(*this);
	}

	virtual ~InputMapping() = default;
	InputMapping(InputMapping&&) = default;
	InputMapping(const InputMapping&) = delete;
	InputMapping& operator=(const InputMapping&) = delete;
	InputMapping& operator=(InputMapping&&) = delete;

	using PhysicalKeyIndex = ::PhysicalKeyIndex;
	using VirtualKeyIndex = ::VirtualKeyIndex;
	static_assert(static_cast<std::size_t>(Mapping::VirtualKeys::KeyCount) == (sizeof(Mapping::VirtualKeyNames) / sizeof(Mapping::VirtualKeyNames[0])));

	static const constexpr auto PhysicalKeyCount = Mapping::PhysicalKeyCount;
	static const constexpr auto PhysicalNegativeOffset = Mapping::PhysicalNegativeOffset;
	static const constexpr auto VirtualKeyCount = static_cast<std::size_t>(Mapping::VirtualKeys::KeyCount);
	static const constexpr auto DefaultVirtualValue = std::numeric_limits<VirtualKeyIndex>::max();
	static const constexpr auto DefaultPhysicalValue = std::numeric_limits<PhysicalKeyIndex>::max();

	inline void keymap(VirtualKeyIndex virtualIndex, std::vector<PhysicalKeyIndex> keys) {
		for (const auto& physicalKey : keys) {
			keymap(virtualIndex, physicalKey);
		}
	}

	inline void keymap(VirtualKeyIndex virtualIndex, PhysicalKeyIndex physicalKey) {
		if (virtualIndex >= VirtualKeyCount ||
		    physicalKey >= PhysicalKeyCount) {
			return;
		}
		keyunmap(physicalKey);
		accessVirtual(physicalKey) = virtualIndex;
		m_reverseIndexMap[virtualIndex].push_back(physicalKey);
	}

	inline void keyset(VirtualKeyIndex virtualIndex, std::size_t index, PhysicalKeyIndex physicalKey) {
		std::cout << "setting physical key " << physicalKey << " to virtual key " << Mapping::VirtualKeyNames[virtualIndex] << " at position " << index << std::endl;

		if (index >= PhysicalKeyCount) {
			return;
		}

		if (index < m_reverseIndexMap[virtualIndex].size()) {
			keyunmap(m_reverseIndexMap[virtualIndex][index]);
		}

		while (index >= m_reverseIndexMap[virtualIndex].size()) {
			m_reverseIndexMap[virtualIndex].emplace_back(DefaultPhysicalValue);
		}
		keyunmap(physicalKey);
		accessVirtual(physicalKey) = virtualIndex;
		m_reverseIndexMap[virtualIndex][index] = physicalKey;
	}

	VirtualKeyIndex lookup(PhysicalKeyIndex physicalKeyCode) const {
		if (physicalKeyCode >= PhysicalKeyCount) {
      return DefaultVirtualValue;
    }
    return accessVirtual(physicalKeyCode);
	}

	inline void keyunmap(PhysicalKeyIndex physicalKey) {
		if (physicalKey >= PhysicalKeyCount) {
			return;
		}
		const auto virtualIndex = accessVirtual(physicalKey);
		if (virtualIndex < VirtualKeyCount) {
			std::cout << "unmapping physical key " << physicalKey << " (was bound to virtual key " << Mapping::VirtualKeyNames[virtualIndex] << ")" << std::endl;
			auto& vec = m_reverseIndexMap[virtualIndex];
			vec.erase(std::remove(vec.begin(), vec.end(), physicalKey), vec.end());
		}
		accessVirtual(physicalKey) = DefaultVirtualValue;
	}

	inline void keyclear(VirtualKeyIndex virtualIndex) {
		auto& vec = m_reverseIndexMap[virtualIndex];
		std::cout << "clearing virtual key " << Mapping::VirtualKeyNames[virtualIndex] << std::endl;
		for (const auto& physicalKey : vec) {
			std::cout << "\t bound to physical key " << physicalKey << std::endl;
			accessVirtual(physicalKey) = DefaultVirtualValue;
		}
		vec.clear();
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(VirtualKeyIndex virtualIndex) const {
		return m_reverseIndexMap[virtualIndex];
	}

	inline VirtualKeyIndex lookup(const std::string& keyName) const {
		if (m_namedMap.find(keyName) == m_namedMap.end()) {
			std::cout << "unable to find virtual key " << keyName << std::endl;
			return DefaultVirtualValue;
		}
		return m_namedMap.at(keyName);
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(const std::string& keyName) const {
		const auto virtualIndex = lookup(keyName);
		return reverseLookup(virtualIndex);
	}

private:
	static std::size_t normalizeIndex(const PhysicalKeyIndex& physicalIndex) {
		return (physicalIndex < 0 && -physicalIndex < PhysicalNegativeOffset) ? (PhysicalKeyCount - physicalIndex) : physicalIndex;
	}

	VirtualKeyIndex& accessVirtual(const PhysicalKeyIndex& physicalIndex) {
		return m_indexMap[InputMapping::normalizeIndex(physicalIndex)];
	}
	
	const VirtualKeyIndex& accessVirtual(const PhysicalKeyIndex& physicalIndex) const {
		return m_indexMap[InputMapping::normalizeIndex(physicalIndex)];
	}

	std::array<VirtualKeyIndex, PhysicalKeyCount + PhysicalNegativeOffset + 1> m_indexMap {};
	std::array<std::vector<PhysicalKeyIndex>, VirtualKeyCount> m_reverseIndexMap {};
	std::unordered_map<std::string, VirtualKeyIndex> m_namedMap {};
};

#endif