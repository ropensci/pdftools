#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-font.h>
#include <poppler-version.h>
#include <poppler-embedded-file.h>
#include <poppler-image.h>
#include <Rcpp.h>
using namespace Rcpp;
using namespace poppler;

// [[Rcpp::export]]
List poppler_info(){
  return List::create(
    _["version"] = poppler::version_string(),
    _["supported_image_formats"] = image::supported_image_formats()
  );
}

std::string layout_string(document::page_layout_enum x) {
  switch(x){
  case document::no_layout: return "no_layout";
  case document::single_page: return "single_page";
  case document::one_column: return "one_column";
  case document::two_column_left: return "two_column_left";
  case document::two_column_right: return "two_column_right";
  case document::two_page_left: return "two_page_left";
  case document::two_page_right: return "two_page_right";
  default: return "";
  }
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

  List keys = List::create();
  std::vector<std::string> keystrings = doc->info_keys();
  for (std::vector<std::string>::iterator keystr = keystrings.begin(); keystr != keystrings.end(); ++keystr) {
    if(keystr->compare("CreationDate") == 0) continue;
    if(keystr->compare("ModDate") == 0) continue;
    std::string value(doc->info_key(*keystr).to_latin1());
    keys.push_back(value, *keystr);
  }

  List out = List::create(
    _["version"] = convert.str(),
    _["pages"] = doc->pages(),
    _["encrypted"] = doc->is_encrypted(),
    _["linearized"] = doc->is_linearized(),
    _["keys"] = keys,
    _["created"] = Datetime(doc->info_date("CreationDate")),
    _["modified"] = Datetime(doc->info_date("ModDate")),
    _["metadata"] = doc->metadata().to_latin1(),
    _["locked"] = doc->is_locked(),
    _["attachments"] = doc->has_embedded_files(),
    _["layout"] = layout_string(doc->page_layout())
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

// [[Rcpp::export]]
List poppler_pdf_files (RawVector x, std::string owner_password, std::string user_password) {
  document *doc = document::load_from_raw_data(	(const char*) RAW(x), x.length(), owner_password, user_password);
  List out = List::create();
  if(doc->has_embedded_files()){
    std::vector<embedded_file*> files = doc->embedded_files();
    for (std::vector<embedded_file*>::iterator file = files.begin(); file != files.end(); ++file){
      byte_array data = (*file)->data();
      RawVector res(data.size());
      std::copy(data.begin(), data.end(), res.begin());
      out.push_back(List::create(
        _["name"] = (*file)->name(),
        _["mime"] = (*file)->mime_type(),
        _["created"] = Datetime((*file)->creation_date()),
        _["modified"] = Datetime((*file)->modification_date()),
        _["description"] = (*file)->description().to_latin1(),
        _["data"] = res
      ));
    }
  }
  return out;
}

