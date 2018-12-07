context("encoding")

test_that("text encoding english", {

  toc <-pdf_toc("hello.pdf")
  expect_equal(toc$children[[1]]$title, "Chapter 1")

  info <- pdf_info("hello.pdf")
  expect_equal(info$keys$Author, "Jeroen Ooms")
  expect_equal(info$keys$Title, "Hello World")

  text <- pdf_text('hello.pdf')
  expect_match(text, 'Hello World')

  data <- pdf_data('hello.pdf')
  expect_length(data[[1]]$text, 15)
  expect_equal(data[[1]]$text[1:3], c("Hello", "World", "Jeroen"))
})
