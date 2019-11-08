#' PDF utilities
#'
#' Utilities based on libpoppler for extracting text, fonts, attachments
#' and metadata from a pdf file.
#'
#' The \code{\link{pdf_text}} function renders all textboxes on a text canvas
#' and returns a character vector of equal length to the number of pages in the
#' PDF file. On the other hand, \code{\link{pdf_data}} is more low level and
#' returns one data frame per page, containing one row for each textbox in the PDF.
#'
#' Note that \code{\link{pdf_data}} requires a recent version of libpoppler
#' which might not be available on all Linux systems.
#' When using \code{\link{pdf_data}} in R packages, condition use on
#' `poppler_config()$has_pdf_data` which shows if this function can be
#' used on the current system. For Ubuntu 16.04 (Xenial) and 18.04 (Bionic)
#' you can use \href{https://github.com/ropensci/pdftools#installation}{the PPA}
#' with backports of Poppler 0.74.0.
#'
#' Poppler is pretty verbose when encountering minor errors in PDF files,
#' in especially \code{\link{pdf_text}}. These messages are usually safe
#' to ignore, use \code{\link{suppressMessages}} to hide them altogether.
#'
#' @export
#' @param pdf file path or raw vector with pdf data
#' @param opw string with owner password to open pdf
#' @param upw string with user password to open pdf
#' @useDynLib pdftools
#' @name pdftools
#' @rdname pdftools
#' @aliases pdftools
#' @importFrom Rcpp sourceCpp
#' @family pdftools
#' @examples # Just a random pdf file
#' pdf_file <- file.path(R.home("doc"), "NEWS.pdf")
#' info <- pdf_info(pdf_file)
#' text <- pdf_text(pdf_file)
#' fonts <- pdf_fonts(pdf_file)
#' files <- pdf_attachments(pdf_file)
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
pdf_data <- function(pdf, opw = "", upw = "") {
  out <- poppler_pdf_data(loadfile(pdf), opw, upw)
  lapply(out, df_as_tibble)
}

#' @rdname pdftools
#' @export
pdf_fonts<- function(pdf, opw = "", upw = "") {
  out <- poppler_pdf_fonts(loadfile(pdf), opw, upw)
  df_as_tibble(out)
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

#' @rdname pdftools
#' @export
pdf_pagesize <- function(pdf, opw = "", upw = "") {
  poppler_pdf_pagesize(loadfile(pdf), opw, upw)
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

df_as_tibble <- function(df){
  stopifnot(is.data.frame(df))
  class(df) <- c("tbl_df", "tbl", "data.frame")
  df
}
