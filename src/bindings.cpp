#define R_NO_REMAP
#define STRICT_R_HEADERS

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

/* Note: 0.72.1 is my internal build of 0.72.0 + backported UTF-8 patches */
#if defined(POPPLER_VERSION_MINOR) && (POPPLER_VERSION_MINOR >= 73 || POPPLER_VERSION_MAJOR > 0 || \
  (POPPLER_VERSION_MINOR == 72 && POPPLER_VERSION_MICRO == 1))
#define POPPLER_HAS_PAGE_TEXT_LIST
#endif

/* Note: Before poppler 0.73, UTF8 conversion was completely broken on MacOS */
#if defined(POPPLER_HAS_PAGE_TEXT_LIST) || defined(__linux__)
#define ustring_to_r ustring_to_utf8
#else
#define ustring_to_r ustring_to_latin1
#endif

using namespace Rcpp;
using namespace poppler;

static char poppler_data[4000] = "";

// [[Rcpp::export]]
void set_poppler_data(std::string path){
  strcpy(poppler_data, path.c_str());
}

// Call this after initiating document but before page
#ifdef BUNDLE_POPPLER_DATA
#include <GlobalParams.h>
void find_poppler_data(){
  static bool initiated = false;
  if (!initiated && strlen(poppler_data)){
    globalParams = new GlobalParams(poppler_data);
    initiated = true;
  }
}
#endif

String ustring_to_utf8(ustring x){
  byte_array buf = x.to_utf8();
  std::string str(buf.begin(), buf.end());
  String y(str.c_str());
  y.set_encoding(CE_UTF8);
  return y;
}

String ustring_to_latin1(ustring x){
  std::string str(x.to_latin1());
  String y(str.c_str());
  y.set_encoding(CE_LATIN1);
  return y;
}

List item_to_list(toc_item *item){
  List out = List();
  std::vector <toc_item*> children = item->children();
  for(size_t i = 0; i < children.size(); i++){
    out.push_back(item_to_list(children[i]));
  }
  return List::create(
    _["title"] = ustring_to_r(item->title()),
    _["children"] = out
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

document *read_raw_pdf(RawVector x, std::string opw, std::string upw){
  document *doc = document::load_from_raw_data(	(const char*) x.begin(), x.length(), opw, upw);
#ifdef BUNDLE_POPPLER_DATA
  find_poppler_data();
#endif
  if(!doc)
    throw std::runtime_error("PDF parsing failure.");
  if(doc->is_locked())
    throw std::runtime_error("PDF file is locked. Invalid password?");
  return doc;
}

// [[Rcpp::export]]
List get_poppler_config(){
  return List::create(
    _["version"] = poppler::version_string(),
    _["can_render"] = page_renderer::can_render(),
    _["supported_image_formats"] = image::supported_image_formats()
  );
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

  /* can't read the doc */
  if(doc->is_locked()){
    return List::create(
      _["version"] = convert.str(),
      _["encrypted"] = doc->is_encrypted(),
      _["linearized"] = doc->is_linearized(),
      _["created"] = Datetime(doc->info_date("CreationDate")),
      _["modified"] = Datetime(doc->info_date("ModDate")),
      _["locked"] = doc->is_locked()
    );
  }

  List keys = List();
  std::vector<std::string> keystrings = doc->info_keys();
  for (size_t i = 0; i < keystrings.size(); i++) {
    std::string keystr = keystrings[i];
    if(keystr.compare("CreationDate") == 0) continue;
    if(keystr.compare("ModDate") == 0) continue;
    keys.push_back(ustring_to_r(doc->info_key(keystr)), keystr);
  }

  return List::create(
    _["version"] = convert.str(),
    _["pages"] = doc->pages(),
    _["encrypted"] = doc->is_encrypted(),
    _["linearized"] = doc->is_linearized(),
    _["keys"] = keys,
    _["created"] = Datetime(doc->info_date("CreationDate")),
    _["modified"] = Datetime(doc->info_date("ModDate")),
    _["metadata"] = ustring_to_r(doc->metadata()),
    _["locked"] = doc->is_locked(),
    _["attachments"] = doc->has_embedded_files(),
    _["layout"] = layout_string(doc->page_layout())
  );
}

// [[Rcpp::export]]
List poppler_pdf_data (RawVector x, std::string opw, std::string upw) {
#ifdef POPPLER_HAS_PAGE_TEXT_LIST
  document *doc = read_raw_pdf(x, opw, upw);
  Rcpp::List out(doc->pages());
  for(int i = 0; i < doc->pages(); i++){
    page *p(doc->create_page(i));
    if(!p) continue; //missing page
    std::vector<text_box> boxes = p->text_list();
    CharacterVector text(boxes.size());
    IntegerVector width(boxes.size());
    IntegerVector height(boxes.size());
    IntegerVector x(boxes.size());
    IntegerVector y(boxes.size());
    Rcpp::LogicalVector space(boxes.size());
    for(size_t j = 0; j < boxes.size(); j++){
      text[j] = ustring_to_r(boxes.at(j).text());
      width[j] = boxes.at(j).bbox().width();
      height[j] = boxes.at(j).bbox().height();
      x[j] = boxes.at(j).bbox().x();
      y[j] = boxes.at(j).bbox().y();
      space[j] = boxes.at(j).has_space_after();
    }
    out[i] = DataFrame::create(
      _["width"] = width,
      _["height"] = height,
      _["x"] = x,
      _["y"] = y,
      _["space"] = space,
      _["text"] = text,
      _["stringsAsFactors"] = false
    );
  }
  return out;
#else //POPPLER_HAS_PAGE_TEXT_LIST
  throw std::runtime_error(std::string("pdf_data() requires poppler >= 0.73. You have ") + POPPLER_VERSION);
#endif
}

// [[Rcpp::export]]
CharacterVector poppler_pdf_text (RawVector x, std::string opw, std::string upw) {
  document *doc = read_raw_pdf(x, opw, upw);
  CharacterVector out(doc->pages());
  for(int i = 0; i < doc->pages(); i++){
    page *p(doc->create_page(i));
    if(!p) continue; //missing page
    page::text_layout_enum show_text_layout = page::physical_layout;

    /* Workaround for bug https://github.com/ropensci/pdftools/issues/7 */
    rectf target(p->page_rect());
    if(p->orientation() == page::landscape || p->orientation() == page::seascape){
      target.set_right(target.right() * 2);
    }

    /* Rescale page to start at 0 (#24) */
    if(target.top() < 0){
      target.set_bottom(target.bottom() - target.top());
      target.set_top(0);
    }

    //Rprintf("Target: %f-%f x %f-%f\n", target.left(), target.right(), target.bottom(), target.top());

    /* Extract text */
    ustring str = p->text(target, show_text_layout);
    out[i] = ustring_to_utf8(str);
  }
  return out;
}

// [[Rcpp::export]]
List poppler_pdf_fonts (RawVector x, std::string opw, std::string upw) {
  document *doc = read_raw_pdf(x, opw, upw);
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
    _["file"] = fonts_file,
    _["stringsAsFactors"] = false
  );
}

// [[Rcpp::export]]
List poppler_pdf_files (RawVector x, std::string opw, std::string upw) {
  document *doc = read_raw_pdf(x, opw, upw);
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
        _["description"] = ustring_to_r(file->description()),
        _["data"] = res
      ));
    }
  }
  return out;
}

