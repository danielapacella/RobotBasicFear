/* 
 * Evorobot* - rend_param.cpp
 * Copyright (C) 2009, Stefano Nolfi
 * LARAL, Institute of Cognitive Science and Technologies, CNR, Roma, Italy 
 * http://laral.istc.cnr.it

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "public.h"
#include "parameters.h"
#include "rend_param.h"

#ifdef EVOGRAPHICS  

#include <QtGui>        // Widget
#include <QKeyEvent>    // Dialog
#include <QFileDialog>  // Dialog
#include <QMessageBox>  // Dialog

struct Parameter {
    char   *name;                 // the name of the parameter
    char   *folder;               // the folder of the parameter
    int     type;          // the type, 0=int 
    int     n1;                   // the minimum value (for int only)
    int     n2;                     // the maximum value (for int only)
    int    *ipointer;             // the pointer to a (int) parameter
    float  *fpointer;             // the pointer to a (float) parameter
    char   *cpointer;             // the pointer to a (char) parameter
    float  **ffpointer;           // the pointer to a pointer to a (float) parameter
    struct Parameter *next;       // the next parameter of the same folder
    struct Parameter *nextfolder; // the first parameter of the next folder
    char   *description;          // parameter description
    int    modified;              // whether the parameter has been modified from its default value 
  };

extern struct Parameter *parlist;
extern Parameter *ccfolder;       // current folder

class QTextEdit;     // testedit
QTextEdit *textEdit; // textedit
void update_textedit(Parameter *currentfolder);

RendParam *rendParam;        // The param rendering widget
ParamDialog *paramDialog;    // The network Dialog
int wparam=0;                // whether the window has been already opened or not
int verbose=0;               // whether we display the parameter description or not

Parameter *currentfolder;    // the current displayed folder
extern int getline(char *word, char *line);
extern void parse_par_line(char *line);
struct Parameter *findfolder(char *folder);
struct Parameter *findparam(Parameter *folder, char *name);


//----------------------------------------------------------------------
//-------------------  PUBLIC FUNCTIONS  -------------------------------
//----------------------------------------------------------------------

/*
 * create the parameter widget and dialog
 */

void
init_rendparam()
{

    if (wparam == 0)
    {
     rendParam = new RendParam;
     paramDialog = new ParamDialog(NULL);
     textEdit = new QTextEdit;
     paramDialog->setWindowTitle("Parameters");
     paramDialog->setWidget(textEdit, 0, 0);
     if (winparametersx >= 0 && winparametersy >= 0)
        paramDialog->move(winparametersx, winparametersy);
     if (winparametersdx > 0 && winparametersdy > 0)
        paramDialog->resize(winparametersdx, winparametersdy);
     paramDialog->show();
     //update_rendparam();
     wparam = 1;
     currentfolder = parlist;
     while (currentfolder != NULL)
     {
      update_textedit(currentfolder);
      currentfolder = currentfolder->nextfolder;
     }
    }

    if (wparam == 1)
    {
     paramDialog->show();
     update_rendparam();
    }
}

void
hide_rendparam()
{
    if (wparam == 1)
      paramDialog->hide();
}


void
update_rendparam()


{

     rendParam->repaint();
     QCoreApplication::processEvents();

}

//----------------------------------------------------------------------
//-------------------  PRIVATE FUNCTIONS  ------------------------------
//----------------------------------------------------------------------

// -----------------------------------------------------
// Dialog and Toolbar
// -----------------------------------------------------

void ParamDialog::createToolBars()
{
    QToolBar *paramtoolbar;

    paramtoolbar = toolBar();

    paramtoolbar->addAction(allfoldersAct);
    paramtoolbar->addAction(parseAct);
    paramtoolbar->addAction(loadandparseAct);
    paramtoolbar->addAction(verboseornotAct);

}

