/*
MIT License

Copyright (c) 2021 Serge Reinov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __GS1_PARSE_H__
#define __GS1_PARSE_H__

#include "gs1-spec.h"

#include <string>
#include <vector>
#include <map>

namespace GS1
{
	class FieldAI
	{
	public:

		const AI *ai; //spec

		std::string text_ai;
		std::string text_body;

		int reference_pos;
		int reference_len;

		bool format_body(std::string &result, bool ISO_date) const;
		inline std::string format_body(bool ISO_date) const { std::string r; format_body(r, ISO_date); return r; }
		inline std::string format_body() const { return format_body(false); }
	};
	
	class ErrorAI
	{
	public:
		enum ErrorCode
		{
			Ok = 0,
			UnknownAI = 1,
			InvalidSpecLen = 2, //invalid gs1-spec (field_len - ai_len <= 0)
			InvalidSpecMaxLen = 3, //invalid gs1-spec (max_len > 0 && max_len <= field_len)
			EmptyBody = 4,
			BodyTooShort = 5
		};

		ErrorCode code;

		const AI *ai; //spec
		std::string text_ai;
		int reference_pos;

		ErrorAI() { reset(0); }
		void reset(int pos) { code = Ok; ai=0; text_ai.clear(); reference_pos=pos; }
	};

	class FieldsGS1
	{
	public:
		bool ParseGS1(const char *gs1_stream);

		int GetCount();
		const FieldAI* Get(int index);
		const FieldAI* GetByAI(const std::string &ai);

	protected:
		std::vector<FieldAI> fields;
		std::map<std::string, int> ai_map; //index by AI::ai
	};

	bool parse_gs1(std::vector<FieldAI> &result, const char *gs1_stream, ErrorAI &error);
	inline bool parse_gs1(std::vector<FieldAI> &result, const char *gs1_stream) { ErrorAI error; return parse_gs1(result, gs1_stream, error); }
}

#endif