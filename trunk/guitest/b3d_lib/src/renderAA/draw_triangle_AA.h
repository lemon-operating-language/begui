#pragma once

#include "common.h"

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

	// run a scanline
	double s_start = p1.y;
	while (true)
	{
		// s_start defines the top of the scanline. find the bottom of it
		double s_end = (int)s_start + 1;

		if (s_end > p3.y)
			s_end = p3.y;

		// now, the top and bottom of the scanline have been defined. Define also
		// the left and right bounds:
		P top0, top1, bottom0, bottom1;
		if (s_end <= p2.y)
		{
			double tt1 = (s_start-p1.y)/(p3.y-p1.y);
			top0 = (1-tt1)*p1 + tt1*p3;
			double tt2 = (s_start-p1.y)/(p2.y-p1.y);
			top1 = (1-tt2)*p1 + tt2*p2;
			
			double bt1 = (s_end-p1.y)/(p3.y-p1.y);
			bottom0 = (1-bt1)*p1 + bt1*p3;
			double bt2 = (s_end-p1.y)/(p2.y-p1.y);
			bottom1 = (1-bt2)*p1 + bt2*p2;
		}
		else
		{
			double tt1 = (s_start-p1.y)/(p3.y-p1.y);
			top0 = (1-tt1)*p1 + tt1*p3;
			double tt2 = (s_start-p2.y)/(p3.y-p2.y);
			top1 = (1-tt2)*p2 + tt2*p3;
			
			double bt1 = (s_end-p1.y)/(p3.y-p1.y);
			bottom0 = (1-bt1)*p1 + bt1*p3;
			double bt2 = (s_end-p2.y)/(p3.y-p2.y);
			bottom1 = (1-bt2)*p2 + bt2*p3;
		}

		// the scanline is defined now as a trapezoid
		// rasterize it by cutting it in pixels

		// swap the points to make sure 0 is on the left, 1 on the right
		if (bottom1.x - bottom0.x < -0.001 || top1.x - top0.x < -0.001) {
			std::swap(top0, top1);
			std::swap(bottom0, bottom1);
		}

		double xleft = (top0.x < bottom0.x) ? top0.x : bottom0.x;
		double xright = (top1.x > bottom1.x) ? top1.x : bottom1.x;

		// draw the scanline
		double x0 = (int)xleft;
		double x1 = x0+1;
		while (x0 < xright)
		{
			if (x1 > xright)
				x1 = xright;

			// our pixel is xleft->xright, s_start->s_end
			bool bFullX = true, bFullY = true;

			// calculate the percentage of the pixel that is covered
			double x0top = x0;
			double x0bottom = x0;
			double x1top = x1;
			double x1bottom = x1;
			double y0top = s_start, y1top = s_start;
			double y0bottom = s_end, y1bottom = s_end;
			if (x0top < top0.x) {
				x0top = top0.x;
				if (x0top > x1) {
					x0top = x1;
					// change y0top
					double f1 = (x1 - bottom0.x)/(top0.x - bottom0.x);
					y0top = f1*top0.y + (1-f1)*bottom0.y;
					bFullY = false;
				}
				else
					bFullX = false;
			}
			if (x0bottom < bottom0.x) {
				x0bottom = bottom0.x;
				if (x0bottom > x1) {
					x0bottom = x1;
					// change y0bottom
					double f1 = (x1 - xleft)/(xright - xleft);
					y0bottom = f1*bottom0.y + (1-f1)*bottom1.y;
					bFullY = false;
				}
				else
					bFullX = false;
			}
			if (x1top > top1.x) {
				x1top = top1.x;
				if (x1top < x0) {
					x1top = x0;
					// change y1top
					double f0 = (x0 - bottom1.x)/(top1.x - bottom1.x);
					y1top = f0*top1.y + (1-f0)*bottom1.y;
					bFullY = false;
				}
				else
					bFullX = false;
			}
			if (x1bottom > bottom1.x) {
				x1bottom = bottom1.x;
				if (x1bottom < x0) {
					x1bottom = x0;
					// change y1bottom
					double f0 = (x0 - xleft)/(xright - xleft);
					y1bottom = f0*bottom0.y + (1-f0)*bottom1.y;
					bFullY = false;
				}
				else
					bFullX = false;
			}

			double coverage = 1;
			if (!bFullX)
			{
				// calculate coverage. The 4 values form a trapezoid, with height = s_end - s_start
				double height = s_end - s_start;
				coverage = (x1top - x0top + x1bottom - x0bottom)*height / 2.0;

				if (coverage< 0 || coverage > 1)
					Console::print("perc = %f\n", coverage);
			}
			else if (!bFullY) {
				coverage = (y1bottom - y1top + y0bottom - y0top) * (x1-x0) / 2.0;
			}

			// render the pixel
			renderer.draw(Vector2((int)x0top, (int)s_start), coverage);

			// progress to the next pixel
			x0 = x1;
			x1 = x0+1;
		}

		if (s_end >= p3.y)
			break;

		// progress to the next scanline
		s_start = s_end;
	}
}