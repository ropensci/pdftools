#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-font.h>
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
    _["version"] = convert.str(),
    _["pages"] = doc->pages(),
    _["encrypted"] = doc->is_encrypted(),
    _["linearized"] = doc->is_linearized(),
    _["keys"] = doc->info_keys(),
    _["created"] = Datetime(doc->info_date("CreationDate")),
    _["modified"] = Datetime(doc->info_date("ModDate")),
    _["producer"] = doc->info_key("Producer").to_latin1(),
    _["metadata"] = doc->metadata().to_latin1()
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

std::string font_string(font_info::type_enum x){
  switch(x) {
    case font_info::unknown: return "unknown";
    case font_info::type1: return "type1";
    case font_info::type1c: return "type1c";
    case font_info::type1c_ot: return "type1c_ot";
    case font_info::type3: return "type3";
    case font_info::truetype: return "truetype";
    case font_info::truetype_ot: return "truetype_ot";
    case font_info::cid_type0: return "cid_type0";
    case font_info::cid_type0c: return "cid_type0c";
    case font_info::cid_type0c_ot: return "cid_type0c_ot";
    case font_info::cid_truetype: return "cid_truetype";
    case font_info::cid_truetype_ot: return "cid_truetype_ot";
    default: return "";
  }
}

// [[Rcpp::export]]
List poppler_pdf_fonts (RawVector x, std::string owner_password, std::string user_password) {
  document *doc = document::load_from_raw_data(	(const char*) RAW(x), x.length(), owner_password, user_password);
  std::vector<font_info> fonts = doc->fonts();
  CharacterVector fonts_name;
  CharacterVector fonts_type;
  CharacterVector fonts_file;
  LogicalVector fonts_embedded;
  for (std::vector<font_info>::iterator font = fonts.begin(); font != fonts.end(); ++font){
    fonts_name.push_back(font->name());
    fonts_type.push_back(font_string(font->type()));
    fonts_file.push_back(font->file());
    fonts_embedded.push_back(font->is_embedded());
  }
  return DataFrame::create(
    _["name"] = fonts_name,
    _["type"] = fonts_type,
    _["file"] = fonts_file,
    _["embedded"] = fonts_embedded
  );
}
