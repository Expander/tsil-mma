// ====================================================================
// This file is part of tsil-mma.
//
// tsil-mma is licenced under the GNU General Public License (GNU GPL)
// version 3.
// ====================================================================

#include <complex>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <mathlink.h>
#include <WolframLibrary.h>

#include "tsil_cpp.h"

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

inline void MLPut(MLINK link, long double c)
{
   MLPutReal128(link, c);
}

template <class T>
void MLPut(MLINK link, std::complex<T> c)
{
   if (std::imag(c) == 0.) {
      MLPut(link, std::real(c));
   } else {
      MLPutFunction(link, "Complex", 2);
      MLPut(link, std::real(c));
      MLPut(link, std::imag(c));
   }
}

template <class T>
void MLPut(MLINK link, const std::vector<T>& v)
{
   MLPutFunction(link, "List", v.size());
   for (std::size_t i = 0; i < v.size(); i++) {
      MLPut(link, v[i]);
   }
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

   if (MLGetReal64(link, &val) == 0) {
      throw std::runtime_error("Cannot read value from parameter list!");
   }

   return val;
}

template<>
long double MLRead(MLINK link)
{
   long double val = 0.;

   if (MLGetReal128(link, &val) == 0) {
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

   if (MLCheckFunction(link, head.c_str(), &argc) == 0) {
      std::cerr << "Error: argument is not a " << head << std::endl;
   }

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

   if (MLTestHead(link, "List", &N) == 0) {
      throw std::runtime_error("TSILEvaluate expects a list"
                               " as the only argument!");
   }

   std::vector<TSIL_REAL> vec(N, 0.);

   for (int i = 0; i < N; i++) {
      vec[i] = MLRead<TSIL_REAL>(link);
   }

   if (MLNewPacket(link) == 0) {
      throw std::runtime_error("Cannot create new packet!");
   }

   return vec;
}

/******************************************************************/

struct TSIL_Mma_Data {
   TSIL_DATA data{};
   TSIL_COMPLEXCPP Ax{}, Ay{}, Az{}, Au{}, Av{};
   TSIL_COMPLEXCPP Ixyv{}, Izuv{};
};

/******************************************************************/

TSIL_Mma_Data make_data(const std::vector<TSIL_REAL>& parsvec)
{
   int c = 0; // counter

   const TSIL_REAL x  = parsvec.at(c++);
   const TSIL_REAL y  = parsvec.at(c++);
   const TSIL_REAL z  = parsvec.at(c++);
   const TSIL_REAL u  = parsvec.at(c++);
   const TSIL_REAL v  = parsvec.at(c++);
   const TSIL_REAL s  = parsvec.at(c++);
   const TSIL_REAL qq = parsvec.at(c++);

   if (c != parsvec.size()) {
      throw std::runtime_error(
         "Bug: Expecting to read " + std::to_string(parsvec.size()) +
         " input parameters from input vector, but only " + std::to_string(c) +
         " parameters have been read.");
   }

   TSIL_Mma_Data data;

   TSIL_SetParameters(&data.data, x, y, z, u, v, qq);
   TSIL_Evaluate(&data.data, s);

   data.Ax = TSIL_A(x, qq);
   data.Ay = TSIL_A(y, qq);
   data.Az = TSIL_A(z, qq);
   data.Au = TSIL_A(u, qq);
   data.Av = TSIL_A(v, qq);

   data.Ixyv = TSIL_I2(x, y, v, qq);
   data.Izuv = TSIL_I2(z, u, v, qq);

   return data;
}

void put_data(TSIL_Mma_Data& data, MLINK link)
{
#include "tsil_global.h"
#include "tsil_names.h"

   const int len = 1 // M
      + NUM_U_FUNCS * NUM_U_PERMS // U
      + 2 * NUM_T_FUNCS * NUM_T_PERMS // T and Tbar
      + NUM_S_FUNCS * NUM_S_PERMS // S
      + NUM_B_FUNCS * NUM_B_PERMS // B
      + NUM_V_FUNCS * NUM_V_PERMS // V
      + 5 // A
      + 2 // I
      ;

   MLPutFunction(link, "List", len);
   MLPutRuleTo(link, c2cpp(TSIL_GetFunction(&data.data, "M")), "Mxyzuv");

   for (int i = 0; i < NUM_U_FUNCS; i++) {
      for (int j = 0; j < NUM_U_PERMS; j++) {
         MLPutRuleTo(link, c2cpp(TSIL_GetFunction(&data.data, uname[i][j])), uname[i][j]);
      }
   }

   for (int i = 0; i < NUM_T_FUNCS; i++) {
      for (int j = 0; j < NUM_T_PERMS; j++) {
         MLPutRuleTo(link, c2cpp(TSIL_GetFunction(&data.data, tname[i][j])), tname[i][j]);
      }
   }

   for (int i = 0; i < NUM_T_FUNCS; i++) {
      for (int j = 0; j < NUM_T_PERMS; j++) {
         MLPutRuleTo(link, c2cpp(TSIL_GetFunction(&data.data, tbarname[i][j])), tbarname[i][j]);
      }
   }

   for (int i = 0; i < NUM_S_FUNCS; i++) {
      for (int j = 0; j < NUM_S_PERMS; j++) {
         MLPutRuleTo(link, c2cpp(TSIL_GetFunction(&data.data, sname[i][j])), sname[i][j]);
      }
   }

   for (int i = 0; i < NUM_B_FUNCS; i++) {
      for (int j = 0; j < NUM_B_PERMS; j++) {
         MLPutRuleTo(link, c2cpp(TSIL_GetFunction(&data.data, bname[i][j])), bname[i][j]);
      }
   }

   for (int i = 0; i < NUM_V_FUNCS; i++) {
      for (int j = 0; j < NUM_V_PERMS; j++) {
         MLPutRuleTo(link, c2cpp(TSIL_GetFunction(&data.data, vname[i][j])), vname[i][j]);
      }
   }

#define MLPutRuleToTSILFunction(name) \
   MLPutRuleTo(link, data.name, #name)

   MLPutRuleToTSILFunction(Ax);
   MLPutRuleToTSILFunction(Ay);
   MLPutRuleToTSILFunction(Az);
   MLPutRuleToTSILFunction(Au);
   MLPutRuleToTSILFunction(Av);

   MLPutRuleToTSILFunction(Ixyv);
   MLPutRuleToTSILFunction(Izuv);

   // MLPutRuleToTSILFunction(Ap);
   // MLPutRuleToTSILFunction(Aeps);
   // MLPutRuleToTSILFunction(B);
   // MLPutRuleToTSILFunction(Bp);
   // MLPutRuleToTSILFunction(dBds);
   // MLPutRuleToTSILFunction(Beps);
   // MLPutRuleToTSILFunction(I2p);
   // MLPutRuleToTSILFunction(I2p2);
   // MLPutRuleToTSILFunction(I2pp);
   // MLPutRuleToTSILFunction(I2p3);

#undef MLPutRuleToTSILFunction
}

} // anonymous namespace

extern "C" {

/******************************************************************/

DLLEXPORT int TSILEvaluate(
   WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILEvaluate")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      TSIL_Mma_Data data;

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

DLLEXPORT int TSILA(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILA")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL qq = parsvec.at(1);

      TSIL_COMPLEXCPP A;

      {
         Redirect_output rd(link);
         A = c2cpp(TSIL_A(x, qq));
      }

      MLPut(link, A);
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

DLLEXPORT int TSILAp(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILAp")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL qq = parsvec.at(1);

      TSIL_COMPLEXCPP Ap;

      {
         Redirect_output rd(link);
         Ap = c2cpp(TSIL_Ap(x, qq));
      }

      MLPut(link, Ap);
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

DLLEXPORT int TSILAeps(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILAeps")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL qq = parsvec.at(1);

      TSIL_COMPLEXCPP Aeps;

      {
         Redirect_output rd(link);
         Aeps = c2cpp(TSIL_Aeps(x, qq));
      }

      MLPut(link, Aeps);
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

DLLEXPORT int TSILB(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILB")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL s  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP B;

      {
         Redirect_output rd(link);
         B = c2cpp(TSIL_B(x, y, s, qq));
      }

      MLPut(link, B);
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

DLLEXPORT int TSILBp(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILBp")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL s  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP Bp;

      {
         Redirect_output rd(link);
         Bp = c2cpp(TSIL_Bp(x, y, s, qq));
      }

      MLPut(link, Bp);
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

DLLEXPORT int TSILdBds(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILdBds")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL s  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP dBds;

      {
         Redirect_output rd(link);
         dBds = c2cpp(TSIL_dBds(x, y, s, qq));
      }

      MLPut(link, dBds);
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

DLLEXPORT int TSILBeps(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILBeps")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL s  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP Beps;

      {
         Redirect_output rd(link);
         Beps = c2cpp(TSIL_Beps(x, y, s, qq));
      }

      MLPut(link, Beps);
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

DLLEXPORT int TSILI(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILI")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP I2;

      {
         Redirect_output rd(link);
         I2 = c2cpp(TSIL_I2(x, y, z, qq));
      }

      MLPut(link, I2);
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

DLLEXPORT int TSILIp(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILIp")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP I2p;

      {
         Redirect_output rd(link);
         I2p = c2cpp(TSIL_I2p(x, y, z, qq));
      }

      MLPut(link, I2p);
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

DLLEXPORT int TSILIp2(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILIp2")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP I2p2;

      {
         Redirect_output rd(link);
         I2p2 = c2cpp(TSIL_I2p2(x, y, z, qq));
      }

      MLPut(link, I2p2);
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

DLLEXPORT int TSILIpp(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILIpp")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP I2pp;

      {
         Redirect_output rd(link);
         I2pp = c2cpp(TSIL_I2pp(x, y, z, qq));
      }

      MLPut(link, I2pp);
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

DLLEXPORT int TSILIp3(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILIp3")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL qq = parsvec.at(3);

      TSIL_COMPLEXCPP I2p3;

      {
         Redirect_output rd(link);
         I2p3 = c2cpp(TSIL_I2p3(x, y, z, qq));
      }

      MLPut(link, I2p3);
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

DLLEXPORT int TSILM(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILM")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x = parsvec.at(0);
      const TSIL_REAL y = parsvec.at(1);
      const TSIL_REAL z = parsvec.at(2);
      const TSIL_REAL u = parsvec.at(3);
      const TSIL_REAL v = parsvec.at(4);
      const TSIL_REAL s = parsvec.at(5);

      TSIL_COMPLEXCPP Mcpp;

      {
         Redirect_output rd(link);

         TSIL_COMPLEX M;
         const auto is_analytic = TSIL_Manalytic(x, y, z, u, v, s, &M);

         if (is_analytic != 0) {
            Mcpp = c2cpp(M);
         } else {
            const TSIL_REAL qq = 1; // M is independent of qq
            TSIL_DATA data{};
            TSIL_SetParameters(&data, x, y, z, u, v, qq);
            TSIL_Evaluate(&data, s);
            Mcpp = c2cpp(TSIL_GetFunction(&data, "M"));
         }
      }

      MLPut(link, Mcpp);
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

DLLEXPORT int TSILS(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILS")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL s  = parsvec.at(3);
      const TSIL_REAL qq = parsvec.at(4);

      TSIL_COMPLEXCPP Scpp;

      {
         Redirect_output rd(link);

         TSIL_COMPLEX S;
         const auto is_analytic = TSIL_Sanalytic(x, y, z, s, qq, &S);

         if (is_analytic != 0) {
            Scpp = c2cpp(S);
         } else {
            const TSIL_REAL d = 1;
            TSIL_DATA data{};
            TSIL_SetParameters(&data, d, y, z, d, x, qq);
            TSIL_Evaluate(&data, s);
            Scpp = c2cpp(TSIL_GetFunction(&data, "Svyz"));
         }
      }

      MLPut(link, Scpp);
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

DLLEXPORT int TSILT(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILT")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL s  = parsvec.at(3);
      const TSIL_REAL qq = parsvec.at(4);

      TSIL_COMPLEXCPP Tcpp;

      {
         Redirect_output rd(link);

         TSIL_COMPLEX T;
         const auto is_analytic = TSIL_Tanalytic(x, y, z, s, qq, &T);

         if (is_analytic != 0) {
            Tcpp = c2cpp(T);
         } else {
            const TSIL_REAL d = 1;
            TSIL_DATA data{};
            TSIL_SetParameters(&data, d, y, z, d, x, qq);
            TSIL_Evaluate(&data, s);
            Tcpp = c2cpp(TSIL_GetFunction(&data, "Tvyz"));
         }
      }

      MLPut(link, Tcpp);
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

DLLEXPORT int TSILTbar(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILTbar")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL s  = parsvec.at(3);
      const TSIL_REAL qq = parsvec.at(4);

      TSIL_COMPLEXCPP Tbarcpp;

      {
         Redirect_output rd(link);

         TSIL_COMPLEX Tbar;
         const auto is_analytic = TSIL_Tbaranalytic(x, y, z, s, qq, &Tbar);

         if (is_analytic != 0) {
            Tbarcpp = c2cpp(Tbar);
         } else {
            const TSIL_REAL d = 1;
            TSIL_DATA data{};
            TSIL_SetParameters(&data, d, y, z, d, x, qq);
            TSIL_Evaluate(&data, s);
            Tbarcpp = c2cpp(TSIL_GetFunction(&data, "TBARvyz"));
         }
      }

      MLPut(link, Tbarcpp);
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

DLLEXPORT int TSILU(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILU")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL u  = parsvec.at(3);
      const TSIL_REAL s  = parsvec.at(4);
      const TSIL_REAL qq = parsvec.at(5);

      TSIL_COMPLEXCPP Ucpp;

      {
         Redirect_output rd(link);

         TSIL_COMPLEX U;
         const auto is_analytic = TSIL_Uanalytic(x, y, z, u, s, qq, &U);

         if (is_analytic != 0) {
            Ucpp = c2cpp(U);
         } else {
            const TSIL_REAL d = 1;
            TSIL_DATA data{};
            TSIL_SetParameters(&data, y, z, x, d, u, qq);
            TSIL_Evaluate(&data, s);
            Ucpp = c2cpp(TSIL_GetFunction(&data, "Uzxyv"));
         }
      }

      MLPut(link, Ucpp);
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

DLLEXPORT int TSILV(WolframLibraryData /* libData */, MLINK link)
{
   if (!check_number_of_args(link, 1, "TSILV")) {
      return LIBRARY_TYPE_ERROR;
   }

   try {
      const auto parsvec = read_list(link);
      const TSIL_REAL x  = parsvec.at(0);
      const TSIL_REAL y  = parsvec.at(1);
      const TSIL_REAL z  = parsvec.at(2);
      const TSIL_REAL u  = parsvec.at(3);
      const TSIL_REAL s  = parsvec.at(4);
      const TSIL_REAL qq = parsvec.at(5);

      TSIL_COMPLEXCPP Vcpp;

      {
         Redirect_output rd(link);

         TSIL_COMPLEX V;
         const auto is_analytic = TSIL_Vanalytic(x, y, z, u, s, qq, &V);

         if (is_analytic != 0) {
            Vcpp = c2cpp(V);
         } else {
            const TSIL_REAL d = 1;
            TSIL_DATA data{};
            TSIL_SetParameters(&data, y, z, x, d, u, qq);
            TSIL_Evaluate(&data, s);
            Vcpp = c2cpp(TSIL_GetFunction(&data, "Vzxyv"));
         }
      }

      MLPut(link, Vcpp);
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
