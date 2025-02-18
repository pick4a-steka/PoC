#pragma once

#include "entering_parameters.h"

class FileFinder {
public:
    explicit FileFinder(const QString &mask, const QString &path);
    ~FileFinder();

    QStringList mask_search();
private:
    QString mask;
    QString path;
};
