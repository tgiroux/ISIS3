#include "RotationFactory.h"
#include "EphemerisRotation.h"
#include "IException.h"


using namespace Isis;

Rotation* RotationFactory::toClassicRotation(EphemerisRotation* rotation) {
  // change source and explicitly upcast
  rotation->setSource(Rotation::Source::Memcache);  
  return (Rotation*)rotation; 
}

Rotation* RotationFactory::toEphemerisRotation(SpiceRotation* rotation, double startTime, double endTime, int cacheSize) { 
    // Check for valid arguments
    if (cacheSize <= 0) {
      QString msg = "Argument cacheSize must not be less or equal to zero";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (startTime > endTime) {
      QString msg = "Argument startTime must be less than or equal to endTime";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if ((startTime != endTime) && (cacheSize == 1)) {
      QString msg = "Cache size must be more than 1 if startTime and endTime differ";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Make sure cache isn't already loaded
    if (rotation->p_source == SpiceRotation::Memcache) {
      QString msg = "A Rotation cache has already been created";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    } 

    // Save full cache parameters
    rotation->p_fullCacheStartTime = startTime;
    rotation->p_fullCacheEndTime = endTime;
    rotation->p_fullCacheSize = cacheSize;

    // Make sure the constant frame is loaded.  This method also does the frame trace.
    if (rotation->p_timeFrames.size() == 0) rotation->InitConstantRotation(startTime);

    // Set the frame type.  If the frame class is PCK, load the constants.
    if (rotation->p_source == SpiceRotation::Spice) {
      rotation->setFrameType();
    }

    rotation->LoadTimeCache();
    cacheSize = rotation->p_cacheTime.size();

    // Loop and load the cache
    for (int i = 0; i < cacheSize; i++) {
      double et = rotation->p_cacheTime[i];
      rotation->SetEphemerisTime(et);
      rotation->p_cache.push_back(rotation->p_CJ);
      if (rotation->p_hasAngularVelocity) rotation->p_cacheAv.push_back(rotation->p_av);
    }

    rotation->p_source = SpiceRotation::Memcache;

    // Downsize already loaded caches (both time and quats)
    if (rotation->p_minimizeCache == SpiceRotation::Yes  &&  cacheSize > 5) {
      rotation->LoadTimeCache();
    }
    std::cout << "Finished conversion" << std::endl; 

    return rotation;
} 


Rotation* RotationFactory::create(json &isdRotation) {
  Rotation* rotation =  new EphemerisRotation();
  rotation->LoadCache(isdRotation);
  return rotation; 
}

Rotation* RotationFactory::create(Table &table) {
  Rotation* rotation =  new EphemerisRotation();
  rotation->LoadCache(table);
  return rotation; 
}

Rotation* RotationFactory::create(int frameCode) {
  return new Rotation(frameCode);
}


