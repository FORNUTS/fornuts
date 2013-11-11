#include "aboutwindow.h"
#include "ui_aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent) :
    QDialog(parent,Qt::WindowCloseButtonHint),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    QString filename = ":/images/images/fournuts.png";
    QImage image(filename);
    ui->graphics->setPixmap(QPixmap::fromImage(image.scaledToWidth(120)));
}

AboutWindow::~AboutWindow()
{
    delete ui;
}
