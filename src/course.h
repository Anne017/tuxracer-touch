/* --------------------------------------------------------------------
EXTREME TUXRACER

Copyright (C) 1999-2001 Jasmin F. Patry (Tuxracer)
Copyright (C) 2010 Extreme Tuxracer Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
---------------------------------------------------------------------*/

#ifndef COURSE_H
#define COURSE_H

#include "bh.h"
#include "mathlib.h"
#include <vector>
#include <map>

#define FLOATVAL(i) (*(GLfloat*)(vnc_array+idx+(i)*sizeof(GLfloat)))
#ifdef USE_GLES1
#define BYTEVAL(i) (*(GLubyte*)(vnc_array+idx+10*sizeof(GLfloat) +\
    i*sizeof(GLubyte)))
#define STRIDE_GL_ARRAY (10 * sizeof(GLfloat) + 4 * sizeof(GLubyte))
#else
#define BYTEVAL(i) (*(GLubyte*)(vnc_array+idx+8*sizeof(GLfloat) +\
    i*sizeof(GLubyte)))
#define STRIDE_GL_ARRAY (8 * sizeof(GLfloat) + 4 * sizeof(GLubyte))
#endif
#define ELEV(x,y) (elevation[(x) + nx*(y)] )
#define NORM_INTERPOL 0.05
#define XCD(_x) ((ETR_DOUBLE)(_x) / (nx-1.0) * curr_course->size.x)
#define ZCD(_y) (-(ETR_DOUBLE)(_y) / (ny-1.0) * curr_course->size.y)
#define NMLPOINT(x,y) TVector3d(XCD(x), ELEV(x,y), ZCD(y) )


#define MAX_COURSES 64
#define MAX_TERR_TYPES 64
#define MAX_OBJECT_TYPES 128
#define MAX_DESCRIPTION_LINES 8

class TTexture;

struct TTerrType {
	string textureFile;
	TTexture* texture;
	size_t sound;
	TColor3 col;

	ETR_DOUBLE friction;
	ETR_DOUBLE depth;
	int vol_type;
	int starttex;
	int tracktex;
	int stoptex;
	bool particles;
	bool trackmarks;
	bool shiny;
};

struct TObjectType {
	string		name;
	string		textureFile;
	TTexture*	texture;
	int			collectable;
	bool		collidable;
	bool		drawable;
	bool		reset_point;
	bool		use_normal;
	TVector3d	normal;
	int			num_items;
	int			poly;
};

struct TCollidable {
	TVector3d pt;
	ETR_DOUBLE height;
	ETR_DOUBLE diam;
	size_t tree_type;
	TCollidable(ETR_DOUBLE x, ETR_DOUBLE y, ETR_DOUBLE z, ETR_DOUBLE height_, ETR_DOUBLE diam_, size_t type)
		: pt(x, y, z), height(height_), diam(diam_), tree_type(type)
	{}
};

struct TItem {
	TVector3d pt;
	ETR_DOUBLE height;
	ETR_DOUBLE diam;
	int collectable;
	const TObjectType* type;
	TItem(ETR_DOUBLE x, ETR_DOUBLE y, ETR_DOUBLE z, ETR_DOUBLE height_, ETR_DOUBLE diam_, const TObjectType* type_)
		: pt(x, y, z), height(height_), diam(diam_), collectable(type_->collectable), type(type_)
	{}
};

struct TCourse {
	string name;
	string dir;
	string author;
	string desc[MAX_DESCRIPTION_LINES];
	size_t num_lines;
	TTexture* preview;
	TVector2d size;
	TVector2d play_size;
	ETR_DOUBLE angle;
	ETR_DOUBLE scale;
	TVector2d start;
	size_t env;
	size_t music_theme;
	bool use_keyframe;
	ETR_DOUBLE finish_brake;
};


class CCourse {
private:
	const TCourse* curr_course;
	map<string, size_t> CourseIndex;
	map<string, size_t> ObjectIndex;
	string		CourseDir;

	int			nx;
	int			ny;
	TVector2d	start_pt;
	int			base_height_value;
	bool		mirrored;

	void		FreeTerrainTextures ();
	void		FreeObjectTextures ();
	void		CalcNormals ();
	void		MakeCourseNormals ();
	bool		LoadElevMap ();
	void		LoadItemList ();
	bool		LoadAndConvertObjectMap ();
	bool		LoadTerrainMap ();
	int			GetTerrain (unsigned char pixel[]) const;

	void		MirrorCourseData ();
public:
	CCourse ();
	~CCourse();

	vector<TCourse>		CourseList;
	vector<TTerrType>	TerrList;
	vector<TObjectType>	ObjTypes;
	vector<TCollidable>	CollArr;
	vector<TItem>		NocollArr;
	vector<TPolyhedron>	PolyArr;

	char		*terrain;
	ETR_DOUBLE		*elevation;
	TVector3d	*nmls;
	GLubyte		*vnc_array;

	void ResetCourse ();
	TCourse* GetCourse (const string& dir);
	size_t GetCourseIdx(const TCourse* course) const;
	bool LoadCourseList ();
	void FreeCourseList ();
	bool LoadCourse(TCourse* course);
	bool LoadTerrainTypes ();
	bool LoadObjectTypes ();
	void MakeStandardPolyhedrons ();
	GLubyte* GetGLArrays() const { return vnc_array; }
	void FillGlArrays();

	const TVector2d& GetDimensions() const { return curr_course->size; }
	const TVector2d& GetPlayDimensions() const { return curr_course->play_size; }
	void GetDivisions (int *nx, int *ny) const;
	ETR_DOUBLE GetCourseAngle () const { return curr_course->angle; }
	ETR_DOUBLE GetBaseHeight (ETR_DOUBLE distance) const;
	ETR_DOUBLE GetMaxHeight (ETR_DOUBLE distance) const;
	size_t GetEnv () const;
	const TVector2d& GetStartPoint () const { return start_pt; }
	const TPolyhedron& GetPoly (size_t type) const;
	void MirrorCourse ();

	void GetIndicesForPoint (ETR_DOUBLE x, ETR_DOUBLE z, int *x0, int *y0, int *x1, int *y1) const;
	void FindBarycentricCoords (ETR_DOUBLE x, ETR_DOUBLE z,
	                            TVector2i *idx0, TVector2i *idx1, TVector2i *idx2, ETR_DOUBLE *u, ETR_DOUBLE *v) const;
	TVector3d FindCourseNormal (ETR_DOUBLE x, ETR_DOUBLE z) const;
	ETR_DOUBLE FindYCoord (ETR_DOUBLE x, ETR_DOUBLE z) const;
	void GetSurfaceType (ETR_DOUBLE x, ETR_DOUBLE z, ETR_DOUBLE weights[]) const;
	int GetTerrainIdx (ETR_DOUBLE x, ETR_DOUBLE z, ETR_DOUBLE level) const;
	TPlane GetLocalCoursePlane (TVector3d pt) const;
};

extern CCourse Course;

#endif
