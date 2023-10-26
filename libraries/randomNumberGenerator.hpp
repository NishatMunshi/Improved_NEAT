#pragma once
#include <random>

template <typename _Tp>
class RandomNumberGenerator
{
    std::mt19937 m_rng; // seed

    std::uniform_int_distribution<_Tp> m_udist;
    const unsigned m_maxValue;

public:
    RandomNumberGenerator(const _Tp _min, const _Tp _max) : m_maxValue(_max)
    {
        m_udist = std::uniform_int_distribution<_Tp>(_min, _max);
        std::srand(time(0));
        const _Tp seedval = std::rand(); // get this from somewhere
        m_rng.seed(seedval);
    }
    inline _Tp generate(void) { return m_udist(m_rng); }
    _Tp generate(const _Tp _min, const _Tp _max) // generates a random _Tp between _min and _max, both inclusive
    {
        std::uniform_int_distribution<_Tp> uintdist(_min, _max);
        return uintdist(m_rng);
    }
    auto max_value(void) const { return m_maxValue; }
   
};

RandomNumberGenerator<int32_t> random_32(INT32_MIN, INT32_MAX);
RandomNumberGenerator<uint32_t> random_U32(0u, UINT32_MAX);
RandomNumberGenerator<uint32_t> random_bool(0, 1);

inline auto random_float(void)
{
    return float(random_32.generate())/INT32_MIN;
}