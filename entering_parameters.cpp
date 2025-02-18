#include "entering_parameters.h"
#include "file_finder.h"
#include "xor.h"
#include "windows.h"

EnterWindow::EnterWindow(int width, int height, QWidget *parent) : QWidget(parent) {
    setFixedSize(width, height);

    this->setStyleSheet("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
                        "stop:0 #142850, stop:1 #27496D);"
                        "color: white;");

    QLabel *label_mask = new QLabel("Введите маску для входных файлов:");
    line_enter_mask = new QLineEdit(this);
    auto *layout_mask = new QVBoxLayout();
    layout_mask->addWidget(label_mask);
    layout_mask->addWidget(line_enter_mask);

    QGroupBox *group_remove = new QGroupBox("Удалить входные файлы?");
    radio_yes = new QRadioButton("Да");
    radio_no = new QRadioButton("Нет");
    auto *layout_remove_input = new QVBoxLayout();
    layout_remove_input->addWidget(radio_yes);
    layout_remove_input->addWidget(radio_no);
    group_remove->setLayout(layout_remove_input);

    auto *label_path_input = new QLabel("Путь к папке с исходными файлами:");
    path_input = new QLineEdit(this);
    path_input->setReadOnly(true);
    browse_input_directory = new QPushButton(this);
    browse_input_directory->setText("...");
    auto *layout_path_input = new QVBoxLayout();
    auto *layout_browse_input = new QHBoxLayout();
    layout_browse_input->addWidget(path_input);
    layout_browse_input->addWidget(browse_input_directory);
    layout_path_input->addWidget(label_path_input);
    layout_path_input->addLayout(layout_browse_input);

    QLabel *label_path_output = new QLabel("Путь для сохранения выходных файлов:");
    path_output = new QLineEdit(this);
    path_output->setReadOnly(true);
    browse_output_directory = new QPushButton(this);
    browse_output_directory->setText("...");
    auto *layout_path_output = new QVBoxLayout();
    auto *layout_browse_output = new QHBoxLayout();
    layout_browse_output->addWidget(path_output);
    layout_browse_output->addWidget(browse_output_directory);
    layout_path_output->addWidget(label_path_output);
    layout_path_output->addLayout(layout_browse_output);

    QGroupBox *group_repeat = new QGroupBox("При повторении имени файл:");
    radio_delete = new QRadioButton("Удалить");
    radio_mod = new QRadioButton("Добавить");
    auto *layout_repeat_name = new QVBoxLayout();
    layout_repeat_name->addWidget(radio_delete);
    layout_repeat_name->addWidget(radio_mod);
    group_repeat->setLayout(layout_repeat_name);

    QLabel *label_timer = new QLabel("Запуск модификации файлов:");
    box_timer = new QComboBox(this);
    box_timer->addItems({"Разовый запуск", "Каждую 1 минуту", "Каждые 5 минут", "Каждые 10 минут", "Каждые 30 минут", "Каждый 1 час"});
    auto *layout_timer = new QVBoxLayout();
    layout_timer->addWidget(label_timer);
    layout_timer->addWidget(box_timer);

    QLabel *label_modification = new QLabel("Введите 8-байтовое значение для операции модификации:");
    enter_modification = new QLineEdit(this);
    auto *layout_modification = new QVBoxLayout();
    layout_modification->addWidget(label_modification);
    layout_modification->addWidget(enter_modification);

    btn_reset = new QPushButton(this);
    btn_cancel = new QPushButton(this);
    btn_done = new QPushButton(this);
    btn_reset->setText("Сброс");
    btn_cancel->setText("Отмена");
    btn_done->setText("Готово");

    auto *layout_btns = new QHBoxLayout();
    layout_btns->addWidget(btn_reset);
    layout_btns->addWidget(btn_cancel);
    layout_btns->addWidget(btn_done);

    auto *layout_main = new QVBoxLayout(this);
    layout_main->addLayout(layout_mask);
    layout_main->addWidget(group_remove);
    layout_main->addLayout(layout_path_input);
    layout_main->addLayout(layout_path_output);
    layout_main->addWidget(group_repeat);
    layout_main->addLayout(layout_timer);
    layout_main->addLayout(layout_modification);
    layout_main->addLayout(layout_btns);

    connect(btn_reset, &QPushButton::clicked, this, &EnterWindow::reset);
    connect(btn_cancel, &QPushButton::clicked, this, &QApplication::quit);
    connect(btn_done, &QPushButton::clicked, this, &EnterWindow::setup_timer);
    connect(browse_input_directory, &QPushButton::clicked, this, [=]() {
        choose_directory(path_input);
    });
    connect(browse_output_directory, &QPushButton::clicked, this, [=]() {
        choose_directory(path_output);
    });
}

