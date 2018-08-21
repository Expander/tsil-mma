// ====================================================================
// This file is part of tsil-mma.
//
// tsil-mma is licenced under the GNU General Public License (GNU GPL)
// version 3.
// ====================================================================

#include <complex>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <mathlink.h>
#include <WolframLibrary.h>

#include "tsil_cpp.h"
#include "tsil_global.h"
#include "tsil_names.h"

namespace {

/********************* put types *********************/

inline void MLPut(MLINK link, const std::string& s)
{
   MLPutSymbol(link, s.c_str());
}

inline void MLPut(MLINK link, int c)
{
   MLPutInteger(link, c);
}

inline void MLPut(MLINK link, double c)
{
   MLPutReal(link, c);
}

inline void MLPut(MLINK link, std::complex<double> c)
{
   if (std::imag(c) == 0.) {
      MLPutReal(link, std::real(c));
   } else {
      MLPutFunction(link, "Complex", 2);
      MLPutReal(link, std::real(c));
      MLPutReal(link, std::imag(c));
   }
}

template <class T>
void MLPut(MLINK link, const std::vector<T>& v)
{
   MLPutFunction(link, "List", v.size());
   for (std::size_t i = 0; i < v.size(); i++)
      MLPut(link, v[i]);
}

/********************* put rules to types *********************/

void MLPutRule(MLINK link, const std::string& name)
{
   MLPutFunction(link, "Rule", 2);
   MLPutUTF8Symbol(link, reinterpret_cast<const unsigned char*>(name.c_str()), name.size());
}

template <class T1, class T2>
void MLPutRuleTo(MLINK link, T1 t, const T2& name)
{
   MLPutRule(link, name);
   MLPut(link, t);
}

/********************* read types *********************/

template<class T> T MLRead(MLINK link);

template<>
double MLRead(MLINK link)
{
   double val = 0.;

   if (!MLGetReal64(link, &val)) {
      throw std::runtime_error("Cannot read value from parameter list!");
   }

   return val;
}

template<>
long double MLRead(MLINK link)
{
   long double val = 0.;

   if (!MLGetReal128(link, &val)) {
      throw std::runtime_error("Cannot read value from parameter list!");
   }

   return val;
}

/******************************************************************/

void put_message(MLINK link,
                 const std::string& message_function,
                 const std::string& message_str)
{
   MLPutFunction(link, "CompoundExpression", 2);
   MLPutFunction(link, message_function.c_str(), 1);
   MLPutUTF8String(link, reinterpret_cast<const unsigned char*>(message_str.c_str()), message_str.size());
}

/******************************************************************/

class Redirect_output {
public:
   explicit Redirect_output(MLINK link_)
      : link(link_)
      , buffer()
      , old_cout(std::cout.rdbuf(buffer.rdbuf()))
      , old_cerr(std::cerr.rdbuf(buffer.rdbuf()))
      {}

   ~Redirect_output() {
      std::cout.rdbuf(old_cout);
      std::cerr.rdbuf(old_cerr);
      flush();
   }