// [[Rcpp::export]]
List poppler_pdf_toc(RawVector x, std::string opw, std::string upw) {
  document *doc = read_raw_pdf(x, opw, upw);
  List out = List();
  toc *contents = doc->create_toc();
  if(!contents)
    return List();
  return item_to_list(contents->root());
}

// [[Rcpp::export]]
RawVector poppler_render_page(RawVector x, int pagenum, double dpi, std::string opw, std::string upw,
                              bool antialiasing = true, bool text_antialiasing = true) {
  if(!page_renderer::can_render())
    throw std::runtime_error("Rendering not supported on this platform!");
  document *doc = read_raw_pdf(x, opw, upw);
  page *p(doc->create_page(pagenum - 1));
  if(!p)
    throw std::runtime_error("Invalid page.");
  page_renderer pr;
  pr.set_render_hint(page_renderer::antialiasing, antialiasing);
  pr.set_render_hint(page_renderer::text_antialiasing, text_antialiasing);
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

// [[Rcpp::export]]
std::vector<std::string> poppler_convert(RawVector x, std::string format, std::vector<int> pages,
                          std::vector<std::string> names, double dpi, std::string opw, std::string upw,
                          bool antialiasing = true, bool text_antialiasing = true, bool verbose = true) {
  if(!page_renderer::can_render())
    throw std::runtime_error("Rendering not supported on this platform!");
  document *doc = read_raw_pdf(x, opw, upw);
  for(size_t i = 0; i < pages.size(); i++){
    int pagenum = pages[i];
    std::string filename = names[i];
    if(verbose)
      Rprintf("Converting page %d to %s...", pagenum, filename.c_str());
    page *p(doc->create_page(pagenum - 1));
    if(!p)
      throw std::runtime_error("Invalid page.");
    page_renderer pr;
    pr.set_render_hint(page_renderer::antialiasing, antialiasing);
    pr.set_render_hint(page_renderer::text_antialiasing, text_antialiasing);
    image img = pr.render_page(p, dpi, dpi);
    if(!img.is_valid())
      throw std::runtime_error("PDF rendering failure.");
    if(!img.save(filename, format, dpi))
      throw std::runtime_error("Failed to save file" + filename);
    if(verbose)
      Rprintf(" done!\n");
  }
  return names;
}

void error_callback(const std::string &msg, void *context){
  Rcpp::Function err_cb = Rcpp::Environment::namespace_env("pdftools")["err_cb"];
  err_cb(Rcpp::String(msg));
}

// [[Rcpp::export]]
void set_error_callback() {
#if defined(POPPLER_VERSION_MINOR) && (POPPLER_VERSION_MINOR >= 30 || POPPLER_VERSION_MAJOR > 0)
  poppler::set_debug_error_function(error_callback, NULL);
#endif
}
