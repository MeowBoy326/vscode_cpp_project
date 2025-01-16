// scheduler.hpp
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

#include <neolib/neolib.hpp>
#include <thread>
#include "i_scheduler.hpp"
#include "i_cpu.hpp"

namespace neolib
{
	namespace vm
	{
		class scheduler : public i_scheduler
		{
		public:
			scheduler(i_cpu& aCpu) : 
				iCpu(aCpu)
			{
			}
		public:
			virtual const uint8_t* load(std::istream& aProgram)
			{
				i_cpu::page* text = iCpu.allocate_text_page();
				uint8_t byte;
				while (aProgram >> byte)
					text->push_back(byte);
				return &text->front();
			}
			virtual i_thread& create_thread(const uint8_t* aEntryPoint)
			{
				iThreads.push_back(std::make_unique<std::thread>([this]()
				{
					iCpu.execute(aEntryPoint);
				}));
			}
			virtual void destroy_thread(i_thread& aThread)
			{
			}
		private:
			i_cpu& iCpu;
			typedef std::unique_ptr<std::thread> thread_pointer;
			std::vector<thread_pointer> iThreads;
		};
	};
}
