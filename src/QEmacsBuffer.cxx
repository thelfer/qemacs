/*!
 * \file  QEmacsBuffer.cxx
 * \brief
 * \author Helfer Thomas
 * \date   27/06/2012
 */

#include <QtCore/QDebug>
#include <QtCore/QSettings>

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QFileInfo>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include "QEmacs/Utilities.hxx"
#include "QEmacs/QEmacsWidget.hxx"
#include "QEmacs/QEmacsPlainTextEdit.hxx"
#include "QEmacs/QEmacsMajorMode.hxx"
#include "QEmacs/QWebEngineViewWrapper.hxx"
#include "QEmacs/QAbstractScrollAreaWrapper.hxx"
#include "QEmacs/QEmacsBuffer.hxx"

namespace qemacs {

  /*!
   * \brief structure in charge of displaying SecondaryTask widgets
   */
  struct QEmacsBuffer::SecondaryTaskTabWidget : public QTabWidget {
    /*!
     * \brief constructor
     * \param[in] p: parent widget
     */
    SecondaryTaskTabWidget(QWidget *const p) : QTabWidget(p) {
      setFontSizeAndContentsMargins(this->tabBar());
      this->tabBar()->setFocusPolicy(Qt::NoFocus);
      this->setTabPosition(QTabWidget::North);
    }  // end of SecondaryTaskTabWidget

    void show() { QTabWidget::show(); }  // end of show

    void focusInEvent(QFocusEvent *) override {
      QTabWidget::setFocus();
      this->currentWidget()->setFocus();
    }  // end of focusInEvent

    void removeTab(int i) {
      auto *w = QTabWidget::widget(i);
      if (w == nullptr) {
        return;
      }
      if (w->close()) {
        QTabWidget::removeTab(i);
      }
      if (this->count() == 0) {
        this->hide();
      }
      return;
    }  // end of removeTab
  };   // end of QEmacsBuffer::SecondaryTaskTabWidget

  QEmacsBuffer::QEmacsBuffer(const int i, QEmacsWidget &w)
      : QWidget(&w),
        qemacs(w),
        splitter(nullptr),
        stw(new SecondaryTaskTabWidget(this)),
        e(new QEmacsPlainTextEdit(w, *this)),
        id(i) {
    this->initialize();
  }  // end of QEmacsBuffer::QEmacsBuffer

  QEmacsBuffer::QEmacsBuffer(const QString &f,
                             const int i,
                             QEmacsWidget &w)
      : QWidget(&w),
        qemacs(w),
        splitter(nullptr),
        stw(new SecondaryTaskTabWidget(this)),
        e(new QEmacsPlainTextEdit(f, w, *this)),
        id(i) {
    this->initialize();
    this->emitNewTreatedFile(this->e->getCompleteFileName());
  }  // end of QEmacsBuffer::QEmacsBuffer

  int QEmacsBuffer::getId() const {
    return this->id;
  }  // end of QEmacsBuffer::getId

  void QEmacsBuffer::emitNewTreatedFile(const QString &f) {
    emit newTreatedFile(f);
  }  // end of QEmacsBuffer::emitNewTreatedFile

  QString QEmacsBuffer::getBufferRawName() const {
    return this->e->getFileName();
  }  // end of QEmacsBuffer::setBufferName

  QString QEmacsBuffer::getBufferName() const {
    const auto s = this->getBufferNameSuffix();
    const auto f = QFileInfo(this->getBufferRawName()).fileName();
    if (!s.isEmpty()) {
      return f + " <" + s + ">";
    }
    return f;
  }  // end of QEmacsBuffer::getBufferName

  QString QEmacsBuffer::getBufferNameSuffix() const {
    return this->bufferNameSuffix;
  }  // end of QEmacsBuffer::setBufferName

  void QEmacsBuffer::focusInEvent(QFocusEvent *) {
    if (!this->e->hasFocus()) {
      this->e->setFocus();
    }
  }  // end of QEmacsBuffer::focusInEvent

