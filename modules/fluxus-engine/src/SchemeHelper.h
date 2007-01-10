// Copyright (C) 2007 Dave Griffiths
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <string>
#include <plt/escheme.h>
#include "dada.h"
#include "Renderer.h"

namespace SchemeHelper
{
	// utility functions to make life easier for binding, and also to allow us to 
	// replace the plt functions used for optimisation later, without having 
	// to rewrite everything...
	
	float FloatFromScheme(Scheme_Object *ob);
	int IntFromScheme(Scheme_Object *ob);
	void FloatsFromScheme(Scheme_Object *src, float *dst, unsigned int size);
	char *StringFromScheme(Scheme_Object *ob);
	Scheme_Object *FloatsToScheme(float *src, unsigned int size);
	dVector VectorFromScheme(Scheme_Object *src);
	dColour ColourFromScheme(Scheme_Object *src);
	dQuat QuatFromScheme(Scheme_Object *src);
	dMatrix MatrixFromScheme(Scheme_Object *src);
	void ArgCheck(const std::string &funcname, const std::string &format, int argc, Scheme_Object **argv);
}
