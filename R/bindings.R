#' Bindings to poppler
#'
#' Extract text or data from PDF files.
#'
#' @export
#' @param pdf file path or raw vector with pdf data
#' @param owner_password string with owner password to open pdf
#' @param user_password string with user password to open pdf
#' @useDynLib poppler
#' @rdname poppler
#' @aliases poppler poppler_version
#' @importFrom Rcpp sourceCpp
#' @export poppler_version
#' @examples pdffile <- "https://cran.r-project.org/doc/manuals/r-release/R-exts.pdf"
#' download.file(pdffile, "R-exts.pdf", mode = "wb")
#' pdf_info("R-exts.pdf")
#' pdf_text("R-exts.pdf")
pdf_info <- function(pdf, owner_password = "", user_password = "") {
  poppler_pdf_info(loadfile(pdf), owner_password, user_password)
}

#' @rdname poppler
#' @export
pdf_text <- function(pdf, owner_password = "", user_password = "") {
  poppler_pdf_text(loadfile(pdf), owner_password, user_password)
}

loadfile <- function(pdf){
  if(is.character(pdf)){
    path <- normalizePath(pdf, mustWork = TRUE)
    pdf <- readBin(path, raw(), file.info(path)$size)
  }
  if(!is.raw(pdf))
    stop("Argument pdf must be a path or raw vector with PDF data")
  pdf
}
