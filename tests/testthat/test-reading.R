context("reading pdf files")

test_that("reading password protected pdf", {
  # Read with password
  expect_warning(expect_true(pdf_info("pdf-example-password.original.pdf")$locked))
  expect_false(pdf_info("pdf-example-password.original.pdf", upw = "test")$locked)

  # Get text with password
  expect_equal(4, length(pdf_text("pdf-example-password.original.pdf", upw = "test")))
  expect_error(expect_warning(pdf_text("pdf-example-password.original.pdf"), "locked"))

  # Reading 'encrypted' file
  info <- pdf_info("pdf-example-encryption.original.pdf")
  expect_true(info$encrypted)
  expect_false(info$locked)
})
