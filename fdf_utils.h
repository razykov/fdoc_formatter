#ifndef FDF_UTILS_H
#define FDF_UTILS_H

#include <QString>

#include "mainwindow.h"
#include "ui_mainwindow.h"

void fdf_text_formatter(Ui::MainWindow *ui, QString * text);
void fdf_clean_src(QString * text);

#endif // FDF_UTILS_H
