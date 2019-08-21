#' OCR text extraction
#'
#' Perform OCR text extraction. This requires you have the `tesseract` package.
#'
#' @export
#' @rdname pdf_ocr
#' @family pdftools
#' @inheritParams pdftools
#' @param language passed to [tesseract][tesseract::tesseract] to specify the
#' languge of the engine.
#' @param pages which pages of the pdf file to extract
#' @param dpi resolution to render image that is passed to [tesseract::ocr].
pdf_ocr_text <- function(pdf, pages = NULL, opw = "", upw = "", language = "eng", dpi = 600){
  engine <- tesseract::tesseract(language)
  images <- pdf_convert(pdf = pdf, pages = pages, opw = opw, upw = upw, dpi = dpi)
  on.exit(unlink(images))
  vapply(images, tesseract::ocr, character(1), engine = engine, USE.NAMES = FALSE)
}

#' @export
#' @rdname pdf_ocr
pdf_ocr_data <- function(pdf, pages = NULL, opw = "", upw = "", language = "eng", dpi = 600){
  engine <- tesseract::tesseract(language)
  images <- pdf_convert(pdf = pdf, pages = pages, opw = opw, upw = upw, dpi = dpi)
  on.exit(unlink(images))
  lapply(images, tesseract::ocr_data, engine = engine)
}
