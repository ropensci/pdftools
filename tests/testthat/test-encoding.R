context("encoding")

test_that("text encoding english", {
  skip_if(poppler_config()$version < "0.41")
  toc <-pdf_toc("hello.pdf")
  expect_equal(toc$children[[1]]$title, "Chapter 1")

  info <- pdf_info("hello.pdf")
  expect_equal(info$keys$Author, "Jeroen Ooms")
  expect_equal(info$keys$Title, "Hello World")

  text <- pdf_text('hello.pdf')
  expect_match(text, 'Hello World')

  skip_if_not(poppler_config()$has_pdf_data)

  data <- pdf_data('hello.pdf')
  expect_length(data[[1]]$text, 15)
  expect_equal(data[[1]]$text[1:3], c("Hello", "World", "Jeroen"))
})

test_that("text encoding korean", {
  #skip_on_os("solaris")
  skip_on_cran()
  skip_if(poppler_config()$version < "0.41")
  info <- pdf_info("gangnam.pdf")
  psy <- "\uC2F8\uC774"
  gangnam <- "\uAC15\uB0A8\uC2A4\uD0C0\uC77C"
  expect_equal(info$keys$Author, psy)
  expect_equal(info$keys$Title, gangnam)

  text <- pdf_text('gangnam.pdf')
  expect_match(text, gangnam)

  skip_if_not(poppler_config()$has_pdf_data)

  data <- pdf_data('gangnam.pdf')
  expect_equal(data[[1]]$text[1:3], c(gangnam, psy, "12/4/2018"))
})

