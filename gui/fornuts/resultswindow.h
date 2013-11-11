#ifndef RESULTSWINDOW_H
#define RESULTSWINDOW_H

#include <QDialog>

namespace Ui {
class ResultsWindow;
}

class ResultsWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit ResultsWindow(QWidget *parent = 0);
    ~ResultsWindow();

    Ui::ResultsWindow *ui;
};

#endif // RESULTSWINDOW_H
