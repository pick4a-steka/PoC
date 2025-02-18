#include "file_finder.h"

FileFinder::FileFinder(const QString &mask, const QString &path): mask(mask), path(path) {}

FileFinder::~FileFinder() {}

// поиск файла по маске
QStringList FileFinder::mask_search() {
    QDir dir(path);
    return dir.entryList(QStringList() << mask, QDir::Files);
}