  void QEmacsBuffer::initialize() {
    // setting
    this->e->setMainFrame(true);
    this->e->setFocus();
    // without SecondaryTasks, the SecondaryTasks tab widget is hidden
    this->stw->setTabPosition(QTabWidget::South);
    this->stw->setDocumentMode(true);
    this->stw->setTabsClosable(true);
    // setFontSizeAndContentsSize(this->stw->tabBar());
    if (this->stw->count() == 0) {
      this->stw->hide();
    }
    this->info = new QHBoxLayout;
    this->timer = new QTimer(this);
    this->bni = new QLabel(this);
    this->rpi = new QLabel(this);
    this->api = new QLabel(this);
    this->ii = new QLabel(this);
    this->mi = new QLabel(this);
    this->ti = new QLabel(this);
    setQLabelFontSizeAndMargins(this->bni);
    setQLabelFontSizeAndMargins(this->rpi);
    setQLabelFontSizeAndMargins(this->api);
    setQLabelFontSizeAndMargins(this->mi);
    setQLabelFontSizeAndMargins(this->ti);
    this->info->addStretch();
    this->info->addWidget(this->bni);
    this->info->addStretch();
    this->info->addWidget(this->rpi);
    this->info->addStretch();
    this->info->addWidget(this->api);
    this->info->addStretch();
    this->info->addWidget(this->ii);
    this->info->addWidget(this->mi);
    this->info->addStretch();
    this->info->addWidget(this->ti);
    this->info->addStretch();
    // main gui
    auto *vl = new QVBoxLayout;
    auto *hl = new QHBoxLayout;
    this->splitter = new QSplitter;
    this->splitter->setOrientation(Qt::Vertical);
    QSizePolicy qs;
    qs.setHorizontalPolicy(QSizePolicy::Minimum);
    qs.setVerticalPolicy(QSizePolicy::Minimum);
    this->e->setSizePolicy(qs);
    qs.setHorizontalPolicy(QSizePolicy::Maximum);
    qs.setVerticalPolicy(QSizePolicy::Maximum);
    this->bni->setSizePolicy(qs);
    this->rpi->setSizePolicy(qs);
    this->api->setSizePolicy(qs);
    this->mi->setSizePolicy(qs);
    this->ti->setSizePolicy(qs);
    hl->setMargin(0);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);
    this->splitter->addWidget(this->e);
    this->splitter->addWidget(this->stw);
    hl->addWidget(this->splitter);
    vl->setMargin(0);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);
    //    vl->addWidget(this->e);
    //    vl->addWidget(this->stw);
    vl->addLayout(hl);
    vl->addLayout(this->info);
    this->updateBufferInformations();
    this->setLayout(vl);
    // timer
    QObject::connect(this->timer, &QTimer::timeout, this,
                     &QEmacsBuffer::updateDate);
    // update every second
    this->timer->start(1000);
    // connecting signals
    QObject::connect(this->e, &QEmacsPlainTextEdit::fileNameChanged,
                     this, &QEmacsBuffer::emitNewTreatedFile);
    QObject::connect(this->e, &QEmacsPlainTextEdit::fileNameChanged,
                     this, &QEmacsBuffer::updateBufferName);
    QObject::connect(this->e,
                     &QEmacsPlainTextEdit::cursorPositionChanged, this,
                     &QEmacsBuffer::updatePosition);
    QObject::connect(this->e,
                     &QEmacsPlainTextEdit::cursorPositionChanged, this,
                     &QEmacsBuffer::updateDate);
    QObject::connect(this->e, &QEmacsPlainTextEdit::majorModeChanged,
                     this, &QEmacsBuffer::updateBufferInformations);
    QObject::connect(this->stw, &SecondaryTaskTabWidget::tabCloseRequested,
                     this, &QEmacsBuffer::closeSecondaryTask);
    this->updateBufferName();
  }

  void QEmacsBuffer::updateBufferName() {
    const auto o = this->getBufferName();
    this->bufferNameSuffix = this->qemacs.chooseBufferNameSuffix(
        this, this->e->getFileName());
    this->updateBufferInformations();
    const auto n = this->getBufferName();
    emit bufferNameChanged(this, o, n);
  }

  QVector<QMenu *> QEmacsBuffer::getSpecificMenus() {
    return this->e->getSpecificMenus();
  }  // end of QEmacsBuffer::getSpecificMenu

  QIcon QEmacsBuffer::getIcon() const {
    return this->e->getIcon();
  }  // end of QEmacsBuffer::getIcon

  void QEmacsBuffer::updateDate() {
    QTime t = QTime::currentTime();
    int h = t.hour();
    int m = t.minute();
    QString tl;
    if (h < 9) {
      tl += "0" + QString::number(h);
    } else {
      tl += QString::number(h);
    }
    tl += ":";
    if (m < 9) {
      tl += "0" + QString::number(m);
    } else {
      tl += QString::number(m);
    }
    this->ti->setText(tl);
  }

  void QEmacsBuffer::updatePosition() {
    const QTextCursor &c = this->e->textCursor();
    const QTextDocument &d = *(this->e->document());
    const int cn = c.blockNumber() + 1;
    const int bn = d.blockCount();
    if (bn > 0) {
      if (cn == bn) {
        this->rpi->setText("100%");
      } else {
        QString p = QString::number(static_cast<int>(
                        qreal(cn) / qreal(bn) * 100.)) +
                    "%";
        this->rpi->setText(p);
      }
    }
    QTextCursor b(c);
    b.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    this->api->setText("(" + QString::number(cn) + "," +
                       QString::number(c.position() - b.position()) +
                       ")");
  }

  void QEmacsBuffer::updateBufferInformations() {
    const auto &s = this->getBufferNameSuffix();
    if (s.isEmpty()) {
      this->bni->setText("<b>" + this->getBufferName() + "</b>");
    } else {
      this->bni->setText("<b>" + this->getBufferName() + " &lt;" + s +
                         "&gt;</b>");
    }
    this->updatePosition();
    QIcon ic = this->e->getIcon();
    if (!ic.isNull()) {
      this->ii->setPixmap(ic.pixmap(12, 12));
      this->ii->show();
    } else {
      this->ii->hide();
    }
    if (this->e->hasMajorMode()) {
      if (!ic.isNull()) {
        this->mi->setText(" " + this->e->getMajorMode().getName());
      } else {
        this->mi->setText(this->e->getMajorMode().getName());
      }
      this->mi->show();
    } else {
      this->mi->hide();
    }
    this->updateDate();
  }  // end QEmacsBuffer::updateBufferInformations

  QEmacsPlainTextEdit &QEmacsBuffer::getMainFrame() {
    return *(this->e);
  }  // end of QEmacsBuffer::getQEmacsTextEdit

  //! \return the current active SecondaryTask if any, nullptr otherwise
  QWidget *QEmacsBuffer::getCurrentSecondaryTask() {
    if(this->stw->isVisible()){
      return this->stw->currentWidget();
    }
    return nullptr;
  }  // end of QEmacsBuffer::getCurrentSecondaryTask

  QString QEmacsBuffer::getCurrentSecondaryTaskTitle() const {
    const auto tb = this->stw->tabBar();
    return tb->tabText(this->stw->currentIndex());
  }  // end of QEmacsBuffer::getCurrentSecondaryTaskTitle

  bool QEmacsBuffer::hasSecondaryTasks() const {
    if (this->stw == nullptr) {
      return false;
    }
    return this->stw->count() != 0;
  }

  QWidget *QEmacsBuffer::addSecondaryTask(const QString &t, QWidget *const s) {
    if (s == nullptr) {
      return nullptr;
    }
    auto *pw = qobject_cast<QWebEngineView *>(s);
    auto *p = qobject_cast<QAbstractScrollArea *>(s);
    if (pw != nullptr) {
      auto *w = new QWebEngineViewWrapper(pw, this);
      this->qemacs.attachSecondaryTask(this,w);
      this->stw->addTab(w, t);
      this->stw->setCurrentWidget(w);
      this->stw->show();
      return w;
    } else if (p != nullptr) {
      auto *w = new QAbstractScrollAreaWrapper(p, this);
      this->qemacs.attachSecondaryTask(this,w);
      this->stw->addTab(w, t);
      this->stw->setCurrentWidget(w);
      this->stw->show();
      return w;
    }
    this->qemacs.attachSecondaryTask(this,s);
    this->stw->addTab(s, t);
    this->stw->setCurrentWidget(s);
    this->stw->show();
    return s;
  }  // end of QEmacsBuffer::addSecondaryTask

  int QEmacsBuffer::getSecondaryTaskIndex(QWidget *const p) const {
    for (int i = 0; i != this->stw->count(); ++i) {
      QWidget *pi = this->stw->widget(i);
      if (pi == p) {
        return i;
      }
      auto *w = qobject_cast<QAbstractScrollAreaWrapper *>(pi);
      if (w != nullptr) {
        if (p == w->getWrappedObject()) {
          return i;
        }
      }
    }
    return -1;
  }  // end of QEmacsBuffer::getSecondaryTaskIndex

  QString QEmacsBuffer::getSecondaryTaskName(QWidget *const p) const {
    QString n;
    const auto i = this->getSecondaryTaskIndex(p);
    if (i != -1) {
      n = this->stw->tabText(i);
    }
    return n;
  }

  void QEmacsBuffer::setSecondaryTaskIcon(QWidget *const p, const QIcon &i) {
    const auto sid = this->getSecondaryTaskIndex(p);
    if (sid != -1) {
      this->stw->setTabIcon(sid, i);
    }
  }

  void QEmacsBuffer::refreshSecondaryTaskTabWidget() {
  }  // end of QEmacsBuffer::refreshSecondaryTaskTabWidget

  void QEmacsBuffer::setSecondaryTaskName(QWidget *const p, const QString &n) {
    const auto i = this->getSecondaryTaskIndex(p);
    if (i != -1) {
      this->stw->setTabText(i, n);
    }
  }  // end of QEmacsBuffer::setSecondaryTaskName

  void QEmacsBuffer::removeSecondaryTask(QWidget *const s) {
    for (int i = 0; i != this->stw->count(); ++i) {
      if (s == this->stw->widget(i)) {
        this->qemacs.detachSecondaryTask(this,s);
        this->stw->removeTab(i);
        if (this->stw->count() == 0) {
          this->stw->hide();
        }
        return;
      }
    }
  }  // end of QEmacsBuffer::removeSecondaryTask

  void QEmacsBuffer::hideSecondaryTask(QWidget *const s) {
    auto h = true;
    for (int i = 0; i != this->stw->count(); ++i) {
      auto *w = this->stw->widget(i);
      if (s == w) {
        s->hide();
      }
      h = h && (!w->isVisible());
    }
    if (h) {
      this->stw->hide();
    }
  }  // end of QEmacsBuffer::hideSecondaryTask

  void QEmacsBuffer::focusCurrentSecondaryTask() {
    if (this->stw != nullptr) {
      auto *s = this->stw->currentWidget();
      if (s != nullptr) {
        s->setFocus();
      }
    }
  }  // end of QEmacsBuffer::focusCurrentSecondaryTask

  bool QEmacsBuffer::areSecondaryTasksVisible() const {
    if (this->stw == nullptr) {
      return false;
    }
    return this->stw->isVisible();
  }

  void QEmacsBuffer::focusMainFrame() {
    this->e->setFocus();
  }  // end of QEmacsBuffer::focusMainFrame

  void QEmacsBuffer::showSecondaryTasks() {
    if (this->stw->count() == 0) {
      this->qemacs.displayInformativeMessage(
          QObject::tr("no SecondaryTask to be shown"));
      return;
    }
    this->stw->show();
  }  // end of QEmacsBuffer::showSecondaryTasks

  void QEmacsBuffer::showSecondaryTasks(const Qt::Orientation o) {
    this->splitter->setOrientation(o);
    this->showSecondaryTasks();
  }  // end of QEmacsBuffer::showSecondaryTasks

  void QEmacsBuffer::hideSecondaryTasks() {
    this->stw->hide();
  }  // end of QEmacsBuffer::hideSecondaryTasks

  void QEmacsBuffer::closeSecondaryTask(int i) {
    this->qemacs.detachSecondaryTask(this, this->stw->widget(i));
    this->stw->removeTab(i);
    if (this->stw->count() == 0) {
      this->stw->hide();
    }
  }  // end of QEmacsBuffer::closeSecondaryTask

  bool QEmacsBuffer::isOkToClose() const {
    return !this->e->document()->isModified();
  }  // end of QEmacsBuffer::isOkToClose

  void QEmacsBuffer::closeCurrentSecondaryTask() {
    if (this->stw->count() != 0) {
      this->closeSecondaryTask(this->stw->currentIndex());
    }
  }  // end of QEmacsBuffer::closeCurrentSecondaryTask

  void QEmacsBuffer::updateMenu() {
    emit updatedMenu();
  }  // end of QEmacsBuffer::updateMenu(){

}  // end of namespace qemacs
