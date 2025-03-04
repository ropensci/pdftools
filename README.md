# pdftools

[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](http://www.repostatus.org/badges/latest/active.svg)](http://www.repostatus.org/#active)
[![CRAN_Status_Badge](http://www.r-pkg.org/badges/version/pdftools)](http://cran.r-project.org/package=pdftools)
[![CRAN RStudio mirror downloads](http://cranlogs.r-pkg.org/badges/pdftools)](http://cran.r-project.org/web/packages/pdftools/index.html)

## Introduction

Scientific articles are typically locked away in PDF format, a format designed primarily for printing but not so great for searching or indexing. The pdftools package allows for extracting text and metadata from pdf files in R. From the extracted plain-text one could find articles discussing a particular drug or species name, without having to rely on publishers providing metadata, or pay-walled search engines.

The pdftools slightly overlaps with the Rpoppler package by Kurt Hornik. The main motivation behind developing pdftools was that Rpoppler depends on glib, which does not work well on Mac and Windows. The pdftools package uses the poppler c++ interface together with Rcpp, which results in a lighter and more portable implementation.


## Installation

On Windows and Mac the binary packages can be installed directly from CRAN:

```r
install.packages("pdftools")
```

Installation on Linux requires the poppler development library. For __Ubuntu 18.04 (Bionic)__ and __Ubuntu 20.04 (Focal)__ we provide backports of poppler version 22.02 to support the latest functionality:

```
sudo add-apt-repository -y ppa:cran/poppler
sudo apt-get update
sudo apt-get install -y libpoppler-cpp-dev
```

On other versions of __Debian__ or __Ubuntu__ simply use::

```
sudo apt-get install libpoppler-cpp-dev
```

If you want to install the package from source on __MacOS__ you need brew:

```
brew install poppler
```

On Fedora:

```
sudo yum install poppler-cpp-devel
```

## Getting started

The `?pdftools` manual page shows a brief overview of the main utilities. The most important function is `pdf_text` which returns a character vector of length equal to the number of pages in the pdf. Each string in the vector contains a plain text version of the text on that page.

```r
library(pdftools)
download.file("http://arxiv.org/pdf/1403.2805.pdf", "1403.2805.pdf", mode = "wb")
txt <- pdf_text("1403.2805.pdf")

# first page text
cat(txt[1])

# second page text
cat(txt[2])
```

In addition, the package has some utilities to extract other data from the PDF file. The `pdf_toc` function shows the table of contents, i.e. the section headers which pdf readers usually display in a menu on the left. It looks pretty in JSON:

```r
# Table of contents
toc <- pdf_toc("1403.2805.pdf")

# Show as JSON
jsonlite::toJSON(toc, auto_unbox = TRUE, pretty = TRUE)
```

Other functions provide information about fonts, attachments and metadata such as the author, creation date or tags.


```r
# Author, version, etc
info <- pdf_info("1403.2805.pdf")

# Table with fonts
fonts <- pdf_fonts("1403.2805.pdf")
```

## Rendering pdf files

Another feature of pdftools is rendering of PDF files to bitmap arrays (images). The poppler library provides all functionality to implement a complete PDF reader, including graphical display of the content. In R we can use `pdf_render_page` to render a page of the PDF into a bitmap, which can be stored as e.g. png or jpeg.

```r
# renders pdf to bitmap array
bitmap <- pdf_render_page("1403.2805.pdf", page = 1)

# save bitmap image
png::writePNG(bitmap, "page.png")
webp::write_webp(bitmap, "page.webp")
```

## Limitations and related packages

### Tables

Data scientists are often interested in data from tables. Unfortunately the pdf format is pretty dumb and does not have notion of a table (unlike for example HTML). Tabular data in a pdf file is nothing more than strategically positioned lines and text, which makes it difficult to extract the raw data with `pdftools`.

```r
txt <- pdf_text("http://arxiv.org/pdf/1406.4806.pdf")

# some tables
cat(txt[18])
cat(txt[19])
```

The [`tabulizer`](https://github.com/ropensci/tabulizer) package is dedicated to extracting tables from PDF, and includes interactive tools for selecting tables. However, `tabulizer` depends on `rJava` and therefore requires additional setup steps or may be impossible to use on systems where Java cannot be installed.

It is possible to use `pdftools` with some creativity to parse tables from PDF documents, which does not require Java to be installed.

### Scanned text

If you want to extract text from scanned text present in a pdf, you'll need to use OCR (optical character recognition). Please refer to the [rOpenSci `tesseract` package](https://github.com/ropensci/tesseract) that provides bindings to the Tesseract OCR engine. In particular read [the section of its vignette about reading from PDF files using `pdftools` and `tesseract`](https://cran.r-project.org/web/packages/tesseract/vignettes/intro.html#read_from_pdf_files).


