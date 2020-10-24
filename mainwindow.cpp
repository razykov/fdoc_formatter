#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fdf_utils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->formatButton, SIGNAL(clicked()), this, SLOT (docFormat()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::docFormat()
{
    QString text = ui->docContent->document()->toPlainText();

    fdf_clean_src(&text);
    fdf_text_formatter(ui, &text);

    ui->docContent->document()->setPlainText(text);
}
