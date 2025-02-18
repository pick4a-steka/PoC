#pragma once

#include "entering_parameters.h"

class XOR {
public:
    explicit XOR(const QByteArray &key, EnterWindow *ew);
    ~XOR();

    void xor_modify_file(const QString &input_path, const QString &output_dir, bool remove_original, bool overwrite_existing);
private:
    QByteArray key;
    EnterWindow *ew;
};
