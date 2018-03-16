  /*!
 * \file   SelectMFMBehaviour.hxx
 * \brief
 * \author Thomas Helfer
 * \date   12/03/2018
 */

#ifndef LIB_QEMACS_SELECTMFMBEHAVIOUR_HXX
#define LIB_QEMACS_SELECTMFMBEHAVIOUR_HXX

#include <QtWidgets/QComboBox>
#include <QtWidgets/QTreeView>
#include "QEmacs/BehaviourDescription.hxx"

namespace qemacs {

  // forward declaration
  struct QEmacsWidget;

  struct QEMACS_MIW_VISIBILITY_EXPORT SelectMFMBehaviour : QWidget {
    //! option of the dialog box
    struct Options {
      //! \brief constructor
      Options();
      //! \brief move constructor
      Options(Options&&);
      //! \brief copy constructor
      Options(const Options&);
      //! \brief move assignement
      Options& operator=(Options&&);
      //! \brief constructor
      Options& operator=(const Options&);
      /*!
       * \brief name of the material property. This is used to
       * initialize the name filter.
       */
      QString name;
      /*!
       * \brief name of the material. This is used to initialize the
       * material filter.
       */
      QString material;
      /*!
       * \brief if this member is not empty, the user will not be able
       * to select the interface of the material property. The material
       * properties displayed would use the given interface.
       */
      QString minterface;
    };
    /*!
     * \brief constructor
     * \param[in] q: qemacs widget
     * \param[in] o: options
     * \param[in] p: parent
     */
    SelectMFMBehaviour(QEmacsWidget& q,
                       const Options& o,
                       QWidget* const p = nullptr);
    //! \return the selected behaviour.
    BehaviourDescription getSelectedBehaviour() const;
    //! destructor
    ~SelectMFMBehaviour() override;

   signals:

    void doubleClicked(const BehaviourDescription&);

    void behaviourDescriptionChanged(const BehaviourDescription &);
    
   protected:
    QTreeView* view;
    // interface
    QComboBox* isb;
    // hypothesis
    QComboBox* hsb;

    BehaviourDescription getSelectedBehaviour(const QModelIndex) const;

   private:
    Q_OBJECT
  };  // end of struct SelectMFMBehaviour

}  // end of namespace qemacs

#endif /* LIB_QEMACS_SELECTMFMBEHAVIOUR_HXX */
