/*! 
 * \file  QWebEngineViewWrapper.cxx
 * \brief
 * \author Helfer Thomas
 * \brief 07 déc. 2012
 */

#include<QtWidgets/QHBoxLayout>
#include"QEmacs/Utilities.hxx"
#include"QEmacs/QWebEngineViewWrapper.hxx"

namespace qemacs
{

  QWebEngineViewWrapper::QWebEngineViewWrapper(QWebEngineView *s,
				   QWidget *p)
    : QWidget(p),
      wrappedObject(s)
  {
    auto *hl  = new QHBoxLayout;
    setQWebEngineViewInLayout(hl,this->wrappedObject);
    this->setLayout(hl);
    }
  
  void
  QWebEngineViewWrapper::focusInEvent(QFocusEvent *)
  {
    this->wrappedObject->setFocus();
  }

} // end of namespace qemacs