void ParamDialog::createActions()
{

    allfoldersAct = new QAction(QIcon(":/images/showparam.png"), tr("&Display all parameters"), this);
    allfoldersAct->setShortcut(tr("Ctrl+A"));
    allfoldersAct->setStatusTip(tr("Display all parameters"));
    connect(allfoldersAct, SIGNAL(triggered()), this, SLOT(show_allfolders()));

    parseAct = new QAction(QIcon(":/images/parse.png"), tr("&Parse text"), this);
    parseAct->setShortcut(tr("Ctrl+P"));
    parseAct->setStatusTip(tr("Parse text"));
    connect(parseAct, SIGNAL(triggered()), this, SLOT(parsetxt()));

    loadandparseAct = new QAction(QIcon(":/images/open.png"), tr("&Load&Parse text"), this);
    loadandparseAct->setShortcut(tr("Ctrl+L"));
    loadandparseAct->setStatusTip(tr("Load&Parse text"));
    connect(loadandparseAct, SIGNAL(triggered()), this, SLOT(loadandparsetxt()));

    verboseornotAct = new QAction(QIcon(":/images/verbose.png"), tr("Display/Undisplay full list with description"), this);
    verboseornotAct->setShortcut(tr("Ctrl+L"));
    verboseornotAct->setStatusTip(tr("Display/Undisplay full list with desc."));
    connect(verboseornotAct, SIGNAL(triggered()), this, SLOT(verbose_or_not()));

}

/*
 * show the parameters of all folders
 */
void ParamDialog::show_allfolders()
{

    textEdit->clear();
    currentfolder = parlist;
    while (currentfolder != NULL)
     {
      update_textedit(currentfolder);
      currentfolder = currentfolder->nextfolder;
     }
}

/*
 * parse the text contained in the parameter widget
 */
void ParamDialog::parsetxt()
{

    char *cst;
    QByteArray carray;
    char line[1024];

    QString c = textEdit->toPlainText();
    carray = c.toAscii();
    cst = carray.data();

    textEdit->clear();
    while (getline(line, cst) > 1)
     {
       parse_par_line(line);
     }
}

/*
 * Add/remove parameters description
 */
void ParamDialog::verbose_or_not()
{

    if (verbose == 0)
      verbose = 1;
    else
      verbose = 0;

    textEdit->clear();
    currentfolder = parlist;
    while (currentfolder != NULL)
     {
      update_textedit(currentfolder);
      currentfolder = currentfolder->nextfolder;
     }

}

/*
 * parse a parameter description line 
 * if the line does not contains a folder description,
 *  assumes that the parameter belong to the last specified folder
 * note that parameters.c use a sister function with different notifications 
 */
void parse_par_line(char *line)
{

    char word[256];
    char message[256];
    Parameter *cpar;
    Parameter *pbuf;
    int n;

    if (getword(word,line,' ') > 1)       
      {
     pbuf = NULL;
     pbuf = findfolder(word);
     if (pbuf != NULL)
           {
         ccfolder = pbuf;
         sprintf(message,"<b>%s</b><br>", ccfolder->folder);
             textEdit->insertHtml(message);
           }
     else
           {
             if (ccfolder == NULL)
              {
        sprintf(message,"error on %s: folder unspecified", word);
                textEdit->insertHtml(message);
          }
             if (ccfolder != NULL)
              {
               cpar = NULL;
               cpar = findparam(ccfolder, word);
           if (cpar != NULL)
                 {
               if (cpar->type == 0 || cpar->type == 1 || cpar->type == 2)
                    {
                      if (getword(word,line,' ') > 0)
                        {
              if (cpar->type == 0)
                            set_iparameter(cpar->name, cpar->folder, atoi(word));
              if (cpar->type == 1)
                            set_fparameter(cpar->name, cpar->folder, atof(word));
              if (cpar->type == 2)
                            set_cparameter(cpar->name, cpar->folder, word);
              sprintf(message,"<font color=\"maroon\">%s <font color=\"maroon\"> <font color=\"olive\">%s<font color=\"olive\"><br>", cpar->name, word);
                          textEdit->insertHtml(message);
                }
             }
               if (cpar->type == 4 || cpar->type == 7)
                    {
              n = 0;
              sprintf(message,"<font color=\"maroon\">%s <font color=\"maroon\"> ", cpar->name);
                      textEdit->insertHtml(message);
                      while (n < cpar->n1 && (getword(word,line,' ') > 0))
                        {
                      if (cpar->type == 4)
                            set_fvparameter(cpar->name, cpar->folder, n, atof(word));
                      if (cpar->type == 7)
                            set_fvpparameter(cpar->name, cpar->folder, n, atof(word));
              n++;
                  sprintf(message,"<font color=\"olive\">%.2f <font color=\"olive\">", atof(word));
                          textEdit->insertHtml(message);
                }                            
              sprintf(message,"<br>", cpar->name, word);
                      textEdit->insertHtml(message);
             }
           }
          else
                    {
             sprintf(message,"error on '%s': not defined in folder %s", word, ccfolder->folder);
                     textEdit->insertHtml(message);
            }
                }

              }
           }
}


