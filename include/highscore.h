#ifndef HIGHSCORE_H
#define HIGHSCORE_H

/*
 * ================================================================
 *  UnlimitedTetris — include/highscore.h
 *  Reading and writing the persistent best score.
 * ================================================================
 *  Three screens print a best score, which before this existed
 *  was always the best of the current session and was gone the
 *  moment the window closed.
 *
 *  The parsing is separated from the file handling so that the
 *  awkward half — what to do with an empty file, a negative
 *  number, a number larger than an int, or somebody's shopping
 *  list — is testable without touching a disk.
 * ================================================================
 */

#include <string>

// Where the record lives: beside the executable, so a copied
// build directory carries its scores with it. Falls back to the
// working directory if the executable path cannot be determined.
std::string highScorePath();

// Interpret the contents of a score file. Anything that is not a
// plain non-negative integer, with optional surrounding
// whitespace, reads as 0 — a corrupt file loses the record rather
// than refusing to start the game. Values above INT_MAX clamp.
int parseHighScore(const std::string& text);

// The on-disk form: the number and a trailing newline.
std::string formatHighScore(int value);

// Read the file at `path`, or 0 if it is missing or unreadable.
int loadHighScore(const std::string& path);

// Write `value` to `path`. Returns false if it could not be
// written; the caller is expected to carry on regardless, because
// a game that will not run on a read-only drive is worse than one
// that forgets its record.
bool saveHighScore(const std::string& path, int value);

#endif // HIGHSCORE_H
