#include "mainwindow.h"
#include "qcustomplot.h"
#include <../../core/forcore.cpp>
#include "ui_mainwindow.h"
#include "ui_resultswindow.h"
#include <QMessageBox>
#include <QDesktopWidget>
using std::ifstream;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //New results window and screen placement.
    rw = new ResultsWindow();

    //Desktop size
    QDesktopWidget *d= QApplication::desktop();
    int ws=d->width();
    int h=d->height();

    //Mainwindow size
    QSize size=this->size();
    int mw=size.width();
    int mh=size.height();
    int cw=(ws/2)-(mw/2);
    int ch=(h/2)-(mh/2);

    //Results window size
    QSize rwsize=rw->size();
    int rwmw=rwsize.width();
    int rwmh=rwsize.height();
    int rwcw=(ws/2)-(rwmw/2);
    int rwch=(h/2)-(rwmh/2);

    //Move windows to correct location
    this->move(cw-mw/2-16,ch);
    rw->move(rwcw+rwmw/2,rwch);
    rw->show();

    rw->ui->Results->removeTab(1);
    rw->ui->Results->removeTab(1);
    rw->ui->Results->removeTab(1);
    rw->ui->Results->removeTab(1);

    //set up first graph
    rw->ui->nPoolPlot->addGraph();
    rw->ui->nPoolPlot->setTitle("Nitrogen Pool");
    rw->ui->nPoolPlot->yAxis->setLabel("Nitrogen Pool (KG/HA)");
    rw->ui->nPoolPlot->xAxis->setLabel("Rotation");
    rw->ui->nPoolPlot->xAxis->setAutoTicks(false);
    rw->ui->nPoolPlot->xAxis->setTickLabels(false);
    rw->ui->nPoolPlot->yAxis->setAutoTicks(false);
    rw->ui->nPoolPlot->yAxis->setTickLabels(false);
    rw->ui->nPoolPlot->replot();

    //set up second graph
    rw->ui->biomassPlot->addGraph();
    rw->ui->biomassPlot->setTitle("Biomass Yield");
    rw->ui->biomassPlot->yAxis->setLabel("Biomass Yeild (KG/HA)");
    rw->ui->biomassPlot->xAxis->setLabel("Rotation");
    rw->ui->biomassPlot->xAxis->setTickLabels(false);
    rw->ui->biomassPlot->xAxis->setAutoTicks(false);
    rw->ui->biomassPlot->yAxis->setTickLabels(false);
    rw->ui->biomassPlot->yAxis->setAutoTicks(false);
    rw->ui->biomassPlot->replot();

    //Run the simulation on startup.
    runSimulation();
}

void   MainWindow::closeEvent(QCloseEvent*)
{
    qApp->quit();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete rw;
}

