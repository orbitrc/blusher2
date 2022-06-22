#ifndef _BL_SIZE_H
#define _BL_SIZE_H

namespace bl {

class Size
{
public:
    Size(double width, double height);

    double width() const;
    double height() const;

    void set_width(double width);
    void set_height(double height);

private:
    double _width;
    double _height;
};

} // namespace bl

#endif // _BL_SIZE_H
