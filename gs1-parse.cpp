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

#include "gs1-parse.h"

#ifdef WIN32
#pragma warning(disable:4996) //about unsafe sprintf
#endif

namespace GS1
{
	
	bool isstrdigit(const char *ptr, int digits_count)
	{
		while(digits_count > 0)
		{
			if (!*ptr || !isdigit(*ptr))
				return false;
			ptr++;
			digits_count--;
		}
		return true;
	}

	bool FieldAI::format_body(std::string &result, bool ISO_date) const
	{
		if (!ai || text_body.empty())
			return false;

		switch(ai->data_format)
		{
		default:
		case GS1_STRING:
			result = text_body;
			break;
		case GS1_DATE:
			{
				//YYMMDD
				if (!isstrdigit(text_body.c_str(), 6))
					return false; //error format
			
				int yy = 2000 + atoi(text_body.substr(0, 2).c_str());
				int mm = atoi(text_body.substr(2, 2).c_str());
				int dd = atoi(text_body.substr(4, 2).c_str());
				if ((mm < 1) || (mm > 12) ||
					(dd < 1) || (dd > 31))
					return false; //err fmt

				char buf[32];
				if (ISO_date)
					sprintf(buf, "%4.4u-%2.2u-%2.2u", yy, mm, dd); //YYYY-MM-DD
				else
					sprintf(buf, "%2.2u-%2.2u-%4.4u", dd, mm, yy); //DD-MM-YYYY

				result = buf;
			}
			break;
		case GS1_DECIMAL:
			{
				if (!isstrdigit(text_body.c_str(), text_body.length()))
					return false; //error format

				int ai_len = strlen(ai->ai);
				if (ai->ai[ai_len-1] != 'n')
					return false;

				int decimal_point = (int)(*text_ai.rbegin() - '0');
				int body_len = text_body.length();
				int pos = body_len - decimal_point;
				if (pos <= 0)
				{
					result = "0.";
					while(pos++ < 0)
						result.push_back('0');
					result += text_body;
					break; //ok
				}

				result = text_body;
				if (decimal_point > 0)
					result.insert(pos, ".");

				while((result[0] == '0') && (result[1] != '.'))
					result.erase(0, 1);
			}
			break;
		}

		return true;
	}

	bool FieldsGS1::ParseGS1(const char *gs1_stream)
	{
		//clear
		ai_map.clear();
		fields.clear();

		bool ok = parse_gs1(fields, gs1_stream);
		if (ok)
		{
			for(int i=0; i<(int)fields.size(); i++)
				ai_map[fields[i].ai->ai] = i;
		}
		return ok;
	}

	int FieldsGS1::GetCount()
	{
		return (int)fields.size();
	}

	const FieldAI* FieldsGS1::Get(int index)
	{
		if (index < (int)fields.size())
			return &fields[index];
		return 0;
	}

	const FieldAI* FieldsGS1::GetByAI(const std::string &ai)
	{
		std::map<std::string, int>::const_iterator it = ai_map.find(ai);
		if (it != ai_map.end())
			return &fields[it->second];
		return 0;
	}


	const char * copy_gs1_body(std::string &body, int min_len, const char *ptr, const char *end)
	{
		body.clear();

		int len = end - ptr;
		if (len < min_len)
			return 0;

		body.reserve(len+1);

		while(*ptr && (ptr < end) && (*ptr != GS1::FNC1))
		{
			body.push_back(*ptr);
			ptr++;
		}

		while(*ptr && (ptr < end) && (*ptr == GS1::FNC1))
			ptr++; //skip FNC1

		if ((int)body.length() < min_len)
			return 0;

		return ptr;
	}

	bool parse_gs1(std::vector<FieldAI> &result, const char *gs1_stream, ErrorAI &error)
	{
		result.clear();

		const char *ptr = gs1_stream;
		int len = strlen(gs1_stream);
		const char *end = ptr + len;

		while(ptr < end)
		{
			error.reset(ptr - gs1_stream);	

			const AI* ai = get_ai(ptr);
			if (!ai)
			{
				error.code = ErrorAI::UnknownAI;
				break; //unknown ai
			}

			const char *start = ptr;

			FieldAI f;
			f.ai = ai;
			f.reference_pos = start - gs1_stream;

			int ai_len = strlen(f.ai->ai);
			f.text_ai = std::string(ptr, ai_len);
			ptr += ai_len;

			error.ai = f.ai;
			error.text_ai = f.text_ai;

			int body_len = f.ai->field_len - ai_len;
			if (body_len <= 0)
			{
				error.code = ErrorAI::InvalidSpecLen;
				return false; //invalid format
			}

			const char *next;
			if (f.ai->max_field_len_optional > 0)
			{
				//vari len
				int body_len_max = f.ai->max_field_len_optional - ai_len;
				if (body_len_max <= body_len)
				{
					error.code = ErrorAI::InvalidSpecMaxLen;
					return false; //invalid format
				}

				next = ptr + body_len_max;
			}
			else
			{
				//const len
				next = ptr + body_len;
			}

			if (next > end)
				next = end;

			next = copy_gs1_body(f.text_body, body_len, ptr, next);
			if (!next)
			{
				error.code = f.text_body.empty() ? ErrorAI::EmptyBody : ErrorAI::BodyTooShort;
				return false; //can't get body (wrong min_len-param or body too short)
			}

			while(*next && (next < end) && (*next == GS1::FNC1))
				next++; //skip FNC1

			ptr = next;
			f.reference_len = ptr - start;

			result.push_back(f);
		}

		return (ptr == end);
	}
}
