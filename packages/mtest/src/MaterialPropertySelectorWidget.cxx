/*!
 * \file   MaterialPropertySelectorWidget.cxx
 * \brief    
 * \author Thomas Helfer
 * \date   01/03/2018
 */
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include "QEmacs/ImportMFMMaterialProperty.hxx"
#include "QEmacs/MaterialPropertySelectorWidget.hxx"

namespace qemacs {

  MaterialPropertySelectorWidget::MaterialPropertySelectorWidget(
      const QString& n, const QString& m, QWidget* const p)
      : QGroupBox(n, p),
        name(n),
        material(m),
        cv(new QLineEdit),
        cle(new QLineEdit),
        cfe(new QLineEdit) {
    auto* const ml = new QVBoxLayout();
    auto* const mt = new QComboBox();
    mt->addItems(QStringList() << "constant"
                               << "castem");
    ml->addWidget(mt);
    auto * mpd = new QStackedWidget;
    // a line edit to set the constant value
    this->cv->setFixedHeight(QFontMetrics(cv->font()).lineSpacing());
    auto* const vv = new QDoubleValidator;
    vv->setNotation(QDoubleValidator::ScientificNotation);
    this->cv->setValidator(vv);
    mpd->addWidget(this->cv);
    // a widget to set a castem material property
    auto* const cw = new QGroupBox;
    auto* const gl = new QGridLayout;
    // library
    gl->addWidget(new QLabel(QObject::tr("Library")), 0, 0);
    this->cle->setReadOnly(true);
    this->cle->setFixedHeight(QFontMetrics(cle->font()).lineSpacing());
    gl->addWidget(this->cle, 0, 1);
    // function
    gl->addWidget(new QLabel(QObject::tr("Function")), 1, 0);
    this->cfe->setReadOnly(true);
    this->cfe->setFixedHeight(QFontMetrics(cfe->font()).lineSpacing());
    gl->addWidget(this->cfe, 1, 1);
    // import buttons
    auto* const co = new QPushButton("Browse");
    co->setIcon(QIcon::fromTheme("document-open"));
    QObject::connect(co, &QPushButton::clicked, this,
                     &MaterialPropertySelectorWidget::import);
    auto* const cmfm = new QPushButton("Import");
    QObject::connect(cmfm, &QPushButton::clicked, this,
                     &MaterialPropertySelectorWidget::importFromMFM);
    //
    auto* const chl = new QHBoxLayout;
    chl->addLayout(gl);
    auto* const bl = new QVBoxLayout;
    bl->addWidget(co);
    bl->addWidget(cmfm);
    chl->addLayout(bl);
    cw->setLayout(chl);
    mpd->addWidget(cw);
    //
    ml->addWidget(mpd);
    this->setLayout(ml);

    QObject::connect(mt, static_cast<void (QComboBox::*)(int)>(
                             &QComboBox::activated),
                     this, [this, mpd](const int i) {
                       if (i != mpd->currentIndex()) {
                         this->cv->clear();
                         this->cle->clear();
                         this->cfe->clear();
                         mpd->setCurrentIndex(i);
                       }
                     });
  }  // end
  // of MaterialPropertySelectorWidget::MaterialPropertySelectorWidget

  void MaterialPropertySelectorWidget::import() {
  }  // end of MaterialPropertySelectorWidget::import

  void MaterialPropertySelectorWidget::importFromMFM() {
    ImportMFMMaterialProperty::Options o;
    o.name = this->name;
    o.material = this->material;
    o.minterface = "Castem";
    ImportMFMMaterialProperty w(o);
    if (w.exec() != QDialog::Accepted) {
      return;
    }
    const auto l = w.getLibrary();
    const auto f = w.getFunction();
    if ((!l.isEmpty()) && (!f.isEmpty())) {
      this->cle->setText(l);
      this->cfe->setText(f);
    }
  }  // end of MaterialPropertySelectorWidget::importFromMFM

  MaterialPropertySelectorWidget::~MaterialPropertySelectorWidget() =
      default;

}  // end of namespace qemacs
