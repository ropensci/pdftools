#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-font.h>
#include <poppler-version.h>
#include <poppler-embedded-file.h>
#include <poppler-image.h>
#include <poppler-toc.h>
#include <poppler-page-renderer.h>
#include <Rcpp.h>
#include <cstring>

using namespace Rcpp;
using namespace poppler;

std::string layout_string(document::page_layout_enum x);
std::string font_string(font_info::type_enum x);
std::string layout_string(document::page_layout_enum x);
String ustring_to_rstring(ustring x);
List item_to_list(toc_item *item);

// [[Rcpp::export]]
List poppler_config(){
  return List::create(
    _["version"] = poppler::version_string(),
    _["can_render"] = page_renderer::can_render(),
    _["supported_image_formats"] = image::supported_image_formats()
  );
}

std::string ustring_to_string(ustring x){
  byte_array str = x.to_utf8();
  return std::string(x.begin(), x.end());
}

// [[Rcpp::export]]
List poppler_pdf_info (RawVector x, std::string opw, std::string upw) {
  document *doc = document::load_from_raw_data(	(const char*) x.begin(), x.length(), opw, upw);
  if(!doc)
    throw std::runtime_error("PDF parsing failure.");
  int major = 0, minor = 0;
  doc->get_pdf_version(&major, &minor);
  std::string version_str;
  std::ostringstream convert;
  convert << major;
  convert << ".";
  convert << minor;

  List keys = List();
  std::vector<std::string> keystrings = doc->info_keys();
  for (size_t i = 0; i < keystrings.size(); i++) {
    std::string keystr = keystrings[i];
    if(keystr.compare("CreationDate") == 0) continue;
    if(keystr.compare("ModDate") == 0) continue;
    keys.push_back(ustring_to_rstring(doc->info_key(keystr)), keystr);
  }

  List out = List::create(
    _["version"] = convert.str(),
    _["pages"] = doc->pages(),
    _["encrypted"] = doc->is_encrypted(),
    _["linearized"] = doc->is_linearized(),
    _["keys"] = keys,
    _["created"] = Datetime(doc->info_date("CreationDate")),
    _["modified"] = Datetime(doc->info_date("ModDate")),
    _["metadata"] = ustring_to_rstring(doc->metadata()),
    _["locked"] = doc->is_locked(),
    _["attachments"] = doc->has_embedded_files(),
    _["layout"] = layout_string(doc->page_layout())
  );
  return out;
}

// [[Rcpp::export]]
CharacterVector poppler_pdf_text (RawVector x, std::string opw, std::string upw) {
  document *doc = document::load_from_raw_data(	(const char*) x.begin(), x.length(), opw, upw);
  if(!doc) throw std::runtime_error("PDF parsing failure.");
  CharacterVector out;
  for(int i = 0; i < doc->pages(); i++){
    page *p(doc->create_page(i));
    page::text_layout_enum show_text_layout = page::physical_layout;
    ustring str = p->text(p->page_rect(), show_text_layout);
    out.push_back(ustring_to_rstring(str));
  }
  return out;
}

// [[Rcpp::export]]
List poppler_pdf_fonts (RawVector x, std::string opw, std::string upw) {
  document *doc = document::load_from_raw_data(	(const char*) x.begin(), x.length(), opw, upw);
  if(!doc)
    throw std::runtime_error("PDF parsing failure.");
  std::vector<font_info> fonts = doc->fonts();
  CharacterVector fonts_name;
  CharacterVector fonts_type;
  CharacterVector fonts_file;
  LogicalVector fonts_embedded;
  for (size_t i = 0; i < fonts.size(); i++) {
    font_info font = fonts[i];
    fonts_name.push_back(font.name());
    fonts_type.push_back(font_string(font.type()));
    fonts_file.push_back(font.file());
    fonts_embedded.push_back(font.is_embedded());
  }
  return DataFrame::create(
    _["name"] = fonts_name,
    _["type"] = fonts_type,
    _["embedded"] = fonts_embedded,
    _["file"] = fonts_file
  );
}

// [[Rcpp::export]]
List poppler_pdf_files (RawVector x, std::string opw, std::string upw) {
  document *doc = document::load_from_raw_data(	(const char*) x.begin(), x.length(), opw, upw);
  if(!doc)
    throw std::runtime_error("PDF parsing failure.");
  List out = List();
  if(doc->has_embedded_files()){
    std::vector<embedded_file*> files = doc->embedded_files();
    for (size_t i = 0; i < files.size(); i++) {
      embedded_file *file = files[i];
      byte_array data = file->data();
      RawVector res(data.size());
      std::copy(data.begin(), data.end(), res.begin());
      out.push_back(List::create(
        _["name"] = file->name(),
        _["mime"] = file->mime_type(),
        _["created"] = Datetime(file->creation_date()),
        _["modified"] = Datetime(file->modification_date()),
        _["description"] = ustring_to_rstring(file->description()),
        _["data"] = res
      ));
    }
  }
  return out;
}

// [[Rcpp::export]]
List poppler_pdf_toc(RawVector x, std::string opw, std::string upw) {
  document *doc = document::load_from_raw_data(	(const char*) x.begin(), x.length(), opw, upw);
  if(!doc)
    throw std::runtime_error("PDF parsing failure.");
  List out = List();
  toc *contents = doc->create_toc();
  if(!contents)
    return List();
  return item_to_list(contents->root());
}

// [[Rcpp::export]]
RawVector poppler_render_page(RawVector x, int pagenum, double dpi, std::string opw, std::string upw) {
  if(!page_renderer::can_render())
    throw std::runtime_error("Rendering not supported on this platform!");
  document *doc = document::load_from_raw_data(	(const char*) x.begin(), x.length(), opw, upw);
  if(!doc)
    throw std::runtime_error("PDF parsing failure.");
  page *p(doc->create_page(pagenum));
  if(!p)
    throw std::runtime_error("Invalid page.");
  page_renderer pr;
  pr.set_render_hint(page_renderer::antialiasing, true);
  pr.set_render_hint(page_renderer::text_antialiasing, true);
  image img = pr.render_page(p, dpi, dpi);
  if(!img.is_valid())
    throw std::runtime_error("PDF rendering failure.");
  size_t len = img.bytes_per_row() * img.height();
  RawVector res(len);
  std::memcpy(res.begin(), img.data(), len);
  int channels = 0;
  switch(img.format()){
    case image::format_mono: channels = 1; break;
    case image::format_rgb24: channels = 3; break;
    case image::format_argb32: channels = 4; break;
    default : std::runtime_error("Invalid image format");
  }
  res.attr("dim") = NumericVector::create(channels, img.width(), img.height());
  return res;
}

List item_to_list(toc_item *item){
  List out = List();
  std::vector <toc_item*> children = item->children();
  for(size_t i = 0; i < children.size(); i++){
    out.push_back(item_to_list(children[i]));
  }
  return List::create(
    _["title"] = ustring_to_rstring(item->title()),
    _["children"] = out
  );
}

String ustring_to_rstring(ustring x){
  byte_array str = x.to_utf8();
  String y(std::string(x.begin(), x.end()));
  y.set_encoding(CE_UTF8);
  return y;
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
