#ifndef UT_HARNESS_H
#define UT_HARNESS_H

/*
 * ================================================================
 *  UnlimitedTetris — tests/harness.h
 *
 *  A dependency-free assertion harness. The project had no test
 *  framework, and adding one would be a larger imposition than
 *  sixty lines of this.
 *
 *  stdout is unbuffered on purpose: a suite run against unfixed
 *  code is exactly when the process dies partway through, and
 *  which check it reached is the interesting part.
 * ================================================================
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace ut {

inline int& passCount() { static int n = 0; return n; }
inline int& failCount() { static int n = 0; return n; }

inline void begin(const char* suiteName)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\n=== %s ===\n", suiteName);
}

inline void section(const char* name) { printf("\n-- %s\n", name); }

inline void ok(const char* what)
{
    ++passCount();
    printf("  ok   %s\n", what);
}

inline void fail(const char* what, const std::string& detail)
{
    ++failCount();
    printf("  FAIL %s\n       %s\n", what, detail.c_str());
}

inline void check(bool cond, const char* what)
{
    if (cond) ok(what); else fail(what, "expected true, got false");
}

inline void eqInt(long long actual, long long expected, const char* what)
{
    if (actual == expected) { ok(what); return; }
    char buf[160];
    snprintf(buf, sizeof buf, "expected %lld, got %lld", expected, actual);
    fail(what, buf);
}

inline void eqStr(const std::string& actual, const std::string& expected,
                  const char* what)
{
    if (actual == expected) { ok(what); return; }
    fail(what, "expected \"" + expected + "\", got \"" + actual + "\"");
}

inline void contains(const std::string& haystack, const std::string& needle,
                     const char* what)
{
    if (haystack.find(needle) != std::string::npos) { ok(what); return; }
    fail(what, "\"" + needle + "\" does not appear in what was drawn");
}

// Relative tolerance, not absolute: an absolute epsilon accepts zero
// for a value of 5e-10, which is the wrong answer a float check is
// there to catch.
inline void nearFloat(double actual, double expected, const char* what,
                      double rel = 1e-6)
{
    double scale = std::fabs(expected) > 1.0 ? std::fabs(expected) : 1.0;
    if (std::fabs(actual - expected) <= rel * scale) { ok(what); return; }
    char buf[160];
    snprintf(buf, sizeof buf, "expected %g, got %g", expected, actual);
    fail(what, buf);
}

inline int report()
{
    printf("\n%d passed, %d failed\n", passCount(), failCount());
    return failCount() == 0 ? 0 : 1;
}

} // namespace ut

#endif // UT_HARNESS_H
