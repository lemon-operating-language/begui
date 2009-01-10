#pragma once

#include "common.h"

template <class P>
double polygon_area(const std::vector<P> &pts)
{
	double area = 0;
	for (size_t i=0; i<pts.size(); ++i)
	{
		size_t j=i+1;
		if (j==pts.size()) j=0;

		const P &p1 = pts[i];
		const P &p2 = pts[j];

		area += (p1.y - p2.y) * (p1.x+p2.x);
	}

	if (area < 0)
		area = -area;
	return area/2.0;
}

template <class P>
void slice_polygon_y(std::vector<P> poly, std::vector<P> &p1, std::vector<P> &p2, double y)
{
	//	for (size_t i=0; i<poly.size(); ++i)
	//		Console::print("\t%f,%f\n", poly[i].x, poly[i].y);
	//	Console::print("--\n");

	// p1: top poly, p2: bottom
	for (size_t i=0; i<poly.size(); ++i)
	{
		size_t j=i-1;
		if (i==0) j=poly.size()-1;

		// intersect line segment i->j with slice y
		if ((poly[i].y > y && poly[j].y < y) ||
			(poly[i].y < y && poly[j].y > y)) {
			double t = (y - poly[j].y)/(poly[i].y - poly[j].y);
			P pt = t*poly[i] + (1-t)*poly[j];
			p1.push_back(pt);
			p2.push_back(pt);
		}
			
		if (poly[i].y <= y)
			p1.push_back(poly[i]);
		if (poly[i].y >= y)
			p2.push_back(poly[i]);
	}
}

template <class P>
void slice_polygon_x(std::vector<P> poly, std::vector<P> &p1, std::vector<P> &p2, double x)
{
	// p1: left poly, p2: right
	for (size_t i=0; i<poly.size(); ++i)
	{
		size_t j=i-1;
		if (i==0) j=poly.size()-1;

		// intersect line segment i->j with slice y
		if ((poly[i].x > x && poly[j].x < x) ||
			(poly[i].x < x && poly[j].x > x)) {
			double t = (x - poly[j].x)/(poly[i].x - poly[j].x);
			P pt = t*poly[i] + (1-t)*poly[j];
			p1.push_back(pt);
			p2.push_back(pt);
		}
			
		if (poly[i].x <= x)
			p1.push_back(poly[i]);
		if (poly[i].x >= x)
			p2.push_back(poly[i]);
	}
}

template <class P, class R>
void draw_triangle_AA(P p1, P p2, P p3, R &renderer)
{
	// sort points in descending y order
	if (p1.y > p2.y) std::swap(p1, p2);
	if (p2.y > p3.y) {
		if (p1.y > p3.y) {
			std::swap(p1, p3);
			std::swap(p2, p3);
		}
		else std::swap(p2, p3);
	}

	// degenerate cases:
	if (p1.y == p3.y)
		return;
	
	// make a polygon
	std::vector<P> poly(3);
	poly[0] = p1;
	poly[1] = p2;
	poly[2] = p3;
	
	std::vector<P> poly1, poly2, poly3, poly4;
	poly1.reserve(5);
	poly2.reserve(5);
	poly3.reserve(5);
	poly4.reserve(5);

	// chop this triangle up
	double s_top = p1.y;
	while (true)
	{
		double slice_y = (int)s_top + 1;

		// slice the polygon
		poly1.clear();
		poly2.clear();
		slice_polygon_y(poly, poly1, poly2, slice_y);

		// poly1 is the part of the triangle over the scanline. It's at most
		// 1px high. Render it!
		if (poly1.size() >= 3)
		{
			// start slicing the polygon along the x-axis
			double min_x = poly1[0].x, max_x = poly1[0].x;
			for (size_t i=0; i<poly1.size(); ++i) {
				if (poly1[i].x < min_x) min_x = poly1[i].x;
				if (poly1[i].x > max_x) max_x = poly1[i].x;
			}

			double s_left = min_x-1;
			while (true)
			{
				double slice_x = (int)s_left + 1;

				poly3.clear();
				poly4.clear();
				slice_polygon_x(poly1, poly3, poly4, slice_x);

				// poly 3 is one pixel or less in size. Render it
				if (poly3.size() >= 3) {
					// calculate the area of the polygon
					double area = polygon_area(poly3);
					renderer.draw(poly3[1], area);
				}

				// poly 4 is the rest of the scanline
				poly1 = poly4;
				
				if (slice_x >= max_x)
					break;
				s_left = slice_x;
			}
		}

		// poly2 is the rest of the polygon, that will be used for the rest of the
		// scanlines
		poly = poly2;

		if (slice_y >= p3.y)
			break;
		s_top = slice_y;
	}
}


template <class P, class R>
void draw_polygon_AA(std::vector<P> &poly, R &renderer)
{
	size_t poly_sz = poly.size();
	if (poly_sz == 0)
		return;

	// find the top and bottom of the polygon
	double min_y=poly[0].y, max_y=poly[0].y;
	for (size_t i=0; i<poly_sz; ++i)
	{
		if (min_y > poly[i].y) min_y=poly[i].y;
		if (max_y < poly[i].y) max_y=poly[i].y;
	}

	std::vector<P> poly1, poly2, poly3, poly4;
	poly1.reserve(2*poly_sz);
	poly2.reserve(2*poly_sz);
	poly3.reserve(2*poly_sz);
	poly4.reserve(2*poly_sz);

	// chop this triangle up
	double s_top = min_y;
	while (true)
	{
		double slice_y = (int)s_top + 1;

		// slice the polygon
		poly1.clear();
		poly2.clear();
		slice_polygon_y(poly, poly1, poly2, slice_y);

		// poly1 is the part of the triangle over the scanline. It's at most
		// 1px high. Render it!
		if (poly1.size() >= 3)
		{
			// start slicing the polygon along the x-axis
			double min_x = poly1[0].x, max_x = poly1[0].x;
			for (size_t i=0; i<poly1.size(); ++i) {
				if (poly1[i].x < min_x) min_x = poly1[i].x;
				if (poly1[i].x > max_x) max_x = poly1[i].x;
			}

			double s_left = min_x-1;
			while (true)
			{
				double slice_x = (int)s_left + 1;

				poly3.clear();
				poly4.clear();
				slice_polygon_x(poly1, poly3, poly4, slice_x);

				// poly 3 is one pixel or less in size. Render it
				if (poly3.size() >= 3) {
					// calculate the area of the polygon
					double area = polygon_area(poly3);
					renderer.draw(poly3[1], area);
				}

				// poly 4 is the rest of the scanline
				poly1 = poly4;
				
				if (slice_x >= max_x)
					break;
				s_left = slice_x;
			}
		}

		// poly2 is the rest of the polygon, that will be used for the rest of the
		// scanlines
		poly = poly2;

		if (slice_y >= max_y)
			break;
		s_top = slice_y;
	}
}