/*! 
 * \file  MakefileMajorMode.cxx
 * \brief
 * \author Helfer Thomas
 * \brief 26 août 2012
 */

#include"QEmacs/MakefileMajorMode.hxx"
// #include"QEmacs/MakefileSyntaxHighlighter.hxx"
#include"QEmacs/QEmacsTextEditBase.hxx"
#include"QEmacs/QEmacsMajorModeFactory.hxx"

namespace qemacs
{
  
  MakefileMajorMode::MakefileMajorMode(QEmacsWidget& w,
			     QEmacsBuffer& b,
			     QEmacsTextEditBase& t)
    : QEmacsMajorModeBase(w,b,t,&t)
  {} // end of MakefileMajorMode::MakefileMajorMode

  QString
  MakefileMajorMode::getName(void) const
  {
    return "Makefile";
  } // end of MakefileMajorMode::MakefileMajorMode

  QString
  MakefileMajorMode::getDescription(void) const
  {
    return "major mode dedicated to Makefile";
  } // end of MakefileMajorMode::MakefileMajorMode

  bool
  MakefileMajorMode::keyPressEvent(QKeyEvent * const)
  {
    // const int k                   = e->key();
    // const Qt::KeyboardModifiers m = e->modifiers(); 
    // if((m==Qt::AltModifier)&&(k==Qt::Key_M)){
      //      this->runCompilation();
    //   return true;
    // }
    return false;
  } // end of LicosMajorMode::keyPressEvent

  QString
  MakefileMajorMode::getCommentSyntax(void)
  {
    return "#";
  } // end of MakefileMajorMode::getCommentSyntax

  void
  MakefileMajorMode::setSyntaxHighlighter(QTextDocument *const)
  {
    //    new MakefileSyntaxHighlighter(d);
  } // end of MakefileMajorMode::setSyntaxHighlighter

  void
  MakefileMajorMode::format(void)
  {}

  MakefileMajorMode::~MakefileMajorMode()
  {} // end of MakefileMajorMode::~MakefileMajorMode()

  static StandardQEmacsMajorModeProxy<MakefileMajorMode> proxy("Makefile",
							       QVector<QRegExp>() << QRegExp("^Makefile$"));

} // end of namespace qemacs
