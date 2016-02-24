#include <poppler-document.h>
#include <poppler-page.h>
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
  document *doc = document::load_from_raw_data(	(const char*) RAW(x), x.length(), owner_password, user_password);
  int major = 0, minor = 0;
  doc->get_pdf_version(&major, &minor);
  std::string version_str;
  std::ostringstream convert;
  convert << major;
  convert << ".";
  convert << minor;
  List out = List::create(
    Named("version") = convert.str(),
    Named("pages") = doc->pages(),
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

// [[Rcpp::export]]
std::vector<std::string> poppler_pdf_text (RawVector x, std::string owner_password, std::string user_password) {
  document *doc = document::load_from_raw_data(	(const char*) RAW(x), x.length(), owner_password, user_password);
  std::vector<std::string> out;
  for(int i = 0; i < doc->pages(); i++){
    page *p(doc->create_page(i));
    page::text_layout_enum show_text_layout = page::physical_layout;
    byte_array str = p->text(p->page_rect(), show_text_layout).to_utf8();
    out.push_back(std::string(str.begin(), str.end()));
  }
  return out;
}
