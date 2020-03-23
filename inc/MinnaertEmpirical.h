#ifndef MinnaertEmpirical_h
#define MinnaertEmpirical_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/06/19 15:18:26 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "NumericalApproximation.h"
#include "PhotoModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Empirical Minnaert photometric model
   *  Derive model albedo using phase dependent Minnaert equation
   *  and calculated empirically.
   *  Limb-darkening k and phase function are arbitrary polynomials
   *  in phase angle.
   *  \code
   *  albedo = brightness*[mu / (mu*munot)**k)]
   *  assumptions:
   *    1. bidirectional reflectance
   *    2. semi-infinite medium
   *                                               k      k-1
   *  reflectance (inc,ema,phase)=albedo  *  munot   * mu
   *           Minnaert               Minnaert
   *  \endcode
   *
   *  Where k is the Minnaert index, an empirical constant (called nu in Hapke)
   *
   *  If k (nu) = 1, Minnaert's law reduces to Lambert's law.
   *  See Theory of Reflectance and Emittance Spectroscopy, 1993;
   *  Bruce Hapke; pg. 191-192.
   *
   * @author 1999-01-08 Randy Kirk
   *
   * @internal
   *  @history 2011-08-17 Janet Barrett - Ported from ISIS2 to ISIS3.
   */
  class MinnaertEmpirical : public PhotoModel {
    public:
      MinnaertEmpirical(Pvl &pvl);
      virtual ~MinnaertEmpirical();

      void SetPhotoPhaseList(QString phasestrlist);
      void SetPhotoKList(QString kstrlist);
      void SetPhotoPhaseCurveList(QString phasecurvestrlist);

      //! Return photometric phase angle list
//      inline std::vector<double> PhotoPhaseList() const {
//        return p_photoPhaseList;
//      };
      //! Return photometric k value list
//      inline std::vector<double> PhotoKList() const {
//        return p_photoKList;
//      };
      //! Return photometric phase curve value list
//      inline std::vector<double> PhotoPhaseCurveList() const {
//        return p_photoPhaseCurveList;
//      };

      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);

    private:
//      int p_photoPhaseAngleCount;
//      std::vector<double> p_photoPhaseList;
//      std::vector<double> p_photoKList;
//      std::vector<double> p_photoPhaseCurveList;
//      NumericalApproximation p_photoKSpline;
//      NumericalApproximation p_photoBSpline;
  };
};

#endif
