#ifndef FCN_WALSH
#define FCN_WALSH

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "ScalarFunction.h"

class FUNCTIONS_LINKAGE_ FCN_walsh : public ScalarFunction
{
public:
  static FCN_walsh *Definition()
  { return &fcn_walsh_; }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_walsh() : ScalarFunction( "WALSH", 2, 2 )
  {}

  static FCN_walsh fcn_walsh_;
};

#endif