void printOutput(vars & progVars)
{

        progVars.NREMSL = 0, progVars.NRMSCR = 0, progVars.NREMSS = 0,
            progVars.NREMSB = 0, progVars.NPBURN = 0, progVars.NSBURN = 0,
            progVars.RAINNS = 0, progVars.FERTNS = 0, progVars.NETDNS = 0,
            progVars.OUTNS = 0, progVars.TLREM = 0, progVars.TBREM = 0,
            progVars.TSREM = 0,	progVars.TCRREM = 0, progVars.NREMSI = 0;

        for(int i=0; i<progVars.NUM; ++i)
        {
            progVars.NREMSL=progVars.NREMSL+progVars.NREML[i];
            progVars.NRMSCR=progVars.NRMSCR+progVars.NREMCR[i];
            progVars.NREMSS=progVars.NREMSS+progVars.NREMS[i];
            progVars.NREMSB=progVars.NREMSB+progVars.NREMB[i];
            progVars.NPBURN=progVars.NPBURN+progVars.NPBHAR[i];
            progVars.NSBURN=progVars.NSBURN+progVars.NSBHAR[i];
            progVars.RAINNS=progVars.RAINNS+progVars.RAINNR[i];
            progVars.FERTNS=progVars.FERTNS+progVars.FERTNR[i];
            progVars.OUTNS=progVars.OUTNS+progVars.OUTNR[i];
            progVars.TLREM=progVars.TLREM+progVars.HLBMAS[i];
            progVars.TBREM=progVars.TBREM+progVars.HBBMAS[i];
            progVars.TSREM=progVars.TSREM+progVars.HSBMAS[i];
            progVars.TCRREM=progVars.TCRREM+progVars.HCRMAS[i];
            progVars.NREMSI=progVars.NREMOV[i]+progVars.NREMSI;
        }

        progVars.TBMNRS=progVars.NREMSL+progVars.NRMSCR+progVars.NREMSS+progVars.NREMSB;
        progVars.TREM=progVars.TLREM+progVars.TBREM+progVars.TSREM+progVars.TCRREM;

        progVars.BEGIN=progVars.ISAGE;

        for(int ii=0; ii<progVars.NUM; ++ii)
        {
            if(ii != 0)
                progVars.BEGIN=0;

            progVars.ENTRNR=progVars.RAINNR[ii]+progVars.FERTNR[ii];

            *progVars.fp<<endl<<"Summary of Nitrogen Budget of Site and Biomass Yields by Rotation"<<endl;
            *progVars.fp<<"-----------------------------------------------------------------"<<endl;

            progVars.SIMTIM=(ii+1)*(progVars.ROTATN)-progVars.ISAGE;
            *progVars.fp<<"Simulation Year: " << progVars.SIMTIM << endl;

            *progVars.fp<<"Initial Age of Stand: " << progVars.BEGIN << endl;

            *progVars.fp<<"Rotation Number: " << (ii+1) << endl<<endl;

            *progVars.fp<<"Inputs of Nitrogen to Site (KG/HA)     Outputs of Nitrogen From Site (KG/HA)"<<endl;
            *progVars.fp<<"----------------------------------     -------------------------------------"<<endl;

            *progVars.fp<<"Fertilization: " << progVars.FERTNR[ii] << "\t\t       Slash Burning:      " << progVars.NSBHAR[ii] <<endl;

            *progVars.fp<<"Precipitation: " << progVars.RAINNR[ii] << "\t\t       Prescribed Burning: " << progVars.NPBHAR[ii] << endl;

            *progVars.fp<<"               ------\t\t       Leaching:           " << progVars.OUTNR[ii] << endl;

            *progVars.fp<<"TOTAL:         " << progVars.ENTRNR << endl;
            *progVars.fp<<"\t\t\t\t       Losses From Biomass Removals" << endl;

            *progVars.fp<<"\t\t\t\t       Stems:        " << progVars.NREMS[ii] << endl;

            *progVars.fp<<"\t\t\t\t       Coarse Roots: " << progVars.NREMCR[ii] << endl;

            *progVars.fp<<"\t\t\t\t       Foliage:      " << progVars.NREML[ii] << endl;

            *progVars.fp<<"\t\t\t\t       Branches:     " << progVars.NREMB[ii] << endl;

            *progVars.fp<<"\t\t\t\t                     -------"<<endl;
            *progVars.fp<<"\t\t\t\t       Total:        " << progVars.NREMOV[ii] << endl;

            progVars.NETDNR=progVars.ENTRNR-progVars.NREMOV[ii]-progVars.NSBHAR[ii]-progVars.NPBHAR[ii]-progVars.OUTNR[ii];
            progVars.NETDNS=progVars.NETDNS+progVars.NETDNR;

            *progVars.fp<<endl<<"Net Change in Total Nitrogen of Site (KG/HA):  " << progVars.NETDNR << endl;

            if(ii == 0)
                progVars.SOILN[0]=progVars.ISOILN+progVars.NETDNR;
            else
                progVars.SOILN[ii]=progVars.SOILN[ii-1]+progVars.NETDNR;

            *progVars.fp<<"Soil Nitrogen Pool at End of Rotation (KG/HA): " << progVars.SOILN[ii] << endl;

            progVars.CNRATO=progVars.SOILC/progVars.SOILN[ii];
            *progVars.fp<<"Carbon:Nitrogen Ration at End of Rotation:     " << progVars.CNRATO << endl;

            *progVars.fp<<endl<<"Biomass Yields (KG/HA)"<<endl;
            *progVars.fp<<"----------------------" << endl;

            *progVars.fp<<"Stems:           " << progVars.HSBMAS[ii] << endl;

            *progVars.fp<<"Coarse Roots:    " << progVars.HCRMAS[ii] << endl;

            *progVars.fp<<"Foliage:         " << progVars.HLBMAS[ii] << endl;

            *progVars.fp<<"Branches:        " << progVars.HBBMAS[ii] << endl;

            *progVars.fp<<"                 -------"<< endl;
            *progVars.fp<<"Total:           " << progVars.HTREM[ii] << endl;

            if(progVars.NREMOV[ii] != 0.0)
                progVars.NRUBR=progVars.HTREM[ii]/progVars.NREMOV[ii];
            else
                progVars.NRUBR=0.0;

            *progVars.fp<<endl<<"Biomass Yield per Unit of Nitrogen Removed: " << progVars.NRUBR << endl;
        }

        *progVars.fp<<endl<<"Summary of Nitrogen Budget and Biomass Yields for Entire Simulation" << endl;
        *progVars.fp<<"-------------------------------------------------------------------"<<endl;

        *progVars.fp<<endl<<"Inputs of Nitrogen to Site (KG/HA)     Outputs of Nitrogen From Site (KG/HA)" << endl;
        *progVars.fp<<"----------------------------------     -------------------------------------" << endl;

        *progVars.fp<<"Fertilization: " << progVars.FERTNS << "\t\t       Slash Burning:      "  << progVars.NSBURN << endl;

        *progVars.fp<<"Precipitation: " << progVars.RAINNS << "\t\t       Prescribed Burning: " << progVars.NPBURN << endl;

        *progVars.fp<<"               ------\t\t       Leaching:           " << progVars.OUTNS << endl;

        progVars.ENTRNR=progVars.RAINNS+progVars.FERTNS;
        *progVars.fp<<"TOTAL:         " << progVars.ENTRNR << endl;
        *progVars.fp<<"\t\t\t\t       Losses From Biomass Removals" << endl;

        *progVars.fp<<"\t\t\t\t       Stems:        " << progVars.NREMSS << endl;

        *progVars.fp<<"\t\t\t\t       Coarse Roots: " << progVars.NRMSCR << endl;

        *progVars.fp<<"\t\t\t\t       Foliage:      " << progVars.NREMSL << endl;

        *progVars.fp<<"\t\t\t\t       Branches:     " << progVars.NREMSB << endl;

        *progVars.fp<<"\t\t\t\t                     -------"<<endl;
        *progVars.fp<<"\t\t\t\t       Total:        " << progVars.NREMSI << endl;

        *progVars.fp<<endl<<"Net Change in Total Nitrogen of Site (KG/HA):    " << progVars.NETDNS << endl;

        *progVars.fp<<"Soil Nitrogen Pool at End of Simulation (KG/HA): " << progVars.SOILN[progVars.NUM]<<endl;

        progVars.CNRATO=progVars.SOILC/progVars.SOILN[progVars.AGE];
        *progVars.fp<<"Carbon:Nitrogen ratio at End of Simulation:     " << progVars.CNRATO << endl;

        *progVars.fp<<endl<<"Biomass Yields (KG/HA)"<<endl<<"----------------------" << endl;

        *progVars.fp<<"Stems:        " << progVars.TSREM << endl;

        *progVars.fp<<"Coarse Roots: " << progVars.TCRREM << endl;

        *progVars.fp<<"Foliage:      " << progVars.TLREM << endl;

        *progVars.fp<<"Branches:     " << progVars.TBREM << endl;

        *progVars.fp<<"              -------"<< endl;
        *progVars.fp<<"Total:        " << progVars.TREM << endl <<endl;

        if(progVars.NREMSI != 0.0)
            progVars.NRUBS=progVars.TREM/progVars.NREMSI;
        else
            progVars.NRUBS=0.0;

        *progVars.fp<<"Biomass Yeild per Unit of Nitrogen Removed: " << progVars.NRUBS << endl;
    }

