#include "QatPlotWidgets/PlotView.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatDataModeling/HistChi2Functional.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include <cmath>
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
#include "QatGenericFunctions/Sqrt.h"
#include "QatGenericFunctions/Parameter.h"
#include "Eigen/Dense"

using namespace std;
using namespace Genfun;


int main (int argc, char * * argv) {



  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + "[1|2|3]"; 

  
  QApplication     app(argc,argv);
  
  QMainWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  int bin=100; //Number of bins
  
  int min=92; //Minimum value of data (estimate)
  int max=110; //Maximum value of data (estimate)
  
  Hist1D histogram (bin, min, max);
  
  ifstream stream("data00.dat");
  
  double x;
  
  Exp exp;
  Sqrt sqrt;
  
  while (stream>>x) {
    
    histogram.accumulate(x);
  }
  
  Parameter psigma("sigma", histogram.variance(),0.01,1.0);
  Parameter pmu("mu", histogram.mean(),histogram.min(),histogram.max());
  
  Variable X;
  
  //GENFUNCTION lorenzianDistribution= 1.0/(M_PI*psigma*(1+((X-pmu)/psigma)*((X-pmu)/psigma))); //Function for Lorentzian Distribution
  GENFUNCTION normaldistribution=exp(-(X-pmu)*(X-pmu)/(2*psigma*psigma))/sqrt(2*M_PI)/psigma; //Function for Lagrangian distribution
 
  HistChi2Functional objectiveFunction(&histogram);
  //GENFUNCTION f= histogram.sum()*lorenzianDistribution;
  GENFUNCTION f= histogram.sum()*normaldistribution;
  
  bool verbose=true;
  MinuitMinimizer minimizer(verbose);
  minimizer.addParameter(&psigma);
  minimizer.addParameter(&pmu);
  minimizer.addStatistic(&objectiveFunction, &f);
  minimizer.minimize(); 
  
  PlotHist1D pH=histogram;
  PlotFunction1D fplot(f*1.0/bin*(max-min));
  
  PRectF rect;
  rect.setXmin(95);
  rect.setXmax(110);
  rect.setYmin(0);
  rect.setYmax(300);
  PlotView view(rect);
  window.setCentralWidget(&view);

 view.add(&pH);
 view.add(&fplot);
 
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Sans Serif") 
	      << PlotStream::Size(16)
	      <<"Gaussian Fit"
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << "x" 
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Sans Serif")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  
  view.show();
  window.show();
  app.exec();
  return 1;
}
