#pragma GCC diagnostic push
#if !__clang__
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <boost/mpi.hpp>
#pragma GCC diagnostic pop

#include <boost/mpi/cartesian_communicator.hpp>

namespace mpi = boost::mpi;
