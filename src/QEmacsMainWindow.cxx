/*!
 * \file   src/QEmacsMainWindow.cxx
 * \brief
 * \author Helfer Thomas
 * \date   27/06/2012
 */

#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFontDialog>
#include <QtWidgets/QApplication>
#include <QtGui/QCloseEvent>
#include "QEmacs/Debug.hxx"
#include "QEmacs/Utilities.hxx"
#include "QEmacs/QEmacsHunspellDictionariesManager.hxx"
#include "QEmacs/QEmacsShortCutStyle.hxx"
#include "QEmacs/QEmacsWidget.hxx"
#include "QEmacs/QEmacsBuffer.hxx"
#include "QEmacs/QEmacsPlainTextEdit.hxx"
#include "QEmacs/QEmacsMajorModeFactory.hxx"
#include "QEmacs/QEmacsMainWindow.hxx"

namespace qemacs {

  QEmacsMainWindow::QEmacsMainWindow(const QStringList &f,
                                     QWidget *const p)
      : QMainWindow(p) {
    auto &s = QEmacsShortCutStyle::getQEmacsShortCutStyle();
    // this->setAttribute(Qt::WA_TranslucentBackground);
    // this->setStyleSheet("background: rgba(255,255,255,100%)");
    auto *e = new QEmacsWidget(this);
    this->setCentralWidget(e);
    this->createMainMenu();
    QObject::connect(&s, &QEmacsShortCutStyle::shortCutStyleChanged,
                     this, &QEmacsMainWindow::updateOptionsMenu);
    QObject::connect(e, &QEmacsWidget::closed, this,
                     &QEmacsMainWindow::close);
    QObject::connect(e, &QEmacsWidget::bufferNameChanged, this,
                     &QEmacsMainWindow::updateBuffersMenu);
    QObject::connect(e, &QEmacsWidget::bufferAdded, this,
                     &QEmacsMainWindow::updateBuffersMenu);
    QObject::connect(e, &QEmacsWidget::bufferRemoved, this,
                     &QEmacsMainWindow::updateBuffersMenu);
    QObject::connect(e, &QEmacsWidget::currentBufferChanged,
                     this, &QEmacsMainWindow::createMainMenu);
    QObject::connect(e, &QEmacsWidget::currentBufferMajorModeChanged,
                     this, &QEmacsMainWindow::createMainMenu);
    QObject::connect(e, &QEmacsWidget::updatedMenu, this,
                     &QEmacsMainWindow::createMainMenu);
    QObject::connect(e, &QEmacsWidget::newTreatedFile, this,
                     &QEmacsMainWindow::addToRecentFiles);
    if (f.empty()) {
      e->createEmptyBuffer();
    } else {
      for (const auto &fn : f) {
        e->openFile(fn);
      }
    }
  } // end of QEmacsMainWindow::QEmacsMainWindow

  void QEmacsMainWindow::addToRecentFiles(const QString &f) {
    QSettings s;
    auto rf = s.value("recent files").toStringList();
    int p = 0;
    // remove previous appearance of the file
    p = rf.indexOf(f);
    while (p != -1) {
      rf.removeAt(p);
      p = rf.indexOf(f);
    }
    rf.push_back(f);
    if (rf.size() >= 50) {
      const auto file = rf.front();
      if (s.contains("positions in files")) {
        auto pl = s.value("positions in files")
                      .value<QMap<QString, QVariant>>();
        if (pl.contains(file)) {
          pl.remove(file);
          s.setValue("positions in files", pl);
        }
      }
      rf.pop_front();
    }
    s.setValue("recent files", rf);
  } // end of QEmacsMainWindow::addToRecentFiles

