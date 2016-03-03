[![CRAN_Status_Badge](http://www.r-pkg.org/badges/version/pdftools)](http://cran.r-project.org/package=pdftools)
![](http://cranlogs.r-pkg.org/badges/pdftools?color=yellow)
![](http://cranlogs.r-pkg.org/badges/grand-total/pdftools?color=yellowgreen)

# pdftools

**Table of contents:**

* [Introduction](#introduction)
* [Installation](#installation)
* [Usage](#usage)
* [Contact](#contact)


## Introduction

Scientific articles are typically locked away in PDF format, a format designed primarily for printing but not so great for searching or indexing. The new pdftools package allows for extracting text and metadata from pdf files in R. From the extracted plain-text one could find articles discussing a particular drug or species name, without having to rely on publishers providing metadata, or pay-walled search engines.

The pdftools slightly overlaps with the Rpoppler package by Kurt Hornik. The main motivation behind developing pdftools was that Rpoppler depends on glib, which does not work well on Mac and Windows. The pdftools package uses the poppler c++ interface together with Rcpp, which results in a lighter and more portable implementation.


## Installation

To install the stable version on CRAN:

```r
install.packages('pdftools')
```

To install the GitHub version:

```R
install.packages.2 <- function (pkg) if (!require(pkg)) install.packages(pkg);
install.packages.2('devtools')
install.packages.2('Rcpp')
# make sure you have Rtools installed first! if not, then run:
#install.packages('installr'); install.Rtools()
devtools::install_github('ropensci/pdftools')
```

And then you may load the package using:

```R
library("pdftools")
```

## Usage

```r
library(pdftools)
download.file("http://arxiv.org/pdf/1403.2805.pdf", "1403.2805.pdf", mode = "wb")
txt <- pdf_text("1403.2805.pdf")

# first page text
cat(txt[1])

# second page text
cat(txt[2])

# Table of contents
toc <- pdf_toc("1403.2805.pdf")

# Show as JSON
jsonlite::toJSON(toc, auto_unbox = TRUE, pretty = TRUE)

# Author, version, etc
info <- pdf_info("1403.2805.pdf")

# Table with fonts
fonts <- pdf_fonts("1403.2805.pdf")
```

## Contact

You are welcome to:

* submit suggestions and bug-reports at: <https://github.com/ropensci/pdftools/issues>
* send a pull request on: <https://github.com/ropensci/pdftools/>


