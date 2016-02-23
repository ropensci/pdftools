#' Bindings to poppler
#'
#' Bla bal
#'
#' @export
#' @useDynLib poppler
#' @rdname poppler
#' @aliases poppler
#' @importFrom Rcpp sourceCpp
#' @export poppler_version
pdf_info <- function(pdf, owner_password = "", user_password = "") {
  if(is.character(pdf)){
    path <- normalizePath(pdf, mustWork = TRUE)
    pdf <- readBin(path, raw(), file.info(path)$size)
  }
  if(!is.raw(pdf))
    stop("Argument pdf must be a path or raw vector with PDF data")
  poppler_pdf_info(pdf, owner_password, user_password)
}