// сброс настроек
void EnterWindow::reset() {
    line_enter_mask->clear();

    reset_radio(radio_yes);
    reset_radio(radio_no);
    reset_radio(radio_delete);
    reset_radio(radio_mod);

    path_input->clear();
    path_output->clear();

    box_timer->setCurrentIndex(0);
    enter_modification->clear();
}

void EnterWindow::reset_radio(QRadioButton *r) {
    r->setAutoExclusive(false);
    r->setChecked(false);
    r->setAutoExclusive(true);
}

void EnterWindow::choose_directory(QLineEdit *path) {
    QString directory = QFileDialog::getExistingDirectory(this, "Выбирите папку", path->text());

    if (!directory.isEmpty()) {
        path->setText(directory);
    }
}

// проверка введенного пользователем ключа
bool EnterWindow::check_key() {
    QString key_string = enter_modification->text();

    if (key_string.length() != 16) {
       QMessageBox::warning(this, "Ошибка", "Введите ровно 8 байт!");
       return false;
    }

    QRegularExpression valid_chars("^[0-9A-Fa-f]{16}$");
    if (!valid_chars.match(key_string).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный HEX-ключ!");
        return false;
    }

    return true;
}

// защита от дурака (если файл заблокирован другим процессом модефицирован он не будет)
bool EnterWindow::is_file_locked(const QString &file_path) {
    HANDLE hFile = CreateFileW(file_path.toStdWString().c_str(), GENERIC_READ | GENERIC_WRITE,
                               0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        qDebug() << "Файл" << file_path << "заблокирован другим процессом!";
        return true;
    }

    CloseHandle(hFile);
    return false;
}

bool EnterWindow::are_fields_filled() {
    if (line_enter_mask->text().isEmpty()) {
        qDebug() << "Ошибка: Маска файлов не заполнена!";
        return false;
    }
    if (path_input->text().isEmpty()) {
        qDebug() << "Ошибка: Путь к входной папке не указан!";
        return false;
    }
    if (path_output->text().isEmpty()) {
        qDebug() << "Ошибка: Путь к выходной папке не указан!";
        return false;
    }
    if (enter_modification->text().isEmpty()) {
        qDebug() << "Ошибка: Ключ модификации должен быть 8 символов!";
        return false;
    }
    if (!radio_yes->isChecked() && !radio_no->isChecked()) {
        qDebug() << "Ошибка: Не выбрано, удалять ли входные файлы!";
        return false;
    }
    if (!radio_delete->isChecked() && !radio_mod->isChecked()) {
        qDebug() << "Ошибка: Не выбрано, что делать при повторении имени!";
        return false;
    }

    return true; // все поля заполнены корректно
}

// установка таймера
void EnterWindow::setup_timer() {
    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &EnterWindow::run);
    connect(box_timer, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EnterWindow::update_timer);

    update_timer(box_timer->currentIndex());
}

// установка интервала таймера
void EnterWindow::update_timer(int index) {
    if (!check_key()) {
        return;
    }

    if (!are_fields_filled()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля перед началом операции!");
        return;
    }

    timer->stop();

    int interval = 0;
    switch (index) {
        case 0: // разовый запуск
            run();
            return;
        case 1: interval = 60 * 1000; break;
        case 2: interval = 5 * 60 * 1000; break;
        case 3: interval = 10 * 60 * 1000; break;
        case 4: interval = 30 * 60 * 1000; break;
        case 5: interval = 60 * 60 * 1000; break;
    }

    run();

    timer->start(interval);
    qDebug() << "Таймер запущен с интервалом" << interval / 1000 << "сек";
}

// запуск модификации
void EnterWindow::run() {
    QString mask = line_enter_mask->text();
    QString input_path = path_input->text();
    QString output_path = path_output->text();

    remove_original = radio_yes->isChecked();
    overwrite_existing = radio_delete->isChecked();

    FileFinder file_finder(mask, input_path);
    QStringList files = file_finder.mask_search();

    if (files.size() == 0) {
        QMessageBox::warning(this, "Ошибка", "По указанной маске нет файлов в исходной папке!");
        return;
    }

    QString key_string = enter_modification->text();
    key_bytes = QByteArray::fromHex(key_string.toUtf8());

    XOR operation_xor(key_bytes, this);

    QDir inputDir(input_path);
    for (const QString &file : std::as_const(files)) {
        QString file_path = inputDir.filePath(file);
        if (is_file_locked(file_path)) {
            continue;
        }
        operation_xor.xor_modify_file(file_path, output_path, remove_original, overwrite_existing);
    }
}
