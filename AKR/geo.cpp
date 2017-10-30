#include "geo.h"
namespace geo{
	int getcircle(point &a, point &b, point &c, point &O, double &r) {
		double a1 = 2.0*(a.x - b.x);
		double b1 = 2.0*(a.y - b.y);
		double c1 = a.x*a.x - b.x*b.x + a.y*a.y - b.y*b.y;
		double a2 = 2.0*(a.x - c.x);
		double b2 = 2.0*(a.y - c.y);
		double c2 = a.x*a.x - c.x*c.x + a.y*a.y - c.y*c.y;
		O.x = (c1*b2 - c2*b1) / (a1*b2 - a2*b1);
		O.y = (c1*a2 - c2*a1) / (b1*a2 - b2*a1);
		r = (a - O).len();
		return 0;
	}

	point findcircle(std::vector<point> &pt) {
		double eps = 1e-8;
		int n = pt.size();
		std::random_shuffle(pt.begin(), pt.end());
		double r = 0.0;
		point O = pt[0];
		for (int i = 1; i < n; i++) {
			if ((pt[i] - O).len() - r > -eps) {
				O.x = (pt[0].x + pt[i].x) / 2.0;
				O.y = (pt[0].y + pt[i].y) / 2.0;
				r = (O - pt[0]).len();
				for (int j = 0; j < i; j++) {
					if ((pt[j] - O).len() - r > -eps) {
						O.x = (pt[i].x + pt[j].x) / 2.0;
						O.y = (pt[i].y + pt[j].y) / 2.0;
						r = (O - pt[i]).len();
						for (int k = 0; k < j; k++) {
							if ((pt[k] - O).len() - r > -eps) {
								getcircle(pt[i], pt[j], pt[k], O, r);
							}
						}
					}
				}
			}
		}
		return O;
	}
}