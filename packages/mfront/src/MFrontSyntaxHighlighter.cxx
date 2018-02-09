/*! 
 * \file  MFrontSyntaxHighlighter.MFront
 * \brief
 * \author Helfer Thomas
 * \brief 30 juin 2012
 */

#include <stdexcept>

#include "MFront/AbstractDSL.hxx"
#include "MFront/DSLFactory.hxx"
#include "MFront/SupportedTypes.hxx"
#include "QEmacs/MFrontSyntaxHighlighter.hxx"

namespace qemacs
{
  
  MFrontSyntaxHighlighter::MFrontSyntaxHighlighter(QTextDocument *p,
						   const QString& n)
    : CxxSyntaxHighlighter(p)
  {
    std::vector<std::string> keys;
    try{
      auto& f = mfront::DSLFactory::getDSLFactory();
      std::shared_ptr<mfront::AbstractDSL> dsl{f.createNewParser(n.toStdString())};
      dsl->getKeywordsList(keys);
    } catch(std::exception&){
      return;
    }
    this->mfrontKeyFormat.setForeground(Qt::blue);
    for(const auto &k: keys) {
      HighlightingRule rule;
      rule.key     = k;
      rule.format  = this->mfrontKeyFormat;
      this->highlightingRules.push_front(rule);
    }
    for(const auto& st : mfront::SupportedTypes::getTypeFlags()){
      HighlightingRule rule;
      rule.key     = st.first;
      rule.format  = this->keyFormat;
      this->highlightingRules.push_front(rule);
    }
  } // end of MFrontSyntaxHighlighter::MFrontSyntaxHighlighter

  MFrontSyntaxHighlighter::~MFrontSyntaxHighlighter() = default;

} // end of namespace qemacs