   void flush() {
      std::string line;
      while (std::getline(buffer, line)) {
         put_message(link, "TSILInfoMessage", line);
      }
   }

private:
   MLINK link;               ///< redirect to this link
   std::stringstream buffer; ///< buffer caching stdout
   std::streambuf* old_cout; ///< original stdout buffer
   std::streambuf* old_cerr; ///< original stderr buffer
};

/******************************************************************/

long number_of_args(MLINK link, const std::string& head)
{
   long argc;

   if (!MLCheckFunction(link, head.c_str(), &argc))
      std::cerr << "Error: argument is not a " << head << std::endl;

   return argc;
}

/******************************************************************/

bool check_number_of_args(MLINK link, long number_of_arguments,
                          const std::string& function_name)
{
   const auto n_given = number_of_args(link, "List");
   const bool ok = n_given == number_of_arguments;

   if (!ok) {
      std::cerr << "Error: " << function_name << " expects "
                << number_of_arguments << " argument ("
                << n_given << " given)." << std::endl;
   }

   return ok;
}

/******************************************************************/

std::vector<TSIL_REAL> read_list(MLINK link)
{
   int N = 0;

   if (!MLTestHead(link, "List", &N)) {
      throw std::runtime_error("TSILEvaluateLoopFunctions expects a list"
                               " as the only argument!");
   }

   std::vector<TSIL_REAL> vec(N, 0.);

   for (int i = 0; i < N; i++) {
      vec[i] = MLRead<TSIL_REAL>(link);
   }

   if (!MLNewPacket(link)) {
      throw std::runtime_error("Cannot create new packet!");
   }

   return vec;
}

/******************************************************************/

TSIL_DATA make_data(const std::vector<TSIL_REAL>& parsvec)
{
   int c = 0; // counter

   const TSIL_REAL x  = parsvec.at(c++);
   const TSIL_REAL y  = parsvec.at(c++);
   const TSIL_REAL z  = parsvec.at(c++);
   const TSIL_REAL u  = parsvec.at(c++);
   const TSIL_REAL v  = parsvec.at(c++);
   const TSIL_REAL s  = parsvec.at(c++);
   const TSIL_REAL qq = parsvec.at(c++);

   TSIL_DATA data;

   TSIL_SetParameters(&data, x, y, z, u, v, qq);
   TSIL_Evaluate(&data, s);

   if (c != parsvec.size()) {
      throw std::runtime_error(
         "Bug: Expecting to read " + std::to_string(parsvec.size()) +
         " input parameters from input vector, but only " + std::to_string(c) +
         " parameters have been read.");
   }

   return data;
}

void put_data(TSIL_DATA& data, MLINK link)
{
   const int len = 1
      + NUM_U_FUNCS * NUM_U_PERMS
      + 2 * NUM_T_FUNCS * NUM_T_PERMS
      + NUM_S_FUNCS * NUM_S_PERMS
      + NUM_B_FUNCS * NUM_B_PERMS
      + NUM_V_FUNCS * NUM_V_PERMS;

#define MLPutRuleToTSILFunction(name) \
   MLPutRuleTo(link, TSIL_GetFunction(&data, name), name)

   MLPutFunction(link, "List", len);
   MLPutRuleTo(link, TSIL_GetFunction(&data, "M"), "Mxyzuv");
   MLPutRuleToTSILFunction("Uzxyv");
   MLPutRuleToTSILFunction("Uzxvy");
   MLPutRuleToTSILFunction("Uuyxv");
   MLPutRuleToTSILFunction("Uuyvx");
   MLPutRuleToTSILFunction("Uxzuv");
   MLPutRuleToTSILFunction("Uxzvu");
   MLPutRuleToTSILFunction("Uyuzv");
   MLPutRuleToTSILFunction("Uyuvz");
   MLPutRuleToTSILFunction("Tvyz");
   MLPutRuleToTSILFunction("Tvzy");
   MLPutRuleToTSILFunction("Tuxv");
   MLPutRuleToTSILFunction("Tuvx");
   MLPutRuleToTSILFunction("Tyzv");
   MLPutRuleToTSILFunction("Tyvz");
   MLPutRuleToTSILFunction("Txuv");
   MLPutRuleToTSILFunction("Txvu");
   MLPutRuleToTSILFunction("Tzyv");
   MLPutRuleToTSILFunction("Tzvy");
   MLPutRuleToTSILFunction("Tvxu");
   MLPutRuleToTSILFunction("Tvux");
   MLPutRuleToTSILFunction("Svyz");
   MLPutRuleToTSILFunction("Szvy");
   MLPutRuleToTSILFunction("Syzv");
   MLPutRuleToTSILFunction("Svzy");
   MLPutRuleToTSILFunction("Syvz");
   MLPutRuleToTSILFunction("Szyv");
   MLPutRuleToTSILFunction("Suxv");
   MLPutRuleToTSILFunction("Svux");
   MLPutRuleToTSILFunction("Sxvu");
   MLPutRuleToTSILFunction("Suvx");
   MLPutRuleToTSILFunction("Sxuv");
   MLPutRuleToTSILFunction("Svxu");
   MLPutRuleToTSILFunction("Bxz");
   MLPutRuleToTSILFunction("Bzx");
   MLPutRuleToTSILFunction("Byu");
   MLPutRuleToTSILFunction("Buy");
   MLPutRuleToTSILFunction("Vzxyv");
   MLPutRuleToTSILFunction("Vzxvy");
   MLPutRuleToTSILFunction("Vuyxv");
   MLPutRuleToTSILFunction("Vuyvx");
   MLPutRuleToTSILFunction("Vxzuv");
   MLPutRuleToTSILFunction("Vxzvu");
   MLPutRuleToTSILFunction("Vyuzv");
   MLPutRuleToTSILFunction("Vyuvz");
   MLPutRuleToTSILFunction("TBARvyz");
   MLPutRuleToTSILFunction("TBARvzy");
   MLPutRuleToTSILFunction("TBARuxv");
   MLPutRuleToTSILFunction("TBARuvx");
   MLPutRuleToTSILFunction("TBARyzv");
   MLPutRuleToTSILFunction("TBARyvz");
   MLPutRuleToTSILFunction("TBARxuv");
   MLPutRuleToTSILFunction("TBARxvu");
   MLPutRuleToTSILFunction("TBARzyv");
   MLPutRuleToTSILFunction("TBARzvy");
   MLPutRuleToTSILFunction("TBARvxu");
   MLPutRuleToTSILFunction("TBARvux");

#undef MLPutRuleToTSILFunction
}

} // anonymous namespace

extern "C" {

/******************************************************************/

DLLEXPORT int TSILEvaluateLoopFunctions(
   WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILEvaluateLoopFunctions"))
      return LIBRARY_TYPE_ERROR;

   try {
      TSIL_DATA data;

      {
         Redirect_output rd(link);
         data = make_data(read_list(link));
      }

      put_data(data, link);
   } catch (const std::exception& e) {
      put_message(link, "TSILErrorMessage", e.what());
      MLPutSymbol(link, "$Failed");
   } catch (...) {
      put_message(link, "TSILErrorMessage", "An unknown exception has been thrown.");
      MLPutSymbol(link, "$Failed");
   }

   return LIBRARY_NO_ERROR;
}

/******************************************************************/

DLLEXPORT mint WolframLibrary_getVersion()
{
   return WolframLibraryVersion;
}

/******************************************************************/

DLLEXPORT int WolframLibrary_initialize(WolframLibraryData /* libData */)
{
   return LIBRARY_NO_ERROR;
}

} // extern "C"
