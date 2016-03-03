#include <Rcpp.h>
#include <R_ext/Rdynload.h>
#include <poppler-global.h>
#include <poppler-version.h>

#if defined(POPPLER_VERSION_MINOR) && (POPPLER_VERSION_MINOR >= 30 || POPPLER_VERSION_MAJOR > 0)

/* export to C */
extern "C" void R_init_pdftools(DllInfo *info);

void error_callback(const std::string &msg, void *context){
  Rcpp::Function err_cb = Rcpp::Environment::namespace_env("base")["message"];
  err_cb(Rcpp::String(msg));
}

void R_init_pdftools(DllInfo *info) {
  poppler::set_debug_error_function(error_callback, NULL);
}

#endif