  void QEmacsMainWindow::closeEvent(QCloseEvent *ev) {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      QObject::disconnect(e, &QEmacsWidget::closed, this,
                          &QEmacsMainWindow::close);
      if (e->close()) {
        ev->accept();
      } else {
        QObject::connect(e, &QEmacsWidget::closed, this,
                         &QEmacsMainWindow::close);
        ev->ignore();
      }
    }
  }  // end of QEmacsMainWindow::close

  void QEmacsMainWindow::openFile() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    const auto &f = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (f.isEmpty()) {
      return;
    }
    e->openFile(f);
  }  // end of QEmacsMainWindow::openFile

  void QEmacsMainWindow::selectFont() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      QSettings s;
      QFontDialog fd;
      fd.setCurrentFont(s.value("font/textedit").value<QFont>());
      if (fd.exec()) {
        auto f = fd.selectedFont();
        f.setStyleStrategy(QFont::PreferAntialias);
        s.setValue("font/textedit", f);
        e->changeMainFramesFont(f);
      }
    }
  }  // end of QEmacsMainWindow::selectFont

  void QEmacsMainWindow::createActions() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    this->na = new QAction(QObject::tr("N&ew"), this);
    this->na->setStatusTip(QObject::tr("New buffer"));
    this->na->setIcon(QIcon::fromTheme("document-new"));
    this->na->setIconVisibleInMenu(true);
    QObject::connect(this->na, &QAction::triggered, e,
                     &QEmacsWidget::createEmptyBuffer);
    this->oa = new QAction(QObject::tr("O&pen"), this);
    this->oa->setStatusTip(QObject::tr("Open a file"));
    this->oa->setIcon(QIcon::fromTheme("document-open"));
    this->oa->setIconVisibleInMenu(true);
    QObject::connect(this->oa, &QAction::triggered, this,
                     &QEmacsMainWindow::openFile);
    this->sa = new QAction(QObject::tr("S&ave current buffer"), this);
    this->sa->setStatusTip(QObject::tr("Save the current buffer"));
    this->sa->setIcon(QIcon::fromTheme("document-save"));
    this->sa->setIconVisibleInMenu(true);
    QObject::connect(this->sa, &QAction::triggered, e,
                     &QEmacsWidget::saveCurrentBuffer);
    this->ka = new QAction(QObject::tr("C&lose current buffer"), this);
    this->ka->setStatusTip(QObject::tr("Close the current buffer"));
    this->ka->setIcon(QIcon::fromTheme("window-close"));
    QObject::connect(this->ka, &QAction::triggered, e,
                     &QEmacsWidget::closeCurrentBuffer);
    this->pra = new QAction(QObject::tr("Print current buffer"), this);
    this->pra->setIcon(QIcon::fromTheme("document-print"));
    this->pra->setIconVisibleInMenu(true);
    this->pra->setStatusTip(QObject::tr("Print the current buffer"));
    QObject::connect(this->pra, &QAction::triggered, this,
                     &QEmacsMainWindow::print);
    this->saa = new QAction(QObject::tr("S&ave all buffers"), this);
    this->saa->setStatusTip(QObject::tr("Save all buffers"));
    this->saa->setIcon(QIcon::fromTheme("document-save"));
    QObject::connect(this->saa, &QAction::triggered, e,
                     &QEmacsWidget::saveAllBuffers);
    // edit actions
    this->ua = new QAction(QObject::tr("U&ndo"), this);
    this->ua->setIcon(QIcon::fromTheme("edit-undo"));
    this->ua->setIconVisibleInMenu(true);
    QObject::connect(this->ua, &QAction::triggered, this,
                     &QEmacsMainWindow::undo);
    this->ra = new QAction(QObject::tr("R&edo"), this);
    this->ra->setIcon(QIcon::fromTheme("edit-redo"));
    this->ra->setIconVisibleInMenu(true);
    QObject::connect(this->ra, &QAction::triggered, this,
                     &QEmacsMainWindow::redo);
    this->sea = new QAction(QObject::tr("S&elect All"), this);
    this->sea->setIcon(QIcon::fromTheme("edit-select-all"));
    this->sea->setIconVisibleInMenu(true);
    QObject::connect(this->sea, &QAction::triggered, this,
                     &QEmacsMainWindow::selectAll);
    this->ca = new QAction(QObject::tr("C&ut"), this);
    this->ca->setStatusTip(QObject::tr("Cut the selected text"));
    this->ca->setIcon(QIcon::fromTheme("edit-cut"));
    this->ca->setIconVisibleInMenu(true);
    QObject::connect(this->ca, &QAction::triggered, this,
                     &QEmacsMainWindow::cut);
    this->ca2 = new QAction(QObject::tr("C&opy"), this);
    this->ca2->setStatusTip(QObject::tr("Copy the selected text to clipboard"));
    this->ca2->setIcon(QIcon::fromTheme("edit-copy"));
    this->ca2->setIconVisibleInMenu(true);
    QObject::connect(this->ca2, &QAction::triggered, this,
                     &QEmacsMainWindow::copy);
    this->pa = new QAction(QObject::tr("P&aste"), this);
    this->pa->setStatusTip(QObject::tr("Paste from clipboard"));
    this->pa->setIcon(QIcon::fromTheme("edit-paste"));
    this->pa->setIconVisibleInMenu(true);
    QObject::connect(this->pa, &QAction::triggered, this,
                     &QEmacsMainWindow::paste);
    // exit actions
    this->ea = new QAction(QObject::tr("E&xit"), this);
    this->ea->setStatusTip(QObject::tr("Exit qemacs"));
    this->ea->setIcon(QIcon::fromTheme("window-close"));
    this->ea->setIconVisibleInMenu(true);
    QObject::connect(this->ea, &QAction::triggered, this,
                     &QEmacsMainWindow::close);
    // font actions
    this->fa = new QAction(QObject::tr("Main Frames' F&ont"), this);
    this->fa->setIcon(QIcon::fromTheme("preferences-desktop-font"));
    this->fa->setStatusTip(QObject::tr("Select Font"));
    QObject::connect(this->fa, &QAction::triggered, this,
                     &QEmacsMainWindow::selectFont);
    this->esca =
        new QAction(QObject::tr("Change to emacs's style shortcuts"), this);
    this->esca->setStatusTip(QObject::tr("Use emacs's style shortcuts"));
    this->esca->setIcon(QIcon::fromTheme("preferences-desktop-keyboard"));
    QObject::connect(this->esca, &QAction::triggered, this,
                     &QEmacsMainWindow::useEmacsShortCuts);
    this->qsca =
        new QAction(QObject::tr("Change to Qt's style shortcuts"), this);
    this->qsca->setStatusTip(QObject::tr("Use Qt's style shortcuts"));
    this->qsca->setIcon(QIcon::fromTheme("preferences-desktop-keyboard"));
    QObject::connect(this->qsca, &QAction::triggered, this,
                     &QEmacsMainWindow::useQtShortCuts);
    // help actions
    this->aa = new QAction(QObject::tr("&About"), this);
    this->aa->setStatusTip(QObject::tr("Show the application's About box"));
    this->aa->setIcon(QIcon::fromTheme("help-about"));
    this->aa->setIconVisibleInMenu(true);
    connect(this->aa, &QAction::triggered, this,
            &QEmacsMainWindow::about);
    this->aa2 = new QAction(QObject::tr("About &Qt"), this);
    this->aa2->setStatusTip(QObject::tr("Show the Qt library's About box"));
    connect(this->aa2, &QAction::triggered, qApp,
            &QApplication::aboutQt);
  }  // end of QEmacsMainWindow::createActions

  void QEmacsMainWindow::updateOptionsMenu() {
#ifdef QEMACS_HUNSPELL_SUPPORT
    using  HunspellDictionaries = QEmacsHunspellDictionariesManager;
#endif /* QEMACS_HUNSPELL_SUPPORT */
    auto &s = QEmacsShortCutStyle::getQEmacsShortCutStyle();
    this->om->clear();
    // font
    this->om->addAction(this->fa);
#ifdef QEMACS_HUNSPELL_SUPPORT
    // available dictionaries
    auto &dm =
        HunspellDictionaries::getQEmacsHunspellDictionariesManager();
    const auto dicts = dm.getAvailableDictionnaries();
    if (!dicts.isEmpty()) {
      auto *const d = this->om->addMenu(QObject::tr("Dictionaries"));
      d->setIcon(QIcon::fromTheme("accessories-dictionary"));
      for (const auto& di : dicts) {
        auto *a = d->addAction(di);
        a->setData(di);
	QObject::connect(a, &QAction::triggered, this,[di,this](){
	    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
	    if (e != nullptr) {
	      auto&t = e->getCurrentBuffer().getMainFrame();
	      t.setSpellCheckLanguage(di);
	    }
	  });
      }
    }
#endif /* QEMACS_HUNSPELL_SUPPORT */
    // short cuts
    if (s.getStyle() != QEmacsShortCutStyle::EMACS) {
      this->om->addAction(this->esca);
    }
    if (s.getStyle() != QEmacsShortCutStyle::QT) {
      this->om->addAction(this->qsca);
    }
  }  // end of QEmacsMainWindow::updateOptionsMenu

  void QEmacsMainWindow::updateBuffersMenu() {
    clearMenu(this->bm);
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      const auto &bnames = e->getBuffersNames();
      const auto &bicons = e->getBuffersIcons();
      const auto &bids = e->getBuffersIds();
      if ((bnames.size() != bicons.size()) ||
          (bnames.size() != bids.size())) {
        return;
      }
      for (int i = 0; i != bicons.size(); ++i) {
        const auto &n = bnames[i];
        const auto &ic = bicons[i];
        const int id = bids[i];
	auto *a = this->bm->addAction(n);
        if (!ic.isNull()) {
          a->setIcon(ic);
          a->setIconVisibleInMenu(true);
        }
	QObject::connect(a, &QAction::triggered, this,[this,id](){
	    auto *qw = qobject_cast<QEmacsWidget *>(this->centralWidget());
	    if (qw != nullptr) {
	      qw->changeBuffer(id);
	    }
	  });
      }
    }
  }  // end of QEmacsMainWindow::updateBuffersMenu

  void QEmacsMainWindow::useEmacsShortCuts() {
    auto &s = QEmacsShortCutStyle::getQEmacsShortCutStyle();
    s.setStyle(QEmacsShortCutStyle::EMACS);
    this->updateOptionsMenu();
  }  // end of QEmacsMainWindow::useEmacsShortCuts

  void QEmacsMainWindow::useQtShortCuts() {
    auto &s = QEmacsShortCutStyle::getQEmacsShortCutStyle();
    s.setStyle(QEmacsShortCutStyle::QT);
    this->updateOptionsMenu();
  }  // end of QEmacsMainWindow::useQtShortCuts

  void QEmacsMainWindow::createMainMenu() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    QSettings settings;
    clearMenuBar(this->menuBar());
    this->createActions();
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
// https://bugreports.qt.io/browse/QTBUG-46812
// https://codereview.qt-project.org/#/c/170352/
//
#endif
    this->fm = this->menuBar()->addMenu(QObject::tr("File"));
    this->fm->addAction(this->na);
    this->fm->addAction(this->oa);
    if (settings.contains("recent files")) {
      const auto files = settings.value("recent files").toStringList();
      debug("recent files:", files);
      auto *rfm =
          this->fm->addMenu(QIcon::fromTheme("document-open-recent"),
                            QObject::tr("&Recent Files"));
      //      rfm->setIconVisibleInMenu(true);
      this->createRecentFilesMenu(rfm, files);
    }
    this->fm->addAction(this->sa);
    this->fm->addAction(this->ka);
    this->fm->addSeparator();
    this->fm->addAction(this->pra);
    this->fm->addSeparator();
    this->fm->addAction(this->saa);
    this->fm->addSeparator();
    this->fm->addAction(this->ea);
    // edit menu
    this->em = this->menuBar()->addMenu(QObject::tr("Edit"));
    this->em->addAction(this->ua);
    this->em->addAction(this->ra);
    this->em->addSeparator();
    this->em->addAction(this->ca);
    this->em->addAction(this->ca2);
    this->em->addAction(this->pa);
    this->em->addAction(this->sea);
    // buffer menu
    this->bm = this->menuBar()->addMenu(QObject::tr("Buffers"));
    this->updateBuffersMenu();
    // qemacs menu
    this->bm = this->menuBar()->addMenu(QObject::tr("QEmacs"));
    auto *const qcmds = this->bm->addMenu(QObject::tr("Commands"));
    auto *const lqcmd = qcmds->addAction(QObject::tr("Launch"));
    QObject::connect(lqcmd, &QAction::triggered, e,
                     static_cast<void (QEmacsWidget::*)(void)>(
                         &QEmacsWidget::launchCommand));
    auto *const qmm = this->bm->addMenu(QObject::tr("Major Mode"));
    auto &mf = QEmacsMajorModeFactory::getQEmacsMajorModeFactory();
    for (const auto &mn : mf.getAvailableQEmacsMajorModesNames()) {
      auto *const qmma = qmm->addAction(mn);
      qmma->setIcon(mf.getQEmacsMajorModeIcon(mn));
      QObject::connect(qmma, &QAction::triggered, e, [e, mn] {
        e->getCurrentBuffer().getMainFrame().setMajorMode(mn);
      });
    }
    // menu associated with the current major mode
    for (const auto &m : e->getCurrentBufferSpecificMenus()) {
      this->menuBar()->addMenu(m);
    }
    // options
    this->om = this->menuBar()->addMenu(QObject::tr("Options"));
    this->updateOptionsMenu();
    // help menu
    this->hm = this->menuBar()->addMenu(QObject::tr("Help"));
    this->hm->addAction(this->aa);
    this->hm->addAction(this->aa2);
  }  // end of QEmacsMainWindow::createMainMenu

  void QEmacsMainWindow::createRecentFilesMenu(
      QMenu *const m, const QStringList &files) {
    auto &mf = QEmacsMajorModeFactory::getQEmacsMajorModeFactory();
    auto n = int{};
    const auto s = files.size();
    if (s == 0) {
      return;
    }
    for (int i = s - 1; (i >= 0) && (n != 5); --i, ++n) {
      const auto&f = files.at(i);
      QFileInfo fi(f);
      if ((fi.exists()) && (fi.isFile()) && (fi.isReadable())) {
        auto *rf = m->addAction(fi.fileName());
        const auto mn = mf.getQEmacsMajorModeNameForFile(fi.fileName());
        rf->setIcon(mf.getQEmacsMajorModeIcon(mn));
        const auto path = fi.absoluteFilePath();
	QObject::connect(rf,&QAction::triggered,this,[this,path]{
	    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
	    if (e != nullptr) {
	      e->openFile(path);
	    }
	  });
      }
    }
  }  // end of QEmacsMainWindow::createRecentFilesMenu

  void QEmacsMainWindow::about() {
    QMessageBox::about(this, tr("About QEmacs"),
                       tr("The <b>QEmacs</b> is a simple "
                          "qt based text editor."));
  }  // end of QEmacsMainWindow::about()

  void QEmacsMainWindow::cut() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      auto& t = e->getCurrentBuffer().getMainFrame();
      t.cut();
    }
  }  // end of QEmacsMainWindow::cut

  void QEmacsMainWindow::undo() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      auto& t = e->getCurrentBuffer().getMainFrame();
      t.undo();
    }
  }  // end of QEmacsMainWindow::undo

  void QEmacsMainWindow::redo() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      auto& t = e->getCurrentBuffer().getMainFrame();
      t.redo();
    }
  }  // end of QEmacsMainWindow::redo

  void QEmacsMainWindow::selectAll() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      auto& t = e->getCurrentBuffer().getMainFrame();
      t.selectAll();
    }
  }  // end of QEmacsMainWindow::selectAll

  void QEmacsMainWindow::copy() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      auto& t = e->getCurrentBuffer().getMainFrame();
      t.copy();
    }
  }  // end of QEmacsMainWindow::copy

  void QEmacsMainWindow::paste() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      auto& t = e->getCurrentBuffer().getMainFrame();
      t.paste();
    }
  }  // end of QEmacsMainWindow::paste

  void QEmacsMainWindow::print() {
    auto *e = qobject_cast<QEmacsWidget *>(this->centralWidget());
    if (e != nullptr) {
      e->print();
    }
  } // end of QEmacsMainWindow::print

  QSize QEmacsMainWindow::sizeHint() const {
    return QSize(800, 600);
  }  // end of QEmacsMainWindow::sizeHint()

}  // end of namespace qemacs
