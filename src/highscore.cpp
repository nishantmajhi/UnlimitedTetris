/*
 * ================================================================
 *  UnlimitedTetris — src/highscore.cpp
 *  Reading and writing the persistent best score.
 * ================================================================
 */

#include "../include/highscore.h"

#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

namespace {

const char* kFileName = "highscore.txt";

// The directory the running executable sits in, with its trailing
// separator, or "" if it cannot be worked out. Not the working
// directory: a shortcut or a drag-and-drop launch can set that to
// anywhere, and the record would then be somewhere else each time.
std::string executableDir()
{
#ifdef _WIN32
    wchar_t buf[MAX_PATH];
    DWORD n = GetModuleFileNameW(NULL, buf, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return std::string();

    int bytes = WideCharToMultiByte(CP_UTF8, 0, buf, (int)n,
                                    NULL, 0, NULL, NULL);
    if (bytes <= 0) return std::string();
    std::string path((size_t)bytes, '\0');
    WideCharToMultiByte(CP_UTF8, 0, buf, (int)n, &path[0], bytes, NULL, NULL);

    size_t cut = path.find_last_of("\\/");
    if (cut == std::string::npos) return std::string();
    return path.substr(0, cut + 1);
#else
    char buf[4096];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof buf - 1);
    if (n <= 0) return std::string();
    buf[n] = '\0';
    std::string path(buf);
    size_t cut = path.find_last_of('/');
    if (cut == std::string::npos) return std::string();
    return path.substr(0, cut + 1);
#endif
}

} // namespace

std::string highScorePath()
{
    return executableDir() + kFileName;
}

int parseHighScore(const std::string& text)
{
    size_t i = 0;
    while (i < text.size() && isspace((unsigned char)text[i])) ++i;
    if (i >= text.size()) return 0;
    if (!isdigit((unsigned char)text[i])) return 0;   // '-' included: no negative records

    long long value = 0;
    size_t digits = 0;
    for (; i < text.size() && isdigit((unsigned char)text[i]); ++i, ++digits) {
        if (value <= INT_MAX) value = value * 10 + (text[i] - '0');
        if (value > INT_MAX) value = (long long)INT_MAX + 1;   // saturate, keep consuming
    }
    if (digits == 0) return 0;

    while (i < text.size() && isspace((unsigned char)text[i])) ++i;
    if (i != text.size()) return 0;                   // trailing junk: not a score file

    return value > INT_MAX ? INT_MAX : (int)value;
}

std::string formatHighScore(int value)
{
    if (value < 0) value = 0;
    return std::to_string(value) + "\n";
}

int loadHighScore(const std::string& path)
{
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return 0;

    // A score file is a handful of bytes. Anything larger is not
    // one, and is not worth reading into memory to find out.
    char buf[64];
    size_t n = fread(buf, 1, sizeof buf - 1, f);
    bool overlong = (n == sizeof buf - 1) && (fgetc(f) != EOF);
    fclose(f);
    if (overlong) return 0;

    buf[n] = '\0';
    return parseHighScore(std::string(buf, n));
}

bool saveHighScore(const std::string& path, int value)
{
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) return false;
    std::string text = formatHighScore(value);
    size_t written = fwrite(text.data(), 1, text.size(), f);
    bool ok = (written == text.size());
    if (fclose(f) != 0) ok = false;
    return ok;
}
