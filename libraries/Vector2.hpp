#include <vector>

template <typename _Tp>
struct Vector2
{
    _Tp x, y;

    Vector2(void) : x(0), y(0) {}
    Vector2(const _Tp &_x, const _Tp &_y) : x(_x), y(_y) {}
    inline Vector2<_Tp> operator+(const Vector2<_Tp> &_other) const
    {
        return Vector2<_Tp>(x + _other.x, y + _other.y);
    }
};
