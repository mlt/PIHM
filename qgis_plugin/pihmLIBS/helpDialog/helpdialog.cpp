#include <QtGui>
#include "helpdialog.h"

helpDialog::helpDialog(QWidget *parent, const char* name, bool modal, const QString& helpFileName,const QString& captionText)
{
  this->setWindowTitle(QApplication::translate("helpDialog", qPrintable(captionText), 0, QApplication::UnicodeUTF8));
  setupUi(this);
  //if ( !name )
  //setName( "helpDialog" );


  connect(helpDialogCloseButton, SIGNAL(clicked()), this, SLOT(close()));
  helpTextBrowser->setFocus();
  QFile f( helpFileName );
  if ( !f.open( QIODevice::ReadOnly ) )
    return;
//	helpTextBrowser->setSearchPaths(QStringList("images"));

  QTextStream ts( &f );
  helpTextBrowser->setText( ts.readAll() );
  helpTextBrowser->setModified( FALSE );
  //this->setWindowTitle( captionText );
  this->setWindowTitle(QApplication::translate("helpDialog", qPrintable(captionText), 0, QApplication::UnicodeUTF8));
  helpTextBrowser->show();
}

