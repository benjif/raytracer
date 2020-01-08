#ifndef _LINEAR_H
#define _LINEAR_H

struct XYZ {
    double x;
    double y;
    double z;

    XYZ normal() const;
    double magnitude() const;

    XYZ operator+(double) const;
    XYZ operator-(double) const;
    XYZ operator*(double) const;
    XYZ operator/(double) const;
    XYZ operator+(const XYZ &) const;
    XYZ operator-(const XYZ &) const;
    XYZ operator-() const;
};

double dot(const XYZ &, const XYZ &);
XYZ cross(const XYZ &, const XYZ &);
double distance(const XYZ &, const XYZ &);

#endif
