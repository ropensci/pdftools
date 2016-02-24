#include <poppler-document.h>
#include <poppler-version.h>
#include <Rcpp.h>
using namespace Rcpp;
using namespace poppler;

// [[Rcpp::export]]
std::string poppler_version(){
  return poppler::version_string();
}

// [[Rcpp::export]]
List poppler_pdf_info (RawVector x, std::string owner_password, std::string user_password) {
  document *doc = document::load_from_raw_data(	(const char*) RAW(x), LENGTH(x), owner_password, user_password);
  int major = 0, minor = 0;
  doc->get_pdf_version(&major, &minor);
  std::string version_str;
  std::ostringstream convert;
  convert << major;
  convert << ".";
  convert << minor;
  List out = List::create(
    Named("version") = convert.str(),
    Named("encrypted") = doc->is_encrypted(),
    Named("linearized") = doc->is_linearized(),
    Named("keys") = doc->info_keys(),
    Named("created") = Datetime(doc->info_date("CreationDate")),
    Named("modified") = Datetime(doc->info_date("ModDate")),
    Named("producer") = doc->info_key("Producer").to_latin1(),
    Named("metadata") = doc->metadata().to_latin1()
  );
  return out;
}
