#pragma once

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QFileDialog>
#include <QComboBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QDir>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QApplication>
#include <QTimer>

class EnterWindow : public QWidget {
    Q_OBJECT

public:
    explicit EnterWindow(int width, int height, QWidget *parent = nullptr);
    virtual ~EnterWindow() {};

    bool check_key();
    bool is_file_locked(const QString &file_path);
    bool are_fields_filled();

private slots:
    void reset();
    void reset_radio(QRadioButton *r);
    void choose_directory(QLineEdit *path);
    void update_timer(int index);
    void setup_timer();
    void run();

private:
    QLineEdit *line_enter_mask;

    QRadioButton *radio_yes;
    QRadioButton *radio_no;

    QLineEdit *path_input;
    QPushButton *browse_input_directory;

    QLineEdit *path_output;
    QPushButton *browse_output_directory;

    QRadioButton *radio_delete;
    QRadioButton *radio_mod;

    QComboBox *box_timer;

    QLineEdit *enter_modification;

    QPushButton *btn_done;
    QPushButton *btn_reset;
    QPushButton *btn_cancel;

    QByteArray key_bytes;
    bool overwrite_existing;
    bool remove_original;

    QTimer *timer;
};