void MainWindow::report(vars & progVars)
{
    progVars.NREMSL = 0, progVars.NRMSCR = 0, progVars.NREMSS = 0,
        progVars.NREMSB = 0, progVars.NPBURN = 0, progVars.NSBURN = 0,
        progVars.RAINNS = 0, progVars.FERTNS = 0, progVars.NETDNS = 0,
        progVars.OUTNS = 0, progVars.TLREM = 0, progVars.TBREM = 0,
        progVars.TSREM = 0,	progVars.TCRREM = 0, progVars.NREMSI = 0;

    for(int i=0; i<progVars.NUM; ++i)
    {
        progVars.NREMSL=progVars.NREMSL+progVars.NREML[i];
        progVars.NRMSCR=progVars.NRMSCR+progVars.NREMCR[i];
        progVars.NREMSS=progVars.NREMSS+progVars.NREMS[i];
        progVars.NREMSB=progVars.NREMSB+progVars.NREMB[i];
        progVars.NPBURN=progVars.NPBURN+progVars.NPBHAR[i];
        progVars.NSBURN=progVars.NSBURN+progVars.NSBHAR[i];
        progVars.RAINNS=progVars.RAINNS+progVars.RAINNR[i];
        progVars.FERTNS=progVars.FERTNS+progVars.FERTNR[i];
        progVars.OUTNS=progVars.OUTNS+progVars.OUTNR[i];
        progVars.TLREM=progVars.TLREM+progVars.HLBMAS[i];
        progVars.TBREM=progVars.TBREM+progVars.HBBMAS[i];
        progVars.TSREM=progVars.TSREM+progVars.HSBMAS[i];
        progVars.TCRREM=progVars.TCRREM+progVars.HCRMAS[i];
        progVars.NREMSI=progVars.NREMOV[i]+progVars.NREMSI;
    }

    progVars.TBMNRS=progVars.NREMSL+progVars.NRMSCR+progVars.NREMSS+progVars.NREMSB;
    progVars.TREM=progVars.TLREM+progVars.TBREM+progVars.TSREM+progVars.TCRREM;

    progVars.BEGIN=progVars.ISAGE;

    for(int ii=0; ii<progVars.NUM; ++ii)
    {
        if(ii != 0)
            progVars.BEGIN=0;

        progVars.ENTRNR=progVars.RAINNR[ii]+progVars.FERTNR[ii];

        progVars.SIMTIM=(ii+1)*(progVars.ROTATN)-progVars.ISAGE;

        progVars.NETDNR=progVars.ENTRNR-progVars.NREMOV[ii]-progVars.NSBHAR[ii]-progVars.NPBHAR[ii]-progVars.OUTNR[ii];
        progVars.NETDNS=progVars.NETDNS+progVars.NETDNR;


        if(ii == 0)
            progVars.SOILN[0]=progVars.ISOILN+progVars.NETDNR;
        else
            progVars.SOILN[ii]=progVars.SOILN[ii-1]+progVars.NETDNR;

        progVars.CNRATO=progVars.SOILC/progVars.SOILN[ii];

        if(progVars.NREMOV[ii] != 0.0)
            progVars.NRUBR=progVars.HTREM[ii]/progVars.NREMOV[ii];
        else
            progVars.NRUBR=0.0;

        if(ii==0)
        {
        rw->ui->SIMTIM->setText(QString::number(progVars.SIMTIM));
        rw->ui->BEGIN->setText(QString::number(progVars.BEGIN));
        rw->ui->ROTATNNUM->setText(QString::number(ii+1));
        rw->ui->FERTNR->setText(QString::number(progVars.FERTNR[ii]));
        rw->ui->NSBHAR->setText(QString::number(progVars.NSBHAR[ii]));
        rw->ui->RAINNR->setText(QString::number(progVars.RAINNR[ii]));
        rw->ui->NPBHAR->setText(QString::number(progVars.NPBHAR[ii]));
        rw->ui->OUTNR->setText(QString::number(progVars.OUTNR[ii]));
        rw->ui->ENTRNR->setText(QString::number(progVars.ENTRNR));
        rw->ui->NREMS->setText(QString::number(progVars.NREMS[ii], 'f', 2));
        rw->ui->NREMCR->setText(QString::number(progVars.NREMCR[ii], 'f', 2));
        rw->ui->NREML->setText(QString::number(progVars.NREML[ii], 'f', 2));
        rw->ui->NREMB->setText(QString::number(progVars.NREMB[ii], 'f', 2));
        rw->ui->NREMOV->setText(QString::number(progVars.NREMOV[ii], 'f', 2));
        rw->ui->NETDNR->setText(QString::number(progVars.NETDNR, 'f', 2));
        rw->ui->SOILN->setText(QString::number(progVars.SOILN[ii], 'f', 2));
        rw->ui->CNRATO->setText(QString::number(progVars.CNRATO, 'f', 2));
        rw->ui->HSBMAS->setText(QString::number(progVars.HSBMAS[ii], 'f', 2));
        rw->ui->HCRMAS->setText(QString::number(progVars.HCRMAS[ii], 'f', 2));
        rw->ui->HLBMAS->setText(QString::number(progVars.HLBMAS[ii], 'f', 2));
        rw->ui->HBBMAS->setText(QString::number(progVars.HBBMAS[ii], 'f', 2));
        rw->ui->HTREM->setText(QString::number(progVars.HTREM[ii], 'f', 2));
        rw->ui->NRUBR->setText(QString::number(progVars.NRUBR, 'f', 2));
        }
        if(ii==1)
        {
        rw->ui->SIMTIM_2->setText(QString::number(progVars.SIMTIM));
        rw->ui->BEGIN_2->setText(QString::number(progVars.BEGIN));
        rw->ui->ROTATNNUM_2->setText(QString::number(ii+1));
        rw->ui->FERTNR_2->setText(QString::number(progVars.FERTNR[ii]));
        rw->ui->NSBHAR_2->setText(QString::number(progVars.NSBHAR[ii]));
        rw->ui->RAINNR_2->setText(QString::number(progVars.RAINNR[ii]));
        rw->ui->NPBHAR_2->setText(QString::number(progVars.NPBHAR[ii]));
        rw->ui->OUTNR_2->setText(QString::number(progVars.OUTNR[ii]));
        rw->ui->ENTRNR_2->setText(QString::number(progVars.ENTRNR));
        rw->ui->NREMS_2->setText(QString::number(progVars.NREMS[ii], 'f', 2));
        rw->ui->NREMCR_2->setText(QString::number(progVars.NREMCR[ii], 'f', 2));
        rw->ui->NREML_2->setText(QString::number(progVars.NREML[ii], 'f', 2));
        rw->ui->NREMB_2->setText(QString::number(progVars.NREMB[ii], 'f', 2));
        rw->ui->NREMOV_2->setText(QString::number(progVars.NREMOV[ii], 'f', 2));
        rw->ui->NETDNR_2->setText(QString::number(progVars.NETDNR, 'f', 2));
        rw->ui->SOILN_2->setText(QString::number(progVars.SOILN[ii], 'f', 2));
        rw->ui->CNRATO_2->setText(QString::number(progVars.CNRATO, 'f', 2));
        rw->ui->HSBMAS_2->setText(QString::number(progVars.HSBMAS[ii], 'f', 2));
        rw->ui->HCRMAS_2->setText(QString::number(progVars.HCRMAS[ii], 'f', 2));
        rw->ui->HLBMAS_2->setText(QString::number(progVars.HLBMAS[ii], 'f', 2));
        rw->ui->HBBMAS_2->setText(QString::number(progVars.HBBMAS[ii], 'f', 2));
        rw->ui->HTREM_2->setText(QString::number(progVars.HTREM[ii], 'f', 2));
        rw->ui->NRUBR_2->setText(QString::number(progVars.NRUBR, 'f', 2));
        }
        if(ii==2)
        {
        rw->ui->SIMTIM_3->setText(QString::number(progVars.SIMTIM));
        rw->ui->BEGIN_3->setText(QString::number(progVars.BEGIN));
        rw->ui->ROTATNNUM_3->setText(QString::number(ii+1));
        rw->ui->FERTNR_3->setText(QString::number(progVars.FERTNR[ii]));
        rw->ui->NSBHAR_3->setText(QString::number(progVars.NSBHAR[ii]));
        rw->ui->RAINNR_3->setText(QString::number(progVars.RAINNR[ii]));
        rw->ui->NPBHAR_3->setText(QString::number(progVars.NPBHAR[ii]));
        rw->ui->OUTNR_3->setText(QString::number(progVars.OUTNR[ii]));
        rw->ui->ENTRNR_3->setText(QString::number(progVars.ENTRNR));
        rw->ui->NREMS_3->setText(QString::number(progVars.NREMS[ii], 'f', 2));
        rw->ui->NREMCR_3->setText(QString::number(progVars.NREMCR[ii], 'f', 2));
        rw->ui->NREML_3->setText(QString::number(progVars.NREML[ii], 'f', 2));
        rw->ui->NREMB_3->setText(QString::number(progVars.NREMB[ii], 'f', 2));
        rw->ui->NREMOV_3->setText(QString::number(progVars.NREMOV[ii], 'f', 2));
        rw->ui->NETDNR_3->setText(QString::number(progVars.NETDNR, 'f', 2));
        rw->ui->SOILN_3->setText(QString::number(progVars.SOILN[ii], 'f', 2));
        rw->ui->CNRATO_3->setText(QString::number(progVars.CNRATO, 'f', 2));
        rw->ui->HSBMAS_3->setText(QString::number(progVars.HSBMAS[ii], 'f', 2));
        rw->ui->HCRMAS_3->setText(QString::number(progVars.HCRMAS[ii], 'f', 2));
        rw->ui->HLBMAS_3->setText(QString::number(progVars.HLBMAS[ii], 'f', 2));
        rw->ui->HBBMAS_3->setText(QString::number(progVars.HBBMAS[ii], 'f', 2));
        rw->ui->HTREM_3->setText(QString::number(progVars.HTREM[ii], 'f', 2));
        rw->ui->NRUBR_3->setText(QString::number(progVars.NRUBR, 'f', 2));
        }
        if(ii==3)
        {
        rw->ui->SIMTIM_5->setText(QString::number(progVars.SIMTIM));
        rw->ui->BEGIN_5->setText(QString::number(progVars.BEGIN));
        rw->ui->ROTATNNUM_5->setText(QString::number(ii+1));
        rw->ui->FERTNR_5->setText(QString::number(progVars.FERTNR[ii]));
        rw->ui->NSBHAR_5->setText(QString::number(progVars.NSBHAR[ii]));
        rw->ui->RAINNR_5->setText(QString::number(progVars.RAINNR[ii]));
        rw->ui->NPBHAR_5->setText(QString::number(progVars.NPBHAR[ii]));
        rw->ui->OUTNR_5->setText(QString::number(progVars.OUTNR[ii]));
        rw->ui->ENTRNR_5->setText(QString::number(progVars.ENTRNR));
        rw->ui->NREMS_5->setText(QString::number(progVars.NREMS[ii], 'f', 2));
        rw->ui->NREMCR_5->setText(QString::number(progVars.NREMCR[ii], 'f', 2));
        rw->ui->NREML_5->setText(QString::number(progVars.NREML[ii], 'f', 2));
        rw->ui->NREMB_5->setText(QString::number(progVars.NREMB[ii], 'f', 2));
        rw->ui->NREMOV_5->setText(QString::number(progVars.NREMOV[ii], 'f', 2));
        rw->ui->NETDNR_5->setText(QString::number(progVars.NETDNR, 'f', 2));
        rw->ui->SOILN_5->setText(QString::number(progVars.SOILN[ii], 'f', 2));
        rw->ui->CNRATO_5->setText(QString::number(progVars.CNRATO, 'f', 2));
        rw->ui->HSBMAS_5->setText(QString::number(progVars.HSBMAS[ii], 'f', 2));
        rw->ui->HCRMAS_5->setText(QString::number(progVars.HCRMAS[ii], 'f', 2));
        rw->ui->HLBMAS_5->setText(QString::number(progVars.HLBMAS[ii], 'f', 2));
        rw->ui->HBBMAS_5->setText(QString::number(progVars.HBBMAS[ii], 'f', 2));
        rw->ui->HTREM_5->setText(QString::number(progVars.HTREM[ii], 'f', 2));
        rw->ui->NRUBR_5->setText(QString::number(progVars.NRUBR, 'f', 2));
        }
        if(ii==4)
        {
        rw->ui->SIMTIM_6->setText(QString::number(progVars.SIMTIM));
        rw->ui->BEGIN_6->setText(QString::number(progVars.BEGIN));
        rw->ui->ROTATNNUM_6->setText(QString::number(ii+1));
        rw->ui->FERTNR_6->setText(QString::number(progVars.FERTNR[ii]));
        rw->ui->NSBHAR_6->setText(QString::number(progVars.NSBHAR[ii]));
        rw->ui->RAINNR_6->setText(QString::number(progVars.RAINNR[ii]));
        rw->ui->NPBHAR_6->setText(QString::number(progVars.NPBHAR[ii]));
        rw->ui->OUTNR_6->setText(QString::number(progVars.OUTNR[ii]));
        rw->ui->ENTRNR_6->setText(QString::number(progVars.ENTRNR));
        rw->ui->NREMS_6->setText(QString::number(progVars.NREMS[ii], 'f', 2));
        rw->ui->NREMCR_6->setText(QString::number(progVars.NREMCR[ii], 'f', 2));
        rw->ui->NREML_6->setText(QString::number(progVars.NREML[ii], 'f', 2));
        rw->ui->NREMB_6->setText(QString::number(progVars.NREMB[ii], 'f', 2));
        rw->ui->NREMOV_6->setText(QString::number(progVars.NREMOV[ii], 'f', 2));
        rw->ui->NETDNR_6->setText(QString::number(progVars.NETDNR, 'f', 2));
        rw->ui->SOILN_6->setText(QString::number(progVars.SOILN[ii], 'f', 2));
        rw->ui->CNRATO_6->setText(QString::number(progVars.CNRATO, 'f', 2));
        rw->ui->HSBMAS_6->setText(QString::number(progVars.HSBMAS[ii], 'f', 2));
        rw->ui->HCRMAS_6->setText(QString::number(progVars.HCRMAS[ii], 'f', 2));
        rw->ui->HLBMAS_6->setText(QString::number(progVars.HLBMAS[ii], 'f', 2));
        rw->ui->HBBMAS_6->setText(QString::number(progVars.HBBMAS[ii], 'f', 2));
        rw->ui->HTREM_6->setText(QString::number(progVars.HTREM[ii], 'f', 2));
        rw->ui->NRUBR_6->setText(QString::number(progVars.NRUBR, 'f', 2));
        }
    }


    progVars.ENTRNR=progVars.RAINNS+progVars.FERTNS;

    progVars.CNRATO=progVars.SOILC/progVars.SOILN[progVars.NUM];


    if(progVars.NREMSI != 0.0)
        progVars.NRUBS=progVars.TREM/progVars.NREMSI;
    else
        progVars.NRUBS=0.0;

}

