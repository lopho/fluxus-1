// Copyright (C) 2005 Dave Griffiths
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

#include "Renderer.h"
#include "PixelPrimitive.h"
#include "State.h"

//#define RENDER_NORMALS
//#define RENDER_BBOX

using namespace fluxus;
	
PixelPrimitive::PixelPrimitive(unsigned int w, unsigned int h) : 
m_Texture(0),
m_Width(w),
m_Height(h)
{
	AddData("c",new TypedPData<dColour>);
	
	// setup the direct access for speed
	PDataDirty();
	
	for (unsigned int x=0; x<h; x++)
	{
		for (unsigned int y=0; y<w; y++)
		{
			m_ColourData->push_back(dColour(1,1,1));
		}
	}
		
	m_Points.push_back(dVector(0,0,0));
	m_Points.push_back(dVector(1,0,0));
	m_Points.push_back(dVector(1,1,0));
	m_Points.push_back(dVector(0,1,0));
	
	glGenTextures(1,(GLuint*)&m_Texture);
}

PixelPrimitive::~PixelPrimitive()
{
	if (m_Texture!=0)
	{
		glDeleteTextures(1,(GLuint*)&m_Texture);
	}
}

void PixelPrimitive::PDataDirty()
{
	// reset pointers
	m_ColourData=GetDataVec<dColour>("c");
}

void PixelPrimitive::Upload()
{
	if (m_Texture!=0)
	{
		glDeleteTextures(1,(GLuint*)&m_Texture);
	}
	
	glBindTexture(GL_TEXTURE_2D,m_Texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D,4,m_Width,m_Height,GL_RGBA,GL_FLOAT,&(*m_ColourData)[0]);

	GetState()->Textures[0]=m_Texture;
}

void PixelPrimitive::Render()
{		
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3fv(m_Points[0].arr());
	glTexCoord2f(1,0);
	glVertex3fv(m_Points[1].arr());
	glTexCoord2f(1,1);
	glVertex3fv(m_Points[2].arr());
	glTexCoord2f(0,1);
	glVertex3fv(m_Points[3].arr());
	glEnd();	
	glEnable(GL_LIGHTING);
}



dBoundingBox PixelPrimitive::GetBoundingBox()
{	
	dBoundingBox box;
	for (vector<dVector>::iterator i=m_Points.begin(); i!=m_Points.end(); ++i)
	{
		box.expand(*i);
	}
	return box;
}

void PixelPrimitive::ApplyTransform(bool ScaleRotOnly)
{
	if (!ScaleRotOnly)
	{
		for (vector<dVector>::iterator i=m_Points.begin(); i!=m_Points.end(); ++i)
		{
			*i=GetState()->Transform.transform(*i);
		}
	}
	else
	{
		for (unsigned int i=0; i<m_Points.size(); i++)
		{
			m_Points[i]=GetState()->Transform.transform_no_trans(m_Points[i]);
		}
	}
	
	GetState()->Transform.init();
}
