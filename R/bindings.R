#' PDF tools
#'
#' Utilities based on libpoppler for extracting text,
#' fonts and metadata from a pdf file.
#'
#' @export
#' @param pdf file path or raw vector with pdf data
#' @param owner_password string with owner password to open pdf
#' @param user_password string with user password to open pdf
#' @useDynLib pdftools
#' @rdname pdftools
#' @aliases pdftools poppler_info
#' @importFrom Rcpp sourceCpp
#' @export poppler_info
#' @examples download.file("http://cran.r-project.org/doc/manuals/r-release/R-exts.pdf",
#'  "R-exts.pdf", mode = "wb")
#' info <- pdf_info("R-exts.pdf")
#' text <- pdf_text("R-exts.pdf")
#' fonts <- pdf_fonts("R-exts.pdf")
#' files <- pdf_attachments("R-exts.pdf")
pdf_info <- function(pdf, owner_password = "", user_password = "") {
  poppler_pdf_info(loadfile(pdf), owner_password, user_password)
}

#' @rdname pdftools
#' @export
pdf_text <- function(pdf, owner_password = "", user_password = "") {
  poppler_pdf_text(loadfile(pdf), owner_password, user_password)
}

#' @rdname pdftools
#' @export
pdf_fonts<- function(pdf, owner_password = "", user_password = "") {
  poppler_pdf_fonts(loadfile(pdf), owner_password, user_password)
}

#' @rdname pdftools
#' @export
pdf_attachments<- function(pdf, owner_password = "", user_password = "") {
  poppler_pdf_files(loadfile(pdf), owner_password, user_password)
}

#' @rdname pdftools
#' @export
pdf_toc<- function(pdf, owner_password = "", user_password = "") {
  poppler_pdf_toc(loadfile(pdf), owner_password, user_password)
}

#' Render PDF to bitmap
#'
#' Renders a PDF page to a bitmap array which can be written to e.g.
#' png, jpeg or webp using the respective R packages. This function is only
#' available if libpoppler was compiled with cairo support.
#'
#' @export
#' @param pdf file path or raw vector with pdf data
#' @param page which page to render
#' @param numeric convert raw output to (0-1) real values
#' @param opw owner password
#' @param upw user password
#' @aliases render
#' @examples \dontrun{
#' download.file("http://arxiv.org/pdf/1403.2805.pdf", "1403.2805.pdf")
#' bitmap <- pdf_render_page("1403.2805.pdf")
#'
#' library(png)
#' library(jpeg)
#' library(webp)
#' png::writePNG(bitmap, "page.png")
#' jpeg::writeJPEG(bitmap, "page.jpeg")
#' webp::write_webp(bitmap, "page.webp")
#'
#' bitmap_raw <- pdf_render_page("1403.2805.pdf", numeric = FALSE)
#' webp::write_webp(bitmap_raw, "page.webp")
#' }
pdf_render_page<- function(pdf, pages = 0, numeric = TRUE, opw = "", upw = "") {
  out <- poppler_render_page(loadfile(pdf), pages, opw, upw)
  if(identical(dim(out)[1], 4)){
    out <- out[c(3,2,1,4),,] ## convert ARGB to RGBA
  }
  if(isTRUE(numeric)){
    out <- structure(as.numeric(out)/255, dim = dim(out))
    out <- aperm(out)
  } else {
    class(out) <- c("rawimg", class(out))
  }
  return(out)
}

loadfile <- function(pdf){
  if(is.character(pdf)){
    if(grepl("^https?://", pdf[1])){
      pdf <- url(pdf)
    } else {
      path <- normalizePath(pdf, mustWork = TRUE)
      pdf <- readBin(path, raw(), file.info(path)$size)
    }
  }
  if(inherits(pdf, "connection")){
    con <- pdf
    pdf <- raw()
    if(!isOpen(con)){
      open(con, "rb")
      on.exit(close(con))
    }
    while(length(buf <- readBin(con, raw(), 1e6))){
      pdf <- c(pdf, buf)
    }
  }
  if(!is.raw(pdf))
    stop("Argument pdf must be a path or raw vector with PDF data")
  pdf
}