void start(vars & progVars )
{

    progVars.SOILN[progVars.ROTNUM] = progVars.ISOILN;

    progVars.ISAGE=progVars.AGE;
    //set initial physiol. age
    progVars.PHSAGE=double(progVars.AGE);

    progVars.FINISH=progVars.ROTATN * (progVars.NUM-1) + (progVars.ROTATN - progVars.ISAGE) + 1;

    //let user know its working
    for (int i=0; i<progVars.FINISH; ++i)
    {

        if(progVars.PBBURN != 0)
        {
            progVars.MARKER = (progVars.AGE-progVars.PBBURN)-((progVars.AGE-progVars.PBBURN)/progVars.FREQBN) * progVars.FREQBN;
            if(progVars.MARKER == 0)
            {
                pburn(progVars);
            }
        }

        if(progVars.AGE == progVars.FERAGE && progVars.FERAGE != 0)
            fert(progVars);

        availN(progVars, i);

        if(progVars.AGE != progVars.ROTATN)
        {
            litter(progVars);

        }
        else
        {
            harv(progVars);
            progVars.PHSAGE=0.0;
            if(progVars.SHBURN != 0)
                slashb(progVars);
        }


        annual(progVars);

        progVars.AGE=progVars.AGE+1;

        if(progVars.AGE == progVars.FTRIG)
            progVars.FERFAC=0.0;
        if(progVars.AGE == progVars.PBTRIG)
            progVars.PBFAC=0.0;
        //accelerate allowable growth
        progVars.PHSAGE=progVars.PHSAGE+progVars.FERFAC+progVars.PBFAC+1.0;
    }

    progVars.FINISH=progVars.ROTATN * progVars.NUM - progVars.AGE;
}

