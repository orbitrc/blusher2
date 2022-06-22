#ifndef _BL_POINT_H
#define _BL_POINT_H

namespace bl {

class Point
{
public:
    Point(double x, double y);

    double x() const;
    double y() const;

    void set_x(double x);
    void set_y(double y);

private:
    double _x;
    double _y;
};

} // namespace bl

#endif // _BL_POINT_H
