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

#ifndef __GS1_SPEC_H__
#define __GS1_SPEC_H__

namespace GS1
{

	const char FNC1 = 0x1D;

	//Rules for when FNC1Required is true:
	//	parsing: expect FNC1 as an end-of-field sign, but don't require it
	//  generation: if this is not the last field, then it is imperative to put FNC1 as an end sign (even reaching the maximum number of characters)

	typedef struct _tag_AI
	{
		const char *ai;
		const char *desc_content;
		const char *desc_format;
		bool FNC1_required;
		const char *data_title;
		int field_len;
		int max_field_len_optional; //for constant length = 0, for variable length should be > field_len
		int data_format;

		operator const char*() const { return ai; }
	} AI;

	const int GS1_STRING = 0;
	const int GS1_DATE = 1;
	const int GS1_DECIMAL = 2;

	const AI* get_ai(const char *gs1_field);
}

#endif