/*
 * load & parse a parameter file
 */
void ParamDialog::loadandparsetxt()

{


     FILE    *fp;
     char       st[1024];
     char       message[128];
     int n;
     QByteArray filen;
     char *f;


     QString fileName = QFileDialog::getOpenFileName(this,
            "Choose a parameter file to load",
                    "",
                    "*.cf");

      if (fileName.endsWith("cf"))
      {
        filen = fileName.toAscii();
        f = filen.data();
    strcpy(st, f);
        if ((fp=fopen(st, "r")) != NULL)
    {
      n = 0;
      while (fgets(st,1024,fp) != NULL && n < 1000)
      {
       parse_par_line(st);
       n++;
      }
        fclose(fp);
        sprintf(message,"loaded parameter file: %s",st);
        display_stat_message(message);
        }
       else
       {
         sprintf(message,"Error: the file %s could not be opended",st);
     display_error(message);
       }
      }


}


/*
 * display the parameter of a folder
 */
void update_textedit(Parameter *cfolder)
{

   char   message[1024];
   struct Parameter *cpar;
   int    b;
   int    i;
   float  *v;
   float  **vv;


   b = 1;
   cpar = cfolder;
   sprintf(message,"<b>%s</b><br>", cpar->folder);
   textEdit->insertHtml(message);
   while(cpar != NULL)
    {
    if (verbose == 1 || cpar->modified == 1)
     {
          sprintf(message,"<font color=\"navy\">%s </font color=\"navy\">", cpar->name);
          textEdit->insertHtml(message);
      if (cpar->type == 0)
        {
         sprintf(message,"<font color=\"red\">%d</font color=\"red\"> ", *cpar->ipointer);
         textEdit->insertHtml(message);
         if (verbose == 1)
          {
           sprintf(message,"<font color=\"gray\">[%d-%d] %s<font color=\"gray\"><br>", cpar->n1, cpar->n2, cpar->description);
           textEdit->insertHtml(message);
          }
         else
          {
           sprintf(message,"<br>");
           textEdit->insertHtml(message);
          }
        }
      if (cpar->type == 1)
        {
         sprintf(message,"<font color=\"red\">%.2f</font color=\"red\"> ", *cpar->fpointer);
         textEdit->insertHtml(message);
         if (verbose == 1)
          {
           sprintf(message,"<font color=\"gray\"> %s<font color=\"gray\"><br>", cpar->description);
           textEdit->insertHtml(message);
          }
          else
          {
           sprintf(message,"<br>");
           textEdit->insertHtml(message);
          }
        }
      if (cpar->type == 2)
        {
         sprintf(message,"<font color=\"red\">%s</font color=\"red\"> ", cpar->cpointer);
         textEdit->insertHtml(message);
         if (verbose == 1)
          {
           sprintf(message,"<font color=\"gray\"> %s<font color=\"gray\"><br>", cpar->description);
           textEdit->insertHtml(message);
          }
          else
          {
           sprintf(message,"<br>");
           textEdit->insertHtml(message);
          }
        }
       if (cpar->type == 3)
            {
         for(i=0; i < cpar->n1; i++, cpar->ipointer++)
         {
          sprintf(message," %d", *cpar->ipointer);
          textEdit->insertPlainText(message);
         }
         sprintf(message," / %s\n", cpar->description);
         textEdit->insertPlainText(message);
        }
       if (cpar->type == 4)
            {
         for(i=0, v = cpar->fpointer; i < cpar->n1; i++, v++)
         {
              sprintf(message,"<font color=\"red\">%.2f</font color=\"red\"> ", *v);
              textEdit->insertHtml(message);
         }
         if (verbose == 1)
          {
           sprintf(message,"<font color=\"gray\"> %s<font color=\"gray\"><br>", cpar->description);
           textEdit->insertHtml(message);
          }
          else
          {
           sprintf(message,"<br>");
           textEdit->insertHtml(message);
          }
        }
       if (cpar->type == 5)
            {
         for(i=0; i < cpar->n1; i++, cpar->cpointer++)
         {
          sprintf(message," %s", cpar->cpointer);
          textEdit->insertPlainText(message);
         }
         sprintf(message," / %s\n", cpar->description);
         textEdit->insertPlainText(message);
        }
       if (cpar->type == 7)
            {
         for(i=0, vv = cpar->ffpointer; i < cpar->n1; i++, vv++)
         {
          v = *vv;
              sprintf(message,"<font color=\"red\">%.2f</font color=\"red\"> ", *v);
              textEdit->insertHtml(message);
         }
         if (verbose == 1)
          {
           sprintf(message,"<font color=\"gray\"> %s<font color=\"gray\"><br>", cpar->description);
           textEdit->insertHtml(message);
          }
          else
          {
           sprintf(message,"<br>");
           textEdit->insertHtml(message);
          }
        }

          }
          cpar = cpar->next; 
         }

}


