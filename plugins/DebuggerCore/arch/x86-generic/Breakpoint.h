/*
Copyright (C) 2006 - 2015 Evan Teran
                          evan.teran@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef X86BREAKPOINT_20060720_H_
#define X86BREAKPOINT_20060720_H_

#include "IBreakpoint.h"
#include "Util.h"
#include <array>
#include <vector>

namespace DebuggerCorePlugin {

class Breakpoint final : public IBreakpoint {
public:
	enum class TypeId {
		Automatic = static_cast<int>(IBreakpoint::TypeId::Automatic),
		INT3,
		INT1,
		HLT,
		CLI,
		STI,
		INSB,
		INSD,
		OUTSB,
		OUTSD,
		UD2,
		UD0,

		TYPE_COUNT
	};
	
	using Type = util::AbstractEnumData<IBreakpoint::TypeId, TypeId>;

public:
	explicit Breakpoint(edb::address_t address);
	~Breakpoint() override;

public:
	edb::address_t      address() const override { return address_; }
	quint64             hitCount() const override { return hitCount_; }
	bool                enabled() const override { return enabled_; }
	bool                oneTime() const override { return oneTime_; }
	bool                internal() const override { return internal_; }
	size_t              size() const override { return originalBytes_.size(); }
	const quint8 *      originalBytes() const override { return &originalBytes_[0]; }
	IBreakpoint::TypeId type() const override { return type_; }

	static std::vector<BreakpointType> supportedTypes();
	static std::vector<size_t>         possibleRewindSizes();

public:
	bool enable() override;
	bool disable() override;
	void hit() override;
	void setOneTime(bool value) override;
	void setInternal(bool value) override;
	void setType(IBreakpoint::TypeId type) override;
	void setType(TypeId type);

private:
	std::vector<quint8> originalBytes_;
	edb::address_t      address_;
	quint64             hitCount_ = 0;
	bool                enabled_  = false;
	bool                oneTime_  = false;
	bool                internal_ = false;
	Type                type_;
};

}

#endif
