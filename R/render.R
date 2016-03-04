#' Render PDF to bitmap
#'
#' Renders a PDF page to a bitmap array which can be written to e.g.
#' png, jpeg or webp using the respective R packages. This function is only
#' available if libpoppler was compiled with cairo support.
#'
#' @export
#' @rdname pdf_render_page
#' @param pdf file path or raw vector with pdf data
#' @param page which page to render
#' @param numeric convert raw output to (0-1) real values
#' @param dpi resolution (dots per inch) to render
#' @param opw owner password
#' @param upw user password
#' @family pdftools
#' @aliases render
#' @examples # Rendering not supported on Windows
#' if(poppler_config()$can_render){
#'
#' file.copy(file.path(Sys.getenv("R_DOC_DIR"), "NEWS.pdf"), "news.pdf")
#' bitmap <- pdf_render_page("news.pdf")
#'
#' # save to bitmap formats
#' png::writePNG(bitmap, "page.png")
#' jpeg::writeJPEG(bitmap, "page.jpeg")
#' webp::write_webp(bitmap, "page.webp")
#'
#' # Higher quality
#' bitmap <- pdf_render_page("news.pdf", page = 1, dpi = 300)
#' png::writePNG(bitmap, "page.png")
#'
#' # slightly more efficient
#' bitmap_raw <- pdf_render_page("news.pdf", numeric = FALSE)
#' webp::write_webp(bitmap_raw, "page.webp")
#' }
pdf_render_page<- function(pdf, page = 1, dpi = 72, numeric = TRUE, opw = "", upw = "") {
  out <- poppler_render_page(loadfile(pdf), page, dpi, opw, upw)
  if(identical(dim(out)[1], 4L)){
    out <- out[c(3,2,1,4),,, drop = FALSE] ## convert ARGB to RGBA
  }
  if(isTRUE(numeric)){
    out <- structure(as.numeric(out)/255, dim = dim(out))
    out <- aperm(out)
  } else {
    class(out) <- c("rawimg", class(out))
  }
  return(out)
}

#' @export
#' @rdname pdf_render_page
poppler_config <- function(){
  get_poppler_config()
}