void MainWindow::importVariablesFromUI(vars & progVars)
{
    progVars.AGE=ui->AGE->value();

    if(ui->SI_45->isChecked())
    {
        progVars.SI=45;
    }
    else if(ui->SI_55->isChecked())
    {
        progVars.SI=55;
    }
    else if(ui->SI_65->isChecked())
    {
        progVars.SI=65;
    }
    else
    {
        progVars.SI=75;
    }

    progVars.ROTATN=ui->ROTATN->value();

    progVars.NUM=ui->NUM->value();

    progVars.FERAGE=ui->FERAGE->value();

    if(ui->FERATE_0->isChecked())
    {
        progVars.FERATE=0;
    }
    else if(ui->FERATE_100->isChecked())
    {
        progVars.FERATE=100;
    }
    else if(ui->FERATE_200->isChecked())
    {
        progVars.FERATE=200;
    }
    else
    {
        progVars.FERATE=300;
    }

    if(ui->PBBURN_NONE->isChecked())
    {
        progVars.PBBURN=0;
        progVars.FREQBN=0;
    }
    else
    {
        progVars.PBBURN=ui->PBBURN->value();

        progVars.FREQBN=ui->FREQBN->value();
    }

    if(ui->BURNTP_NONE->isChecked())
    {
        progVars.BURNTP=0;
    }
    else if(ui->BURNTP_WARM->isChecked())
    {
        progVars.BURNTP=1;
    }
    else
    {
        progVars.BURNTP=2;
    }

    if(ui->HARVT_WHOLE->isChecked())
    {
        progVars.HARVT=1;
    }
    else
    {
        progVars.HARVT=2;
    }

    if(ui->SHBURN_NO->isChecked())
    {
        progVars.SHBURN=0;
    }
    else
    {
        progVars.SHBURN=1;
    }

    if(ui->SBURNT_NONE->isChecked())
    {
        progVars.SBURNT=0;
    }
    else if(ui->SBURNT_WARM->isChecked())
    {
        progVars.SBURNT=1;
    }
    else
    {
        progVars.SBURNT=2;
    }

    progVars.LTRANS=ui->LTRANS->value();

    progVars.RTRANS=ui->RTRANS->value();

    progVars.FRFPRO=ui->FRFPRO->value();

    progVars.SOILC=ui->SOILC->value();

    progVars.ISOILN=ui->ISOILN->value();

    progVars.LEAFNF=ui->LEAFNF->value();

    progVars.ROOTNF=ui->ROOTNF->value();

    progVars.NINPUT=ui->NINPUT->value();

    progVars.NOUTPUT=ui->NOUTPUT->value();
}