/*
 * add text in the parameter window
 */
void
show_text(char *message)
{

  if (wparam == 0)
    init_rendparam();

  textEdit->insertHtml(message);
}


// -----------------------------------------------------
// Widget
// -----------------------------------------------------

RendParam::RendParam(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
}

QSize RendParam::minimumSizeHint() const
{
    return QSize(200, 250);
}

QSize RendParam::sizeHint() const
{
    return QSize(200, 250);
}

void RendParam::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RendParam::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RendParam::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RendParam::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RendParam::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}


void RendParam::paintEvent(QPaintEvent *)
{

}





// -----------------------------------------------------
// Dialog
// -----------------------------------------------------

ParamDialog::ParamDialog(QWidget *parent) :
    QDialog(parent),
    m_mainLayout(NULL),
    m_layout(NULL),
    m_toolBar(NULL)
{
    // Creating the main layout. Direction is BottomToTop so that m_layout (added first)
    // is at bottom, while the toolbar (added last) is on top
    m_mainLayout = new QBoxLayout(QBoxLayout::BottomToTop, this);
    //m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // Creating the layout for external widgets
    m_layout = new QGridLayout();
    //m_layout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addLayout(m_layout, 1);

        createActions();
    createToolBars();
}

ParamDialog::~ParamDialog()
{
}

void ParamDialog::setWidget(QWidget *w, int r, int c)
{
    m_layout->addWidget(w, r, c);
}

QToolBar* ParamDialog::toolBar()
{
    if (m_toolBar == NULL) {
        // Creating toolbar and adding it to the main layout
        m_toolBar = new QToolBar(this);
        m_mainLayout->addWidget(m_toolBar, 0);
    }

    return m_toolBar;
}

void ParamDialog::closeEvent(QCloseEvent *)
{
    emit dialogClosed();
}

void ParamDialog::keyReleaseEvent(QKeyEvent* event)
{
    /*
    if (event->matches(QKeySequence::Print)) {
        // Taking a screenshow of the widget
        shotWidget();
    } else {
        // Calling parent function
        QDialog::keyReleaseEvent(event);
    }*/
}

void ParamDialog::shotWidget()
{
    /*
    // Taking a screenshot of this widget
    QPixmap shot(size());
    render(&shot);

    // Asking the user where to save the shot
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Shot"), "./widget.png", tr("Images (*.png *.xpm *.jpg)"));
    if (!fileName.isEmpty()) {
        shot.save(fileName);

        QMessageBox::information(this, QString("File Saved"), QString("The widget shot has been saved"));
    }*/
}




#endif
