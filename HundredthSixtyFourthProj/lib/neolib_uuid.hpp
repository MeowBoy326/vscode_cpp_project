// uuid.hpp - v1.1
/*
 *  Copyright (c) 2007 Leigh Johnston.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of Leigh Johnston nor the names of any
 *       other contributors to this software may be used to endorse or
 *       promote products derived from this software without specific prior
 *       written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "neolib.hpp"
#include <string>
#include <cstdint>
#include <array>
#include <tuple>
#include <utility>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/functional/hash.hpp>

namespace neolib
{
	struct uuid
	{
		uint32_t iPart1;
		uint16_t iPart2;
		uint16_t iPart3;
		uint16_t iPart4;
		std::array<uint8_t, 6> iPart5;
	};

	inline bool operator==(const uuid& lhs, const uuid& rhs)
	{
		return lhs.iPart1 == rhs.iPart1 && lhs.iPart2 == rhs.iPart2 && lhs.iPart3 == rhs.iPart3 && lhs.iPart4 == rhs.iPart4 && lhs.iPart5 == rhs.iPart5;
	}

	inline bool operator!=(const uuid& lhs, const uuid& rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator<(const uuid& lhs, const uuid& rhs)
	{
		return std::tie(lhs.iPart1, lhs.iPart2, lhs.iPart3, lhs.iPart4, lhs.iPart5) <
			std::tie(rhs.iPart1, rhs.iPart2, rhs.iPart3, rhs.iPart4, rhs.iPart5);
	}

	inline uuid make_uuid(const std::string& aHyphenatedHexString /* "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" */)
	{
		uuid result;
		result.iPart1 = static_cast<uint32_t>(std::strtoul(aHyphenatedHexString.substr(0, 8).c_str(), 0, 16));
		result.iPart2 = static_cast<uint16_t>(std::strtoul(aHyphenatedHexString.substr(9, 4).c_str(), 0, 16));
		result.iPart3 = static_cast<uint16_t>(std::strtoul(aHyphenatedHexString.substr(14, 4).c_str(), 0, 16));
		result.iPart4 = static_cast<uint16_t>(std::strtoul(aHyphenatedHexString.substr(19, 4).c_str(), 0, 16));
		uint64_t bytes = static_cast<uint64_t>(std::strtoull(aHyphenatedHexString.substr(24, 12).c_str(), 0, 16));
		for (size_t i = 0; i <= 5; ++i)
			result.iPart5[i] = static_cast<uint8_t>((bytes >> (5 - i) * 8) & 0xFF);
		return result;
	}

	struct unable_to_generate_uuid : std::runtime_error { unable_to_generate_uuid() : std::runtime_error("neolib::unable_to_generate_uuid") {} };

	uuid generate_uuid();

	inline std::istream& operator>>(std::istream& aStream, uuid& aId)
	{
		std::string hyphenatedHexString;
		aStream >> hyphenatedHexString;
		aId = make_uuid(hyphenatedHexString);
		return aStream;
	}

	inline std::ostream& operator<<(std::ostream& aStream, const uuid& aId)
	{
		char oldFill = aStream.fill('0');
		aStream << std::hex << std::uppercase << std::setw(8) << aId.iPart1 << "-";
		aStream << std::hex << std::uppercase << std::setw(4) << aId.iPart2 << "-";
		aStream << std::hex << std::uppercase << std::setw(4) << aId.iPart3 << "-";
		aStream << std::hex << std::uppercase << std::setw(4) << aId.iPart4 << "-";
		for (size_t i = 0; i <= 5; ++i)
			aStream << std::hex << std::uppercase << std::setw(2) << static_cast<uint32_t>(aId.iPart5[i]);
		aStream << std::dec;
		aStream.fill(oldFill);
		return aStream;
	}

	inline std::string to_string(const uuid& aId)
	{
		std::ostringstream oss;
		oss << aId;
		return oss.str();
	}
}

namespace std
{
	template <> struct hash<neolib::uuid>
	{
		typedef neolib::uuid argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& aUuid) const
		{
			auto const& t = std::tie(aUuid.iPart1, aUuid.iPart2, aUuid.iPart3, aUuid.iPart4, aUuid.iPart5);
			return boost::hash<decltype(t)>{}(t);
		}
	};
}
