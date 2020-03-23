#ifndef Albedo_h
#define Albedo_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/06/18 17:26:08 $
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

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Albedo normalization
   *
   * Consistent dividing out of photometric model at given an-
   * gles and putting it back in at reference incidence but zero
   * phase.  Let the reference incidence default to zero.  For
   * Hapke model only, the photometric function multiplied back
   * in will be modified to take out opposition effect.  This
   * requires saving the actual value of B0 while temporarily
   * setting it to zero in the common block to compute the over-
   * all normalization.
   *
   *
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2007-08-15 Steven Lambright - Refactored code
   *  @history 2008-03-07 Janet Barrett - Changed name of Incmatch
   *                      parmater to Incmat
   *  @history 2008-06-18 Christopher Austin - Fixed documentation errors
   *  @history 2010-11-10 Janet Barrett - Added reference parameters for
   *                      phase and emission so user can specify normalization
   *                      conditions in initialization
   *  @history 2010-11-30 Janet Barrett - Added ability to use photometric angles
   *                      from the ellipsoid or the DEM
   *  @history 2017-07-03 Makayla Shepherd - Updated documentation. References #4807.
   *
   */
  class Albedo : public NormModel {
    public:
      Albedo(Pvl &pvl, PhotoModel &pmodel);
      virtual ~Albedo() {};

    protected:
      /**
       * Performs the normalization.
       * 
       * @param pha The phase angle.
       * @param inc The incidence angle.
       * @param ema The emission angle.
       * @param dn The DN value
       * @param albedo ???
       * @param mult The multiplier of the image
       * @param base The base of the image
       */
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                        double dn, double &albedo, double &mult, double &base) {};
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double deminc, double demema, double dn, double &albedo,
                                      double &mult, double &base);

    private:
      //! Set parameters needed for albedo normalization
      void SetNormPharef(const double pharef);
      void SetNormIncref(const double incref);
      void SetNormEmaref(const double emaref);
      void SetNormIncmat(const double incmat);
      void SetNormThresh(const double thresh);
      void SetNormAlbedo(const double albedo);

      double p_normPsurfref; //!< ???
      double p_normPharef; //!< The reference phase angle
      double p_normIncref; //!< The reference incidence angle
      double p_normEmaref; //!< The reference emission angle
      double p_normThresh; //!< Used to amplify variations in the input image
      double p_normIncmat; //!< Incmat
      double p_normAlbedo; //!< The albedo
  };
};

#endif
