#ifndef Longitude_h
#define Longitude_h

/**
 * @file
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "Angle.h"
#include <QList>
#include <QPair>

namespace Isis {
  class PvlGroup;

  /**
   * This class is designed to encapsulate the concept of a Longitude. This is
   *   used primary for surface points but is also a general purpose class.
   *   This class accepts values past the longitude domain limits. This adds
   *   the concept of longitude direction to the Angle class.
   *
   * @ingroup Utility
   *
   * @author 2010-10-12 Steven Lambright
   *
   * @internal
   *   @history 2011-01-25 Steven Lambright - Added a constructor which takes a
   *                           mapping group.
   *   @history 2012-06-29 Kimberly Oyama and Steven Lambright - added to360Range()
   *                           to calculate where the longitude range is in 0-360.
   *                           Also updated the unit tests to test inRange() and
   *                           to360Range(). References #958.
   *   @history 2012-08-10 Kimberly Oyama - Modified force360Domain() so that if
   *                           the angle is 360, it is left at 360 instead of being
   *                           changed to 0. Added a test case to the unit test to
   *                           exercise this change. Fixes #999.
   */
  class Longitude : public Angle {
    public:
      /**
       * Possible longitude directions: Is a positive longitude towards east
       *   or towards west?
       */
      enum Direction {
        //! As the longitude increases the actual position is more east
        PositiveEast,
        //! As the longitude increases the actual position is more west
        PositiveWest
      };

      /**
       * Use LongitudeDomain360 if 0-360 is the primary range of the longitude
       *   values with 180 being the 'center'. use LongitudeDomain180 if 0 is
       *   the 'center' longitude. This is used for converting between longitude 
       *   directions, because a 'center' longitude of 0 (-180 to 180) implies
       *   negation is all that needs done to reverse the longitude direction,
       *   whereas if you are in the 360 domain, where 180 is the center, more
       *   needs done.
       */
      enum Domain {
        //! As the longitude increases the actual position is more east
        Domain360,
        //! As the longitude increases the actual position is more west
        Domain180
      };

      Longitude();
      Longitude(double longitude,
                Angle::Units longitudeUnits,
                Direction lonDir = PositiveEast,
                Domain lonDomain = Domain360);
      Longitude(Angle longitude,
                Direction lonDir = PositiveEast,
                Domain lonDomain = Domain360);
      Longitude(double longitude,
                PvlGroup mapping,
                Angle::Units longitudeUnits);
      Longitude(const Longitude &longitudeToCopy);
      ~Longitude();

      double positiveEast(Angle::Units units = Angle::Radians) const;
      void setPositiveEast(double longitude,
                           Angle::Units units = Angle::Radians);
      double positiveWest(Angle::Units units = Angle::Radians) const;
      void setPositiveWest(double longitude,
                           Angle::Units units = Angle::Radians);

      Longitude force180Domain() const;
      Longitude force360Domain() const;

      bool inRange(Longitude min, Longitude max) const;
      static QList< QPair<Longitude, Longitude> > to360Range(Longitude startLon, Longitude endLon);

      /**
       * Same as positiveEast.
       *
       * @see positiveEast
       
      operator double() const {
        return positiveEast();
      }*/

      Longitude& operator=(const Longitude & longitudeToCopy);

    private:
      /**
       * This is necessary for converting to PositiveWest and back.
       */
      Domain m_currentDomain;
  };
}

#endif
