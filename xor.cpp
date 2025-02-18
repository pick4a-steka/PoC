#include "xor.h"

XOR::XOR(const QByteArray &key, EnterWindow *ew) : key(key), ew(ew) {}

XOR::~XOR() {}

// логика модификации
void XOR::xor_modify_file(const QString &input_path, const QString &output_dir, bool remove_original, bool overwrite_existing) {
    QFile input_file(input_path);
    if (!input_file.open(QIODevice::ReadOnly)) {
        qDebug() << "Ошибка открытия файла:" << input_file.errorString();
        QMessageBox::warning(ew, "Ошибка", input_file.errorString());
        return;
    }

    QByteArray file_data = input_file.readAll();
    input_file.close();

    qDebug() << "Размер считанных данных:" << file_data.size() << "байт из файла" << input_path;

    int key_size = key.size();
    if (!key_size) {
        qDebug() << "Ключ пустой";
        return;
    }

    if (file_data.isEmpty()) {
        qDebug() << "Файл пуст, пропускаем обработку: " << input_path;
        return;
    }

    // применяем XOR
    for (int i = 0; i < file_data.size(); ++i) {
        file_data[i] = static_cast<char>(file_data[i] ^ key[i % key_size]);
    }

    // формируем имя нового файла
    QFileInfo file_info(input_path);
    QString output_path = output_dir + "/" + file_info.baseName() + "_mod.bin";

    if (overwrite_existing) {
        // если файл существует - удаляем его
        if (QFile::exists(output_path)) {
            QFile::remove(output_path);
            qDebug() << "Файл перезаписан:" << output_path;
        }
    }
    else {
        // если файл существует - добавляем префикс
        int cnt = 1;
        while (QFile::exists(output_path)) {
            output_path = output_dir + "/" + file_info.baseName() + "_mod_" + QString::number(cnt) + ".bin";
            cnt++;
        }
        qDebug() << "Файл сохранен с новым именем:" << output_path;
    }

    QFile output_file(output_path);
    if (!output_file.open(QIODevice::WriteOnly)) {
        qDebug() << "Ошибка создания файла:" << output_file.errorString();
        QMessageBox::warning(ew, "Ошибка", output_file.errorString());
        return;
    }

    output_file.write(file_data); // запись измененных данных
    output_file.close();

    if (remove_original) {
        if (QFile::remove(input_path)) {
            qDebug() << "Оригинальный файл удален:" << input_path;
        }
        else {
            qDebug() << "Не удалось найти оригинальный файл";
        }
    }
}
