#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <../../core/forcore.h>
#include <QFileDialog>
#include "aboutwindow.h"
#include "resultswindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void importVariablesFromUI(vars &);
    void runSimulation();
    void exportVariablesToUI(vars &);
    void saveOutputFile();
    void openInputFile();
    void saveInputFile();
    void exportResultsToUI(vars &);
    void openExample1();
    void openExample2();
    void openExample3();
    void about();
    void saveGraphFile();
    void report(vars &);
    void saveCSV();

private:
    Ui::MainWindow *ui;
    ResultsWindow *rw;
protected:
    void  closeEvent(QCloseEvent*);
};

#endif // MAINWINDOW_H
