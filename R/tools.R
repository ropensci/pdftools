#' PDF utilities
#'
#' Utilities based on libpoppler for extracting text, fonts,
#' attachements and metadata from a pdf file.
#'
#' @export
#' @param pdf file path or raw vector with pdf data
#' @param opw string with owner password to open pdf
#' @param upw string with user password to open pdf
#' @useDynLib pdftools
#' @rdname pdftools
#' @aliases pdftools
#' @importFrom Rcpp sourceCpp
#' @family pdftools
#' @examples download.file("http://cran.r-project.org/doc/manuals/r-release/R-exts.pdf",
#'  "R-exts.pdf", mode = "wb")
#' info <- pdf_info("R-exts.pdf")
#' text <- pdf_text("R-exts.pdf")
#' fonts <- pdf_fonts("R-exts.pdf")
#' files <- pdf_attachments("R-exts.pdf")
pdf_info <- function(pdf, opw = "", upw = "") {
  poppler_pdf_info(loadfile(pdf), opw, upw)
}

#' @rdname pdftools
#' @export
pdf_text <- function(pdf, opw = "", upw = "") {
  poppler_pdf_text(loadfile(pdf), opw, upw)
}

#' @rdname pdftools
#' @export
pdf_fonts<- function(pdf, opw = "", upw = "") {
  poppler_pdf_fonts(loadfile(pdf), opw, upw)
}

#' @rdname pdftools
#' @export
pdf_attachments<- function(pdf, opw = "", upw = "") {
  poppler_pdf_files(loadfile(pdf), opw, upw)
}

#' @rdname pdftools
#' @export
pdf_toc<- function(pdf, opw = "", upw = "") {
  poppler_pdf_toc(loadfile(pdf), opw, upw)
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
