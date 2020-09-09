#ifndef InputMapping_H
#define InputMapping_H

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
	static const constexpr auto VirtualKeyCount = static_cast<std::size_t>(Mapping::VirtualKeys::KeyCount);
	static const constexpr auto DefaultVirtualValue = std::numeric_limits<VirtualKeyIndex>::max();

	inline void keymap(VirtualKeyIndex virtualIndex, std::vector<PhysicalKeyIndex> keys) {
		for (const auto& physicalKey : keys) {
			keymap(virtualIndex, physicalKey);
		}
	}

	inline void keymap(VirtualKeyIndex virtualIndex, PhysicalKeyIndex physicalKey) {
		assert (virtualIndex < VirtualKeyCount);
		assert (physicalKey < PhysicalKeyCount);
		keyunmap(physicalKey);
		m_indexMap[physicalKey] = virtualIndex;
		m_reverseIndexMap[virtualIndex].push_back(physicalKey);
	}

	inline void keyset(VirtualKeyIndex virtualIndex, std::size_t index, PhysicalKeyIndex physicalKey) {
		if (index >= PhysicalKeyCount) {
			return;
		}

		while (index >= m_reverseIndexMap[virtualIndex].size()) {
			m_reverseIndexMap[virtualIndex].emplace_back(DefaultVirtualValue);
		}
		keyunmap(physicalKey);
		m_indexMap[physicalKey] = virtualIndex;
		m_reverseIndexMap[virtualIndex][index] = physicalKey;
	}

	VirtualKeyIndex lookup(PhysicalKeyIndex physicalKeyCode) const {
		if (physicalKeyCode < 0 || static_cast<std::size_t>(physicalKeyCode) >= PhysicalKeyCount) {
      return DefaultVirtualValue;
    }
    return m_indexMap[static_cast<std::size_t>(physicalKeyCode)];
	}

	inline void keyunmap(PhysicalKeyIndex physicalKey) {
		const auto virtualIndex = m_indexMap[physicalKey];
		if (virtualIndex < VirtualKeyCount) {
			auto& vec = m_reverseIndexMap[virtualIndex];
			vec.erase(std::remove(vec.begin(), vec.end(), physicalKey), vec.end());
		}
		m_indexMap[physicalKey] = DefaultVirtualValue;
	}

	inline void keyclear(VirtualKeyIndex virtualIndex) {
		auto& vec = m_reverseIndexMap[virtualIndex];
		for (const auto& physicalKey : vec) {
			m_indexMap[physicalKey] = DefaultVirtualValue;
		}
		vec.clear();
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(VirtualKeyIndex virtualIndex) const {
		return m_reverseIndexMap[virtualIndex];
	}

	inline VirtualKeyIndex lookup(const std::string& keyName) const {
		if (m_namedMap.find(keyName) == m_namedMap.end()) {
			return std::numeric_limits<VirtualKeyIndex>::max();
		}
		return m_namedMap.at(keyName);
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(const std::string& keyName) const {
		const auto virtualIndex = lookup(keyName);
		return reverseLookup(virtualIndex);
	}

private:
	std::array<VirtualKeyIndex, PhysicalKeyCount> m_indexMap {};
	std::array<std::vector<PhysicalKeyIndex>, VirtualKeyCount> m_reverseIndexMap {};
	std::unordered_map<std::string, VirtualKeyIndex> m_namedMap {};
};

#endif