#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#include "raycast.h"


double length(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

_ngg_tuple_playercoords_to_mapcoords playercoords_to_mapcoords(double camx, double camz, GrayImage *map)
{
	double halfw = map->w / 2.0;

	double horizy = map->h / 2.0;
	double mapx = halfw + (halfw * camx);
	double mapy = horizy + (horizy * camz);
	return (_ngg_tuple_playercoords_to_mapcoords){mapx, mapy};
}

void render_stateless(GrayImage *out, GrayImage *map, GrayImage *raymap, double camx, double camy, double camz, double fov, double viewangle)
{
	assert(camx >= -1 && camx <= 1); /* raycast.ngg:35 */

	assert(camy >= -1 && camy <= 1); /* raycast.ngg:36 */
	assert(camz >= -1 && camz <= 1); /* raycast.ngg:37 */

	gray_image_clear(out);

	if(raymap) {
		gray_image_clear(raymap);
		gray_image_copy_from(raymap, map);
	}

	_ngg_tuple_playercoords_to_mapcoords _ngg_tmp_0 = playercoords_to_mapcoords(camx, camz, map);
	double rayoy = _ngg_tmp_0.m1;
	double rayox = _ngg_tmp_0.m0;

	assert(fov <= M_PI); /* raycast.ngg:49 */

	GrayImage *rstatmap = (GrayImage *) malloc(sizeof(GrayImage));
	if(rstatmap == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	gray_image_construct(rstatmap, out->w, out->h);
	gray_image_clear(rstatmap);

	double fovhalf = fov / 2;
	double maxnormdist = fabs(MAXRAYLEN * cos(fovhalf));

	double eyedist = (out->h / 2) / tan(M_PI / 6);

	double rlx = rayox + (cos(viewangle - fovhalf) * MAXRAYLEN);
	double rly = rayoy + (sin(viewangle - fovhalf) * MAXRAYLEN);
	double rrx = rayox + (cos(viewangle + fovhalf) * MAXRAYLEN);
	double rry = rayoy + (sin(viewangle + fovhalf) * MAXRAYLEN);

	for(int i = 0; i < out->w; i += 1) {
		double dbli = ((double) i) / out->w;

		double rayex = rlx + (dbli * (rrx - rlx));
		double rayey = rly + (dbli * (rry - rly));

		double raylen = length(rayox, rayoy, rayex, rayey);
		if((raylen - MAXRAYLEN) > 0.0001) {
			printf("bug: raylen > MAXRAYLEN: %f > %d\n", raylen, MAXRAYLEN);
			raylen = MAXRAYLEN;
		}

		double cosray = ((double) maxnormdist) / raylen;

		double xdist = rayex - rayox;
		double absxdist = fabs(xdist);
		double ydist = rayey - rayoy;
		double absydist = fabs(ydist);

		if((!absydist) && (!absxdist)) {
			continue;
		}

		_Bool draw_by_x = absxdist > absydist;
		double lastmapclr = gray_image_get_pixel(map, (int) rayox, (int) rayoy);
		int stp = 1;

		if(draw_by_x) {
			if(rayox >= rayex) {
				stp = -1;
			}
		} else {
			if(rayoy >= rayey) {
				stp = -1;
			}
		}

		RayPoint frmpt = ray_point_default();
		frmpt.mapx = rayox;
		frmpt.mapy = rayoy;
		frmpt.mapclr = lastmapclr;
		frmpt.dist = 0;
		frmpt.light = 1;

		RayPoint topt = frmpt;

		double x = rayox;
		double y = rayoy;

		while(true) {
			double rayprog = 0.0;

			if(draw_by_x) {
				y = v_from_u(x, rayox, absxdist, rayoy, ydist);
				double xpos_from_o = fabs(x - rayox);
				rayprog = xpos_from_o / absxdist;
			} else {
				x = v_from_u(y, rayoy, absydist, rayox, xdist);
				double ypos_from_o = fabs(y - rayoy);
				rayprog = ypos_from_o / absydist;
			}

			if(gray_image_oob(map, (int) x, (int) y)) {
				render_column_floor(out, rstatmap, i, camy, eyedist, frmpt, topt);
				break;
			}

			double normdist = fabs(cosray * (rayprog * raylen));
			if(normdist > maxnormdist) {
				printf("bug: normdist > maxnormdist: %f > %f\n", normdist, maxnormdist);
				normdist = maxnormdist;
			}

			normdist = normdist / maxnormdist;

			if(normdist >= 1) {
				break;
			}

			if(raymap) {
				double mappix = gray_image_get_pixel(raymap, (int) x, (int) y);
				double newpix = mappix + (0.1 * (1 - normdist));
				if(newpix > 1) {
					newpix = 1;
				}

				gray_image_set_pixel(raymap, (int) x, (int) y, newpix);
			}

			double mapclr = gray_image_get_pixel(map, (int) x, (int) y);
			assert(mapclr >= 0); /* raycast.ngg:180 */

			if(normdist > 0) {
				RayPoint pt = ray_point_default();
				pt.mapx = x;

				pt.mapy = y;

				pt.dist = normdist;
				pt.light = 1 - normdist;
				pt.mapclr = mapclr;

				if(0 == frmpt.dist) {
					frmpt = pt;
				}

				if((mapclr == lastmapclr) && (normdist < 1)) {
					topt = pt;
				} else {
					ColumnRenderStatus crstat = render_column_floor(out, rstatmap, i, camy, eyedist, frmpt, topt);

					if(crstat.stop_ray) {
						break;
					}

					if(normdist < 1) {
						ColumnRenderStatus crstat = render_column_wall(out, rstatmap, i, camy, eyedist, topt, pt);

						if(crstat.stop_ray) {
							break;
						}
					}

					frmpt = pt;
				}
			}

			lastmapclr = mapclr;

			if(draw_by_x) {
				x = x + stp;
			} else {
				y = y + stp;
			}

			_Bool _tmp_1;
			if(stp == 1) {
				_tmp_1 = (_Bool) (x <= rayex);
			} else {
				_tmp_1 = x >= rayex;
			}

			_Bool _tmp_2;
			if(stp == 1) {
				_tmp_2 = (_Bool) (y <= rayey);
			} else {
				_tmp_2 = y >= rayey;
			}

			_Bool _tmp_3;
			if(draw_by_x) {
				_tmp_3 = (_Bool) _tmp_1;
			} else {
				_tmp_3 = (_Bool) _tmp_2;
			}

			_Bool ok = _tmp_3;

			if(!ok) {
				render_column_floor(out, rstatmap, i, camy, eyedist, frmpt, topt);
				break;
			}
		}
	}

	if(rstatmap) {
		gray_image_destruct(rstatmap);
		free(rstatmap);
	}
}

int projh(int vph, double camy, double eyedist, double objh, double dist)
{
	if(dist <= 0) {
		return 0;
	}

	int vmiddle = (int) (vph / 2);

	objh = objh - camy;

	int objph = (int) (eyedist * (objh / dist));

	return vmiddle - objph;
}

ColumnRenderStatus render_column_floor(GrayImage *out, GrayImage *rstatmap, int outx, double camy, double eyedist, RayPoint pt1, RayPoint pt2)
{
	ColumnRenderStatus stat = column_render_status_default();

	stat.stop_ray = false;

	int pt1outy = (int) projh(out->h, camy, eyedist, pt1.mapclr, pt1.dist);
	int pt2outy = (int) projh(out->h, camy, eyedist, pt2.mapclr, pt2.dist);

	return render_column_part(out, rstatmap, outx, pt1outy, pt2outy, pt1.light, pt2.light, camy, stat);
}

ColumnRenderStatus render_column_wall(GrayImage *out, GrayImage *rstatmap, int outx, double camy, double eyedist, RayPoint pt1, RayPoint pt2)
{
	ColumnRenderStatus stat = column_render_status_default();

	stat.stop_ray = false;

	int pt1outy = (int) projh(out->h, camy, eyedist, pt1.mapclr, pt1.dist);
	int pt2outy = (int) projh(out->h, camy, eyedist, pt2.mapclr, pt2.dist);

	return render_column_part(out, rstatmap, outx, pt1outy, pt2outy, pt2.light, pt2.light, camy, stat);
}

ColumnRenderStatus render_column_part(GrayImage *out, GrayImage *rstatmap, int outx, int pt1y, int pt2y, double pt1light, double pt2light, double camy, ColumnRenderStatus stat)
{
	_Bool noswap = pt1y < pt2y;
	int y1 = (noswap ? pt1y : pt2y);
	double l1 = (noswap ? pt1light : pt2light);
	int y2 = (noswap ? pt2y : pt1y);
	double l2 = (noswap ? pt2light : pt1light);

	if(((y1 < 0) && (y2 < 0)) || ((y1 >= out->h) && (y2 >= out->h))) {
		return stat;
	}

	int y1bak = y1;
	double l1bak = l1;
	if(y1 < 0) {
		double diff = ((double) (0 - y1)) / (y2 - y1);
		assert(diff >= 0 && diff <= 1); /* raycast.ngg:345 */
		y1 = 0;
	}

	if(y2 >= out->h) {
		double diff = ((double) (y2 - out->h)) / (y2 - y1bak);
		assert(diff >= 0 && diff <= 1); /* raycast.ngg:352 */
		y2 = out->h - 1;
	}

	int halfh = out->h / 2;
	double horizy = halfh + (camy * halfh);

	for(int outy = y1; outy <= y2; outy += 1) {
		double filled = gray_image_get_pixel(rstatmap, outx, outy);
		if(!filled) {
			_Bool sync_wall_lighting_with_floor = true;
			_Bool is_floor = l1 != l2;

			double light = l2;
			if(is_floor) {
				light = fabs(outy - horizy) / horizy;
			} else if(sync_wall_lighting_with_floor) {
				light = fabs(y2 - horizy) / horizy;
			}

			if(is_floor) {
				light *= 0.9;
			}

			gray_image_set_pixel(out, outx, outy, light);
			gray_image_set_pixel(rstatmap, outx, outy, 1);
		}
	}

	return stat;
}

double v_from_u(double u, double uorigin, double udist, double vorigin, double vdist)
{
	double upos = fabs(u - uorigin);

	double fact = upos / udist;
	double vpos = fact * vdist;
	double iv = vorigin + vpos;
	return iv;
}

_ngg_tuple_playercoords_to_mapcoords _ngg_tuple_playercoords_to_mapcoords_default()
{
	_ngg_tuple_playercoords_to_mapcoords s;
	s.m0 = 0.0;
	s.m1 = 0.0;
	return s;
}

ColumnRenderStatus column_render_status_default()
{
	ColumnRenderStatus s;
	s.stop_ray = false;
	return s;
}

RayPoint ray_point_default()
{
	RayPoint s;
	s.mapx = 0.0;
	s.mapy = 0.0;
	s.mapclr = 0.0;
	s.dist = 0.0;
	s.light = 0.0;
	return s;
}
