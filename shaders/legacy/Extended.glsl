#ifdef STAGE_TESS_CTRL

#include <Tess_Ctrl_Output>

#endif

#ifdef STAGE_TESS_EVAL

#include <Tess_Eval_Input>

#endif

#ifdef STAGE_GEOMETRY

#include <Geometry_Input>
#include <Geometry_Output>

#endif

#ifdef STAGE_COMPUTE

#include <Compute_Threads>

#endif