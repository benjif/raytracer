#ifndef _LINEAR_H
#define _LINEAR_H

struct XYZ {
    double x;
    double y;
    double z;

    XYZ();
    XYZ(double, double, double);

    XYZ operator+(double) const;
    XYZ operator-(double) const;
    XYZ operator*(double) const;
    XYZ operator/(double) const;
    XYZ operator+(const XYZ &) const;
    XYZ operator-(const XYZ &) const;
    XYZ operator-() const;
};

double dot(const XYZ &a, const XYZ &b);
double distance(const XYZ &a, const XYZ &b);

#endif
