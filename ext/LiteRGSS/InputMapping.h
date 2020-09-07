#ifndef InputMapping_H
#define InputMapping_H
#include <vector>
#include <unordered_map>

template <class Mapping> 
class PhysicalVirtualInputMapping : 
	public Mapping {
public:
	using PhysicalKeyIndex = typename Mapping::PhysicalKeyIndex;
	using VirtualKeyIndex = typename Mapping::VirtualKeyIndex;
	static constexpr const std::size_t VirtualKeyCount = sizeof(Mapping::VirtualKeyNames) / sizeof(Mapping::VirtualKeyNames[0]);
	static_assert(static_cast<std::size_t>(Mapping::VirtualKeys::KeyCount) == VirtualKeyCount);

	PhysicalVirtualInputMapping() {
		for (std::size_t i = 0; i < static_cast<std::size_t>(Mapping::VirtualKeys::KeyCount); i++) {
			m_namedMap.emplace(Mapping::VirtualKeyNames[i], i );
		}
	}

	~PhysicalVirtualInputMapping() = default;

	inline VirtualKeyIndex lookup(const std::string& keyName) const {
		if (m_namedMap.find(keyName) == m_namedMap.end()) {
			return std::numeric_limits<VirtualKeyIndex>::max();
		}
		return m_namedMap.at(keyName);
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(const std::string& keyName) const {
		const auto virtualIndex = lookup(keyName);
		return Mapping::reverseLookup(virtualIndex);
	}

	const std::vector<PhysicalKeyIndex>& reverseLookup(VirtualKeyIndex virtualIndex) const {
		return Mapping::reverseLookup(virtualIndex);
	}

private:
	std::unordered_map<std::string, VirtualKeyIndex> m_namedMap {};
};

#endif