void MainWindow::runSimulation()
{
    vars progVars;
    zero(progVars);
    importVariablesFromUI(progVars);
    string temp=checkInput(progVars);
    if(temp!="")
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error",temp.c_str());
        messageBox.setFixedSize(500,200);
    }
    else
    {
    start(progVars);
    report(progVars);
    exportResultsToUI(progVars);
    }
}

void MainWindow::exportResultsToUI(vars & progVars)
{
    switch(progVars.NUM)
    {
        case 1:
        for(; rw->ui->Results->count() > 4; )
            rw->ui->Results->removeTab(1);
        break;
        case 2:
        if(rw->ui->Results->count() > 5)
        {
            for(; rw->ui->Results->count() > 5; )
                rw->ui->Results->removeTab(2);}
        else
        {
            rw->ui->Results->insertTab(1,rw->ui->rotation2,"Rotation 2");
        }
        break;
        case 3:
        if(rw->ui->Results->count() > 6)
        {
            for(; rw->ui->Results->count() > 6; )
                rw->ui->Results->removeTab(3);
        }
        else
        {
            rw->ui->Results->insertTab(1,rw->ui->rotation2,"Rotation 2");
            rw->ui->Results->insertTab(2,rw->ui->rotation3,"Rotation 3");
        }
        break;
        case 4:
        if(rw->ui->Results->count() > 7)
        {
            for(; rw->ui->Results->count() > 7; )
                rw->ui->Results->removeTab(4);
        }
        else
        {
            rw->ui->Results->insertTab(1,rw->ui->rotation2,"Rotation 2");
            rw->ui->Results->insertTab(2,rw->ui->rotation3,"Rotation 3");
            rw->ui->Results->insertTab(3,rw->ui->rotation4,"Rotation 4");
        }
        break;
        case 5:
        rw->ui->Results->insertTab(1,rw->ui->rotation2,"Rotation 2");
        rw->ui->Results->insertTab(2,rw->ui->rotation3,"Rotation 3");
        rw->ui->Results->insertTab(3,rw->ui->rotation4,"Rotation 4");
        rw->ui->Results->insertTab(4,rw->ui->rotation5,"Rotation 5");
        break;

    }

    //clearing data for redraws
    rw->ui->nPoolPlot->graph(0)->clearData();
    rw->ui->biomassPlot->graph(0)->clearData();

    //graphing data on nPoolPlot
    QVector<double> x(progVars.NUM), yN(progVars.NUM), yB(progVars.NUM);
    for (int i=0; i<progVars.NUM; ++i)
    {
        yN[i] = progVars.SOILN[i];
        yB[i] = progVars.HTREM[i];
        x[i] = i+1;
    }
    rw->ui->nPoolPlot->graph(0)->addData(x,yN);
    rw->ui->nPoolPlot->graph(0)->rescaleAxes();
    rw->ui->nPoolPlot->xAxis->setTickLabels(true);
    rw->ui->nPoolPlot->xAxis->setTickVector(x);
    rw->ui->nPoolPlot->yAxis->setAutoTicks(true);
    rw->ui->nPoolPlot->yAxis->setTickLabels(true);
    rw->ui->nPoolPlot->replot();

    //graphing data on biomassPlot
    rw->ui->biomassPlot->graph()->addData(x,yB);
    rw->ui->biomassPlot->graph()->rescaleAxes();
    rw->ui->biomassPlot->xAxis->setTickLabels(true);
    rw->ui->biomassPlot->xAxis->setTickVector(x);
    rw->ui->biomassPlot->yAxis->setAutoTicks(true);
    rw->ui->biomassPlot->yAxis->setTickLabels(true);
    rw->ui->biomassPlot->replot();

    //Results
    rw->ui->NSBURN->setText(QString::number(progVars.NSBURN));
    rw->ui->NPBURN->setText(QString::number(progVars.NPBURN));
    rw->ui->FERTNS->setText(QString::number(progVars.FERTNS));
    rw->ui->RAINNS->setText(QString::number(progVars.RAINNS));
    rw->ui->OUTNS->setText(QString::number(progVars.OUTNS));
    rw->ui->ENTRNR_RESULTS->setText(QString::number(progVars.ENTRNR));
    rw->ui->NREMSS->setText(QString::number(progVars.NREMSS, 'f', 2));
    rw->ui->NRMSCR->setText(QString::number(progVars.NRMSCR, 'f', 2));
    rw->ui->NREMSL->setText(QString::number(progVars.NREMSL, 'f', 2));
    rw->ui->NREMSB->setText(QString::number(progVars.NREMSB, 'f', 2));
    rw->ui->NREMSI->setText(QString::number(progVars.NREMSI, 'f', 2));
    rw->ui->NETDNS->setText(QString::number(progVars.NETDNS, 'f', 2));
    rw->ui->SOILN_RESULTS->setText(QString::number(progVars.SOILN[progVars.NUM], 'f', 2));
    rw->ui->CNRATO_RESULTS->setText(QString::number(progVars.CNRATO, 'f', 2));
    rw->ui->TSREM->setText(QString::number(progVars.TSREM, 'f', 2));
    rw->ui->TCRREM->setText(QString::number(progVars.TCRREM, 'f', 2));
    rw->ui->TLREM->setText(QString::number(progVars.TLREM, 'f', 2));
    rw->ui->TBREM->setText(QString::number(progVars.TBREM, 'f', 2));
    rw->ui->TREM->setText(QString::number(progVars.TREM, 'f', 2));
    rw->ui->NRUBS->setText(QString::number(progVars.NRUBS, 'f', 2));
}

