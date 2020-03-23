#ifndef BusyLeafItem_H
#define BusyLeafItem_H

#include "AbstractLeafItem.h"
#include "AbstractNullDataItem.h"


class QString;
class QVariant;


namespace Isis {

  /**
   * @brief A leaf item that is not ready for user interaction
   *
   * This class represents a leaf item in the tree that is still being
   * calculated (i.e. during filtering).
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class BusyLeafItem : public AbstractNullDataItem, public AbstractLeafItem {
      Q_OBJECT

    public:
      BusyLeafItem(AbstractTreeItem *parent = 0);
      virtual ~BusyLeafItem();
      virtual QVariant getData() const;
      virtual bool isSelectable() const;


    private: // Disallow copying of this class
      BusyLeafItem(const BusyLeafItem &other);
      const BusyLeafItem &operator=(const BusyLeafItem &other);
  };
}

#endif
