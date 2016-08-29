
#ifndef UTILS_H__
#define UTILS_H__

#include "tDefs.h"

namespace BaseSimulator {

//!< utils Utilities namespace for providing globally needed constants, types, and helper methods   
namespace utils {

#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795 //!< 31-digit pi constant
#endif

#define IS_ODD(x) ((x) % 2)     //!< returns 1 if x is odd, 0 otherwise
#define IS_EVEN(x) (!IS_ODD(x)) //!< returns 1 if x is even, 0 otherwise

//!< @brief Return true if a <= x <= b, false otherwise
inline static bool isInRange(int x, int a, int b) { return (a <= x && x <= b); };

const float M_SQRT2_2 = sqrt(2.0) / 2.0; //!< $\frac{\sqrt{2}}{2}$
const double M_SQRT3_2 = sqrt(3.0) / 2.0; //!< $\frac{\sqrt{3}}{2}$
const double EPS = 1E-5;                  //!< Epsilon

//!< Return true if file at path fileName exists and can be read, false otherwise
inline static bool file_exists(const string fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

//!< An exception for marking functions as not implemented
struct NotImplementedException : std::exception {
    const char* what() const noexcept { return "not yet implemented!\n"; }
};

//!< Swaps the value of two pointers a and b
inline static void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

} // namespace utils

} // namespace BaseSimulator

#endif
