#ifndef P3763_raycast_h
#define P3763_raycast_h

typedef struct _ngg_tuple_playercoords_to_mapcoords _ngg_tuple_playercoords_to_mapcoords;
typedef struct ColumnRenderStatus ColumnRenderStatus;
typedef struct RayPoint RayPoint;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image.h"
#define MAXRAYLEN (800)
struct _ngg_tuple_playercoords_to_mapcoords {
	double m0;
	double m1;
};

struct ColumnRenderStatus {
	_Bool stop_ray;
};

struct RayPoint {
	double mapx;
	double mapy;
	double mapclr;
	double dist;
	double light;
};

double length(double x1, double y1, double x2, double y2);
_ngg_tuple_playercoords_to_mapcoords playercoords_to_mapcoords(double camx, double camz, GrayImage *map);
void render_stateless(GrayImage *out, GrayImage *map, GrayImage *raymap, double camx, double camy, double camz, double fov, double viewangle);
int projh(int vph, double camy, double eyedist, double objh, double dist);
ColumnRenderStatus render_column_floor(GrayImage *out, GrayImage *rstatmap, int outx, double camy, double eyedist, RayPoint pt1, RayPoint pt2);
ColumnRenderStatus render_column_wall(GrayImage *out, GrayImage *rstatmap, int outx, double camy, double eyedist, RayPoint pt1, RayPoint pt2);
ColumnRenderStatus render_column_part(GrayImage *out, GrayImage *rstatmap, int outx, int pt1y, int pt2y, double pt1light, double pt2light, double camy, ColumnRenderStatus stat);
double v_from_u(double u, double uorigin, double udist, double vorigin, double vdist);
_ngg_tuple_playercoords_to_mapcoords _ngg_tuple_playercoords_to_mapcoords_default();
ColumnRenderStatus column_render_status_default();
RayPoint ray_point_default();

#endif /* P3763_raycast_h */
