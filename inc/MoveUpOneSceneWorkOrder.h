#ifndef MoveUpOneSceneWorkOrder_H
#define MoveUpOneSceneWorkOrder_H

#include "MosaicSceneWorkOrder.h"

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief Move images, one by one, on top of the immediately-above intersecting image in a scene
   * This workorder is synchronous and undoable
   *
   * This shows up as "Bring Forward" to the user.
   *
   * @author 2012-10-04 Stuart Sides and Steven Lambright
   *
   * @internal
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   */
  class MoveUpOneSceneWorkOrder : public MosaicSceneWorkOrder {
      Q_OBJECT
    public:
      MoveUpOneSceneWorkOrder(MosaicSceneWidget *scene, Project *project);
      MoveUpOneSceneWorkOrder(Project *project);
      MoveUpOneSceneWorkOrder(const MoveUpOneSceneWorkOrder &other);
      ~MoveUpOneSceneWorkOrder();

      virtual MoveUpOneSceneWorkOrder *clone() const;

      void execute();
      void undoExecution();

    private:
      MoveUpOneSceneWorkOrder &operator=(const MoveUpOneSceneWorkOrder &rhs);
  };
}

#endif