void MainWindow::exportVariablesToUI(vars & progVars)
{
    ui->AGE->setValue(progVars.AGE);

    if(progVars.SI==45)
    {
        ui->SI_45->setChecked(true);
    }
    else if(progVars.SI==55)
    {
        ui->SI_55->setChecked(true);
    }
    else if(progVars.SI==65)
    {
        ui->SI_65->setChecked(true);
    }
    else
    {
        ui->SI_75->setChecked(true);
    }

    ui->ROTATN->setValue(progVars.ROTATN);

    ui->NUM->setValue(progVars.NUM);

    ui->FERAGE->setValue(progVars.FERAGE);

    if(progVars.FERATE==0)
    {
        ui->FERATE_0->setChecked(true);
    }
    else if(progVars.FERATE==100)
    {
        ui->FERATE_100->setChecked(true);
    }
    else if(progVars.FERATE==200)
    {
        ui->FERATE_200->setChecked(true);
    }
    else
    {
        ui->FERATE_300->setChecked(true);
    }

    //Need to add check box for no burn
    if(progVars.PBBURN==0||progVars.FREQBN==0)
    {
        ui->PBBURN_NONE->setChecked(true);
        ui->PBBURN->setValue(progVars.PBBURN);
        ui->FREQBN->setValue(progVars.FREQBN);
    }
    else
    {
        ui->PBBURN->setValue(progVars.PBBURN);

        ui->FREQBN->setValue(progVars.FREQBN);
    }

    if(progVars.BURNTP==0)
    {
        ui->BURNTP_NONE->setChecked(true);
    }
    else if(progVars.BURNTP==1)
    {
        ui->BURNTP_WARM->setChecked(true);
    }
    else
    {
        ui->BURNTP_HOT->setChecked(true);
    }

    if(progVars.HARVT==1)
    {
        ui->HARVT_WHOLE->setChecked(true);
    }
    else
    {
         ui->HARVT_STEM->setChecked(true);
    }

    if(progVars.SHBURN==0)
    {
        ui->SHBURN_NO->setChecked(true);
    }
    else
    {
        ui->SHBURN_YES->setChecked(true);
    }

    if(progVars.SBURNT==0)
    {
        ui->SBURNT_NONE->setChecked(true);
    }
    else if(progVars.SBURNT==1)
    {
        ui->SBURNT_WARM->setChecked(true);
    }
    else
    {
        ui->SBURNT_HOT->setChecked(true);
    }

    ui->LTRANS->setValue(progVars.LTRANS);

    ui->RTRANS->setValue(progVars.RTRANS);

    ui->FRFPRO->setValue(progVars.FRFPRO);

    ui->SOILC->setValue(progVars.SOILC);

    ui->ISOILN->setValue(progVars.ISOILN);

    ui->LEAFNF->setValue(progVars.LEAFNF);

    ui->ROOTNF->setValue(progVars.ROOTNF);

    ui->NINPUT->setValue(progVars.NINPUT);

    ui->NOUTPUT->setValue(progVars.NOUTPUT);
}


void MainWindow::saveInputFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Input"),"/path/to/file/",tr("ini Files (*.ini)"));
    if(fileName!="")
    {
    vars progVars;
    progVars.saveInputFileName=fileName.toStdString();
    importVariablesFromUI(progVars);
    saveInputToFile(progVars);
    }
}

void MainWindow::openInputFile()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Input"),"/path/to/file/",tr("ini Files (*.ini)"));
    if(fileName!="")
    {
    vars progVars;
    progVars.inputFileName=fileName.toStdString();
    inputFromFile(progVars);
    exportVariablesToUI(progVars);
    }
}

void MainWindow::saveOutputFile()
{

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Output"),"/path/to/file/",tr("Text Files (*.txt)"));
    if(fileName!="")
    {
    vars progVars;
    progVars.outputFileName=fileName.toStdString();
    zero(progVars);
    importVariablesFromUI(progVars);
    ofstream fout;
    fout.open(progVars.outputFileName.c_str());
    progVars.fp=&fout;
    start(progVars);
    printOutput(progVars);
    fout.close();
    }
}

void MainWindow::saveCSV()
{

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Output"),"/path/to/file/",tr("Comma Separated Values (*.csv)"));
    if(fileName!="")
    {
    vars progVars;
    progVars.outputFileName=fileName.toStdString();
    zero(progVars);
    importVariablesFromUI(progVars);
    start(progVars);
    saveOutputCSV(progVars);
    }
}

void MainWindow::openExample1()
{
    ifstream my_file("examples/example1.ini");
    if(my_file.good())
    {
    vars progVars;
    progVars.inputFileName="examples/example1.ini";
    inputFromFile(progVars);
    exportVariablesToUI(progVars);
    }
    else
    {
    QMessageBox messageBox;
    messageBox.critical(0,"Error","Invalid File:\n \"Missing example1.ini in examples directory.\"");
    messageBox.setFixedSize(500,200);
    }
}

void MainWindow::openExample2()
{
    ifstream my_file("examples/example2.ini");
    if(my_file.good())
    {
    vars progVars;
    progVars.inputFileName="examples/example2.ini";
    inputFromFile(progVars);
    exportVariablesToUI(progVars);
    }
    else
    {
    QMessageBox messageBox;
    messageBox.critical(0,"Error","Invalid File:\n \"Missing example2.ini in examples directory.\"");
    messageBox.setFixedSize(500,200);
    }
}

void MainWindow::openExample3()
{
    ifstream my_file("examples/example3.ini");
    if(my_file.good())
    {
    vars progVars;
    progVars.inputFileName="examples/example3.ini";
    inputFromFile(progVars);
    exportVariablesToUI(progVars);
    }
    else
    {
    QMessageBox messageBox;
    messageBox.critical(0,"Error","Invalid File:\n \"Missing example3.ini in examples directory.\"");
    messageBox.setFixedSize(500,200);
    }
}

void MainWindow::about()
{
    AboutWindow a;
    a.setModal(true);
    a.exec();
}

void MainWindow::saveGraphFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Biomass Plot"),"/path/to/file/",tr("png Files (*.png);;jpg Files (*.jpg);;bmp Files (*.bmp)"));
    string fileNameExtension=fileName.toStdString();
    if(fileName!="")
    {
        fileNameExtension=fileNameExtension.substr(fileNameExtension.length()-3,fileNameExtension.length()-1);
        if(fileNameExtension=="png")
        {
            rw->ui->biomassPlot->savePng(fileName);
        }
        else if(fileNameExtension=="jpg")
        {
            rw->ui->biomassPlot->saveJpg(fileName);
        }
        else
        {
            rw->ui->biomassPlot->saveBmp(fileName);
        }
    }

   fileName = QFileDialog::getSaveFileName(this, tr("Save Nitrogen Pool Plot"),"/path/to/file/",tr("png Files (*.png);;jpg Files (*.jpg);;bmp Files (*.bmp)"));
    if(fileName!="")
    {
        fileNameExtension=fileNameExtension.substr(fileNameExtension.length()-3,fileNameExtension.length()-1);
        if(fileNameExtension=="png")
        {
            rw->ui->nPoolPlot->savePng(fileName);
        }
        else if(fileNameExtension=="jpg")
        {
            rw->ui->nPoolPlot->saveJpg(fileName);
        }
        else
        {
            rw->ui->nPoolPlot->saveBmp(fileName);
        }
    }
}
